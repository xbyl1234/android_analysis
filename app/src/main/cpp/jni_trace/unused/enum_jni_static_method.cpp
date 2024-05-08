#include <map>

#include "../dlfc/dlfcn_nougat.h"
#include "../utils/str_helper.h"
#include "../utils/jni_helper.h"
#include "../utils/log.h"
#include "../parse_java_sig.h"
#include "enum_jni_static_method.h"
#include "global_code.h"

using std::string;
using std::map;
using std::count;

//void find_class(JNIEnv *env) {
//    if (g_origin_jvm->AttachCurrentThread(&env, NULL) != JNI_OK)
//        return;
//    jclass tmpActivityClass = env->FindClass("android/app/Activity");
//    jclass activityClass = (jclass) env->NewGlobalRef(tmpActivityClass);
//
//    jmethodID getClassLoaderMethodId = env->GetMethodID(activityClass, "getClassLoader",
//                                                        "()Ljava/lang/ClassLoader;");
//    jobject classLoaderObj = env->CallObjectMethod(gActivity, getClassLoaderMethodId);
//
//    jclass tmpClassLoaderClass = env->FindClass("java/lang/ClassLoader");
//    jclass classLoader = (jclass) env->NewGlobalRef(tmpClassLoaderClass);
//
//    jmethodID findClassMethodId = env->GetMethodID(classLoader, "loadClass",
//                                                   "(Ljava/lang/String;)Ljava/lang/Class;");
//
//    //Get a reference to the MyTestClass class
//    //通过classloader 获取自定义类
//    jstring strClassName = env->NewStringUTF("com/mytest/MyTestClass");
//    jclass tmpjavaMyTestClassClass = (jclass) env->CallObjectMethod(classLoaderObj,
//                                                                    findClassMethodId,
//                                                                    strClassName);
//    jclass javaSXRCompositorClass = (jclass) env->NewGlobalRef(tmpjavaSXRCompositorClass);
//}

void
update_method_info(JNIEnv *env, native_method_info *info) {
    jclass jClass = nullptr, jMethod = nullptr;
    jmethodID jforName = nullptr, jgetDeclaredMethods = nullptr, jgetName = nullptr, jtoString = nullptr;
    jobject jtar_Class_obj = nullptr, jmethods_list_obj = nullptr, jmethod_name = nullptr, item = nullptr;
    jstring jstring_method_decl = nullptr, jstring_tar_class_name = nullptr;

    logd("get_method_info %s", info->class_name.c_str());
    auto clear_exception = [&]() {
        if (env->ExceptionCheck()) {
            jthrowable exc = env->ExceptionOccurred();
            env->ExceptionClear();
            jboolean isCopy = false;
            jmethodID toString = env->GetMethodID(env->FindClass("java/lang/Object"),
                                                  "toString",
                                                  "()Ljava/lang/String;");
            auto s = (jstring) env->CallObjectMethod(exc, toString);
            const char *utf = env->GetStringUTFChars(s, &isCopy);
            loge("has exception!!! %s", utf);
        }
    };

    auto release = [&]() {
        clear_exception();
        if (jstring_tar_class_name != nullptr) {
            env->DeleteLocalRef(jstring_tar_class_name);
        }
        if (jtar_Class_obj != nullptr) {
            env->DeleteLocalRef(jtar_Class_obj);
        }
        if (jmethods_list_obj != nullptr) {
            env->DeleteLocalRef(jmethods_list_obj);
        }
        if (jmethod_name != nullptr) {
            env->DeleteLocalRef(jmethod_name);
        }
    };
    jClass = env->FindClass("java/lang/Class");
    jforName = env->GetStaticMethodID(jClass, "forName",
                                      "(Ljava/lang/String;)Ljava/lang/Class;");
    jgetDeclaredMethods = env->GetMethodID(jClass, "getDeclaredMethods",
                                           "()[Ljava/lang/reflect/Method;");
    jMethod = env->FindClass("java/lang/reflect/Method");
    jgetName = env->GetMethodID(jMethod, "getName", "()Ljava/lang/String;");
    jtoString = env->GetMethodID(jMethod, "toString", "()Ljava/lang/String;");


    string jni_class_name = info->class_name;
    jstring_tar_class_name = string2jstring(env, jni_class_name);
    jtar_Class_obj = env->CallStaticObjectMethod(jClass, jforName, jstring_tar_class_name);
    if (jtar_Class_obj == nullptr) {
//        release();
        clear_exception();
        replace_all(jni_class_name, ".", "/");
        jtar_Class_obj = env->FindClass(jni_class_name.c_str());
        if (jtar_Class_obj == nullptr) {
            loge("get_method_info class %s jtar_Class_obj is null!!!", jni_class_name.c_str());
            release();
            return;
        }
    }
    jmethods_list_obj = env->CallObjectMethod(jtar_Class_obj, jgetDeclaredMethods);

    int len = env->GetArrayLength((jarray) jmethods_list_obj);
    for (int index = 0; index < len; index++) {
        item = env->GetObjectArrayElement((jobjectArray) jmethods_list_obj, index);
        jmethod_name = env->CallObjectMethod((jobject) item, jgetName);
        string method_name = jstring2string(env, (jstring) jmethod_name);

        if (method_name == info->method_name) {
            jstring_method_decl = (jstring) env->CallObjectMethod((jobject) item, jtoString);
            string method_decl = jstring2string(env, jstring_method_decl);
            env->DeleteLocalRef(jstring_method_decl);
            loge("method_decl %s", method_decl.c_str());
            if (parse_java_method_sig(method_decl, info->class_name, info->method_name,
                                      info->args_type, info->ret_type)) {
                info->available = true;
            } else{
                loge("update_method_info parse_java_method_sig %s error!", method_decl.c_str());
            }
        }

        if (item != nullptr) {
            env->DeleteLocalRef(item);
        }
    }
    release();
}

