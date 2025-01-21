#include "hook.h"
#include "art_method_name.h"


DefineHookStub(GetObjectArrayElement, jobject, JNIEnv *env, jobjectArray array, jsize index) {
    auto stack = GetStack0();
    auto result = pHook_GetObjectArrayElement(env, array, index);
    int targetIdx = jniTrace.CheckTargetModule(stack);
    if (targetIdx == -1 || passJniTrace) {
        return result;
    }
    passJniTrace = true;
    defer([] { passJniTrace = false; });

    jvalue tmp;
    tmp.l = result;
    string valueStr = SerializeJavaObject(env, "L", tmp);
    log2file("GetObjectArrayElement %d idx: %d, value: %s, ret: %s, target: %d",
             array, index, valueStr.c_str(), stack2str(stack).c_str(), targetIdx);
    return result;
}

DefineHookStub(SetObjectArrayElement, void, JNIEnv *env, jobjectArray array, jsize index,
               jobject value) {
    auto stack = GetStack0();
    pHook_SetObjectArrayElement(env, array, index, value);
    int targetIdx = jniTrace.CheckTargetModule(stack);
    if (targetIdx == -1 || passJniTrace) {
        return;
    }
    passJniTrace = true;
    defer([] { passJniTrace = false; });

    jvalue tmp;
    tmp.l = value;
    string valueStr = SerializeJavaObject(env, "L", tmp);
    log2file("SetObjectArrayElement %d idx: %d, value: %s, ret: %s, target: %d",
             array, index, valueStr.c_str(), stack2str(stack).c_str(), targetIdx);
}
//
//jvalue *
//DealGetArrayElements(const string &tags, const string &type, const vector<Stack> &stack,
//                     jvalue *(*hook)(void *array, jboolean *isCopy), void *array,
//                     jboolean *isCopy) {
//
//}
//
//void
//DealSetArrayRegion(const string &tags, const string &type, const vector<Stack> &stack,
//                   void(*hook)(void *array, jsize start, jsize len, const void *buf), void *array,
//                   jsize start, jsize len, const void *buf) {
//
//}
//
//#define DefineGetArrayElements(type, sigType) DefineHookStub(Get##type##ArrayElements, jvalue*, void* array, jboolean* isCopy) { \
//  return  DealGetArrayElements("Get" #type "ArrayElements",sigType, GetStack0(), pHook_Get##type##ArrayElements,array,isCopy);\
//}
//
//
//DefineGetArrayElements(Boolean, "Z")
//
//DefineGetArrayElements(Byte, "B")
//
//DefineGetArrayElements(Char, "C")
//
//DefineGetArrayElements(Short, "S")
//
//DefineGetArrayElements(Int, "I")
//
//DefineGetArrayElements(Long, "J")
//
//DefineGetArrayElements(Float, "F")
//
//DefineGetArrayElements(Double, "D")
//
//#define DefineSetArrayRegion(type, sigType) DefineHookStub(Set##type##ArrayRegion,void, void* array, jsize start, jsize len,const void* buf) { \
//  DealSetArrayRegion("Set" #type "ArrayRegion",sigType, GetStack0(), pHook_Set##type##ArrayRegion, array, start, len, buf);\
//}
//
//DefineSetArrayRegion(Boolean, "Z")
//
//DefineSetArrayRegion(Byte, "B")
//
//DefineSetArrayRegion(Char, "C")
//
//DefineSetArrayRegion(Short, "S")
//
//DefineSetArrayRegion(Int, "I")
//
//DefineSetArrayRegion(Long, "J")
//
//DefineSetArrayRegion(Float, "F")
//
//DefineSetArrayRegion(Double, "D")

DefineHookStub(GetArrayLength, jsize, JNIEnv *env, jarray);

DefineHookStub(NewObjectArray, jobjectArray, JNIEnv *env, jsize, jclass, jobject);

DefineHookStub(NewBooleanArray, jbooleanArray, JNIEnv *env, jsize);

DefineHookStub(NewByteArray, jbyteArray, JNIEnv *env, jsize);

DefineHookStub(NewCharArray, jcharArray, JNIEnv *env, jsize);

DefineHookStub(NewShortArray, jshortArray, JNIEnv *env, jsize);

DefineHookStub(NewIntArray, jintArray, JNIEnv *env, jsize);

