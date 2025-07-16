#include <stdio.h>
#include <thread>
#include <sys/sysinfo.h>


#include "../third/byopen/hack_dlopen.h"
#include "../third/utils/log.h"
#include "../third/utils/utils.h"
#include "../common_hook.h"
#include "../dump_so.h"
#include "../base/hook.h"
#include "../base/when_hook.h"
#include "../global/global.h"

using namespace std;

#define TargetLibName   "libnms.so"
extern char *targetLibBase;

DefineHookStub(deal_proto, void*, const void **proto) {
    void **proto1 = (void **) proto[3];
    void *app_process_addr = proto1[23];
    logd("pangle app_process_addr: %p", app_process_addr);
    return pHook_deal_proto(proto);
}

void hook_pangle_pkg() {
    WhenSoInitHook(TargetLibName, [&](const string &path, void *addr, const string &funcType) {
        auto handle = hack_dlopen(TargetLibName, 0);
        targetLibBase = (char *) handle->biasaddr;
        setLogRetOffset(targetLibBase);
        InlineHookAddr(targetLibBase, 0x15A78, deal_proto);
        hack_dlclose(handle);
    });
}


void hook_pangle_log() {
    WhenSoInitHook(TargetLibName, [&](const string &path, void *addr, const string &funcType) {
        auto handle = hack_dlopen(TargetLibName, 0);
        targetLibBase = (char *) handle->biasaddr;
        setLogRetOffset(targetLibBase);
        LOGI("hook pangle log");
//        InlineHookAddr(targetLibBase, 0xA128, hook_safe_sprinf);
//        InlineHookAddr(targetLibBase, 0xA1BC, hook_sprinf);
    });
}


bool dump_pangle_so() {
    return WhenSoInitHook(TargetLibName,
                          [&](const string &path, void *addr, const string &funcType) {
                              auto handle = hack_dlopen(TargetLibName, 0);
                              targetLibBase = (char *) handle->biasaddr;
                              setLogRetOffset(targetLibBase);
                              LOGI("will dump_pangle_so, base: %p", targetLibBase);

                              thread *mytobj = new thread([]() {
                                  sleep(5);
                                  dump_so(TargetLibName, "/data/data/" + getPkgName());
                              });

                          });
}


DefineHookStub(ttEncrypt, jbyteArray, JNIEnv * jni, void *clz, jbyteArray data, int len) {
    logi("ttEncrypt: %d", len);
    jbyte *decode = jni->GetByteArrayElements(data, 0);
    char *decodeHexStr = new char[len * 2 + 1]{};
    hex2str((char *) decode, len, decodeHexStr, len * 2 + 1);
    jbyteArray encode = pHook_ttEncrypt(jni, clz, data, len);
    int encodeLen = jni->GetArrayLength(encode);
    char *encodeHexStr = new char[encodeLen * 2 + 1]{};
    hex2str((char *) jni->GetByteArrayElements(encode, 0), encodeLen, encodeHexStr,
            encodeLen * 2 + 1);
    logi("decode: %s", decodeHexStr);
    logi("encode: %s", encodeHexStr);
    return encode;
}


void hook_pangle_log_pkg() {
    WhenSoInitHook("libEncryptorP.so", [&](const string &path, void *addr, const string &funcType) {
        auto handle = hack_dlopen("libEncryptorP.so", 0);
        targetLibBase = (char *) handle->biasaddr;
        setLogRetOffset(targetLibBase);
        InlineHookAddr(targetLibBase, 0x07A04, ttEncrypt);
        hack_dlclose(handle);
    });
}
