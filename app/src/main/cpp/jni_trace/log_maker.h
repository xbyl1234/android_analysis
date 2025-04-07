#include "../third/json/json.h"
#include "hook.h"
#include "art_method_name.h"

class Logs {
    bool passMethod = false;
    vector<string> args_type;
    string retType;
    string retSerialize;

    jclass clz;
    jobject obj;
    string name;
    string class_name;
    string method_name;
    string method_pretty_name;
    JNIEnv *env;
    vector<Stack> stack;
    vector<string> argsSerialize;
public:
    void setStack(const vector<Stack> &_stack) {
        this->stack = _stack;
        passMethod = jniTrace.CheckTargetModule(stack) == -1;
    }

    void setJniEnv(JNIEnv *_env) {
        this->env = _env;
    }

    void setName(const string &_name) {
        this->name = _name;
    }

    template<class T>
    void setParams(const string &_type, T _value) {
        args_type.push_back(_type);
        argsSerialize.push_back(format_value(env, _value));
    }

    void setParams(const string &_type, jvalue _value) {
        args_type.push_back(_type);
        argsSerialize.push_back(SerializeJavaObject(env, _type, _value));
    }

    void setCallParams(jclass _clz, jobject _obj, jmethodID method, va_list va) {
        if (passMethod) {
            return;
        }
        this->clz = _clz;
        this->obj = _obj;
        if (!parseMethod(method)) {
            return;
        }
        auto s = SerializeJavaObjectVaList(env, args_type, va);
        argsSerialize.insert(argsSerialize.end(), s.begin(), s.end());
    }

    void setCallParams(jclass _clz, jobject _obj, jmethodID method, const jvalue *jv) {
        if (passMethod) {
            return;
        }
        this->clz = _clz;
        this->obj = _obj;
        if (!parseMethod(method)) {
            return;
        }
        auto s = SerializeJavaObjectList(env, args_type, jv);
        argsSerialize.insert(argsSerialize.end(), s.begin(), s.end());
    }

    void setCallResult(uint64_t result) {
        if (passMethod) {
            return;
        }
        jvalue value;
        value.l = (jobject) result;
        retSerialize = SerializeJavaObject(env, retType, value);
    }

    template<class T>
    void setResult(const string &_retType, T result) {
        if (passMethod) {
            return;
        }
        this->retType = _retType;
        retSerialize = format_value(env, result);
    }

    void log() {
        if (passMethod) {
            return;
        }
        Json::Value json;
        json["tid"] = gettid();
        json["name"] = name;
        if (!method_pretty_name.empty()) {
            json["invoke"] = method_pretty_name;
        }
        for (int i = 0; i < args_type.size(); ++i) {
            Json::Value item;
            item["type"] = args_type[i].c_str();
            item["value"] = argsSerialize[i].c_str();
            json["args"].append(item);
        }
        if (!retSerialize.empty()) {
            Json::Value item;
            item["type"] = retType.c_str();
            item["value"] = retSerialize.c_str();
            json["return"] = item;
        }
        for (const auto &item: stack) {
            json["stack"].append(xbyl::format_string("%s:%p,", item.name.c_str(), item.offset));
        }
        log2file("%s", Json2String(json).c_str());
    }

    bool parseMethod(jmethodID method) {
        method_pretty_name = jniHelper.GetMethodName(method,
                                                     method_name_type::pretty_name);
        if (jniTrace.CheckPassJavaMethod(method, method_pretty_name)) {
            loge("pass method: %s", method_pretty_name.c_str());
            passMethod = true;
            return false;
        }
        if (!parse_java_method_sig(method_pretty_name, class_name, method_name, args_type,
                                   retType)) {
            loge("parse_java_method_sig error: %s", method_pretty_name.c_str());
            return false;
        }
        return true;
    }
};