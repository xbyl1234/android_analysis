
#include <jni.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/mman.h>

#include "../utils/log.h"
#include "../utils/jni_helper.h"
#include "../route_func/route_func.h"
#include "methods_info.h"
#include "printf_java_args.h"
#include "printf_args_helper.h"
#include "jni_env_hook.h"
#include "../parse_java_sig.h"
#include "global_code.h"
#include "switch.h"

using std::string;
using std::vector;


#if defined(__arm64__) || defined(__aarch64__)

int64_t hook_call_methods(int64_t x0,
                          int64_t x1,
                          int64_t x2,
                          int64_t x3,
                          int64_t x4,
                          int64_t x5,
                          int64_t x6,
                          int64_t x7,
                          void *context,
                          void *stack_args,
                          void *ret_point)
#else

uint32_t hook_call_methods(uint32_t r0,
                           uint32_t r1,
                           uint32_t r2,
                           uint32_t r3,
                           void *context,
                           void *stack_args,
                           void *ret_point)
#endif
{
    auto *pcontext = (method_info_t *) context;
    logi("hook jni call methods %s, args %ld, ret-> %p", pcontext->name.c_str(),
         pcontext->args_type.size(),
         (void *) ((int64_t) ret_point - (int64_t) g_tar_module->load_addr));

    JNIEnv *origin_env = get_origin_env();
    if (clean_exception(origin_env)) {
        loge("hook_native_func has pending exception at %s!", pcontext->name.c_str());
    }


#if defined(__arm64__) || defined(__aarch64__)
    uint64_t java_args_offset = 0;
#if IS_PRINT_PARAMS
    printf_java_func_args(origin_env, java_args_offset, x0, x1, x2, x3, x4, x5, x6, x7, pcontext,
                          stack_args, pcontext->args_type, pcontext->args_type.size() - 1);
#endif
#else
    uint32_t java_args_offset = 0;
#if IS_PRINT_PARAMS
    printf_java_func_args(origin_env, java_args_offset, REG_ID(0), REG_ID(1), REG_ID(2), REG_ID(3),
                          pcontext,
                          stack_args, pcontext->args_type, pcontext->args_type.size() - 1);
#endif
#endif
    int mod_index;
    jmethodID method_id;
    if (pcontext->index <= 63) {
        mod_index = pcontext->index % 3;
        method_id = (jmethodID) REG_ID(2);
    } else if (pcontext->index <= 93) {
        mod_index = pcontext->index % 3;
        method_id = (jmethodID) REG_ID(3);
    } else if (pcontext->index <= 143) {
        mod_index = (pcontext->index + 1) % 3;
        method_id = (jmethodID) REG_ID(2);
    } else {
        loge("hook jni occour error!!! %s", pcontext->name.c_str());
        return 0;
    }

    string method_pretty_name = get_method_name(method_id, method_name_type::pretty_name);
    if (method_pretty_name.empty()) {
        loge("%s", "hook jni get_method_name occour error!!!");
        return 0;
    }

    string class_name;
    string method_name;
    vector<string> args_type;
    string ret_type;
    if (!parse_java_method_sig(method_pretty_name, class_name, method_name, args_type, ret_type)) {
        loge("%s", "hook jni parse_java_method_sig occour error!!!");
        return 0;
    }


    int stack_args_count;
    if (mod_index == 0) {
        stack_args_count = args_type.size() + pcontext->args_type.size() - REG_COUNT - 1;
    } else {
        stack_args_count = pcontext->args_type.size() - REG_COUNT;
    }
    if (stack_args_count < 0) {
        stack_args_count = 0;
    }
//    logd("---------route-----------%d", pcontext->args_type.size());
    logd("                            java call sub method %s", method_pretty_name.c_str());
#if defined(__arm64__) || defined(__aarch64__)
#if IS_PRINT_PARAMS
    if (mod_index == 0 || mod_index == 1) {
        printf_java_func_args(origin_env, java_args_offset, x0, x1, x2, x3, x4, x5, x6, x7,
                              pcontext, stack_args, args_type, args_type.size());
    } else {
        printf_java_func_args_end_with_array(origin_env, java_args_offset, x0, x1, x2, x3, x4, x5,
                                             x6, x7, pcontext, stack_args, args_type);
    }
#endif
    int64_t ret = route_jni_func((int64_t) origin_env, x1, x2, x3, x4, x5, x6, x7,
                                 stack_args_count,
                                 stack_args,
                                 pcontext->origin_call);
#else
#if IS_PRINT_PARAMS
    if (mod_index == 0||mod_index == 1) {
        printf_java_func_args(origin_env, java_args_offset, REG_ID(0), REG_ID(1), REG_ID(2),
                              REG_ID(3), pcontext, stack_args, args_type, args_type.size());
    } else {
        printf_java_func_args_end_with_array(origin_env, java_args_offset, REG_ID(0), REG_ID(1),
                                             REG_ID(2), REG_ID(3), pcontext, stack_args, args_type);
    }
#endif
    uint32_t ret = route_jni_func((uint32_t) origin_env, REG_ID(1), REG_ID(2), REG_ID(3),
                                  stack_args_count, stack_args, pcontext->origin_call);
#endif
//    logd("%s", "---------end----------");
#if IS_PRINT_PARAMS
    logi("                            ret: %s",
         format_args(origin_env, ret_type, ret).c_str());
#endif
    return ret;
}

