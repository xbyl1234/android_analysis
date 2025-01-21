#include <jni.h>
#include "../utils/jni_helper.h"
#include "../utils/log.h"
#include "printf_java_args.h"

#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

using FormatFunc = string (*)(JNIEnv *env, uint64_t obj, string args_type);

#define GET_Java_Format_Fuc(type)               format_java_##type
#define DECLARE_Java_Format_Func(type)           string format_java_##type(JNIEnv *env, uint64_t obj,string args_type)


DECLARE_Java_Format_Func(void) {
    return "void";
}

DECLARE_Java_Format_Func(bool) {
    if (obj) {
        return "true";
    } else {
        return "false";
    }
}

DECLARE_Java_Format_Func(byte) {
    char buf[254];
    sprintf(buf, "%d", obj);
    return buf;
}

DECLARE_Java_Format_Func(char) {
    char buf[254];
    sprintf(buf, "%d", obj);
    return buf;
}

DECLARE_Java_Format_Func(short) {
    char buf[254];
    sprintf(buf, "%d", obj);
    return buf;
}

DECLARE_Java_Format_Func(int) {
    char buf[254];
    sprintf(buf, "%d", obj);
    return buf;
}

DECLARE_Java_Format_Func(long) {
    char buf[254];
    sprintf(buf, "%d", obj);
    return buf;
}

DECLARE_Java_Format_Func(float) {
    char buf[254];
    sprintf(buf, "%d", obj);
    return buf;
}

DECLARE_Java_Format_Func(double) {
    char buf[254];
    sprintf(buf, "%lf", obj);
    return buf;
}

DECLARE_Java_Format_Func(java_lang_String) {
    if (obj == 0) {
        return "null";
    }
    return jstring2string(env, (jstring) obj);
}

#define USE_IN_JAVA_PARSE

//DECLARE_PRINTF_FUNC(in_jni_gson_parse) {
//    if (obj == 0) {
//        return "null";
//    }
//    jclass cJSONObject = env->FindClass("org/json/JSONObject");
//    if (!cJSONObject) {
//        clean_exception(env);
//        return "cant find org/json/JSONObject";
//    }
//
//    jmethodID jinit = env->GetMethodID(cJSONObject, "<init>", "()V");
//    jobject obj_json = env->NewObject(cJSONObject, jinit);
//    jmethodID cput = env->GetMethodID(cJSONObject, "put",
//                                      "(Ljava/lang/String;Ljava/lang/Object;)Lorg/json/JSONObject;");
//
//    jstring key = string2jstring(env, args_type);
//    env->CallObjectMethod(obj_json, cput, key, (jobject) obj);
//    if (clean_exception(env)) {
//        env->DeleteLocalRef(obj_json);
//        return "printf this object occur a exception! from put";
//    }
//
//    jmethodID jtoString = env->GetMethodID(cJSONObject, "toString", "()Ljava/lang/String;");
//    auto jret = (jstring) env->CallObjectMethod(obj_json, jtoString);
//    if (clean_exception(env)) {
//        env->DeleteLocalRef(obj_json);
//        return "printf this object occur a exception! from toString";
//    }
//    string ret = jstring2string(env, jret);
//    env->DeleteLocalRef(obj_json);
//    env->DeleteLocalRef(jret);
//    return ret;
//}

DECLARE_Java_Format_Func(in_jni_gson_parse) {
    if (obj == 0) {
        return "null";
    }
    jclass cJSONObject = env->FindClass("org/json/JSONObject");
    if (!cJSONObject) {
        clean_exception(env);
        return "cant find org/json/JSONObject";
    }

    jmethodID jinit = env->GetMethodID(cJSONObject, "<init>", "()V");
    jobject obj_json = env->NewObject(cJSONObject, jinit);
    jmethodID cput = env->GetMethodID(cJSONObject, "put",
                                      "(Ljava/lang/String;Ljava/lang/Object;)Lorg/json/JSONObject;");

    jstring key = string2jstring(env, args_type);
    env->CallObjectMethod(obj_json, cput, key, (jobject) obj);
    if (clean_exception(env)) {
        env->DeleteLocalRef(obj_json);
        return "printf this object occur a exception! from put";
    }

    jmethodID jtoString = env->GetMethodID(cJSONObject, "toString", "()Ljava/lang/String;");
    auto jret = (jstring) env->CallObjectMethod(obj_json, jtoString);
    if (clean_exception(env)) {
        env->DeleteLocalRef(obj_json);
        return "printf this object occur a exception! from toString";
    }
    string ret = jstring2string(env, jret);
    env->DeleteLocalRef(obj_json);
    env->DeleteLocalRef(jret);
    return ret;
}

