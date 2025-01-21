#include "hook.h"
#include "art_method_name.h"

DefineHookStub(GetVersion, jint, JNIEnv *);

DefineHookStub(DefineClass, jclass, JNIEnv *env, const char*, jobject, const jbyte*, jsize);

DefineHookStub(FindClass, jclass, JNIEnv *env, const char*);

DefineHookStub(ToReflectedMethod, jobject, JNIEnv *env, jclass, jmethodID, jboolean);

DefineHookStub(GetSuperclass, jclass, JNIEnv *env, jclass);

DefineHookStub(IsAssignableFrom, jboolean, JNIEnv *env, jclass, jclass);

DefineHookStub(ToReflectedField, jobject, JNIEnv *env, jclass, jfieldID, jboolean);

DefineHookStub(Throw, jint, JNIEnv *env, jthrowable);

DefineHookStub(ThrowNew, jint, JNIEnv *, jclass, const char *);

DefineHookStub(ExceptionOccurred, jthrowable, JNIEnv*);

DefineHookStub(ExceptionDescribe, void, JNIEnv*);

DefineHookStub(ExceptionClear, void, JNIEnv*);

DefineHookStub(FatalError, void, JNIEnv *env, const char*);

DefineHookStub(NewGlobalRef, jobject, JNIEnv *env, jobject);

DefineHookStub(DeleteGlobalRef, void, JNIEnv *env, jobject);

DefineHookStub(DeleteLocalRef, void, JNIEnv *env, jobject);

DefineHookStub(IsSameObject, jboolean, JNIEnv *env, jobject, jobject);

DefineHookStub(NewLocalRef, jobject, JNIEnv *env, jobject);

DefineHookStub(EnsureLocalCapacity, jint, JNIEnv *env, jint);

DefineHookStub(AllocObject, jobject, JNIEnv *env, jclass);

DefineHookStub(NewObject, jobject, JNIEnv *env, jclass, jmethodID, ...);

DefineHookStub(NewObjectV, jobject, JNIEnv *env, jclass, jmethodID, va_list);

DefineHookStub(NewObjectA, jobject, JNIEnv *env, jclass, jmethodID, const jvalue*);

DefineHookStub(GetObjectClass, jclass, JNIEnv *env, jobject);

DefineHookStub(IsInstanceOf, jboolean, JNIEnv *env, jobject, jclass);

DefineHookStub(GetMethodID, jmethodID, JNIEnv *env, jclass, const char*, const char*);

DefineHookStub(GetFieldID, jfieldID, JNIEnv *env, jclass, const char*, const char*);

DefineHookStub(GetStaticMethodID, jmethodID, JNIEnv *env, jclass, const char*, const char*);

DefineHookStub(GetStaticFieldID, jfieldID, JNIEnv *env, jclass, const char*, const char*);

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