#include <string>

#include "../third/utils/utils.h"
#include "../third/utils/log.h"
#include "parse_java_sig.h"

using namespace std;

bool is_java_base_args_sig(char sig) {
    switch (sig) {
        case 'V':
        case 'Z':
        case 'B':
        case 'C':
        case 'S':
        case 'I':
        case 'J':
        case 'F':
        case 'D':
            return true;
        default:
            return false;
    }
}

bool split_args_sig(const string &args_sig, vector<string> &ret_sig) {
    //    Ljava_lang_String;[[I
    char *psig = (char *) args_sig.c_str();
    int p = 0;
    auto get_class_args_type = [](const char *psig, int &start) -> string {
        int end = start;

        while (true) {
            if (psig[end] == 0) {
                start = -1;
                return "";
            }
            if (psig[end] == ';') {
                break;
            }
            end++;
        }

        string ret(&psig[start], end - start + 1);
        start = end + 1;
        return ret;
    };

    auto get_array_args_count = [](const char *psig, int &start) -> int {
        int end = start;
        int array = 0;
        while (true) {
            if (psig[end] == 0) {
                start = -1;
                return -1;
            }
            if (psig[end] != '[')
                break;
            end++;
        }
        array = end - start;
        start = end;
        return array;
    };


    int array = 0;
    while (true) {
        if (psig[p] == 0) {
            break;
        }
        if (psig[p] == '[') {
            array = get_array_args_count(psig, p);
            if (p == -1) {
                return false;
            }
        } else if (psig[p] == 'L') {
            string sig;
            for (int i = 0; i < array; ++i) {
                sig += "[";
            }
            array = 0;
            sig = get_class_args_type(psig, p);
            if (p == -1) {
                return false;
            }
            ret_sig.push_back(sig);
        } else if (is_java_base_args_sig(psig[p])) {
            string sig;
            for (int i = 0; i < array; ++i) {
                sig += "[";
            }
            array = 0;
            ret_sig.push_back(sig + string(&psig[p], 1));
            p++;
        } else {
            return false;
        }
    }
    return true;
}

bool parse_java_signature(const char *signature,
                          vector<string> &args_type,
                          string &ret_type) {
    string cpy_signature = signature;
    int p1 = cpy_signature.find('(');
    int p2 = cpy_signature.find(')');
    if (p1 == string::npos || p2 == string::npos || p1 > p2) {
        return false;
    }
    string args_sig = cpy_signature.substr(p1 + 1, p2 - p1 - 1);
    if (!args_sig.empty()) {
        if (!split_args_sig(args_sig, args_type)) {
            return false;
        }
    }
    ret_type = cpy_signature.substr(p2 + 1);
    return true;
}

bool parse_java_lang_name(const char *lang_name,
                          string &class_name,
                          string &method_name,
                          vector<string> &args_type,
                          bool &is_override) {
//    Java_com_hook_native_1hook_1helper_junregister_1file_1hook
//    Java.com.hook.native_hook_helper.junregister_file_hook
    if (strlen(lang_name) < 6 ||
        !(lang_name[0] == 'J' &&
          lang_name[1] == 'a' &&
          lang_name[2] == 'v' &&
          lang_name[3] == 'a' &&
          lang_name[4] == '_')) {
        return false;
    }
    string sys_name_cpy = &lang_name[5];
    if (sys_name_cpy.find("_0") != string::npos) {
        logi("%s", "草拟吗的中午符号!");
        return false;
    }

    auto sp_sys_name = string_split(sys_name_cpy, "__");
    if (sp_sys_name.size() != 1 && sp_sys_name.size() != 2) {
        return false;
    }
    if (sys_name_cpy.find("__") == string::npos) {
        sp_sys_name.emplace_back("");
        is_override = false;
    } else {
        is_override = true;
    }

    string api_sig = sp_sys_name[0];
    string args_sig = sp_sys_name[1];

    api_sig = replace_all(api_sig, "_", ".");
    api_sig = replace_all(api_sig, ".1", "_");
    if (count(api_sig.begin(), api_sig.end(), '.') < 1) {
        return false;
    }
    int end = api_sig.find_last_of('.');
    class_name = api_sig.substr(0, end);
    method_name = api_sig.substr(end + 1);
    if (class_name.empty() || method_name.empty()) {
        return false;
    }
    if (is_override) {
        //函数自带参数签名
        args_sig = replace_all(args_sig, "_2", ";");
        args_sig = replace_all(args_sig, "_3", "[");
//        Java_com_hook_test_test__Ljava_lang_String_2_3_3I
//        Java_com_hook_test_test__Ljava_lang_String;[[I
        if (!split_args_sig(args_sig, args_type)) {
            return false;
        }
    }
    return true;
}


