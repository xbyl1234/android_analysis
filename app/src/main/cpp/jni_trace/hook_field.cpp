#include "hook.h"
#include "art_method_name.h"
#include "log_maker.h"

void
DealSetFieldValue(const string &tags, const string &type, const vector<Stack> &stack,
                  void(*hook)(JNIEnv *, jobject, jfieldID, jvalue v), JNIEnv *env, jobject obj,
                  jfieldID field, jvalue v) {
    Logs logs;
    logs.setStack(stack);
    logs.setJniEnv(env);
    logs.setName(tags);
    logs.setParams("jobject", obj);
    logs.setParams("jfieldID", field);
    logs.setParams(type, v);
    hook(env, obj, field, v);
    logs.log();
}

jobject
DealGetFieldValue(const string &tags, const string &type, const vector<Stack> &stack,
                  jobject(*hook)(JNIEnv *, jobject, jfieldID), JNIEnv *env, jobject obj,
                  jfieldID field) {
    Logs logs;
    logs.setStack(stack);
    logs.setJniEnv(env);
    logs.setName(tags);
    logs.setParams("jobject", obj);
    logs.setParams("jfieldID", field);
    auto result = hook(env, obj, field);
    logs.setResult(type, result);
    logs.log();
    return result;
}

#define DefineGetFieldHook(type, sigType) \
DefineHookStubCheckThreadPassJniTrace_Field(Get##type##Field, jobject, JNIEnv *,env, jobject, obj, jfieldID, field) { \
   return DealGetFieldValue("Get" #type "Field",sigType, _stack, pHook_Get##type##Field, env, obj, field);\
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

#define DefineSetFieldHook(type, sigType) \
DefineHookStubCheckThreadPassJniTrace_Field(Set##type##Field, void, JNIEnv*, env, jobject, obj, jfieldID, field, jvalue, v) { \
    DealSetFieldValue("Set" #type "Field",sigType, _stack, pHook_Set##type##Field, env, obj, field,v);\
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