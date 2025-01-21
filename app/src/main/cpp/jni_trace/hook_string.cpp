#include "hook.h"
#include "art_method_name.h"

#define CheckStack0AndCall(Sym, ...)    auto stack = GetStack0();                                       \
                                        auto result = pHook_##Sym(__VA_ARGS__);                         \
                                        int targetIdx = jniTrace.CheckTargetModule(stack);                                                                \
                                        if (targetIdx==-1 || passJniTrace) {       \
                                            return result;                                              \
                                        }                                                               \
                                        passJniTrace = true;                                            \
                                        defer([] { passJniTrace = false; })


//_ZN3art3JNI12NewStringUTFEP7_JNIEnvPKc
DefineHookStub(NewStringUTF, jstring, JNIEnv *env, const char *utf) {
    CheckStack0AndCall(NewStringUTF, env, utf);

    string logs = format_string("jni call NewStringUTF: \n");
    logs += format_string("\t\t\t\t\tret %p: %s\n", result, utf);
    logs += format_string("\t\t\t\t\tat: %p, target: %d\n", stack[0].offset, targetIdx);
    log2file("%s", logs.c_str());
    return result;
}

DefineHookStub(GetStringUTFChars, const char*, JNIEnv *env, jstring java_string,
               jboolean *is_copy) {
    CheckStack0AndCall(GetStringUTFChars, env, java_string, is_copy);

    string logs = format_string("jni call GetStringUTFChars: \n");
    logs += format_string("\t\t\t\t\tret %p: %s\n", java_string, result);
    logs += format_string("\t\t\t\t\tat: %p, target: %d\n", stack[0].offset, targetIdx);
    log2file("%s", logs.c_str());
    return result;
}

DefineHookStub(GetStringUTFLength, jsize, JNIEnv *env, jstring jstr) {
    CheckStack0AndCall(GetStringUTFLength, env, jstr);

    string logs = format_string("jni call GetStringUTFLength: \n");
    auto cstr = pHook_GetStringUTFChars(env, jstr, nullptr);
    logs += format_string("\t\t\t\t\tstr %p: %s\n", jstr, cstr);
    logs += format_string("\t\t\t\t\tret: %d\n", result);
    logs += format_string("\t\t\t\t\tat: %p, target: %d\n", stack[0].offset, targetIdx);
    log2file("%s", logs.c_str());
    delete cstr;
    return result;
}

//DefineHookStub(NewString, jstring, JNIEnv *env, const jchar *unicodeChars, jsize len) {
//    CheckStack0AndCall(NewString, env, unicodeChars, len);
//}
//DefineHookStub(GetStringLength, jsize, JNIEnv *env, jstring string) {
//}
//DefineHookStub(GetStringChars, const jchar*, JNIEnv *env, jstring string, jboolean *isCopy) {
//}
//DefineHookStub(ReleaseStringChars, void, JNIEnv *env, jstring string, const jchar *chars) {
//}
//DefineHookStub(ReleaseStringUTFChars, void, JNIEnv *env, jstring string, const char *utf) {
//}
