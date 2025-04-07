#include <jni.h>
#include <unistd.h>
#include "../global/global.h"
#include "../third/utils/log.h"
#include "../third/utils/jni_helper.hpp"
#include "../base/when_hook.h"
#include "../dump_so.h"
#include "../global/global.h"
#include "../base/when_hook.h"
#include "../base/hook.h"

extern "C"
JNIEXPORT jboolean JNICALL init(JNIEnv *env, jclass frida_helper);

jclass loadFridaHelperDex(JNIEnv *env) {
    const char *dexPath = "/data/frida_helper.dex";
    jclass classLoader = env->FindClass("dalvik/system/DexClassLoader");
    if (classLoader == nullptr) {
        LOGI("Failed to find DexClassLoader");
        return nullptr;
    }
    jmethodID constructor = env->GetMethodID(classLoader, "<init>",
                                             "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
    if (constructor == nullptr) {
        LOGI("Failed to find DexClassLoader constructor");
        return nullptr;
    }
    jstring dexPathString = env->NewStringUTF(dexPath);
    jstring optimizedDirectory = env->NewStringUTF("/data/local/tmp/");
    jobject nullObj = nullptr;
    jobject classLoaderInstance = env->NewObject(classLoader, constructor, dexPathString,
                                                 optimizedDirectory, nullObj, nullObj);
    if (classLoaderInstance == nullptr) {
        LOGI("Failed to create DexClassLoader instance");
        return nullptr;
    }
    jclass frida_helper = (jclass) env->CallObjectMethod(classLoaderInstance,
                                                         env->GetMethodID(classLoader, "loadClass",
                                                                          "(Ljava/lang/String;)Ljava/lang/Class;"),
                                                         env->NewStringUTF(
                                                                 "com.frida.frida_helper"));
    if (frida_helper == nullptr) {
        LOGI("Failed to load com.frida.frida_helper");
        return nullptr;
    }
//    jclass fridaHelperClass = env->FindClass("com/frida/frida_helper");
//    if (fridaHelperClass == nullptr) {
//        LOGI("Failed to find com.frida.frida_helper class");
//        return false;
//    }
//    LOGI("Successfully loaded com.frida.frida_helper class");
    return frida_helper;
}

DefineHookStub(decode_str_ai, void*, char *s, char *t) {
    auto result = pHook_decode_str_ai(s, t);
    auto stack = GetStack01();
    string stackStr;
    for (const auto &item: stack) {
        stackStr += xbyl::format_string("%p,", item.offset);
    }
    log2file("immortal decode_str %s, %s", stackStr.c_str(), t);
    return result;
}

DefineHookStub(decode_str_ai2, void*, char *s, char *t) {
    auto result = pHook_decode_str_ai2(s, t);
    auto stack = GetStack01();
    string stackStr;
    for (const auto &item: stack) {
        stackStr += xbyl::format_string("%p,", item.offset);
    }
    log2file("coordinator decode_str %s, %s", stackStr.c_str(), t);
    return result;
}
DefineHookStub(decode_str_ai3, void*, char *s, char *t) {
    auto result = pHook_decode_str_ai3(s, t);
    auto stack = GetStack01();
    string stackStr;
    for (const auto &item: stack) {
        stackStr += xbyl::format_string("%p,", item.offset);
    }
    log2file("mars decode_str %s, %s", stackStr.c_str(), t);
    return result;
}

//DefineHookStub(coord_check1, int64_t, void *a1, void *a2) {
//    int64_t result = pHook_coord_check1(a1, a2);
//    logi("coord check1: %d", result);
//    return 1;
//}
//
//DefineHookStub(coord_check2, int64_t, void *a1, void *a2) {
//    int64_t result = pHook_coord_check2(a1, a2);
//    logi("coord check2: %d", result);
//    return 1;
//}
//
//DefineHookStub(coord_check3, int64_t, void *a1, void *a2) {
//    int64_t result = pHook_coord_check3(a1, a2);
//    logi("coord check3: %d", result);
//    return 1;
//}
//
//DefineHookStub(immortal_check1, int64_t, void *a1, void *a2) {
//    int64_t result = pHook_immortal_check1(a1, a2);
//    logi("coord check1: %d", result);
//    return 1;
//}
//
//DefineHookStub(immortal_check2, int64_t, void *a1, void *a2) {
//    int64_t result = pHook_immortal_check2(a1, a2);
//    logi("coord check2: %d", result);
//    return 1;
//}

void other() {
    WhenSoLoaded("libimmortal.so",
                 [&](const string &path, void *addr, const string &funcType) {
                     auto handle = hack_dlopen("libimmortal.so", 0);
                     auto base = (char *) handle->biasaddr;
                     logi("onload libimmortal");
                     InlineHookAddr(base, 0xA7B4, decode_str_ai);
//                     InlineHookAddr(base, 0xAFA0, immortal_check1);
//                     InlineHookAddr(base, 0x06B38, immortal_check2);
                     hack_dlclose(handle);
                 });

    WhenSoLoaded("libcoordinator.so",
                 [&](const string &path, void *addr, const string &funcType) {
                     auto handle = hack_dlopen("libcoordinator.so", 0);
                     auto base = (char *) handle->biasaddr;
                     logi("onload libcoordinator");
                     InlineHookAddr(base, 0x08F08, decode_str_ai2);
//                     InlineHookAddr(base, 0x09030, coord_check1);
//                     InlineHookAddr(base, 0x9290, coord_check2);
//                     InlineHookAddr(base, 0x07358, coord_check3);
                     hack_dlclose(handle);
                 });

    WhenSoLoaded("libmars.so",
                 [&](const string &path, void *addr, const string &funcType) {
                     auto handle = hack_dlopen("libmars.so", 0);
                     auto base = (char *) handle->biasaddr;
                     logi("onload libmars");
                     InlineHookAddr(base, 0x75A4, decode_str_ai3);
                     hack_dlclose(handle);
                 });

}

extern "C" JNIEXPORT void JNICALL
init_jni_hook(JNIEnv *env, const char *pkg_name) {
    setPkgName(pkg_name);
    LOGI("analyse inject pid: %d, %s", getpid(), getPkgName().c_str());
    jclass frida_helper = loadFridaHelperDex(env);
    if (frida_helper == nullptr) {
        LOGI("not find frida helper");
        return;
    }
    init(env, frida_helper);
    other();
}