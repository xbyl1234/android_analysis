#pragma once

#include <string>
#include <vector>

#include "../base/hook.h"

using std::string;
using std::vector;
namespace format {
    template<typename T>
    std::string getTypeName() {
        return typeid(T).name();
    }

#define DECLARE_Java_Format_Func(type)   string format_java_##type(JNIEnv *env, const jvalue& obj,const string& args_type)

    using ScopedObjectAccessAlreadyRunnable = void *;

    string format_java_object_value(JNIEnv *env, void *value);

    string SerializeJavaObject(JNIEnv *env, const string &argsTypes, jvalue args);

    bool JavaSignIsObject(const string &t);

    vector<string>
    SerializeJavaObjectVaList(JNIEnv *env, const vector<string> &argsTypes, va_list args);

    vector<string>
    SerializeJavaObjectList(JNIEnv *env, const vector<string> &argsTypes, const jvalue *args);

    vector<jvalue> VaParams2List(const vector<string> &argsTypes, va_list args);

#define Declare_Cpp_Format_Fuc(type)  string format_value(JNIEnv *env,const type value);

    Declare_Cpp_Format_Fuc(jobjectRefType);

    Declare_Cpp_Format_Fuc(jboolean);

    Declare_Cpp_Format_Fuc(jbyte);

    Declare_Cpp_Format_Fuc(jchar);

    Declare_Cpp_Format_Fuc(jshort);

    Declare_Cpp_Format_Fuc(jint);

    Declare_Cpp_Format_Fuc(jlong);

    Declare_Cpp_Format_Fuc(jfloat);

    Declare_Cpp_Format_Fuc(jdouble);

    Declare_Cpp_Format_Fuc(jstring);

    Declare_Cpp_Format_Fuc(jclass);

    Declare_Cpp_Format_Fuc(jfieldID);

    Declare_Cpp_Format_Fuc(jweak);

    Declare_Cpp_Format_Fuc(jthrowable);

    Declare_Cpp_Format_Fuc(char *);

    Declare_Cpp_Format_Fuc(jobjectArray);

    Declare_Cpp_Format_Fuc(jbooleanArray);

    Declare_Cpp_Format_Fuc(jbyteArray);

    Declare_Cpp_Format_Fuc(jcharArray);

    Declare_Cpp_Format_Fuc(jshortArray);

    Declare_Cpp_Format_Fuc(jintArray);

    Declare_Cpp_Format_Fuc(jlongArray);

    Declare_Cpp_Format_Fuc(jfloatArray);

    Declare_Cpp_Format_Fuc(jdoubleArray);

    Declare_Cpp_Format_Fuc(JNINativeMethod *);
}