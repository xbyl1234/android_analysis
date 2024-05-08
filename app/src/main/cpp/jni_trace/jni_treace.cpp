#include <jni.h>
#include <vector>
#include <string>
#include <regex>
#include <set>
#include <dlfcn.h>

#include <unwindstack/LocalUnwinder.h>
#include "../third/byopen/hack_dlopen.h"
#include "../third/utils/jni_helper.hpp"
#include "../third/utils/linux_helper.h"
#include "../third/utils/utils.h"
#include "../third/utils/log.h"
#include "../base/when_hook.h"
#include "../base/hook.h"

#include "parse_java_sig.h"
#include "jni_helper.h"
#include "jni_treace.h"
#include "format.h"
#include "art_method_name.h"

using namespace std;

jobject CallStaticObjectMethod(JNIEnv *env, void *soc, jclass, jmethodID mid, ...);

JniTrace jniTrace;
JniHelper jniHelper;
__thread int callDeep = 0;
__thread bool passCallMethod = false;


DECLARE_PRINTF_FUNC(in_java_parse) {
    logd("parse in java: %s %p ", args_type.c_str(), obj.l);
    if (obj.l == nullptr) {
        return "null";
    }
//#ifndef Android12
//    if (!jniTrace.jniHelper.HandleScopeContains(env->env, obj.l)) {
//        return format_string("invalid object[%s: %p]", args_type.c_str(), obj.l);
//    }
//    if (env->env->IsSameObject(obj.l, nullptr)) {
//        return format_string("deleted object[%s: %p]", args_type.c_str(), obj.l);
//    }
//#endif
    jobject gref = obj.l;
    passCallMethod = true;
    auto jret = env->env->CallStaticObjectMethod(jniTrace.frida_helper, jniTrace.object_2_string,
                                                 gref);
//    auto jret = (jstring) CallStaticObjectMethod(env->env, env->soc, jniTrace.frida_helper,
//                                                 jniTrace.object_2_string, gref);
    passCallMethod = false;
    if (jret == nullptr) {
        return "null";
    }
    const char *ret = env->env->GetStringUTFChars((jstring) jret, nullptr);
    string result = ret;
    delete[] ret;
    return result;
}

ExternHookStub(GetStringUTFLength, jsize, JNIEnv *env, jstring jstr);

ExternHookStub(GetStringUTFChars, const char*, JNIEnv *env, jstring java_string, jboolean *is_copy);

ExternHookStub(NewStringUTF, jstring, JNIEnv *env, const char *utf);

ExternHookStub(RegisterNatives, jint, JNIEnv *env, jclass java_class,
               const JNINativeMethod *methods, jint method_count);

ExternHookStub(InvokeVirtualOrInterfaceWithVarArgs, jvalue, ScopedObjectAccessAlreadyRunnable soa,
               jobject obj, jmethodID mid, va_list args);

ExternHookStub(InvokeWithVarArgs, jvalue, ScopedObjectAccessAlreadyRunnable soa, jobject obj,
               jmethodID mid, va_list args);

ExternHookStub(SetObjectArrayElement, void, JNIEnv *env, jobjectArray array, jsize index,
               jobject value);

ExternHookStub(GetObjectArrayElement, jobject, JNIEnv *env, jobjectArray array, jsize index);

#define ExternGetFieldHook(type) ExternHookStub(Get##type##Field, jobject, JNIEnv *env, jobject obj, jfieldID field)

#define ExternSetFieldHook(type) ExternHookStub(Set##type##Field, void, JNIEnv *env, jobject obj, jfieldID field, jvalue v);

ExternGetFieldHook(Object);

ExternGetFieldHook(Boolean);

ExternGetFieldHook(Byte);

ExternGetFieldHook(Char);

ExternGetFieldHook(Short);

ExternGetFieldHook(Int);

ExternGetFieldHook(Long);

ExternGetFieldHook(Float);

ExternGetFieldHook(Double);

ExternSetFieldHook(Object);

ExternSetFieldHook(Boolean);

ExternSetFieldHook(Byte);

ExternSetFieldHook(Char);

ExternSetFieldHook(Short);

ExternSetFieldHook(Int);

ExternSetFieldHook(Long);

ExternSetFieldHook(Float);

ExternSetFieldHook(Double);


#define AddSymbolInfo(symName)  SymbolInfo{.isReg=false, .sym=  Sym##symName, .stub=(void*)Hook_##symName, .org=(void**) &pHook_##symName}
#define AddSymbolInfoReg(symName)  SymbolInfo{.isReg=true, .sym=  Sym##symName, .stub=(void*)Hook_##symName, .org=(void**) &pHook_##symName}
vector<SymbolInfo> JniTrace::jniHooks;

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_andriod_analyse_hook_Native_nativeInitJniTrace(JNIEnv *env, jclass clazz,
                                                        jclass frida_helper) {
    if (!jniTrace.init) {
        if (!initSynName()) {
            logi("initSynName error");
            return false;
        }
        vector<string> passJavaMethod = {
                "void android.view.Choreographer.postFrameCallback(android.view.Choreographer$FrameCallback)",
                "void android.view.Choreographer.removeFrameCallback(android.view.Choreographer$FrameCallback)",
                "boolean android.os.Handler.postDelayed(java.lang.Runnable, long)",
                "long java.lang.Long.longValue()",
                "long java.lang.System.nanoTime()"
        };
        JniTrace::jniHooks = {
                AddSymbolInfo(NewStringUTF),
                AddSymbolInfo(GetStringUTFChars),
                AddSymbolInfo(GetStringUTFLength),
//                AddSymbolInfo(RegisterNatives),
                AddSymbolInfoReg(InvokeVirtualOrInterfaceWithVarArgs),
                AddSymbolInfoReg(InvokeWithVarArgs),
                AddSymbolInfo(GetObjectField),
                AddSymbolInfo(GetBooleanField),
                AddSymbolInfo(GetByteField),
                AddSymbolInfo(GetCharField),
                AddSymbolInfo(GetShortField),
                AddSymbolInfo(GetIntField),
                AddSymbolInfo(GetLongField),
                AddSymbolInfo(GetFloatField),
                AddSymbolInfo(GetDoubleField),
                AddSymbolInfo(SetObjectField),
                AddSymbolInfo(SetBooleanField),
                AddSymbolInfo(SetByteField),
                AddSymbolInfo(SetCharField),
                AddSymbolInfo(SetShortField),
                AddSymbolInfo(SetIntField),
                AddSymbolInfo(SetLongField),
                AddSymbolInfo(SetFloatField),
                AddSymbolInfo(SetDoubleField),
                AddSymbolInfo(GetObjectArrayElement),
                AddSymbolInfo(SetObjectArrayElement),
        };
        for (auto item: JniTrace::jniHooks) {
            logi("--%s  %p %p", item.sym.c_str(), item.stub, item.org);
        }
//        const string target = "libprotect.so";
        const string target = "libunity.so";
//        const string target = "libdevices.so";
        logi("init jni trace: %s", target.c_str());

        fake_dlctx_ref_t handleLibArt = hack_dlopen("libart.so", 0);
        defer([&]() {
            hack_dlclose(handleLibArt);
        });
        if (!handleLibArt) {
            loge("open handleLibArt error!");
            return false;
        }
        if (!jniHelper.Init(handleLibArt)) {
            loge("jniHelper Init error!");
            return false;
        }
        jniTrace.Init((jclass) env->NewGlobalRef(frida_helper), handleLibArt, {target},
                      passJavaMethod);
        jniTrace.Hook();
    }
    return true;
}