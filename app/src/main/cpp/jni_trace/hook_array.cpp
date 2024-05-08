#include "hook.h"
#include "art_method_name.h"


DefineHookStub(GetObjectArrayElement, jobject, JNIEnv *env, jobjectArray array, jsize index) {
    auto stack = GetStack0();
    auto result = pHook_GetObjectArrayElement(env, array, index);
    if (!jniTrace.CheckTargetModule(stack)) {
        return result;
    }
    UnionJniEnv unionJniEnv{env, nullptr};
    jvalue tmp;
    tmp.l = result;
    string valueStr = SerializeJavaObject(&unionJniEnv, "L", tmp);
    log2file("GetObjectArrayElement %d idx: %d, value: %s, ret: %s", array, index, valueStr.c_str(),
             stack2str(stack).c_str());
    return result;
}

DefineHookStub(SetObjectArrayElement, void, JNIEnv *env, jobjectArray array, jsize index,
               jobject value) {
    auto stack = GetStack0();
    pHook_SetObjectArrayElement(env, array, index, value);
    if (!jniTrace.CheckTargetModule(stack)) {
        return;
    }
    UnionJniEnv unionJniEnv{env, nullptr};
    jvalue tmp;
    tmp.l = value;
    string valueStr = SerializeJavaObject(&unionJniEnv, "L", tmp);
    log2file("SetObjectArrayElement %d idx: %d, value: %s, ret: %s", array, index, valueStr.c_str(),
             stack2str(stack).c_str());
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