//#ifdef USE_IN_JAVA_PARSE
DECLARE_Java_Format_Func(in_java_parse) {
#if not defined(USE_IN_JAVA_PARSE)
    return GET_FORMAT_FUNC(in_jni_gson_parse)(env, obj, args_type);
#endif

    if (obj == 0) {
        return "null";
    }
    jclass native_hook_helper = env->FindClass("com/plug/export/frida_helper");
    if (clean_exception(env)) {
//        return "not find frida_helper";
        return GET_Java_Format_Fuc(in_jni_gson_parse)(env, obj, args_type);
    }

    jmethodID format_args = env->GetStaticMethodID(native_hook_helper, "object_2_string",
                                                   "(Ljava/lang/Object;)Ljava/lang/String;");

#if defined(__arm64__) || defined(__aarch64__)
    jstring jret = (jstring) env->CallStaticObjectMethod(native_hook_helper, format_args, obj);
#else
    jstring jret = (jstring) env->CallStaticObjectMethod(native_hook_helper, format_args,
                                                         (uint32_t) obj);
#endif
    if (jret == nullptr) {
        return "null";
    }
    return jstring2string(env, jret);
}


DECLARE_Java_Format_Func(jclass) {
    char buf[254];
    sprintf(buf, "%p", obj);
    return buf;
}

DECLARE_Java_Format_Func(jweak) {
    char buf[254];
    sprintf(buf, "cant format jweak:%p", obj);
    return buf;
}

DECLARE_Java_Format_Func(jfieldID) {
    char buf[254];
    sprintf(buf, "%p", obj);
    return buf;
}

DECLARE_Java_Format_Func(jmethodID) {
    char buf[254];
    sprintf(buf, "%p", obj);
    return buf;
}

DECLARE_Java_Format_Func(char_4) {
    if (obj == 0) {
        return "char* is null pointer";
    }
    return (char *) obj;
}

DECLARE_Java_Format_Func(jthrowable) {
    if (obj == 0) {
        return "no exception";
    } else {
        env->ExceptionClear();
        return GET_Java_Format_Fuc(in_java_parse)(env, obj, "Exception");
    }
}

//DECLARE_PRINTF_FUNC(void_4) {
//    char buf[254];
//    sprintf(buf, "%p", obj);
//    return buf;
//}

struct FormatBean {
    type_func_format_type format_func;
    int type_size;
    bool need_jnienv;
};


