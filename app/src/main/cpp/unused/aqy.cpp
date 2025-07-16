#include <stdio.h>
#include <thread>
#include <sys/sysinfo.h>

#include "../third/utils/linux_helper.h"
#include "../third/byopen/hack_dlopen.h"
#include "../third/utils/log.h"
#include "../third/json/json.h"
#include "../third/utils/utils.h"
#include "../third/base64/base64.h"
#include "../common_hook.h"
#include "../dump_so.h"
#include "../base/when_hook.h"
#include "../base/hook.h"


static char *targetBase;
static Json::Value devices;
#define  DeviceFilePath "/data/data/com.qiyi.video/device"

void InitDevice() {
    if (access(DeviceFilePath, F_OK) == 0) {
        string data = ReadFile(DeviceFilePath);
        if (data.empty()) {
            goto gen;
        }
        devices = ToJson(data);
        return;
    }

    gen:
    devices["/system/framework"] = gen_hexstr(32);
    devices["/vendor/lib"] = gen_hexstr(32);
    devices["/system/bin"] = gen_hexstr(32);
    devices["/vendor/firmware"] = gen_hexstr(32);
    devices["/data/system"] = gen_hexstr(32);
    char buff[16];
    gen_hex(16, buff);
    devices["pkg_path"] =
            "/data/app/com.qiyi.video-" +
            base64_encode((unsigned char const *) buff, 16) +
            "/base.apk";
    string data = Json2String(devices);
    WritFile(DeviceFilePath, data.c_str(), data.size());
}

string getDeviceParams(const string &key) {
    auto result = devices[key];
    if (result.isNull()) {
        LOGI("get devices params %s is null!", key.c_str());
        exit(0);
    }
    return result.asString();
}

DefineHookStub(json2str, char *, void *result, size_t a2, void *a3, void *a4) {
    char *ret = pHook_json2str(result, a2, a3, a4);
    LOGI("pkg: %s", ret);
    log2file(xbyl::format_string("ret: %p, pkg: %s",
                           (uint64_t) GetStack(0) - (uint64_t) targetBase,
                           ret));
    return ret;
}

DefineHookStub(set_value, void *, void *result, void *a2, int a3, const char *a4) {
    LOGI("set_value: %d  %s", a3, a4);
    log2file(xbyl::format_string("set_value: %d  %s", a3, a4));
    if (a3 == 57) {
        string fake = getDeviceParams("pkg_path");
        LOGI("fake pkg path %s", fake.c_str());
        return pHook_set_value(result, a2, a3, fake.c_str());
    }
    return pHook_set_value(result, a2, a3, a4);
}

DefineHookStub(check_file_stat, char*, const char *dirPath, const char *fileName, char flag) {
    auto result = pHook_check_file_stat(dirPath, fileName, flag);
    if (result != nullptr) {
        string fake = getDeviceParams(dirPath);
        LOGI("fake %s hash %s -> %s", dirPath, result, fake.c_str());
        strcpy(result, fake.c_str());
    }
    return result;
}

DefineHookStub(check_tun, void*) {
    auto ret = pHook_check_tun();
    *(targetBase + 0x6B514) = 0;
    LOGI("fake set check tun 0");
    return ret;
}


DefineHookStub(make_json_value, void*, const char *a1) {
    void *result;
    int64_t retAddr = (char *) GetStack(0) - targetBase;
    if (retAddr == 0x26864) {
        result = pHook_make_json_value("false");
        LOGI("fake set check libc export false");
    } else {
        result = pHook_make_json_value(a1);
    }
    return result;
}

void hook_aqy() {
    InitDevice();

    WhenSoInitHook("libprotect.so", [&](const string &path, void *addr, const string &funcType) {
        MapsHelper maps;
        maps.refresh("libprotect.so");
        targetBase = (char *) maps.get_module_base("libprotect.so");

        InlineHookAddr(targetBase, 0x37840, json2str);
        InlineHookAddr(targetBase, 0x3B99C, set_value);
        InlineHookAddr(targetBase, 0x25A48, check_tun);
        InlineHookAddr(targetBase, 0x37DD4, make_json_value);
        InlineHookAddr(targetBase, 0x260EC, check_file_stat);
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_android_analyse_hook_app_ayq_nativeHookPkg(JNIEnv *env, jclass clazz) {
    hook_aqy();
}