#include <jni.h>
#include <map>
#include <string>
#include <vector>

#include "../third/utils/jni_helper.hpp"
#include "../third/utils/log.h"

#include "format.h"

using std::map;
using std::string;
using std::vector;

DECLARE_PRINTF_FUNC(void) { return ""; }

DECLARE_PRINTF_FUNC(bool) { return obj.z ? "true" : "false"; }

DECLARE_PRINTF_FUNC(byte) { return format_string("%d", obj.i); }

DECLARE_PRINTF_FUNC(char) { return format_string("%d", obj.i); }

DECLARE_PRINTF_FUNC(short) { return format_string("%d", obj.s); }

DECLARE_PRINTF_FUNC(int) { return format_string("%d", obj.i); }

DECLARE_PRINTF_FUNC(long) { return format_string("%ld", obj.j); }

DECLARE_PRINTF_FUNC(float) { return format_string("%f", obj.f); }

DECLARE_PRINTF_FUNC(double) { return format_string("%lf", obj.d); }

DECLARE_PRINTF_FUNC(jclass) { return format_string("%p", obj.l); }

DECLARE_PRINTF_FUNC(jfieldID) { return format_string("%p", obj.l); }

DECLARE_PRINTF_FUNC(jweak) { return format_string("%p", obj.l); }

DECLARE_PRINTF_FUNC(jmethodID) { return format_string("%p", obj.l); }


DECLARE_PRINTF_FUNC(java_lang_String) {
    if (obj.l == 0) {
        return "null";
    }
    return jstring2string(env->env, (jstring) obj.l);
}

//#ifdef USE_IN_JAVA_PARSE
DECLARE_PRINTF_FUNC(in_java_parse);

DECLARE_PRINTF_FUNC(jthrowable) {
    if (obj.l == 0) {
        return "no exception";
    } else {
        env->env->ExceptionClear();
        return GET_FORMAT_FUNC(in_java_parse)(env, obj, "Exception");
    }
}

DECLARE_PRINTF_FUNC(char_4) {
    if (obj.l == 0) {
        return "char* is null pointer";
    }
    return (char *) obj.l;
}

//DECLARE_PRINTF_FUNC(void_4) {
//    char buf[254];
//    sprintf(buf, "%p", obj);
//    return buf;
//}

struct FormatBean {
    FormatFunc format_func;
    bool need_jnienv;
};