DefineHookStub(NewLongArray, jlongArray, JNIEnv *env, jsize);

DefineHookStub(NewFloatArray, jfloatArray, JNIEnv *env, jsize);

DefineHookStub(NewDoubleArray, jdoubleArray, JNIEnv *env, jsize);

DefineHookStub(GetBooleanArrayElements, jboolean*, JNIEnv *env, jbooleanArray, jboolean*);

DefineHookStub(GetByteArrayElements, jbyte*, JNIEnv *env, jbyteArray, jboolean*);

DefineHookStub(GetCharArrayElements, jchar*, JNIEnv *env, jcharArray, jboolean*);

DefineHookStub(GetShortArrayElements, jshort*, JNIEnv *env, jshortArray, jboolean*);

DefineHookStub(GetIntArrayElements, jint*, JNIEnv *env, jintArray, jboolean*);

DefineHookStub(GetLongArrayElements, jlong*, JNIEnv *env, jlongArray, jboolean*);

DefineHookStub(GetFloatArrayElements, jfloat*, JNIEnv *env, jfloatArray, jboolean*);

DefineHookStub(GetDoubleArrayElements, jdouble*, JNIEnv *env, jdoubleArray, jboolean*);

DefineHookStub(ReleaseBooleanArrayElements, void, JNIEnv *env, jbooleanArray, jboolean*, jint);

DefineHookStub(ReleaseByteArrayElements, void, JNIEnv *env, jbyteArray, jbyte*, jint);

DefineHookStub(ReleaseCharArrayElements, void, JNIEnv *env, jcharArray, jchar*, jint);

DefineHookStub(ReleaseShortArrayElements, void, JNIEnv *env, jshortArray, jshort*, jint);

DefineHookStub(ReleaseIntArrayElements, void, JNIEnv *env, jintArray, jint*, jint);

DefineHookStub(ReleaseLongArrayElements, void, JNIEnv *env, jlongArray, jlong*, jint);

DefineHookStub(ReleaseFloatArrayElements, void, JNIEnv *env, jfloatArray, jfloat*, jint);

DefineHookStub(ReleaseDoubleArrayElements, void, JNIEnv *env, jdoubleArray, jdouble*, jint);

DefineHookStub(GetBooleanArrayRegion, void, JNIEnv *env, jbooleanArray, jsize, jsize, jboolean*);

DefineHookStub(GetByteArrayRegion, void, JNIEnv *env, jbyteArray, jsize, jsize, jbyte*);

DefineHookStub(GetCharArrayRegion, void, JNIEnv *env, jcharArray, jsize, jsize, jchar*);

DefineHookStub(GetShortArrayRegion, void, JNIEnv *env, jshortArray, jsize, jsize, jshort*);

DefineHookStub(GetIntArrayRegion, void, JNIEnv *env, jintArray, jsize, jsize, jint*);

DefineHookStub(GetLongArrayRegion, void, JNIEnv *env, jlongArray, jsize, jsize, jlong*);

DefineHookStub(GetFloatArrayRegion, void, JNIEnv *env, jfloatArray, jsize, jsize, jfloat*);

DefineHookStub(GetDoubleArrayRegion, void, JNIEnv *env, jdoubleArray, jsize, jsize, jdouble*);

DefineHookStub(SetBooleanArrayRegion, void, JNIEnv *env, jbooleanArray, jsize, jsize,
               const jboolean*);

DefineHookStub(SetByteArrayRegion, void, JNIEnv *env, jbyteArray, jsize, jsize, const jbyte*);

DefineHookStub(SetCharArrayRegion, void, JNIEnv *env, jcharArray, jsize, jsize, const jchar*);

DefineHookStub(SetShortArrayRegion, void, JNIEnv *env, jshortArray, jsize, jsize, const jshort*);

DefineHookStub(SetIntArrayRegion, void, JNIEnv *env, jintArray, jsize, jsize, const jint*);

DefineHookStub(SetLongArrayRegion, void, JNIEnv *env, jlongArray, jsize, jsize, const jlong*);

DefineHookStub(SetFloatArrayRegion, void, JNIEnv *env, jfloatArray, jsize, jsize, const jfloat*);

DefineHookStub(SetDoubleArrayRegion, void, JNIEnv *env, jdoubleArray, jsize, jsize, const jdouble*);
