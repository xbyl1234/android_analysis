#include "hook.h"
#include "art_method_name.h"
#include "log_maker.h"
//
DefineHookStubCheckThreadPassJniTrace_Class(DefineClass, jclass, JNIEnv*, env, const char*,
                                             clzName, jobject, obj, const jbyte*, buffer, jsize,
                                             size){
Logs logs;
logs.setStack(_stack);
logs.setJniEnv(env);
logs.setName("DefineClass");
logs.setParams("char*",clzName);
logs.setParams("char*",(char*)buffer);
auto result = pHook_DefineClass(env,clzName,obj,buffer,size);
logs.setResult("jclass",result);
logs.log();
return result;
}
//
DefineHookStubCheckThreadPassJniTrace_Class(FindClass, jclass, JNIEnv*, env, const char*, clzName){
Logs logs;
logs.setStack(_stack);
logs.setJniEnv(env);
logs.setName("FindClass");
logs.setParams("char*",clzName);
auto result = pHook_FindClass(env,clzName);
//
logs.setResult("jclass",result);
logs.log();
return result;
}
//
DefineHookStubCheckThreadPassJniTrace_Class(NewLocalRef, jobject, JNIEnv*, env, jobject, obj){
Logs logs;
logs.setStack(_stack);
logs.setJniEnv(env);
logs.setName("NewLocalRef");
logs.setParams("jobject", obj);
auto result = pHook_NewLocalRef(env,obj);
//
logs.setResult("jobject",result);
logs.log();
return result;
}
//
DefineHookStubCheckThreadPassJniTrace_Class(AllocObject, jobject, JNIEnv*, env, jclass, clz){
Logs logs;
logs.setStack(_stack);
logs.setJniEnv(env);
logs.setName("AllocObject");
logs.setParams("jclass",clz);
auto result = pHook_AllocObject(env,clz);
//
logs.setResult("jobject",result);
logs.log();
return result;
}
//
DefineHookStubCheckThreadPassJniTrace_Class(NewObjectV, jobject, JNIEnv*, env, jclass, clz,
                                             jmethodID, method, va_list, params){
Logs logs;
logs.setStack(_stack);
logs.setJniEnv(env);
logs.setName("NewObjectV");
logs.setParams("jclass",clz);
logs.setCallParams(nullptr, nullptr, method, params);
auto result = pHook_NewObjectV(env,clz,method,params);
logs.setResult("jobject",result);
logs.log();
return result;
}
//
DefineHookStubCheckThreadPassJniTrace_Class(NewObjectA, jobject, JNIEnv*, env, jclass, clz,
                                             jmethodID, method, const jvalue*, params){
Logs logs;
logs.setStack(_stack);
logs.setJniEnv(env);
logs.setName("NewObjectA");
logs.setParams("jclass",clz);
logs.setCallParams(nullptr, nullptr, method, params);
auto result = pHook_NewObjectA(env,clz,method,params);
//
//
logs.setResult("jobject",result);
logs.log();
return result;
}
//
DefineHookStubCheckThreadPassJniTrace_Class(GetObjectClass, jclass, JNIEnv*, env, jobject, obj){
Logs logs;
logs.setStack(_stack);
logs.setJniEnv(env);
logs.setName("GetObjectClass");
logs.setParams("jobject", obj);
auto result = pHook_GetObjectClass(env,obj);
//
//logs.setResult();
logs.log();
return result;
}
//
DefineHookStubCheckThreadPassJniTrace_Class(GetMethodID, jmethodID, JNIEnv*, env, jclass, clz,
                                             const char*, method, const char*, sig){
Logs logs;
logs.setStack(_stack);
logs.setJniEnv(env);
logs.setName("GetMethodID");
logs.setParams("jclass",clz);
//
//
logs.setParams("char*",method);
logs.setParams("char*",sig);
auto result = pHook_GetMethodID(env,clz,method,sig);
logs.log();
return result;}
//
DefineHookStubCheckThreadPassJniTrace_Class(GetFieldID, jfieldID, JNIEnv*, env, jclass, clz,
                                             const char*, field, const char*, sig){
Logs logs;
logs.setStack(_stack);
logs.setJniEnv(env);
logs.setName("GetFieldID");
logs.setParams("jclass",clz);
//
//
logs.setParams("char*",field);
logs.setParams("char*",sig);
auto result = pHook_GetFieldID(env,clz,field,sig);
logs.log();
return result;
}
//
DefineHookStubCheckThreadPassJniTrace_Class(GetStaticMethodID, jmethodID, JNIEnv*, env, jclass,
                                             clz, const char*, method, const char*, sig){
Logs logs;
logs.setStack(_stack);
logs.setJniEnv(env);
logs.setName("GetStaticMethodID");
logs.setParams("jclass",clz);
//
logs.setParams("char*",method);
logs.setParams("char*",sig);
auto result = pHook_GetMethodID(env,clz,method,sig);
logs.log();
return result;}

