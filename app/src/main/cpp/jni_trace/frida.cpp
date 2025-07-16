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

void other() {

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