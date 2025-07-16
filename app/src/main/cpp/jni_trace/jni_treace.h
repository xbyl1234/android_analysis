#pragma once

#include <string>
#include <set>
#include <jni.h>
#include <dlfcn.h>
#include <functional>

#include "../third/byopen/hack_dlopen.h"
#include "../third/utils/utils.h"
#include "../third/utils/log.h"
#include "../base/hook.h"

#include "jni_helper.h"
#include "jni_sym.h"

using namespace std;

using CheckTargetModuleFunc = function<int(const vector<Stack> &frame)>;

class JniTrace {
public:
    explicit JniTrace() {}

    bool
    Init(jclass frida_helper, fake_dlctx_ref_t handleLibArt,
         CheckTargetModuleFunc checkTargetModuleFunc, const vector<string> &passJavaMethod) {
        JNIEnv *env = jniHelper.GetEnv();
        if (!this->sym.init(handleLibArt, env)) {
            return false;
        }
        this->checkTargetModuleFunc = checkTargetModuleFunc;
        for (const auto &item: passJavaMethod) {
            this->passJavaMethod.insert(item);
        }
        this->frida_helper = frida_helper;
        object_2_string = env->GetStaticMethodID(frida_helper, "object_2_string",
                                                 "(Ljava/lang/Object;)Ljava/lang/String;");
        init = true;
        return true;
    }

    bool Hook() {
        return hookAll(&sym.jniHooks);
    }

    bool CheckPassJavaMethod(jmethodID method) {
        return passJavaMethodId.contains(method);
    }

    bool CheckPassJavaMethod(jmethodID methodId, const string &method) {
        bool ret = passJavaMethod.contains(method);
        if (ret) {
            passJavaMethodId.insert(methodId);
        }
        return ret;
    }

    int CheckTargetModule(const vector<Stack> &frame) {
        return checkTargetModuleFunc(frame);
    }

public:
    bool init = false;
    jclass frida_helper;
    jmethodID object_2_string;
    CheckTargetModuleFunc checkTargetModuleFunc;
    set<string> passJavaMethod;
    set<jmethodID> passJavaMethodId;
    jni_sym sym;
};

extern JniTrace jniTrace;
extern __thread bool passJniTrace;
extern __thread bool passCallMethod;


#define ForwardArgs7(type, value)  value
#define ForwardArgs6(type, value, ...)  value __VA_OPT__(,) __VA_OPT__(ForwardArgs7(__VA_ARGS__))
#define ForwardArgs5(type, value, ...)  value __VA_OPT__(,) __VA_OPT__(ForwardArgs6(__VA_ARGS__))
#define ForwardArgs4(type, value, ...)  value __VA_OPT__(,) __VA_OPT__(ForwardArgs5(__VA_ARGS__))
#define ForwardArgs3(type, value, ...)  value __VA_OPT__(,) __VA_OPT__(ForwardArgs4(__VA_ARGS__))
#define ForwardArgs2(type, value, ...)  value __VA_OPT__(,) __VA_OPT__(ForwardArgs3(__VA_ARGS__))
#define ForwardArgs1(type, value, ...)  value __VA_OPT__(,) __VA_OPT__(ForwardArgs2(__VA_ARGS__))
#define ForwardArgs(...) ForwardArgs1(__VA_ARGS__)

#define ForwardType7(type, value)  type, value
#define ForwardType6(type, value, ...)  type value __VA_OPT__(,) __VA_OPT__(ForwardType7(__VA_ARGS__))
#define ForwardType5(type, value, ...)  type value __VA_OPT__(,) __VA_OPT__(ForwardType6(__VA_ARGS__))
#define ForwardType4(type, value, ...)  type value __VA_OPT__(,) __VA_OPT__(ForwardType5(__VA_ARGS__))
#define ForwardType3(type, value, ...)  type value __VA_OPT__(,) __VA_OPT__(ForwardType4(__VA_ARGS__))
#define ForwardType2(type, value, ...)  type value __VA_OPT__(,) __VA_OPT__(ForwardType3(__VA_ARGS__))
#define ForwardType1(type, value, ...)  type value __VA_OPT__(,) __VA_OPT__(ForwardType2(__VA_ARGS__))
#define ForwardType(...) ForwardType1(__VA_ARGS__)


#define DefineHookStubCheckThreadPassJniTrace(StackFun, Func, Ret, ...)           \
    Ret logHook_##Func(const vector<Stack> &_stack,ForwardType(__VA_ARGS__));     \
    Ret (*pHook_##Func)(ForwardType(__VA_ARGS__));                                \
    Ret Hook_##Func(ForwardType(__VA_ARGS__)){                                    \
        auto stack = StackFun();                                                  \
        if (passJniTrace|| jniTrace.CheckTargetModule(stack) == -1) {             \
            return pHook_##Func(ForwardArgs(__VA_ARGS__));                        \
        }                                                                         \
        passJniTrace = true;                                                      \
        defer([] { passJniTrace = false; });                                      \
        return logHook_##Func(stack,ForwardArgs(__VA_ARGS__));                    \
    }                                                                             \
    Ret logHook_##Func(const vector<Stack> &_stack, ForwardType(__VA_ARGS__))


#define MethodStack GetStackInfo
#define ArrayStack GetStackInfo
#define FieldStack GetStackInfo
#define RegisterNativesStack GetStackInfo
#define StringStack GetStackInfo
#define ClassStack GetStackInfo
#define DefineHookStubCheckThreadPassJniTrace_Field(Func, Ret, ...)  DefineHookStubCheckThreadPassJniTrace(FieldStack, Func, Ret,  __VA_ARGS__)
#define DefineHookStubCheckThreadPassJniTrace_Array(Func, Ret, ...)  DefineHookStubCheckThreadPassJniTrace(ArrayStack, Func, Ret,  __VA_ARGS__)
#define DefineHookStubCheckThreadPassJniTrace_String(Func, Ret, ...)  DefineHookStubCheckThreadPassJniTrace(StringStack, Func, Ret,  __VA_ARGS__)
#define DefineHookStubCheckThreadPassJniTrace_Class(Func, Ret, ...)  DefineHookStubCheckThreadPassJniTrace(StringStack, Func, Ret,  __VA_ARGS__)