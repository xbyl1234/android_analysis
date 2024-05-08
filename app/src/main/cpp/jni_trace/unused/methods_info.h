#pragma  once

#include <jni.h>
#include <string>
#include <vector>


using std::string;
using std::vector;

struct method_info_t {
    int index;
    string name;
    string ret_type;
    vector<string> args_type;
    int va_args_index;
    void *origin_call;
};

extern method_info_t *jni_get_value_method_info;
extern method_info_t *jni_set_value_method_info;
extern method_info_t *jni_release_method_info;
extern method_info_t *jni_new_method_info;
extern method_info_t *jni_class_method_info;
extern method_info_t *jni_jni_method_info;
extern method_info_t *jni_except_method_info;
extern method_info_t *jni_other_method_info;
extern method_info_t *jni_call_method_info;
extern method_info_t *jni_all_methods[];

extern method_info_t *jni_jvm_method_info;

extern method_info_t *libc_only_watch_method_info;