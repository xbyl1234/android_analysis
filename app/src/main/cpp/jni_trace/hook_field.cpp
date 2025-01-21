#include "hook.h"
#include "art_method_name.h"

void
DealSetFieldValue(const string &tags, const string &type, const vector<Stack> &stack,
                  void(*hook)(JNIEnv *, jobject, jfieldID, jvalue v), JNIEnv *env, jobject obj,
                  jfieldID field, jvalue v) {
    hook(env, obj, field, v);
    int targetIdx = jniTrace.CheckTargetModule(stack);
    if (targetIdx == -1 || passJniTrace) {
        return;
    }
    passJniTrace = true;
    defer([] { passJniTrace = false; });

    string fieldStr = jniHelper.Object2Str(env, (jobject) field);
    string valueStr = SerializeJavaObject(env, type, v);
    log2file("set field: %s, value: %s, ret: %s, target: %d",
             fieldStr.c_str(), valueStr.c_str(), stack2str(stack).c_str(), targetIdx);
}

jobject
DealGetFieldValue(const string &tags, const string &type, const vector<Stack> &stack,
                  jobject(*hook)(JNIEnv *, jobject, jfieldID), JNIEnv *env, jobject obj,
                  jfieldID field) {
    auto result = hook(env, obj, field);
    int targetIdx = jniTrace.CheckTargetModule(stack);
    if (targetIdx == -1 || passJniTrace) {
        return result;
    }
    passJniTrace = true;
    defer([] { passJniTrace = false; });

    string fieldStr = jniHelper.Object2Str(env, (jobject) field);
    jvalue v;
    v.l = result;
    string valueStr = SerializeJavaObject(env, type, v);
    log2file("get field: %s, value: %s, ret: %s, target: %d",
             fieldStr.c_str(), valueStr.c_str(), stack2str(stack).c_str(), targetIdx);
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


DefineHookStub(GetStaticObjectField, jobject, JNIEnv *env, jclass, jfieldID);

DefineHookStub(GetStaticBooleanField, jboolean, JNIEnv *env, jclass, jfieldID);

DefineHookStub(GetStaticByteField, jbyte, JNIEnv *env, jclass, jfieldID);

DefineHookStub(GetStaticCharField, jchar, JNIEnv *env, jclass, jfieldID);

DefineHookStub(GetStaticShortField, jshort, JNIEnv *env, jclass, jfieldID);

DefineHookStub(GetStaticIntField, jint, JNIEnv *env, jclass, jfieldID);

DefineHookStub(GetStaticLongField, jlong, JNIEnv *env, jclass, jfieldID);

DefineHookStub(GetStaticFloatField, jfloat, JNIEnv *env, jclass, jfieldID);

DefineHookStub(GetStaticDoubleField, jdouble, JNIEnv *env, jclass, jfieldID);

DefineHookStub(SetStaticObjectField, void, JNIEnv *env, jclass, jfieldID, jobject);

DefineHookStub(SetStaticBooleanField, void, JNIEnv *env, jclass, jfieldID, jboolean);

DefineHookStub(SetStaticByteField, void, JNIEnv *env, jclass, jfieldID, jbyte);

DefineHookStub(SetStaticCharField, void, JNIEnv *env, jclass, jfieldID, jchar);

DefineHookStub(SetStaticShortField, void, JNIEnv *env, jclass, jfieldID, jshort);

DefineHookStub(SetStaticIntField, void, JNIEnv *env, jclass, jfieldID, jint);

DefineHookStub(SetStaticLongField, void, JNIEnv *env, jclass, jfieldID, jlong);

DefineHookStub(SetStaticFloatField, void, JNIEnv *env, jclass, jfieldID, jfloat);

DefineHookStub(SetStaticDoubleField, void, JNIEnv *env, jclass, jfieldID, jdouble);