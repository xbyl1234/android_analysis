#include "hook.h"
#include "art_method_name.h"
#include "log_maker.h"
//
//DefineHookStubCheckThreadPassJniTrace_Stack0(DefineClass, jclass, JNIEnv*, env, const char*,
//                                             clzName, jobject, obj, const jbyte*, buffer, jsize,
//                                             size){
//Logs logs;
//logs.setStack(_stack);
//logs.setJniEnv(env);
//logs.setName("DefineClass");
//logs.setParams();
//
//
//logs.setResult();
//logs.log();
//return result;
//}
//
//DefineHookStubCheckThreadPassJniTrace_Stack0(FindClass, jclass, JNIEnv*, env, const char*, clzName){
//Logs logs;
//logs.setStack(_stack);
//logs.setJniEnv(env);
//logs.setName("FindClass");
//logs.setParams();
//
//
//logs.setResult();
//logs.log();
//return result;}
//
//DefineHookStubCheckThreadPassJniTrace_Stack0(NewLocalRef, jobject, JNIEnv*, env, jobject, obj){
//Logs logs;
//logs.setStack(_stack);
//logs.setJniEnv(env);
//logs.setName("NewLocalRef");
//logs.setParams();
//
//
//logs.setResult();
//logs.log();
//return result;}
//
//DefineHookStubCheckThreadPassJniTrace_Stack0(AllocObject, jobject, JNIEnv*, env, jclass, clz){
//Logs logs;
//logs.setStack(_stack);
//logs.setJniEnv(env);
//logs.setName("AllocObject");
//logs.setParams();
//
//
//logs.setResult();
//logs.log();
//return result;}
//
//DefineHookStubCheckThreadPassJniTrace_Stack0(NewObjectV, jobject, JNIEnv*, env, jclass, clz,
//                                             jmethodID, method, va_list, params){
//Logs logs;
//logs.setStack(_stack);
//logs.setJniEnv(env);
//logs.setName("NewObjectV");
//logs.setParams();
//
//
//logs.setResult();
//logs.log();
//return result;}
//
//DefineHookStubCheckThreadPassJniTrace_Stack0(NewObjectA, jobject, JNIEnv*, env, jclass, clz,
//                                             jmethodID, method, const jvalue*, params){
//Logs logs;
//logs.setStack(_stack);
//logs.setJniEnv(env);
//logs.setName("NewObjectA");
//logs.setParams("jclass",clz);
//logs.setParams("jmethodID",method);
//logs.setParams("jmethodID",method);
//
//
//logs.setResult();
//logs.log();
//return result;}
//
//DefineHookStubCheckThreadPassJniTrace_Stack0(GetObjectClass, jclass, JNIEnv*, env, jobject, obj){
//Logs logs;
//logs.setStack(_stack);
//logs.setJniEnv(env);
//logs.setName("GetObjectClass");
//logs.setParams();
//
//
//logs.setResult();
//logs.log();
//return result;}
//
//DefineHookStubCheckThreadPassJniTrace_Stack0(GetMethodID, jmethodID, JNIEnv*, env, jclass, clz,
//                                             const char*, method, const char*, sig){
//Logs logs;
//logs.setStack(_stack);
//logs.setJniEnv(env);
//logs.setName("GetMethodID");
//logs.setParams();
//
//
//logs.setResult();
//logs.log();
//return result;}
//
//DefineHookStubCheckThreadPassJniTrace_Stack0(GetFieldID, jfieldID, JNIEnv*, env, jclass, clz,
//                                             const char*, field, const char*, sig){
//Logs logs;
//logs.setStack(_stack);
//logs.setJniEnv(env);
//logs.setName("GetFieldID");
//logs.setParams();
//
//
//logs.setResult();
//logs.log();
//return result;}
//
//DefineHookStubCheckThreadPassJniTrace_Stack0(GetStaticMethodID, jmethodID, JNIEnv*, env, jclass,
//                                             clz, const char*, method, const char*, sig){
//Logs logs;
//logs.setStack(_stack);
//logs.setJniEnv(env);
//logs.setName("GetStaticMethodID");
//logs.setParams();
//
//
//logs.setResult();
//logs.log();
//return result;}
//
//DefineHookStubCheckThreadPassJniTrace_Stack0(GetStaticFieldID, jfieldID, JNIEnv*, env, jclass, clz,
//                                             const char*, field, const char*, sig){
//Logs logs;
//logs.setStack(_stack);
//logs.setJniEnv(env);
//logs.setName("GetStaticFieldID");
//logs.setParams();
//
//
//logs.setResult();
//logs.log();
//return result;}
//
//DefineHookStubCheckThreadPassJniTrace_Stack0(Throw, jint, JNIEnv*, env, jthrowable, e){
//Logs logs;
//logs.setStack(_stack);
//logs.setJniEnv(env);
//logs.setName("Throw");
//logs.setParams();
//
//
//logs.setResult();
//logs.log();
//return result;}
//
//DefineHookStubCheckThreadPassJniTrace_Stack0(ThrowNew, jint, JNIEnv*, env, jclass, clz,
//                                             const char *, msg){
//Logs logs;
//logs.setStack(_stack);
//logs.setJniEnv(env);
//logs.setName("ThrowNew");
//logs.setParams();
//
//
//logs.setResult();
//logs.log();
//return result;}
//
//DefineHookStubCheckThreadPassJniTrace_Stack0(ExceptionOccurred, jthrowable, JNIEnv*, env){
//Logs logs;
//logs.setStack(_stack);
//logs.setJniEnv(env);
//logs.setName("ExceptionOccurred");
//logs.setParams();
//
//
//logs.setResult();
//logs.log();
//return result;}