func_type_call_back_RegisterNatives call_back_JNINativeMethod = nullptr;

#if defined(__arm64__) || defined(__aarch64__)

int64_t hook_other_methods(int64_t x0,
                           int64_t x1,
                           int64_t x2,
                           int64_t x3,
                           int64_t x4,
                           int64_t x5,
                           int64_t x6,
                           int64_t x7,
                           void *context,
                           void *stack_args,
                           void *ret_point)
#else

uint32_t hook_other_methods(uint32_t r0,
                            uint32_t r1,
                            uint32_t r2,
                            uint32_t r3,
                            void *context,
                            void *stack_args,
                            void *ret_point)
#endif
{
    auto *pcontext = (method_info_t *) context;
    logi("hook jni call methods %s, args %ld, ret-> %p",
         pcontext->name.c_str(),
         pcontext->args_type.size(),
         (void *) ((int64_t) ret_point - (int64_t) g_tar_module->load_addr));
//    logd("reg %p %p %p %p", r0, r1, r2, r3);
    JNIEnv *origin_env = get_origin_env();
    if (clean_exception(origin_env)) {
        loge("hook_native_func has pending exception at %s!", pcontext->name.c_str());
    }

#if defined(__arm64__) || defined(__aarch64__)
    uint64_t java_args_offset = 0;
#if IS_PRINT_PARAMS
    printf_java_func_args(origin_env, java_args_offset, x0, x1, x2, x3, x4, x5, x6, x7, pcontext,
                          stack_args, pcontext->args_type, pcontext->args_type.size());
#endif
#else
    uint32_t java_args_offset = 0;
#if IS_PRINT_PARAMS
    printf_java_func_args(origin_env, java_args_offset, REG_ID(0), REG_ID(1), REG_ID(2), REG_ID(3),
                          pcontext, stack_args, pcontext->args_type, pcontext->args_type.size());
#endif
#endif

    if (pcontext->index == 215) {
//      jint JNINativeMethod(JNIEnv *, jclass, JNINativeMethod *, jint)
        return call_back_JNINativeMethod((JNIEnv *) origin_env,
                                         (jclass) REG_ID(1),
                                         (JNINativeMethod *) REG_ID(2),
                                         (jint) REG_ID(3),
                                         (func_type_RegisterNatives) pcontext->origin_call);
    }

//    logd("---------route-----------%d", pcontext->args_type.size());
#if defined(__arm64__) || defined(__aarch64__)
    int64_t ret = route_jni_func((int64_t) origin_env, x1, x2, x3, x4, x5, x6, x7,
                                 pcontext->args_type.size() >= 8 ?
                                 pcontext->args_type.size() - 8 : 0,
                                 stack_args,
                                 pcontext->origin_call);
#else
    uint32_t ret = route_jni_func((int32_t) origin_env, r1, r2, r3,
                                  pcontext->args_type.size() >= 4 ?
                                  pcontext->args_type.size() - 4 : 0,
                                  stack_args,
                                  pcontext->origin_call);
#endif
//    logd("%s", "---------end-----------");
#if IS_PRINT_PARAMS
    logi("                       ret: %s",
         format_args(origin_env, pcontext->ret_type, ret).c_str());
#endif
    return ret;
}