void
get_method_info(JNIEnv *env, const string &class_name, const vector<native_method_info *> &infos) {
    jclass jClass = nullptr, jMethod = nullptr;
    jmethodID jforName = nullptr, jgetDeclaredMethods = nullptr, jgetName = nullptr, jtoString = nullptr;
    jobject jtar_Class_obj = nullptr, jmethods_list_obj = nullptr, jmethod_name = nullptr, item = nullptr;
    jstring jstring_method_decl = nullptr, jstring_tar_class_name = nullptr;

    logd("get_method_info %s", class_name.c_str());
    auto clear_exception = [&]() {
        if (env->ExceptionCheck()) {
            jthrowable exc = env->ExceptionOccurred();
            env->ExceptionClear();
            jboolean isCopy = false;
            jmethodID toString = env->GetMethodID(env->FindClass("java/lang/Object"),
                                                  "toString",
                                                  "()Ljava/lang/String;");
            auto s = (jstring) env->CallObjectMethod(exc, toString);
            const char *utf = env->GetStringUTFChars(s, &isCopy);
            loge("has exception!!! %s", utf);
        }
    };

    auto release = [&]() {
        clear_exception();
        if (jstring_tar_class_name != nullptr) {
            env->DeleteLocalRef(jstring_tar_class_name);
        }
        if (jtar_Class_obj != nullptr) {
            env->DeleteLocalRef(jtar_Class_obj);
        }
        if (jmethods_list_obj != nullptr) {
            env->DeleteLocalRef(jmethods_list_obj);
        }
        if (jmethod_name != nullptr) {
            env->DeleteLocalRef(jmethod_name);
        }
    };

    jClass = env->FindClass("java/lang/Class");
    jforName = env->GetStaticMethodID(jClass, "forName",
                                      "(Ljava/lang/String;)Ljava/lang/Class;");
    jgetDeclaredMethods = env->GetMethodID(jClass, "getDeclaredMethods",
                                           "()[Ljava/lang/reflect/Method;");
    jMethod = env->FindClass("java/lang/reflect/Method");
    jgetName = env->GetMethodID(jMethod, "getName", "()Ljava/lang/String;");
    jtoString = env->GetMethodID(jMethod, "toString", "()Ljava/lang/String;");

    string jni_class_name = class_name;
//    replace_all(jni_class_name, ".", "/");

    jstring_tar_class_name = string2jstring(env, jni_class_name);
    jtar_Class_obj = env->CallStaticObjectMethod(jClass, jforName, jstring_tar_class_name);
    if (jtar_Class_obj == nullptr) {
//        release();
        clear_exception();

        replace_all(jni_class_name, ".", "/");
        jtar_Class_obj = env->FindClass(jni_class_name.c_str());
        if (jtar_Class_obj == nullptr) {
            loge("get_method_info class %s jtar_Class_obj is null!!!", jni_class_name.c_str());
            release();
            return;
        }
    }
    jmethods_list_obj = env->CallObjectMethod(jtar_Class_obj, jgetDeclaredMethods);

    int len = env->GetArrayLength((jarray) jmethods_list_obj);
    for (int index = 0; index < len; index++) {
        item = env->GetObjectArrayElement((jobjectArray) jmethods_list_obj, index);
        jmethod_name = env->CallObjectMethod((jobject) item, jgetName);
        string method_name = jstring2string(env, (jstring) jmethod_name);
        for (const auto &info: infos) {
            if (method_name == info->method_name) {
                jstring_method_decl = (jstring) env->CallObjectMethod((jobject) item, jtoString);
                string method_decl = jstring2string(env, jstring_method_decl);
                env->DeleteLocalRef(jstring_method_decl);
                loge("method_decl %s", method_decl.c_str());
                if (parse_java_method_sig(method_decl, info->class_name, info->method_name,
                                          info->args_type, info->ret_type)) {
                    info->available = true;
                } else{
                    loge("get_method_info parse_java_method_sig %s error!", method_decl.c_str());
                }
            }
        }
        if (item != nullptr) {
            env->DeleteLocalRef(item);
        }
    }
    release();
}

vector<native_method_info> enum_static_jni_func(JNIEnv *env, elf_info *tar_module) {
    map<string, vector<native_method_info *>> need_query;
    vector<native_method_info> infos;
    for (int index = 0; index < tar_module->nsyms; index++) {
        const char *sym_name = fake_get_sym_name(tar_module, index);
//        logi("sym   %s", sym_name);
        native_method_info info;
        if (parse_java_lang_name(sym_name, info.class_name, info.method_name, info.args_type,
                                 info.is_override)) {
            info.addr = fake_dlsym(tar_module, sym_name);
            info.sym_name = sym_name;
            infos.push_back(info);
        }
    }
    for (auto &info :infos) {
        if (!info.is_override) {
            need_query[info.class_name].push_back(&info);
        }
    }
    for (auto &kv: need_query) {
        get_method_info(env, kv.first, kv.second);
    }
    return infos;
}

void *get_jni_onload_ptr(elf_info *tar_module) {
    return fake_dlsym(tar_module, "JNI_OnLoad");
}