#pragma once


using ScopedObjectAccessAlreadyRunnable = void *;

struct UnionJniEnv {
    JNIEnv *env;
    ScopedObjectAccessAlreadyRunnable soc;
};

using FormatFunc = string (*)(UnionJniEnv *env, const jvalue &obj, const string &args_type);

#define GET_FORMAT_FUNC(type)       format_java_##type
#define DECLARE_PRINTF_FUNC(type)   string format_java_##type(UnionJniEnv *env, const jvalue& obj,const string& args_type)

string SerializeJavaObject(UnionJniEnv *env, const string &argsTypes, jvalue args);

vector<string> SerializeJavaObjectVaList(UnionJniEnv *env, const vector<string> &argsTypes, va_list args);

bool JavaSignIsObject(const string &t);