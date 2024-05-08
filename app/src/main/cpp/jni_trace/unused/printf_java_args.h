#pragma once

#include <jni.h>
#include <string>

using std::string;

int get_args_size(const string &args_type);

string format_args(JNIEnv *env, const string &args_type, uint64_t obj);