//DefineHookStubCheckThreadPassJniTrace_Stack0(NewObject, jobject, JNIEnv*, env, jclass, jmethodID, ...);

DefineHookStub(GetVersion, jint, JNIEnv *);

DefineHookStub(ToReflectedMethod, jobject, JNIEnv *env, jclass, jmethodID, jboolean);

DefineHookStub(GetSuperclass, jclass, JNIEnv *env, jclass);

DefineHookStub(IsAssignableFrom, jboolean, JNIEnv *env, jclass, jclass);

DefineHookStub(ToReflectedField, jobject, JNIEnv *env, jclass, jfieldID, jboolean);

DefineHookStub(ExceptionDescribe, void, JNIEnv*);

DefineHookStub(ExceptionClear, void, JNIEnv*);

DefineHookStub(FatalError, void, JNIEnv *env, const char*);

DefineHookStub(NewGlobalRef, jobject, JNIEnv *env, jobject);

DefineHookStub(DeleteGlobalRef, void, JNIEnv *env, jobject);

DefineHookStub(DeleteLocalRef, void, JNIEnv *env, jobject);

DefineHookStub(IsSameObject, jboolean, JNIEnv *env, jobject, jobject);

DefineHookStub(EnsureLocalCapacity, jint, JNIEnv *env, jint);

DefineHookStub(IsInstanceOf, jboolean, JNIEnv *env, jobject, jclass);

DefineHookStub(UnregisterNatives, jint, JNIEnv *env, jclass);

DefineHookStub(MonitorEnter, jint, JNIEnv *env, jobject);

DefineHookStub(MonitorExit, jint, JNIEnv *env, jobject);

DefineHookStub(GetJavaVM, jint, JNIEnv *env, JavaVM**);

DefineHookStub(GetStringRegion, void, JNIEnv *env, jstring, jsize, jsize, jchar*);

DefineHookStub(GetStringUTFRegion, void, JNIEnv *env, jstring, jsize, jsize, char*);

DefineHookStub(GetPrimitiveArrayCritical, void*, JNIEnv *env, jarray, jboolean*);

DefineHookStub(ReleasePrimitiveArrayCritical, void, JNIEnv *env, jarray, void*, jint);

DefineHookStub(GetStringCritical, const jchar*, JNIEnv *env, jstring, jboolean*);

DefineHookStub(ReleaseStringCritical, void, JNIEnv *env, jstring, const jchar*);

DefineHookStub(NewWeakGlobalRef, jweak, JNIEnv *env, jobject);

DefineHookStub(DeleteWeakGlobalRef, void, JNIEnv *env, jweak);

DefineHookStub(ExceptionCheck, jboolean, JNIEnv*);

DefineHookStub(NewDirectByteBuffer, jobject, JNIEnv *env, void*, jlong);

DefineHookStub(GetDirectBufferAddress, void*, JNIEnv *env, jobject);

DefineHookStub(GetDirectBufferCapacity, jlong, JNIEnv *env, jobject);

DefineHookStub(GetObjectRefType, jobjectRefType, JNIEnv *env, jobject);