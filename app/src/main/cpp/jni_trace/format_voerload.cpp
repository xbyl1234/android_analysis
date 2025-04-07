#include <string>
#include <vector>

#include "../base/hook.h"
#include "format.h"

using std::string;
using std::vector;

namespace format {
#define Define_Cpp_Format_Func(type)  string format_value(JNIEnv *env,const type value)

    Define_Cpp_Format_Func(jobjectRefType) { return xbyl::format_string("%d", value); }

    Define_Cpp_Format_Func(jboolean) {}

    Define_Cpp_Format_Func(jbyte) { return xbyl::format_string("%d", value); }

    Define_Cpp_Format_Func(jchar) { return xbyl::format_string("%d", value); }

    Define_Cpp_Format_Func(jshort) { return xbyl::format_string("%d", value); }

    Define_Cpp_Format_Func(jint) { return xbyl::format_string("%d", value); }

    Define_Cpp_Format_Func(jlong) { return xbyl::format_string("%ld", value); }

    Define_Cpp_Format_Func(jfloat) { return xbyl::format_string("%f", value); }

    Define_Cpp_Format_Func(jdouble) { return xbyl::format_string("%ld", value); }

    Define_Cpp_Format_Func(jstring) {
        if (value == nullptr) {
            return "null";
        }
        return jstring2string(env, value);
    }

    Define_Cpp_Format_Func(jclass) {
        return format_java_object_value(env, value);
    }

    Define_Cpp_Format_Func(jfieldID) {
        return format_java_object_value(env, value);
    }

    Define_Cpp_Format_Func(jweak) {
        return xbyl::format_string("jweak:%p", value);
    }

    Define_Cpp_Format_Func(jthrowable) {
        return format_java_object_value(env, value);
    }

    Define_Cpp_Format_Func(char *) {
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

    Define_Cpp_Format_Func(jarray) {
    }

    Define_Cpp_Format_Func(jobjectArray) {
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

    Define_Cpp_Format_Func(jbooleanArray) {
        format_array(Boolean);
        return result;
    }

    Define_Cpp_Format_Func(jbyteArray) {
        format_array(Byte);
        return result;
    }

    Define_Cpp_Format_Func(jcharArray) {
        format_array(Char);
        return result;
    }

    Define_Cpp_Format_Func(jshortArray) {
        format_array(Short);
        return result;
    }

    Define_Cpp_Format_Func(jintArray) {
        format_array(Int);
        return result;
    }

    Define_Cpp_Format_Func(jlongArray) {
        format_array(Long);
        return result;
    }

    Define_Cpp_Format_Func(jfloatArray) {
        format_array(Float);
        return result;
    }

    Define_Cpp_Format_Func(jdoubleArray) {
        format_array(Double);
        return result;
    }

    Define_Cpp_Format_Func(JNINativeMethod *) {
        if (value == nullptr) {
            return "null";
        }
        return xbyl::format_string("name: %s, signature: %s, fnPtr: %p",
                                   value->name, value->signature, value->fnPtr);
    }
}