map<string, format_info_t> formatFuncMap = {
        {"V",                  {GET_Java_Format_Fuc(void),             0,                 false}},
        {"Z",                  {GET_Java_Format_Fuc(bool),             sizeof(jint),      false}},
        {"B",                  {GET_Java_Format_Fuc(byte),             sizeof(jint),      false}},
        {"C",                  {GET_Java_Format_Fuc(char),             sizeof(jint),      false}},
        {"S",                  {GET_Java_Format_Fuc(short),            sizeof(jint),      false}},
        {"I",                  {GET_Java_Format_Fuc(int),              sizeof(jint),      false}},
        {"J",                  {GET_Java_Format_Fuc(long),             sizeof(jlong),     false}},
        {"F",                  {GET_Java_Format_Fuc(float),            sizeof(jdouble),   false}},
        {"D",                  {GET_Java_Format_Fuc(double),           sizeof(jdouble),   false}},
        {"void",               {GET_Java_Format_Fuc(void),             0,                 false}},
        {"jboolean",           {GET_Java_Format_Fuc(bool),             sizeof(jint),      false}},
        {"jbyte",              {GET_Java_Format_Fuc(byte),             sizeof(jint),      false}},
        {"jchar",              {GET_Java_Format_Fuc(char),             sizeof(jint),      false}},
        {"jshort",             {GET_Java_Format_Fuc(short),            sizeof(jint),      false}},
        {"jint",               {GET_Java_Format_Fuc(int),              sizeof(jint),      false}},
        {"jlong",              {GET_Java_Format_Fuc(long),             sizeof(jlong),     false}},
        {"jfloat",             {GET_Java_Format_Fuc(float),            sizeof(jdouble),   false}},
        {"jdouble",            {GET_Java_Format_Fuc(double),           sizeof(jdouble),   false}},
        {"jsize",              {GET_Java_Format_Fuc(int),              sizeof(jint),      false}},
        {"jstring",            {GET_Java_Format_Fuc(java_lang_String), sizeof(jobject),   true}},
        {"Ljava/lang/String;", {GET_Java_Format_Fuc(java_lang_String), sizeof(jobject),   true}},
        {"jarray",             {GET_Java_Format_Fuc(in_java_parse),    sizeof(jobject),   true}},
        {"jobjectArray",       {GET_Java_Format_Fuc(in_java_parse),    sizeof(jobject),   true}},
        {"jbooleanArray",      {GET_Java_Format_Fuc(in_java_parse),    sizeof(jobject),   true}},
        {"jbyteArray",         {GET_Java_Format_Fuc(in_java_parse),    sizeof(jobject),   true}},
        {"jcharArray",         {GET_Java_Format_Fuc(in_java_parse),    sizeof(jobject),   true}},
        {"jshortArray",        {GET_Java_Format_Fuc(in_java_parse),    sizeof(jobject),   true}},
        {"jintArray",          {GET_Java_Format_Fuc(in_java_parse),    sizeof(jobject),   true}},
        {"jlongArray",         {GET_Java_Format_Fuc(in_java_parse),    sizeof(jobject),   true}},
        {"jfloatArray",        {GET_Java_Format_Fuc(in_java_parse),    sizeof(jobject),   true}},
        {"jdoubleArray",       {GET_Java_Format_Fuc(in_java_parse),    sizeof(jobject),   true}},
        {"jobject",            {GET_Java_Format_Fuc(in_java_parse),    sizeof(jobject),   true}},
        {"jthrowable",         {GET_Java_Format_Fuc(jthrowable),       sizeof(jobject),   true}},
        {"jvalue",             {GET_Java_Format_Fuc(in_java_parse),    sizeof(jvalue),    true}},
        {"jclass",             {GET_Java_Format_Fuc(jclass),           sizeof(jclass),    true}},
        {"jweak",              {GET_Java_Format_Fuc(jweak),            sizeof(jweak),     true}},
        {"jfieldID",           {GET_Java_Format_Fuc(jfieldID),         sizeof(jfieldID),  true}},
        {"jmethodID",          {GET_Java_Format_Fuc(jmethodID),        sizeof(jmethodID), true}},
        {"jobjectRefType",     {GET_Java_Format_Fuc(int),              sizeof(jint),      false}},
        {"char*",              {GET_Java_Format_Fuc(char_4),           sizeof(char *),    false}},
        {"size_t",             {GET_Java_Format_Fuc(long),             sizeof(size_t),    false}},
        {"int",                {GET_Java_Format_Fuc(int),              sizeof(jint),      false}}
//        ,
//        {"void*",          {GET_FORMAT_FUNC(void_4),           sizeof(void *)}}
};

int get_args_size(const string &args_type) {
    string cpy_sig = args_type;
    auto item = formatFuncMap.find(cpy_sig);
    if (item != formatFuncMap.end()) {
        return item->second.type_size;
    }

    if (cpy_sig[0] == '[' || cpy_sig[0] == 'L') {
        return sizeof(jobject);
    }

    if (args_type.find('*')) {
        return sizeof(void *);
    }

    logi("get_args_size error!!! %s", args_type.c_str());
    return 0;
}

string format_args(JNIEnv *env, const string &args_type, uint64_t obj) {
    char buf[254];
    string cpy_sig = args_type;

    auto item = formatFuncMap.find(cpy_sig);
    if (env != nullptr) {
        if (item != formatFuncMap.end()) {
//            loge("format_args: args type %s", args_type.c_str());
            return item->second.format_func(env, obj, args_type);
        }

        if (cpy_sig[0] == '[' || cpy_sig[0] == 'L') {
//            loge("format_args in java: args type %s", args_type.c_str());
            return GET_Java_Format_Fuc(in_java_parse)(env, obj, args_type);
        }
    } else {
        if (item != formatFuncMap.end() && !item->second.need_jnienv) {
//            loge("format_args: args type %s", args_type.c_str());
            return item->second.format_func(env, obj, args_type);
        }
    }

    if (args_type.find('*')) {
        sprintf(buf, "format a pointer %s:%p", args_type.c_str(), obj);
        return buf;
    }

    sprintf(buf, "cant format %s:%p", args_type.c_str(), obj);
    return buf;
}


//string format_args(JNIEnv *env, const vector<string> &args_type, uint64_t obj) {
//    char buf[254];
//    string cpy_sig = args_type;
//    if (cpy_sig[0] == '[' || cpy_sig[0] == 'L') {
//        return GET_FORMAT_FUNC(in_java_parse)(env, obj);
//    }
//
//    if (args_type.find('*')) {
//        sprintf(buf, "format a pointer %s:%p", args_type.c_str(), obj);
//        return buf;
//    }
//
//    auto item = map_jni_type_2_args_type.find(cpy_sig);
//    if (item != map_jni_type_2_args_type.end()) {
//        return item->second(env, obj);
//    }
//
//    sprintf(buf, "cant format %s:%p", args_type.c_str(), obj);
//    return buf;
//}