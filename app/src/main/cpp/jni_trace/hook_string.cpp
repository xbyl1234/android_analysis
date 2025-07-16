#include "hook.h"
#include "art_method_name.h"
#include "log_maker.h"

//_ZN3art3JNI12NewStringUTFEP7_JNIEnvPKc
DefineHookStubCheckThreadPassJniTrace_String(NewStringUTF, jstring, JNIEnv *,env, const char *,utf) {
    Logs logs;
    logs.setStack(_stack);
    logs.setJniEnv(env);
    logs.setName("NewStringUTF");
    logs.setParams("char*", utf);
    auto result = pHook_NewStringUTF(env, utf);
    logs.setResult("jint", result);
    logs.log();
    return result;
}

DefineHookStubCheckThreadPassJniTrace_String(GetStringUTFChars, const char*, JNIEnv *,env, jstring, java_string,
               jboolean *,is_copy) {
    Logs logs;
    logs.setStack(_stack);
    logs.setJniEnv(env);
    logs.setName("GetStringUTFChars");
    logs.setParams("jstring", java_string);
    auto result = pHook_GetStringUTFChars(env, java_string, is_copy);
    logs.setResult("char*", result);
    logs.log();
    return result;
}

DefineHookStubCheckThreadPassJniTrace_String(GetStringUTFLength, jsize, JNIEnv *,env, jstring ,jstr) {
    Logs logs;
    logs.setStack(_stack);
    logs.setJniEnv(env);
    logs.setName("GetStringUTFLength");
    logs.setParams("jstring", jstr);
    auto result = pHook_GetStringUTFLength(env, jstr);
    logs.setResult("jint", result);
    logs.log();
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
