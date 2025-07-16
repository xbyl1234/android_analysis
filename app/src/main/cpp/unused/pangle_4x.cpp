#include <string>
#include <fstream>
#include <sys/stat.h>
#include <android/log.h>
#include <unistd.h>
#include <jni.h>

#include "../third/dobby/include/dobby.h"
#include "../third/byopen/hack_dlopen.h"


using namespace std;

//    const-string v0, "pangle_hook"
//    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V

//<?xml version="1.0" encoding="utf-8"?>
//<network-security-config>
//    <base-config cleartextTrafficPermitted="true">
//        <trust-anchors>
//            <certificates src="system" />
//            <certificates src="user" />
//        </trust-anchors>
//    </base-config>
//</network-security-config>

#define logi(...) __android_log_print(ANDROID_LOG_DEBUG, "MyModule2", __VA_ARGS__)
#define EXPORT extern "C"  __attribute__ ((visibility ("default")))
EXPORT
void stub_proto2str();

using func_proto2str = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t,
                                    uint64_t, uint64_t, uint64_t, uint64_t);
func_proto2str pproto2str;
static int idx = 0;
const string rootPath = "/storage/emulated/0/Android/data/com.mergetale.arlyug";





bool writ_file(const string &path, const char *buf, int len) {
    std::fstream file(path.c_str(), std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file.write(buf, len);
    file.flush();
    file.close();
    return true;
}

void readStdString(char *p, char **buff, uint32_t *len) {
    *buff = (char *) *(void **) (p + 16);
    *len = *(uint32_t *) (p + 12);
}

EXPORT
void *get_pproto2str() {
    return (void *) (pproto2str);
}

EXPORT
void proto2str(uint64_t x3) {
    sleep(2);
    char *buff = nullptr;
    uint32_t len = 0;
    readStdString((char *) *((void **) x3), &buff, &len);

    logi("pangle send len: %d", len);

    idx += 1;
    if (!writ_file(rootPath + "/pkg_" + to_string(time(nullptr)) + "_" + to_string(idx),
                   buff, len)) {
        logi("write file error: %d", errno);
    }
}

EXPORT
void capture() {
    hack_linker_init();
    module_info_t info;
    if (!hack_get_module_info("libPglmetasec_ov.so", &info)) {
        logi("not find target library module!");
        return;
    }

    mkdir(rootPath.c_str(), 16893);
    DobbyHook((char *) info.module_address + 0x97C8C, (dobby_dummy_func_t) stub_proto2str,
              (dobby_dummy_func_t *) &pproto2str);
}


static bool hadHook = false;
using func_hook_android_dlopen_ext = void *(*)(const char *filename, int flag, const void *extinfo);
func_hook_android_dlopen_ext phook_android_dlopen_ext;


__attribute__((__weak__))
void *hook_android_dlopen_ext(const char *filename, int flag, const void *extinfo) {
    void *ret = phook_android_dlopen_ext(filename, flag, extinfo);
    if (!hadHook && strstr(filename, "libPglmetasec_ov.so")) {
        logi("load target library finish!");
        hadHook = true;
        capture();
    }
    return ret;
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    logi("hook pangle load!");
    auto pdlopel = DobbySymbolResolver("linker64", "android_dlopen_ext");
    DobbyHook(pdlopel, (dobby_dummy_func_t) hook_android_dlopen_ext,
              (dobby_dummy_func_t *) &phook_android_dlopen_ext);
    return JNI_VERSION_1_6;
}