#pragma once

#include <jni.h>
#include "../third/byopen/hack_dlopen.h"

#define HackDlsym(Handle, Value, Symbol)            Value = (decltype(Value)) hack_dlsym(Handle,Symbol); \
                                                    if (Value == nullptr) {                              \
                                                        notFind=Symbol;                                  \
                                                        break;                                           \
                                                    }
#define JniEnv2Thread(env) *(void **) ((uint64_t) env + sizeof(void *))

class JniHelper {
public:
    bool Init(fake_dlctx_ref_t art) {
        string notFind;
        while (true) {
            sdkInt = get_sdk_int();
            HackDlsym(art, pJNI_GetCreatedJavaVMs, "JNI_GetCreatedJavaVMs")
            HackDlsym(art, pGetEnv, "_ZN3art3JII6GetEnvEP7_JavaVMPPvi")
            HackDlsym(art, _ZN3art9ArtMethod12JniShortNameEv, "_ZN3art9ArtMethod12JniShortNameEv")
            HackDlsym(art, _ZN3art9ArtMethod11JniLongNameEv, "_ZN3art9ArtMethod11JniLongNameEv")
            HackDlsym(art, _ZN3art9ArtMethod12PrettyMethodEb, "_ZN3art9ArtMethod12PrettyMethodEb")
            HackDlsym(art, JNIEnvExt_AddLocalReference,
                      "_ZN3art9JNIEnvExt17AddLocalReferenceIP8_jstringEET_NS_6ObjPtrINS_6mirror6ObjectEEE")
            HackDlsym(art, JNIEnvExt_DeleteLocalRef, "_ZN3art9JNIEnvExt14DeleteLocalRefEP8_jobject")
            if (sdkInt == 29) {
                HackDlsym(art, Thread_HandleScopeContains,
                          "_ZNK3art6Thread19HandleScopeContainsEP8_jobject")
            }
            break;
        }
        if (!notFind.empty()) {
            return false;
        }
        if (!GetJvm()) {
            return false;
        }
        auto env = GetEnv();
        if (!env) {
            return false;
        }
        jclass obj = env->FindClass("java/lang/Object");
        if (!obj) {
            return false;
        }
        methodID_toString = env->GetMethodID(obj, "toString", "()Ljava/lang/String;");
        if (!methodID_toString) {
            return false;
        }
        return true;
    }

    JNIEnv *GetEnv() {
        JNIEnv *env;
        pGetEnv(jvm, (void **) &env, JNI_VERSION_1_4);
        return env;
    }

    string GetMethodName(jmethodID method_id, method_name_type type, bool with_sign = true) {
        string name;
        switch (type) {
            case method_name_type::short_name:
                name = _ZN3art9ArtMethod12JniShortNameEv(method_id);
                break;
            case method_name_type::long_name:
                name = _ZN3art9ArtMethod11JniLongNameEv(method_id);
                break;
            case method_name_type::pretty_name:
                name = _ZN3art9ArtMethod12PrettyMethodEb(method_id, with_sign);
                break;
        }
        return name;
    }

    jvalue DeleteLocalRef(JNIEnv *env, jvalue obj) {
        return JNIEnvExt_DeleteLocalRef(env, obj);

    }

    jvalue AddLocalReference(JNIEnv *env, jvalue obj) {
        return JNIEnvExt_AddLocalReference(env, obj);
    }

    bool HandleScopeContains(JNIEnv *env, jobject obj) {
        if (sdkInt == 29) {
            return Thread_HandleScopeContains(JniEnv2Thread(env), obj);
        }
        return true;
    }

    string Object2Str(JNIEnv *env, jobject obj) {
        auto toString_ret = (jstring) (env->CallObjectMethod(obj, methodID_toString));
        const char *toString = env->GetStringUTFChars(toString_ret, nullptr);
        string result = toString;
        delete toString;
        return result;
    }

private:
    string (*_ZN3art9ArtMethod12JniShortNameEv)(jmethodID);

    string (*_ZN3art9ArtMethod11JniLongNameEv)(jmethodID);

    string (*_ZN3art9ArtMethod12PrettyMethodEb)(jmethodID, bool);

    jint (*pJNI_GetCreatedJavaVMs)(JavaVM **, jsize, jsize *);

    jint (*pGetEnv)(JavaVM *, void **, jint);

    jvalue (*JNIEnvExt_AddLocalReference)(JNIEnv *env, jvalue obj);

    jvalue (*JNIEnvExt_DeleteLocalRef)(JNIEnv *env, jvalue obj);

    bool (*Thread_HandleScopeContains)(void *thd, jobject obj);

    bool GetJvm() {
        jsize unused;
        pJNI_GetCreatedJavaVMs(&jvm, sizeof(jvm), &unused);
        return jvm != nullptr;
    }

    jmethodID methodID_toString;
public:
    int sdkInt;
    JavaVM *jvm;
};

extern JniHelper jniHelper;