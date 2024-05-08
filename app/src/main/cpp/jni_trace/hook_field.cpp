#include "hook.h"
#include "art_method_name.h"

void
DealSetFieldValue(const string &tags, const string &type, const vector<Stack> &stack,
                  void(*hook)(JNIEnv *, jobject, jfieldID, jvalue v), JNIEnv *env, jobject obj,
                  jfieldID field, jvalue v) {
    hook(env, obj, field, v);
    UnionJniEnv unionJniEnv{env, nullptr};
    string fieldStr = jniHelper.Object2Str(env, (jobject) field);
    string valueStr = SerializeJavaObject(&unionJniEnv, type, v);
    log2file("set field: %s, value: %s, ret: %s", fieldStr.c_str(), valueStr.c_str(),
             stack2str(stack).c_str());
}

jobject
DealGetFieldValue(const string &tags, const string &type, const vector<Stack> &stack,
                  jobject(*hook)(JNIEnv *, jobject, jfieldID), JNIEnv *env, jobject obj,
                  jfieldID field) {
//    callDeep++;
//    defer([&]() {
//        callDeep--;
//    });
    auto result = hook(env, obj, field);
    if (!jniTrace.CheckTargetModule(stack)) {
        return result;
    }
    UnionJniEnv unionJniEnv{env, nullptr};
    string fieldStr = jniHelper.Object2Str(env, (jobject) field);
    jvalue v;
    v.l = result;
    string valueStr = SerializeJavaObject(&unionJniEnv, type, v);
    log2file("get field: %s, value: %s, ret: %s", fieldStr.c_str(), valueStr.c_str(),
             stack2str(stack).c_str());
    return result;
}

#define DefineGetFieldHook(type, sigType) DefineHookStub(Get##type##Field, jobject, JNIEnv *env, jobject obj, jfieldID field) { \
   return DealGetFieldValue("Get" #type "Field",sigType, GetStack0(), pHook_Get##type##Field, env, obj, field);\
}

DefineGetFieldHook(Object, "L")

DefineGetFieldHook(Boolean, "Z")

DefineGetFieldHook(Byte, "B")

DefineGetFieldHook(Char, "C")

DefineGetFieldHook(Short, "S")

DefineGetFieldHook(Int, "I")

DefineGetFieldHook(Long, "J")

DefineGetFieldHook(Float, "F")

DefineGetFieldHook(Double, "D")


#define DefineSetFieldHook(type, sigType) DefineHookStub(Set##type##Field, void, JNIEnv *env, jobject obj, jfieldID field, jvalue v) { \
    DealSetFieldValue("Set" #type "Field",sigType, GetStack0(), pHook_Set##type##Field, env, obj, field,v);\
}

DefineSetFieldHook(Object, "L")

DefineSetFieldHook(Boolean, "Z")

DefineSetFieldHook(Byte, "B")

DefineSetFieldHook(Char, "C")

DefineSetFieldHook(Short, "S")

DefineSetFieldHook(Int, "I")

DefineSetFieldHook(Long, "J")

DefineSetFieldHook(Float, "F")

DefineSetFieldHook(Double, "D")