string conv_java_type_2_sig_type(const string &type_name) {
    int array = 0;
    string cpy_str = type_name;
    if (type_name.find('[') != string::npos &&
        type_name.find(']') != string::npos) {
        array = count(type_name.begin(), type_name.end(), '[');
        cpy_str = replace_all(cpy_str, "[", "");
        cpy_str = replace_all(cpy_str, "]", "");
    }
    auto make_sig = [](const string &sig, int array) -> string {
        string ret;
        for (int i = 0; i < array; ++i) {
            ret += "[";
        }
        return ret + sig;
    };
    if (cpy_str == "void") {
        return make_sig("V", array);
    }
    if (cpy_str == "boolean") {
        return make_sig("Z", array);
    }
    if (cpy_str == "byte") {
        return make_sig("B", array);
    }
    if (cpy_str == "char") {
        return make_sig("C", array);
    }
    if (cpy_str == "short") {
        return make_sig("S", array);
    }
    if (cpy_str == "int") {
        return make_sig("I", array);
    }
    if (cpy_str == "long") {
        return make_sig("J", array);
    }
    if (cpy_str == "float") {
        return make_sig("F", array);
    }
    if (cpy_str == "double") {
        return make_sig("D", array);
    }
    return make_sig("L" + replace_all(cpy_str, ".", "/") + ";", array);
}

bool parse_java_method_sig(const string &decl,
                           string &class_name,
                           string &method_name,
                           vector<string> &args_type,
                           string &ret_type) {
//    loge("decl %s", decl.c_str());
    if (count(decl.begin(), decl.end(), ' ') < 1) {
        return false;
    }

    //  native void com.hook.test.test2(java.lang.String,int[][])
    //  java.lang.reflect.Method[] java.lang.Class.getDeclaredMethods()
    //  java.lang.Class java.lang.Class.forName(java.lang.String)

    int start = decl.find('(');
    int end = decl.find(')');
    if (end - start < 1 ||
        start == string::npos ||
        end == string::npos) {
        return false;
    }
    string args = decl.substr(start + 1, end - start - 1);
    args = replace_all(args, " ", "");
    args_type = string_split(args, ",");
    for (auto &item: args_type) {
        item = conv_java_type_2_sig_type(item);
    }

    int p = decl.find_last_of(' ', start);
    if (start - p <= 1 || p == string::npos) {
        return false;
    }

    string class_method = decl.substr(p + 1, start - p - 1);
    if (class_method.empty()) {
        return false;
    }
    int ppoint = class_method.find_last_of('.');
    if (ppoint == string::npos) {
        return false;
    }
    method_name = class_method.substr(ppoint + 1);
    class_name = class_method.substr(0, ppoint);
    if (method_name.empty() || class_name.empty()) {
        return false;
    }

    int p2 = decl.find_last_of(' ', p - 1);
    if (p2 == string::npos) {
        ret_type = decl.substr(0, p);
    } else {
        if (p - p2 - 1 < 1) {
            return false;
        }
        ret_type = decl.substr(p2 + 1, p - p2 - 1);
    }
    if (ret_type.empty()) {
        return false;
    }
    ret_type = conv_java_type_2_sig_type(ret_type);
    return true;
}

args_type_t conv_sig_2_args_type(const string &sig) {
    args_type_t ret;
    ret.sig = sig;
    ret.inner_base_type = sig;
    if (ret.inner_base_type[0] == '[') {
        ret.array_dim = count(ret.inner_base_type.begin(), ret.inner_base_type.end(), '.');
        ret.inner_base_type = replace_all(ret.inner_base_type, "[", "");
    }
    if (sig[0] == 'L') {
        ret.inner_base_type = ret.inner_base_type.substr(1, ret.inner_base_type.length() - 2);
        ret.inner_base_type = replace_all(ret.inner_base_type, "_", "_1");
        ret.inner_base_type = replace_all(ret.inner_base_type, "/", "_");
    }
    return ret;
}