JNIEnv *
create_hook_jni_env(JNIEnv *tar_env, func_type_call_back_RegisterNatives call_back) {
    call_back_JNINativeMethod = call_back;
    void **tar_interface = (void **) tar_env->functions;
    auto *new_jni = new JNIEnv;
    auto *new_interface = new JNINativeInterface;
    new_interface->reserved0 = tar_env->functions->reserved0;
    new_interface->reserved1 = tar_env->functions->reserved1;
    new_interface->reserved2 = tar_env->functions->reserved2;
    new_interface->reserved3 = tar_env->functions->reserved3;

    new_pfunc_t hook_funcs[] = {hook_other_methods,
                                hook_other_methods,
                                hook_other_methods,
                                hook_other_methods,
                                hook_other_methods,
                                hook_other_methods,
                                hook_other_methods,
                                hook_other_methods,
                                hook_call_methods};

    for (int out_index = 0; jni_all_methods[out_index] != nullptr; ++out_index) {
        for (int index = 0; jni_all_methods[out_index][index].index != 0; index++) {
            auto shell_code = create_shellcode_hooking_func(hook_funcs[out_index],
                                                            &jni_all_methods[out_index][index]);
            jni_all_methods[out_index][index].origin_call = tar_interface[jni_all_methods[out_index][index].index];
            ((void **) new_interface)[jni_all_methods[out_index][index].index] = shell_code;
        }
    }
    new_jni->functions = new_interface;
    return new_jni;
}

//jni_get_value_method_info,
//jni_set_value_method_info,
//jni_release_method_info,
//jni_new_method_info,
//jni_class_method_info,
//jni_jni_method_info,
//jni_except_method_info,
//jni_other_method_info,
//jni_call_method_info,

void init_global_hook_jni_env(JNIEnv *tar_env, func_type_call_back_RegisterNatives call_back) {
    call_back_JNINativeMethod = call_back;
    void **tar_interface = (void **) tar_env->functions;
    SET_MEMORY_PROTECT(tar_interface, PROT_READ | PROT_WRITE);

    new_pfunc_t hook_funcs[] = {hook_other_methods,
                                hook_other_methods,
                                hook_other_methods,
                                hook_other_methods,
                                hook_other_methods,
                                hook_other_methods,
                                hook_other_methods,
                                hook_other_methods,
                                hook_call_methods};

    for (int out_index = 0; jni_all_methods[out_index] != nullptr; ++out_index) {
        for (int index = 0; jni_all_methods[out_index][index].index != 0; index++) {
            auto shell_code = create_shellcode_hooking_func(hook_funcs[out_index],
                                                            &jni_all_methods[out_index][index]);
            jni_all_methods[out_index][index].origin_call = tar_interface[jni_all_methods[out_index][index].index];
            tar_interface[jni_all_methods[out_index][index].index] = shell_code;
        }
    }
}

//struct JNIInvokeInterface {
//    void*       reserved0;
//    void*       reserved1;
//    void*       reserved2;
//
//    jint        (*DestroyJavaVM)(JavaVM*);
//    jint        (*AttachCurrentThread)(JavaVM*, JNIEnv**, void*);
//    jint        (*DetachCurrentThread)(JavaVM*);
//    jint        (*GetEnv)(JavaVM*, void**, jint);
//    jint        (*AttachCurrentThreadAsDaemon)(JavaVM*, JNIEnv**, void*);
//};

jint fake_AttachCurrentThread(JavaVM *jvm, JNIEnv **penv, void *param) {
    JNIEnv *new_env = nullptr;
    jint ret = g_origin_jvm->AttachCurrentThread(&new_env, param);
    if (ret == JNI_OK) {
        push_origin_env(new_env);
        *penv = g_fake_env;
    } else {
        loge("hook_jvm_methods  jvm->AttachCurrentThread error %d!!!", ret);
        *penv = nullptr;
    }
    return ret;
}

