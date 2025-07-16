#include <jni.h>
#include "third/utils/log.h"
#include "base/when_hook.h"
#include "base/hook.h"
#include "third/byopen/hack_dlopen.h"

#define libcoldstart "libcoldstart.so"
#define libliger "libliger-common.so"

bool proxygen_SSLVerification_verifyWithMetrics() {
    return true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_android_analyse_hook_Native_passFbSslPinning(JNIEnv *env, jclass clazz) {
    WhenSoInitHook(libcoldstart, [&](const string &path, void *addr, const string &funcType) {
//        proxygen::SSLVerification::verifyWithMetrics
        auto handle = hack_dlopen(libcoldstart, 0);
        hack_enum_dlsym(
                handle, [](const char *symbol, void *addr, void *ctx) -> bool {
                    if (strstr(symbol, "proxygen") != nullptr &&
                        strstr(symbol, "SSLVerification") != nullptr &&
                        strstr(symbol, "verifyWithMetrics") != nullptr) {
                        logi("find fb verifyWithMetrics %p", addr);
                        DobbyHook(addr,
                                  (dobby_dummy_func_t) proxygen_SSLVerification_verifyWithMetrics,
                                  (dobby_dummy_func_t *) nullptr);
                        return false;
                    }
                    return true;
                }, nullptr);
        unsigned char code[] = {0x00, 0xb4, 0x80, 0x82, 0x4c, 0x39};
        uint8_t newCode[] = {0xb5};
        char *p = (char *) memmem(handle->biasaddr, handle->filesize, code, sizeof(char));
        if (p == nullptr) {
            loge("not find fb code");
            return;
        }
        logi("patch fb code: %p", p);
        DobbyCodePatch(p + 1, newCode, 1);
        hack_dlclose(handle);
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_com_android_analyse_hook_Native_passInsSslPinning(JNIEnv *env, jclass clazz) {
    WhenSoInitHook(libliger, [&](const string &path, void *addr, const string &funcType) {
//        proxygen::SSLVerification::verifyWithMetrics
        auto handle = hack_dlopen(libliger, 0);
        hack_enum_dlsym(
                handle, [](const char *symbol, void *addr, void *ctx) -> bool {
                    if (strstr(symbol, "proxygen") != nullptr &&
                        strstr(symbol, "SSLVerification") != nullptr &&
                        strstr(symbol, "verifyWithMetrics") != nullptr) {
                        logi("find ins verifyWithMetrics %p", addr);
                        DobbyHook(addr,
                                  (dobby_dummy_func_t) proxygen_SSLVerification_verifyWithMetrics,
                                  (dobby_dummy_func_t *) nullptr);
                        return false;
                    }
                    return true;
                }, nullptr);
        unsigned char code[] = {0x00, 0xb4, 0x80, 0x82, 0x4c, 0x39};
        uint8_t newCode[] = {0xb5};
        char *p = (char *) memmem(handle->biasaddr, handle->filesize, code, sizeof(char));
        if (p == nullptr) {
            loge("not find ins code");
            return;
        }
        logi("patch ins code: %p", p);
        DobbyCodePatch(p + 1, newCode, 1);
        hack_dlclose(handle);
    });
}