map<string, FormatBean> formatFuncMap = {
        // for java
        {"V", {GET_FORMAT_FUNC(void),   false}},
        {"Z", {GET_FORMAT_FUNC(bool),   false}},
        {"B", {GET_FORMAT_FUNC(byte),   false}},
        {"C", {GET_FORMAT_FUNC(char),   false}},
        {"S", {GET_FORMAT_FUNC(short),  false}},
        {"I", {GET_FORMAT_FUNC(int),    false}},
        {"J", {GET_FORMAT_FUNC(long),   false}},
        {"F", {GET_FORMAT_FUNC(float),  false}},
        {"D", {GET_FORMAT_FUNC(double), false}},
//        {"void",               {GET_FORMAT_FUNC(void),             false}},
//        {"jboolean",           {GET_FORMAT_FUNC(bool),             false}},
//        {"jbyte",              {GET_FORMAT_FUNC(byte),             false}},
//        {"jchar",              {GET_FORMAT_FUNC(char),             false}},
//        {"jshort",             {GET_FORMAT_FUNC(short),            false}},
//        {"jint",               {GET_FORMAT_FUNC(int),              false}},
//        {"jlong",              {GET_FORMAT_FUNC(long),             false}},
//        {"jfloat",             {GET_FORMAT_FUNC(float),            false}},
//        {"jdouble",            {GET_FORMAT_FUNC(double),           false}},
//        {"jsize",              {GET_FORMAT_FUNC(int),              false}},
//        {"jstring",            {GET_FORMAT_FUNC(java_lang_String), true}},

//        {"Ljava/lang/String;", {GET_FORMAT_FUNC(java_lang_String), true}},
//        {"jarray",             {GET_FORMAT_FUNC(in_java_parse),    true}},
//        {"jobjectArray",       {GET_FORMAT_FUNC(in_java_parse),    true}},
//        {"jbooleanArray",      {GET_FORMAT_FUNC(in_java_parse),    true}},
//        {"jbyteArray",         {GET_FORMAT_FUNC(in_java_parse),    true}},
//        {"jcharArray",         {GET_FORMAT_FUNC(in_java_parse),    true}},
//        {"jshortArray",        {GET_FORMAT_FUNC(in_java_parse),    true}},
//        {"jintArray",          {GET_FORMAT_FUNC(in_java_parse),    true}},
//        {"jlongArray",         {GET_FORMAT_FUNC(in_java_parse),    true}},
//        {"jfloatArray",        {GET_FORMAT_FUNC(in_java_parse),    true}},
//        {"jdoubleArray",       {GET_FORMAT_FUNC(in_java_parse),    true}},
//        {"jobject",            {GET_FORMAT_FUNC(in_java_parse),    true}},
//        {"jthrowable",         {GET_FORMAT_FUNC(jthrowable),       true}},
//        {"jvalue",             {GET_FORMAT_FUNC(in_java_parse),    true}},
//        {"jclass",             {GET_FORMAT_FUNC(jclass),           true}},
//        {"jweak",              {GET_FORMAT_FUNC(jweak),            true}},
//        {"jfieldID",           {GET_FORMAT_FUNC(jfieldID),         true}},
//        {"jmethodID",          {GET_FORMAT_FUNC(jmethodID),        true}},
//        {"jobjectRefType",     {GET_FORMAT_FUNC(int),              false}},
//        {"char*",              {GET_FORMAT_FUNC(char_4),           false}},
//        {"size_t",             {GET_FORMAT_FUNC(long),             false}},
//        {"int",                {GET_FORMAT_FUNC(int),              false}}
//        {"void*",          {GET_FORMAT_FUNC(void_4),           sizeof(void *)}}
};

string format_args(UnionJniEnv *env, const string &args_type, jvalue obj) {
    char buf[254];
    string cpy_sig = args_type;

    auto item = formatFuncMap.find(cpy_sig);
    if (env != nullptr) {
        if (item != formatFuncMap.end()) {
            return item->second.format_func(env, obj, args_type);
        }

        if (cpy_sig[0] == '[' || cpy_sig[0] == 'L') {
            return GET_FORMAT_FUNC(in_java_parse)(env, obj, args_type);
        }
    } else {
        if (item != formatFuncMap.end() && !item->second.need_jnienv) {
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

bool JavaSignIsObject(const string &t) {
    char ft = t.at(0);
    return ft == '[' || ft == 'L';
}

string SerializeJavaObject(UnionJniEnv *env, const string &argsTypes, jvalue args) {
    return format_args(env, argsTypes, args);
}

vector<string>
SerializeJavaObjectVaList(UnionJniEnv *env, const vector<string> &argsTypes, va_list args) {
    vector<string> result;
    vector<jvalue> values;
    for (const auto &item: argsTypes) {
        switch (item.at(0)) {
            case 'Z':
            case 'B':
            case 'C':
            case 'S':
            case 'I': {
                jvalue tmp;
                tmp.i = va_arg(args, jint);
                values.push_back(tmp);
            }
                break;
            case 'F': {
                jvalue tmp;
                tmp.d = va_arg(args, jdouble);
                values.push_back(tmp);
            }
                break;
            case 'L': {
                jvalue tmp;
                tmp.l = va_arg(args, jobject);
                values.push_back(tmp);
            }
                break;
            case 'D': {
                jvalue tmp;
                tmp.d = va_arg(args, jdouble);
                values.push_back(tmp);
            }
                break;
            case 'J': {
                jvalue tmp;
                tmp.j = va_arg(args, jlong);
                values.push_back(tmp);
            }
                break;
            case '[': {
                jvalue tmp;
                tmp.l = va_arg(args, jobject);
                values.push_back(tmp);
            }
                break;
            default:
                loge("unknow java sign: %s", item.c_str());
        }
    }

    for (int i = 0; i < values.size(); ++i) {
        result.push_back(format_args(env, argsTypes[i], values[i]));
    }
    return result;
}
