#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

struct args_type_t {
    string sig;
    string inner_base_type;
    int array_dim;
};

bool parse_java_signature(const char *signature,
                          vector<string> &args_type,
                          string &ret_type);

bool parse_java_method_sig(const string &decl,
                           string &class_name,
                           string &method_name,
                           vector<string> &args_type,
                           string &ret_type);

bool parse_java_lang_name(const char *lang_name,
                          string &class_name,
                          string &method_name,
                          vector<string> &args_type,
                          bool &is_override);


args_type_t conv_sig_2_args_type(const string &sig);