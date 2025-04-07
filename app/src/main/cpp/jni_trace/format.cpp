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
namespace format {

#define GET_Java_Format_Fuc(type)       format_java_##type

    DECLARE_Java_Format_Func(in_java_parse);

    DECLARE_Java_Format_Func(void) { return ""; }

    DECLARE_Java_Format_Func(bool) { return obj.z ? "true" : "false"; }

    DECLARE_Java_Format_Func(byte) { return xbyl::format_string("%d", obj.i); }

    DECLARE_Java_Format_Func(char) { return xbyl::format_string("%d", obj.i); }

    DECLARE_Java_Format_Func(short) { return xbyl::format_string("%d", obj.s); }

    DECLARE_Java_Format_Func(int) { return xbyl::format_string("%d", obj.i); }

    DECLARE_Java_Format_Func(long) { return xbyl::format_string("%ld", obj.j); }

    DECLARE_Java_Format_Func(float) { return xbyl::format_string("%f", obj.f); }

    DECLARE_Java_Format_Func(double) { return xbyl::format_string("%lf", obj.d); }

    inline jvalue toJValue(void *o) {
        jvalue r;
        r.l = (jobject) o;
        return r;
    }

    string format_java_object_value(JNIEnv *env, void *value) {
        return xbyl::format_string("%s:%p",
                                   GET_Java_Format_Fuc(in_java_parse)(env,
                                                                      toJValue(value),
                                                                      getTypeName<decltype(value)>()).c_str());
    }

    using FormatFunc = string (*)(JNIEnv *env, const jvalue &obj, const string &args_type);

    struct FormatBean {
        FormatFunc format_func;
        bool need_jnienv;
    };

    map<string, FormatBean> JavaObjectFormatFuncMap = {
            // for java
            {"V", {GET_Java_Format_Fuc(void),   false}},
            {"Z", {GET_Java_Format_Fuc(bool),   false}},
            {"B", {GET_Java_Format_Fuc(byte),   false}},
            {"C", {GET_Java_Format_Fuc(char),   false}},
            {"S", {GET_Java_Format_Fuc(short),  false}},
            {"I", {GET_Java_Format_Fuc(int),    false}},
            {"J", {GET_Java_Format_Fuc(long),   false}},
            {"F", {GET_Java_Format_Fuc(float),  false}},
            {"D", {GET_Java_Format_Fuc(double), false}},
    };

    string format_args(JNIEnv *env, const string &args_type, jvalue obj) {
        char buf[254];
        string cpy_sig = args_type;

        auto item = JavaObjectFormatFuncMap.find(cpy_sig);
        if (env != nullptr) {
            if (item != JavaObjectFormatFuncMap.end()) {
                return item->second.format_func(env, obj, args_type);
            }

            if (cpy_sig[0] == '[' || cpy_sig[0] == 'L') {
                return GET_Java_Format_Fuc(in_java_parse)(env, obj, args_type);
            }
        } else {
            if (item != JavaObjectFormatFuncMap.end() && !item->second.need_jnienv) {
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

    string SerializeJavaObject(JNIEnv *env, const string &argsTypes, jvalue args) {
        return format_args(env, argsTypes, args);
    }

    vector<jvalue> VaParams2List(const vector<string> &argsTypes, va_list args) {
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
        return values;
    }

    vector<string>
    SerializeJavaObjectVaList(JNIEnv *env, const vector<string> &argsTypes, va_list args) {
        vector<string> result;
        vector<jvalue> values = VaParams2List(argsTypes, args);
        for (int i = 0; i < values.size(); ++i) {
            result.push_back(format_args(env, argsTypes[i], values[i]));
        }
        return result;
    }

    vector<string>
    SerializeJavaObjectList(JNIEnv *env, const vector<string> &argsTypes,const jvalue *args) {
        vector<string> result;
        for (int i = 0; i < argsTypes.size(); ++i) {
            result.push_back(format_args(env, argsTypes[i], args[i]));
        }
        return result;
    }
}