jint fake_GetEnv(JavaVM *jvm, void **penv, jint param) {
    JNIEnv *new_env = nullptr;
    jint ret = g_origin_jvm->GetEnv((void **) &new_env, param);
    if (ret == JNI_OK) {
        push_origin_env(new_env);
        *penv = g_fake_env;
    } else {
        loge("hook_jvm_methods  jvm->GetEnv error %d!!!", ret);
        *penv = nullptr;
    }
    return ret;
}

#if defined(__arm64__) || defined(__aarch64__)

int64_t hook_jvm_methods(int64_t x0,
                         int64_t x1,
                         int64_t x2,
                         int64_t x3,
                         int64_t x4,
                         int64_t x5,
                         int64_t x6,
                         int64_t x7,
                         void *context,
                         void *stack_args,
                         void *ret_point)
#else

uint32_t hook_jvm_methods(uint32_t r0,
                          uint32_t r1,
                          uint32_t r2,
                          uint32_t r3,
                          void *context,
                          void *stack_args,
                          void *ret_point)
#endif
{
    auto *pcontext = (method_info_t *) context;
    logi("hook jvm call methods %s, ret-> %p", pcontext->name.c_str(),
         (void *) ((int64_t) ret_point - (int64_t) g_tar_module->load_addr));

    switch (pcontext->index) {
        case 4:
            return fake_AttachCurrentThread((JavaVM *) REG_ID(0), (JNIEnv **) REG_ID(1),
                                            (void *) REG_ID(2));
        case 6:
            return fake_GetEnv((JavaVM *) REG_ID(0), (void **) REG_ID(1), (jint) REG_ID(2));
    }

#if defined(__arm64__) || defined(__aarch64__)
    int64_t ret = route_jni_func((int64_t) g_origin_jvm, x1, x2, x3, x4, x5, x6, x7,
                                 pcontext->args_type.size() >= 8 ?
                                 pcontext->args_type.size() - 8 : 0,
                                 stack_args,
                                 pcontext->origin_call);
#else
    uint32_t ret = route_jni_func((int32_t) g_origin_jvm, r1, r2, r3,
                                  pcontext->args_type.size() >= 4 ?
                                  pcontext->args_type.size() - 4 : 0,
                                  stack_args,
                                  pcontext->origin_call);
#endif

    logi("                       ret: %x", ret);
    return ret;
}

JavaVM *create_hook_java_vm(JavaVM *jvm) {
    auto *tar_jvm = (JNIInvokeInterface *) jvm->functions;
    auto *new_jvm_interface = new JNIInvokeInterface;

    auto *new_jvm = new JavaVM;
    new_jvm_interface->reserved0 = tar_jvm->reserved0;
    new_jvm_interface->reserved1 = tar_jvm->reserved1;
    new_jvm_interface->reserved2 = tar_jvm->reserved2;

    for (int index = 0; jni_jvm_method_info[index].index != 0; index++) {
        auto shell_code =
                +create_shellcode_hooking_func(hook_jvm_methods,
                                               &jni_jvm_method_info[index]);

        jni_jvm_method_info[index].origin_call = ((void **) tar_jvm)[jni_jvm_method_info[index].index];
        ((void **) new_jvm_interface)[jni_jvm_method_info[index].index] = shell_code;
    }
    new_jvm->functions = new_jvm_interface;
    return new_jvm;
}

void init_hook_java_vm(JavaVM *jvm) {
    auto *tar_jvm = (JNIInvokeInterface *) jvm->functions;
    SET_MEMORY_PROTECT(tar_jvm, PROT_READ | PROT_WRITE);

    for (int index = 0; jni_jvm_method_info[index].index != 0; index++) {
        auto shell_code = create_shellcode_hooking_func(hook_jvm_methods,
                                                        &jni_jvm_method_info[index]);

        jni_jvm_method_info[index].origin_call = ((void **) tar_jvm)[jni_jvm_method_info[index].index];
        ((void **) tar_jvm)[jni_jvm_method_info[index].index] = shell_code;
    }
}