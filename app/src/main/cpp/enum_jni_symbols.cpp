#include <jni.h>
#include "third/utils/log.h"
#include "third/byopen/hack_dlopen.h"
#include <jni.h>

bool enum_dlsym(const char *symbol, void *addr, void *ctx) {
    if (strstr(symbol, "art") && strstr(symbol, "JNI")) {
        LOGI("symbol %s %p", symbol, addr);
    }
    return true;
}

void enum_jni_symbols() {
    auto handle = hack_dlopen("libart.so", 0);
    hack_enum_dlsym(handle, enum_dlsym, nullptr);
    hack_dlclose(handle);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_android_analyse_hook_Native_nativeEnumSymbols(JNIEnv *env, jclass clazz) {
    enum_jni_symbols();
}