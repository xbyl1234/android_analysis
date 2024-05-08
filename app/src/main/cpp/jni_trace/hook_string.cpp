#include "hook.h"
#include "art_method_name.h"

//_ZN3art3JNI12NewStringUTFEP7_JNIEnvPKc
DefineHookStub(NewStringUTF, jstring, JNIEnv *env, const char *utf) {
    callDeep++;
    defer([&]() {
        callDeep--;
    });
    if (callDeep > 1) {
        return pHook_NewStringUTF(env, utf);
    }
    auto stack = GetStack0();
    auto result = pHook_NewStringUTF(env, utf);
    if (jniTrace.CheckTargetModule(stack)) {
        string logs = format_string("jni call NewStringUTF: \n");
        logs += format_string("\t\t\t\t\tstr %p: %s\n", utf, utf);
        logs += format_string("\t\t\t\t\tret: %p\n", result);
        logs += format_string("\t\t\t\t\tat: %p\n", stack[0].offset);
        log2file("%s", logs.c_str());
    }
    return result;
}

DefineHookStub(GetStringUTFChars, const char*, JNIEnv *env, jstring java_string,
               jboolean *is_copy) {
    callDeep++;
    defer([&]() {
        callDeep--;
    });
    if (callDeep > 1) {
        return pHook_GetStringUTFChars(env, java_string, is_copy);
    }
    auto stack = GetStack0();
    auto result = pHook_GetStringUTFChars(env, java_string, is_copy);
    if (jniTrace.CheckTargetModule(stack)) {
        string logs = format_string("jni call GetStringUTFChars: \n");
        logs += format_string("\t\t\t\t\tret %p: %s\n", java_string, result);
        logs += format_string("\t\t\t\t\tat: %p\n", stack[0].offset);
        log2file("%s", logs.c_str());
    }
    return result;
}

DefineHookStub(GetStringUTFLength, jsize, JNIEnv *env, jstring jstr) {
    callDeep++;
    defer([&]() {
        callDeep--;
    });
    if (callDeep > 1) {
        return pHook_GetStringUTFLength(env, jstr);
    }
    auto stack = GetStack0();
    auto result = pHook_GetStringUTFLength(env, jstr);
    if (jniTrace.CheckTargetModule(stack)) {
        string logs = format_string("jni call GetStringUTFLength: \n");
        auto cstr = pHook_GetStringUTFChars(env, jstr, nullptr);
        logs += format_string("\t\t\t\t\tstr %p: %s\n", jstr, cstr);
        logs += format_string("\t\t\t\t\tret: %d\n", result);
        logs += format_string("\t\t\t\t\tat: %p\n", stack[0].offset);
        log2file("%s", logs.c_str());
        delete cstr;
    }
    return result;
}
