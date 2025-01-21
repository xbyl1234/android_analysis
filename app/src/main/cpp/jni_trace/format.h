#pragma once

#include <string>
#include <vector>

#include "../base/hook.h"

using std::string;
using std::vector;
namespace format {
#define DECLARE_Java_Format_Func(type)   string format_java_##type(JNIEnv *env, const jvalue& obj,const string& args_type)

    using ScopedObjectAccessAlreadyRunnable = void *;

    inline string format_java_object_value(JNIEnv *env, void *value);

    string SerializeJavaObject(JNIEnv *env, const string &argsTypes, jvalue args);

    bool JavaSignIsObject(const string &t);

    vector<string>
    SerializeJavaObjectVaList(JNIEnv *env, const vector<string> &argsTypes, va_list args);

    vector<string>
    SerializeJavaObjectList(JNIEnv *env, const vector<string> &argsTypes,
                            const vector<jvalue> &args);

    vector<jvalue> VaParams2List(const vector<string> &argsTypes, va_list args);

#define DECLARE_Cpp_Format_Func(type)   template<type>\
                                    string format_value(JNIEnv *env, type value)

#define GET_Cpp_Format_Fuc(type) format_value<type>

    template<class T>
    string format_value(JNIEnv *env, T value) {
        return "should not call this func: " + std::string(typeid(T).name());
    }

    template<typename T>
    std::string getTypeName() {
        return typeid(T).name();
    }

    DECLARE_Cpp_Format_Func(jobjectRefType) { return format_string("%d", value); }

    DECLARE_Cpp_Format_Func(jboolean) {}

    DECLARE_Cpp_Format_Func(jbyte) { return format_string("%d", value); }

    DECLARE_Cpp_Format_Func(jchar) { return format_string("%d", value); }

    DECLARE_Cpp_Format_Func(jshort) { return format_string("%d", value); }

    DECLARE_Cpp_Format_Func(jint) { return format_string("%d", value); }

    DECLARE_Cpp_Format_Func(jlong) { return format_string("%ld", value); }

    DECLARE_Cpp_Format_Func(jfloat) { return format_string("%f", value); }

    DECLARE_Cpp_Format_Func(jdouble) { return format_string("%ld", value); }

    DECLARE_Cpp_Format_Func(jstring) {
        if (value == nullptr) {
            return "null";
        }
        return jstring2string(env, value);
    }

    DECLARE_Cpp_Format_Func(jclass) {
        return format_java_object_value(env, value);
    }

    DECLARE_Cpp_Format_Func(jfieldID) {
        return format_java_object_value(env, value);
    }

    DECLARE_Cpp_Format_Func(jweak) {
        return format_string("jweak:%p", value);
    }

    DECLARE_Cpp_Format_Func(jthrowable) {
        return format_java_object_value(env, value);
    }

    DECLARE_Cpp_Format_Func(const char*) {
        if (value == nullptr) {
            return "null";
        }
        return value;
    }

#define format_array(type)  string result = "[";\
                            auto data = env->Get##type##ArrayElements(value, NULL);\
                            jsize length = env->GetArrayLength(value);\
                            for (int i = 0; i < length; i++) {\
                                result += format_value(env,data[i]) + ",";\
                            }\
                            result += "]";\
                            env->Release##type##ArrayElements(value, data, 0)

    DECLARE_Cpp_Format_Func(jarray) {
    }

    DECLARE_Cpp_Format_Func(jobjectArray) {
        string result = "[";
        jsize length = env->GetArrayLength(value);
        for (int i = 0; i < length; i++) {
            jobject objElement = env->GetObjectArrayElement(value, i);
            result += format_value(env, objElement) + ",";;
            env->DeleteLocalRef(objElement);
        }
        result += "]";
        return result;
    }

    DECLARE_Cpp_Format_Func(jbooleanArray) {
        format_array(Boolean);
        return result;
    }

    DECLARE_Cpp_Format_Func(jbyteArray) {
        format_array(Byte);
        return result;
    }

    DECLARE_Cpp_Format_Func(jcharArray) {
        format_array(Char);
        return result;
    }

    DECLARE_Cpp_Format_Func(jshortArray) {
        format_array(Short);
        return result;
    }

    DECLARE_Cpp_Format_Func(jintArray) {
        format_array(Int);
        return result;
    }

    DECLARE_Cpp_Format_Func(jlongArray) {
        format_array(Long);
        return result;
    }

    DECLARE_Cpp_Format_Func(jfloatArray) {
        format_array(Float);
        return result;
    }

    DECLARE_Cpp_Format_Func(jdoubleArray) {
        format_array(Double);
        return result;
    }

    template<class Ret, class... _ArgTypes>
    string SerializeJniObject(JNIEnv *env, const vector<Stack> &stack, const string &name, Ret ret,
                              _ArgTypes...args) {
        string result;
        int index = 0;
        result += "jni call " + name + " at " + stack2str(stack);
        std::initializer_list<int>{
                (result +=
                         "args" + to_string(index++) + ": " + format_value(env, args) + "\n", 0)...
        };
        result += "result: " + format_value(env, ret);
        return result;
    }
}