DefineHookStubCheckThreadPassJniTrace_Class(GetStaticFieldID, jfieldID, JNIEnv*, env, jclass, clz,
                                             const char*, field, const char*, sig){
Logs logs;
logs.setStack(_stack);
logs.setJniEnv(env);
logs.setName("GetStaticFieldID");
logs.setParams("jclass",clz);
logs.setParams("char*",field);
logs.setParams("char*",sig);
auto result = pHook_GetFieldID(env,clz,field,sig);
logs.log();
return result;}

DefineHookStubCheckThreadPassJniTrace_Class(Throw, jint, JNIEnv*, env, jthrowable, e){
Logs logs;
logs.setStack(_stack);
logs.setJniEnv(env);
logs.setName("Throw");
logs.setParams("jthrowable",e);
auto result = pHook_Throw(env,e);
logs.log();
return result;}

DefineHookStubCheckThreadPassJniTrace_Class(ThrowNew, jint, JNIEnv*, env, jclass, clz,
                                             const char *, msg){
Logs logs;
logs.setStack(_stack);
logs.setJniEnv(env);
logs.setName("ThrowNew");
logs.setParams("jclass",clz);
logs.setParams("char*",msg);
auto result = pHook_ThrowNew(env,clz,msg);
logs.log();
return result;}

//DefineHookStubCheckThreadPassJniTrace_Class(NewObject, jobject, JNIEnv*, env, jclass, jmethodID, ...);
//
//DefineHookStub(GetVersion, jint, JNIEnv *);
//
//DefineHookStub(ToReflectedMethod, jobject, JNIEnv *env, jclass, jmethodID, jboolean);
//
//DefineHookStub(GetSuperclass, jclass, JNIEnv *env, jclass);
//
//DefineHookStub(IsAssignableFrom, jboolean, JNIEnv *env, jclass, jclass);
//
//DefineHookStub(ToReflectedField, jobject, JNIEnv *env, jclass, jfieldID, jboolean);
//
//DefineHookStub(ExceptionDescribe, void, JNIEnv*);
//
//DefineHookStub(ExceptionClear, void, JNIEnv*);
//
//DefineHookStub(FatalError, void, JNIEnv *env, const char*);
//
//DefineHookStub(NewGlobalRef, jobject, JNIEnv *env, jobject);
//
//DefineHookStub(DeleteGlobalRef, void, JNIEnv *env, jobject);
//
//DefineHookStub(DeleteLocalRef, void, JNIEnv *env, jobject);
//
//DefineHookStub(IsSameObject, jboolean, JNIEnv *env, jobject, jobject);
//
//DefineHookStub(EnsureLocalCapacity, jint, JNIEnv *env, jint);
//
//DefineHookStub(IsInstanceOf, jboolean, JNIEnv *env, jobject, jclass);
//
//DefineHookStub(UnregisterNatives, jint, JNIEnv *env, jclass);
//
//DefineHookStub(MonitorEnter, jint, JNIEnv *env, jobject);
//
//DefineHookStub(MonitorExit, jint, JNIEnv *env, jobject);
//
//DefineHookStub(GetJavaVM, jint, JNIEnv *env, JavaVM**);
//
//DefineHookStub(GetStringRegion, void, JNIEnv *env, jstring, jsize, jsize, jchar*);
//
//DefineHookStub(GetStringUTFRegion, void, JNIEnv *env, jstring, jsize, jsize, char*);
//
//DefineHookStub(GetPrimitiveArrayCritical, void*, JNIEnv *env, jarray, jboolean*);
//
//DefineHookStub(ReleasePrimitiveArrayCritical, void, JNIEnv *env, jarray, void*, jint);
//
//DefineHookStub(GetStringCritical, const jchar*, JNIEnv *env, jstring, jboolean*);
//
//DefineHookStub(ReleaseStringCritical, void, JNIEnv *env, jstring, const jchar*);
//
//DefineHookStub(NewWeakGlobalRef, jweak, JNIEnv *env, jobject);
//
//DefineHookStub(DeleteWeakGlobalRef, void, JNIEnv *env, jweak);
//
//DefineHookStub(ExceptionCheck, jboolean, JNIEnv*);
//
//DefineHookStub(NewDirectByteBuffer, jobject, JNIEnv *env, void*, jlong);
//
//DefineHookStub(GetDirectBufferAddress, void*, JNIEnv *env, jobject);
//
//DefineHookStub(GetDirectBufferCapacity, jlong, JNIEnv *env, jobject);
//
//DefineHookStub(GetObjectRefType, jobjectRefType, JNIEnv *env, jobject);