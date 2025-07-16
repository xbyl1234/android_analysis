#include <jni.h>

#include "format.h"
#include "jni_sym.h"
#include "art_method_name.h"

using namespace format;

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

#define ExternCallHook(name, result, ...) ExternHookStub(name, result, __VA_ARGS__,...); \
ExternHookStub(name##V,result,__VA_ARGS__,va_list);                              \
ExternHookStub(name##A,result,__VA_ARGS__, const jvalue*);


ExternCallHook(CallObjectMethod, jobject, JNIEnv*, jobject, jmethodID)

ExternCallHook(CallBooleanMethod, jboolean, JNIEnv*, jobject, jmethodID)

ExternCallHook(CallByteMethod, jbyte, JNIEnv*, jobject, jmethodID)

ExternCallHook(CallCharMethod, jchar, JNIEnv*, jobject, jmethodID)

ExternCallHook(CallShortMethod, jshort, JNIEnv*, jobject, jmethodID)

ExternCallHook(CallIntMethod, jint, JNIEnv*, jobject, jmethodID)

ExternCallHook(CallLongMethod, jlong, JNIEnv*, jobject, jmethodID)

ExternCallHook(CallFloatMethod, jfloat, JNIEnv*, jobject, jmethodID)

ExternCallHook(CallDoubleMethod, jdouble, JNIEnv*, jobject, jmethodID)

ExternCallHook(CallVoidMethod, void, JNIEnv*, jobject, jmethodID)

ExternCallHook(CallNonvirtualObjectMethod, jobject, JNIEnv*, jobject, jclass, jmethodID)

ExternCallHook(CallNonvirtualBooleanMethod, jboolean, JNIEnv*, jobject, jclass, jmethodID)

ExternCallHook(CallNonvirtualByteMethod, jbyte, JNIEnv*, jobject, jclass, jmethodID)

ExternCallHook(CallNonvirtualCharMethod, jchar, JNIEnv*, jobject, jclass, jmethodID)

ExternCallHook(CallNonvirtualShortMethod, jshort, JNIEnv*, jobject, jclass, jmethodID)

ExternCallHook(CallNonvirtualIntMethod, jint, JNIEnv*, jobject, jclass, jmethodID)

ExternCallHook(CallNonvirtualLongMethod, jlong, JNIEnv*, jobject, jclass, jmethodID)

ExternCallHook(CallNonvirtualFloatMethod, jfloat, JNIEnv*, jobject, jclass, jmethodID)

ExternCallHook(CallNonvirtualDoubleMethod, jdouble, JNIEnv*, jobject, jclass, jmethodID)

ExternCallHook(CallNonvirtualVoidMethod, void, JNIEnv*, jobject, jclass, jmethodID)

ExternHookStub(DefineClass, jclass, JNIEnv*, const char*, jobject, const jbyte*, jsize)

ExternHookStub(FindClass, jclass, JNIEnv*, const char*)

ExternHookStub(NewLocalRef, jobject, JNIEnv*, jobject)

ExternHookStub(AllocObject, jobject, JNIEnv*, jclass)

ExternHookStub(NewObjectV, jobject, JNIEnv*, jclass, jmethodID, va_list)

ExternHookStub(NewObjectA, jobject, JNIEnv*, jclass, jmethodID, const jvalue*)

ExternHookStub(GetObjectClass, jclass, JNIEnv*, jobject)

ExternHookStub(GetMethodID, jmethodID, JNIEnv*, jclass, const char*, const char*)

ExternHookStub(GetFieldID, jfieldID, JNIEnv*, jclass, const char*, const char*)

ExternHookStub(GetStaticMethodID, jmethodID, JNIEnv*, jclass, const char*, const char*)

ExternHookStub(GetStaticFieldID, jfieldID, JNIEnv*, jclass, const char*, const char*)

ExternHookStub(Throw, jint, JNIEnv*, jthrowable)

ExternHookStub(ThrowNew, jint, JNIEnv*, jclass, const char *)



#define AddSymbolInfo(symName)  SymbolInfo{.isReg=false, .sym=  #symName, .stub=(void*)Hook_##symName, .org=(void**) &pHook_##symName}
#define AddSymbolInfoBySym(symName)  SymbolInfo{.isReg=true, .sym=  #symName, .stub=(void*)Hook_##symName, .org=(void**) &pHook_##symName}
#define AddSymbolInfoByJniEnv(symName, env)  SymbolInfo{.isReg=false, .sym=  #symName, .stub=(void*)Hook_##symName, .org=(void**) &pHook_##symName,.target=(void*)env->functions->symName}

bool jni_sym::init(fake_dlctx_ref_t handleLibArt, JNIEnv *env) {
    jniHooks = {
            AddSymbolInfoByJniEnv(NewStringUTF, env),
            AddSymbolInfoByJniEnv(GetStringUTFChars, env),
            AddSymbolInfoByJniEnv(GetStringUTFLength, env),
            AddSymbolInfoByJniEnv(RegisterNatives, env),
            AddSymbolInfoByJniEnv(GetObjectField, env),
            AddSymbolInfoByJniEnv(GetBooleanField, env),
            AddSymbolInfoByJniEnv(GetByteField, env),
            AddSymbolInfoByJniEnv(GetCharField, env),
            AddSymbolInfoByJniEnv(GetShortField, env),
            AddSymbolInfoByJniEnv(GetIntField, env),
            AddSymbolInfoByJniEnv(GetLongField, env),
            AddSymbolInfoByJniEnv(GetFloatField, env),
            AddSymbolInfoByJniEnv(GetDoubleField, env),
            AddSymbolInfoByJniEnv(SetObjectField, env),
            AddSymbolInfoByJniEnv(SetBooleanField, env),
            AddSymbolInfoByJniEnv(SetByteField, env),
            AddSymbolInfoByJniEnv(SetCharField, env),
            AddSymbolInfoByJniEnv(SetShortField, env),
            AddSymbolInfoByJniEnv(SetIntField, env),
            AddSymbolInfoByJniEnv(SetLongField, env),
            AddSymbolInfoByJniEnv(SetFloatField, env),
            AddSymbolInfoByJniEnv(SetDoubleField, env),
            AddSymbolInfoByJniEnv(GetObjectArrayElement, env),
            AddSymbolInfoByJniEnv(SetObjectArrayElement, env),
            AddSymbolInfoByJniEnv(CallObjectMethod, env),
            AddSymbolInfoByJniEnv(CallObjectMethodV, env),
            AddSymbolInfoByJniEnv(CallObjectMethodA, env),
            AddSymbolInfoByJniEnv(CallBooleanMethod, env),
            AddSymbolInfoByJniEnv(CallBooleanMethodV, env),
            AddSymbolInfoByJniEnv(CallBooleanMethodA, env),
            AddSymbolInfoByJniEnv(CallByteMethod, env),
            AddSymbolInfoByJniEnv(CallByteMethodV, env),
            AddSymbolInfoByJniEnv(CallByteMethodA, env),
            AddSymbolInfoByJniEnv(CallCharMethod, env),
            AddSymbolInfoByJniEnv(CallCharMethodV, env),
            AddSymbolInfoByJniEnv(CallCharMethodA, env),
            AddSymbolInfoByJniEnv(CallShortMethod, env),
            AddSymbolInfoByJniEnv(CallShortMethodV, env),
            AddSymbolInfoByJniEnv(CallShortMethodA, env),
            AddSymbolInfoByJniEnv(CallIntMethod, env),
            AddSymbolInfoByJniEnv(CallIntMethodV, env),
            AddSymbolInfoByJniEnv(CallIntMethodA, env),
            AddSymbolInfoByJniEnv(CallLongMethod, env),
            AddSymbolInfoByJniEnv(CallLongMethodV, env),
            AddSymbolInfoByJniEnv(CallLongMethodA, env),
            AddSymbolInfoByJniEnv(CallFloatMethod, env),
            AddSymbolInfoByJniEnv(CallFloatMethodV, env),
            AddSymbolInfoByJniEnv(CallFloatMethodA, env),
            AddSymbolInfoByJniEnv(CallDoubleMethod, env),
            AddSymbolInfoByJniEnv(CallDoubleMethodV, env),
            AddSymbolInfoByJniEnv(CallDoubleMethodA, env),
            AddSymbolInfoByJniEnv(CallVoidMethod, env),
            AddSymbolInfoByJniEnv(CallVoidMethodV, env),
            AddSymbolInfoByJniEnv(CallVoidMethodA, env),
            AddSymbolInfoByJniEnv(CallNonvirtualObjectMethod, env),
            AddSymbolInfoByJniEnv(CallNonvirtualObjectMethodV, env),
            AddSymbolInfoByJniEnv(CallNonvirtualObjectMethodA, env),
            AddSymbolInfoByJniEnv(CallNonvirtualBooleanMethod, env),
            AddSymbolInfoByJniEnv(CallNonvirtualBooleanMethodV, env),
            AddSymbolInfoByJniEnv(CallNonvirtualBooleanMethodA, env),
            AddSymbolInfoByJniEnv(CallNonvirtualByteMethod, env),
            AddSymbolInfoByJniEnv(CallNonvirtualByteMethodV, env),
            AddSymbolInfoByJniEnv(CallNonvirtualByteMethodA, env),
            AddSymbolInfoByJniEnv(CallNonvirtualCharMethod, env),
            AddSymbolInfoByJniEnv(CallNonvirtualCharMethodV, env),
            AddSymbolInfoByJniEnv(CallNonvirtualCharMethodA, env),
            AddSymbolInfoByJniEnv(CallNonvirtualShortMethod, env),
            AddSymbolInfoByJniEnv(CallNonvirtualShortMethodV, env),
            AddSymbolInfoByJniEnv(CallNonvirtualShortMethodA, env),
            AddSymbolInfoByJniEnv(CallNonvirtualIntMethod, env),
            AddSymbolInfoByJniEnv(CallNonvirtualIntMethodV, env),
            AddSymbolInfoByJniEnv(CallNonvirtualIntMethodA, env),
            AddSymbolInfoByJniEnv(CallNonvirtualLongMethod, env),
            AddSymbolInfoByJniEnv(CallNonvirtualLongMethodV, env),
            AddSymbolInfoByJniEnv(CallNonvirtualLongMethodA, env),
            AddSymbolInfoByJniEnv(CallNonvirtualFloatMethod, env),
            AddSymbolInfoByJniEnv(CallNonvirtualFloatMethodV, env),
            AddSymbolInfoByJniEnv(CallNonvirtualFloatMethodA, env),
            AddSymbolInfoByJniEnv(CallNonvirtualDoubleMethod, env),
            AddSymbolInfoByJniEnv(CallNonvirtualDoubleMethodV, env),
            AddSymbolInfoByJniEnv(CallNonvirtualDoubleMethodA, env),
            AddSymbolInfoByJniEnv(CallNonvirtualVoidMethod, env),
            AddSymbolInfoByJniEnv(CallNonvirtualVoidMethodV, env),
            AddSymbolInfoByJniEnv(CallNonvirtualVoidMethodA, env),
//            AddSymbolInfoByJniEnv(DefineClass, env),
//            AddSymbolInfoByJniEnv(FindClass, env),
//            AddSymbolInfoByJniEnv(NewLocalRef, env),
//            AddSymbolInfoByJniEnv(AllocObject, env),
//            AddSymbolInfoByJniEnv(NewObjectV, env),
//            AddSymbolInfoByJniEnv(NewObjectA, env),
//            AddSymbolInfoByJniEnv(GetObjectClass, env),
//            AddSymbolInfoByJniEnv(GetMethodID, env),
//            AddSymbolInfoByJniEnv(GetFieldID, env),
//            AddSymbolInfoByJniEnv(GetStaticMethodID, env),
//            AddSymbolInfoByJniEnv(GetStaticFieldID, env),
//            AddSymbolInfoByJniEnv(Throw, env),
//            AddSymbolInfoByJniEnv(ThrowNew, env),
//            AddSymbolInfoByJniEnv(ExceptionOccurred, env),
    };

//    auto names = getSynName(env);
//    vector<SymbolInfo *> needResolve;
//    for (int i = 0; i < jniHooks.size(); ++i) {
//        if (!jniHooks[i].sym.empty()) {
//            auto find = names.find(jniHooks[i].sym);
//            if (find != names.end()) {
//                jniHooks[i].sym = find->second;
//                needResolve.push_back(&jniHooks[i]);
//            }
//        }
//    }
//    if (!resolve(handleLibArt, &needResolve)) {
//        loge("SymbolInfo::resolve error!");
//        return false;
//    }

    for (int i = 0; i < jniHooks.size(); ++i) {
        logi("hook info: %s, %p",
             jniHooks[i].sym.c_str(),
             jniHooks[i].target);
    }
    return true;
}
