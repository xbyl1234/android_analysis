#include "hook.h"
#include "art_method_name.h"
#include "log_maker.h"

DefineHookStub(RegisterNatives, jint,
               JNIEnv *env, jclass java_class, const JNINativeMethod *methods, jint method_count) {
    Logs logs;
    logs.setStack(RegisterNativesStack());
    logs.setJniEnv(env);
    logs.setName("RegisterNatives");
    logs.setParams("jclass", java_class);
    logs.setParams("jint", method_count);
    for (int i = 0; i < method_count; ++i) {
        logs.setParams("JNINativeMethod*", &methods[i]);
    }
    auto result = pHook_RegisterNatives(env, java_class, methods, method_count);
    logs.setResult("jint", result);
    return result;
}
