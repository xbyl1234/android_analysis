#include <jni.h>

#include "third/utils/log.h"
#include "third/byopen/hack_dlopen.h"

int find_jnioffset(JNIEnv *env) {
    auto handle = hack_dlopen("libandroid_runtime.so", 1);
    auto native_addr = hack_dlsym(handle,
                                  "_Z32android_os_Process_getUidForNameP7_JNIEnvP8_jobjectP8_jstring");
    jclass processClass = env->FindClass("android/os/Process");
    if (processClass == nullptr) {
        return -1;
    }
    jmethodID getMethods = env->GetMethodID(env->GetObjectClass(processClass),
                                            "getDeclaredMethods", "()[Ljava/lang/reflect/Method;");
    if (getMethods == nullptr) {
        return -1;
    }
    jobjectArray methods = (jobjectArray) env->CallObjectMethod(processClass, getMethods);
    if (methods == nullptr) {
        return -1;
    }
    jsize length = env->GetArrayLength(methods);
    jclass methodClass = env->FindClass("java/lang/reflect/Method");
    jmethodID toString = env->GetMethodID(methodClass, "toString", "()Ljava/lang/String;");
    jmethodID getModifiers = env->GetMethodID(methodClass, "getModifiers", "()I");

    jclass executableClass = env->FindClass("java/lang/reflect/Executable");
    jmethodID getArtMethod = env->GetMethodID(executableClass,
                                              "getArtMethod",
                                              "()J");
    for (jsize i = 0; i < length; i++) {
        jobject method = env->GetObjectArrayElement(methods, i);
        jint modifiers = env->CallIntMethod(method, getModifiers);
        if (modifiers & 256) {
            jstring methodName = (jstring) env->CallObjectMethod(method, toString);
            const char *name = env->GetStringUTFChars(methodName, nullptr);
            if (strstr(name, "getUidForName") != nullptr) {
                uint64_t art = env->CallLongMethod(method, getArtMethod);
                for (int idx = 0; idx < 30; ++idx) {
                    if (*(void **) (art + idx * sizeof(void *)) == native_addr) {
                        return idx;
                    }
                }
            }
            env->ReleaseStringUTFChars(methodName, name);
            env->DeleteLocalRef(methodName);
        }
        env->DeleteLocalRef(method);
    }
    env->DeleteLocalRef(methods);
    env->DeleteLocalRef(methodClass);
    env->DeleteLocalRef(processClass);
    return -1;
}

void find_native_method_addr(JNIEnv *env, jobject targetClz) {
    logi("find_native_method_addr %p", targetClz);
    int jnioffset = find_jnioffset(env);
    if (jnioffset == -1) {
        loge("failed to get jni offset");
        return;
    }
    logi("jnioffset %d", jnioffset);
    jclass classObj = env->GetObjectClass(targetClz);
    jmethodID getMethods = env->GetMethodID(classObj, "getDeclaredMethods",
                                            "()[Ljava/lang/reflect/Method;");
    jobjectArray methods = (jobjectArray) env->CallObjectMethod(targetClz, getMethods);
    jsize length = env->GetArrayLength(methods);
    jclass methodClass = env->FindClass("java/lang/reflect/Method");
    jmethodID toString = env->GetMethodID(methodClass, "toString", "()Ljava/lang/String;");
    jmethodID getModifiers = env->GetMethodID(methodClass, "getModifiers", "()I");
    jmethodID getArtMethod = env->GetMethodID(env->FindClass("java/lang/reflect/Executable"),
                                              "getArtMethod", "()J");

    for (jsize i = 0; i < length; i++) {
        jobject method = env->GetObjectArrayElement(methods, i);
        jint modifiers = env->CallIntMethod(method, getModifiers);
        if (modifiers & 256) {
            jstring methodName = (jstring) env->CallObjectMethod(method, toString);
            const char *name = env->GetStringUTFChars(methodName, nullptr);
            uint64_t art = env->CallLongMethod(method, getArtMethod);

            Dl_info info{};
            void *native_addr = *(void **) (art + jnioffset * sizeof(void *));
            if (dladdr(native_addr, &info) != 0) {
                logi("RegisterNatives: %s %s %p", info.dli_fname, name,
                     (uint64_t) native_addr - (uint64_t) info.dli_fbase);
            } else {
                loge("RegisterNatives: %p", native_addr);
            }
            env->ReleaseStringUTFChars(methodName, name);
            env->DeleteLocalRef(methodName);
        }
        env->DeleteLocalRef(method);
    }

    env->DeleteLocalRef(methods);
    env->DeleteLocalRef(methodClass);
}