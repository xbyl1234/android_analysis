#pragma once

#include <jni.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

struct native_method_info {
    string class_name;
    string method_name;
    bool is_override = false;
    //暂未保存native函数前两个参数
    vector<string> args_type;
    bool available = false;
    void *addr;
    void *origin_call;
//    string args_sig;
    string ret_type;
    string sym_name;
};

vector<native_method_info> enum_static_jni_func(JNIEnv *env, elf_info *tar_module);

void *get_jni_onload_ptr(elf_info *tar_module);

void update_method_info(JNIEnv *env, native_method_info *info);