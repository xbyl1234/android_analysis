#include <fstream>

#include "../utils/log.h"
#include "../json/json.h"
#include "methods_info.h"


using std::string;
using std::fstream;
extern char methods_json[];

method_info_t *jni_get_value_method_info;
method_info_t *jni_set_value_method_info;
method_info_t *jni_release_method_info;
method_info_t *jni_new_method_info;
method_info_t *jni_class_method_info;
method_info_t *jni_jni_method_info;
method_info_t *jni_except_method_info;
method_info_t *jni_other_method_info;
method_info_t *jni_call_method_info;
method_info_t *jni_jvm_method_info;

method_info_t *jni_all_methods[10];

method_info_t *libc_only_watch_method_info;

void parse_method_info_json(method_info_t **info, const Json::Value &json) {
    *info = new method_info_t[json.size() + 1]();
    method_info_t *pinfo = *info;
    for (int index = 0; index < json.size(); ++index) {
        pinfo[index].index = json[index]["index"].asInt();
        pinfo[index].name = json[index]["name"].asString();
        for (const auto &item :json[index]["args_type"]) {
            pinfo[index].args_type.push_back(item.asString());
        }
        pinfo[index].va_args_index = json[index]["va_args_index"].asInt();
        pinfo[index].ret_type = json[index]["ret_type"].asString();
        pinfo[index].origin_call = nullptr;
    }
}

void init_jni_method_info() {
//    string json_file_path = "/sdcard/jni_methods_info.json";
//    fstream fjson;
//    fjson.open(json_file_path);
//    if (!fjson.is_open()) {
//        logi("init_jni_method_info open json file %s error!!!!!", json_file_path.c_str());
//        return;
//    }
    Json::Value json;
    Json::Reader reader;
    if (!reader.parse(methods_json, json, false)) {
        logi("%s", "init_jni_method_info error!!!!!");
        return;
    }

    parse_method_info_json(&jni_get_value_method_info, json["jni_get_value_method_info"]);
    parse_method_info_json(&jni_set_value_method_info, json["jni_set_value_method_info"]);
    parse_method_info_json(&jni_release_method_info, json["jni_release_method_info"]);
    parse_method_info_json(&jni_new_method_info, json["jni_new_method_info"]);
    parse_method_info_json(&jni_class_method_info, json["jni_class_method_info"]);
    parse_method_info_json(&jni_jni_method_info, json["jni_jni_method_info"]);
    parse_method_info_json(&jni_except_method_info, json["jni_except_method_info"]);
    parse_method_info_json(&jni_other_method_info, json["jni_other_method_info"]);
    parse_method_info_json(&jni_call_method_info, json["jni_call_method_info"]);
    parse_method_info_json(&jni_jvm_method_info, json["jni_jvm_method_info"]);
    parse_method_info_json(&libc_only_watch_method_info, json["libc_only_watch_method_info"]);

    jni_all_methods[0] = jni_get_value_method_info;
    jni_all_methods[1] = jni_set_value_method_info;
    jni_all_methods[2] = jni_release_method_info;
    jni_all_methods[3] = jni_new_method_info;
    jni_all_methods[4] = jni_class_method_info;
    jni_all_methods[5] = jni_jni_method_info;
    jni_all_methods[6] = jni_except_method_info;
    jni_all_methods[7] = jni_other_method_info;
    jni_all_methods[8] = jni_call_method_info;
    jni_all_methods[9] = 0;
}

struct auto_init_method_info_t {
    auto_init_method_info_t() {
        init_jni_method_info();
    }
} auto_init_method_info;
