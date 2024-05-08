#include "hook.h"
#include "art_method_name.h"

DefineHookStub(RegisterNatives, jint,
               JNIEnv *env, jclass java_class, const JNINativeMethod *methods, jint method_count) {
    auto stack = GetStack0();
    string clzName = jniHelper.Object2Str(env, java_class);
    for (int i = 0; i < method_count; ++i) {
        auto info = GetStackInfo(1, methods[i].fnPtr);
        log2file("RegisterNatives clz: %s, method: %s, %s, target: %s %p, at: %s %p",
                 clzName.c_str(), methods[i].name, methods[i].signature, info[0].name.c_str(),
                 info[0].offset, stack[0].name.c_str(), stack[0].offset);
    }
    return pHook_RegisterNatives(env, java_class, methods, method_count);
}
