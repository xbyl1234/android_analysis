#include <jni.h>
#include <string>
#include <map>
#include <mutex>
#include "../dobby/include/dobby.h"
#include "../utils/log.h"
#include "../utils/jni_helper.h"
#include "../route_func/route_func.h"
#include "printf_java_args.h"
#include "printf_args_helper.h"
#include "../libxhook/xhook.h"
#include "global_code.h"
#include "jni_native_hook.h"
#include "jni_env_hook.h"
#include "enum_jni_static_method.h"
#include "../parse_java_sig.h"
#include "switch.h"

using std::mutex;
using std::string;
using std::map;


void *origin_jni_onload_ptr = nullptr;
vector<native_method_info> static_jni_func_infos;
vector<native_method_info *> reg_jni_func_infos;
elf_info *g_tar_module;

#if defined(__arm64__) || defined(__aarch64__)

int64_t hook_native_func(int64_t x0,
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

uint32_t hook_native_func(uint32_t r0,
                          uint32_t r1,
                          uint32_t r2,
                          uint32_t r3,
                          void *context,
                          void *stack_args,
                          void *ret_point)
#endif
{
    auto *origin_env = (JNIEnv *) REG_ID(0);
    push_origin_env(origin_env);
    auto *pcontext = (native_method_info *) context;
    bool has_exception = false;

    if (clean_exception(origin_env)) {
        loge("hook_native_func has pending exception at %s.%s!", pcontext->class_name.c_str(),
             pcontext->method_name.c_str());
        has_exception = true;
    }

    if (!pcontext->available) {
        logi("hook native call %s->%s ,to update method info...",
             pcontext->class_name.c_str(),
             pcontext->method_name.c_str());
        update_method_info(origin_env, pcontext);
        if (!pcontext->available) {
            loge("%s", "md fuck go dead....");
        }
    }
    logi("hook native call %s->%s, args count %ld, ret-> %p",
         pcontext->class_name.c_str(),
         pcontext->method_name.c_str(),
         pcontext->args_type.size() + 2,
         (void *) ((int64_t) ret_point - (int64_t) g_tar_module->load_addr));

#if defined(__arm64__) || defined(__aarch64__)

    uint64_t java_args_offset = 2 * REG_BIT_SIZE;
#if IS_PRINT_PARAMS
    printf_java_func_args(origin_env, java_args_offset, x0, x1, x2, x3, x4, x5, x6, x7, pcontext,
                          stack_args, pcontext->args_type, pcontext->args_type.size());
#endif
    int64_t ret = route_jni_func((uint64_t) g_fake_env, x1, x2, x3, x4, x5, x6, x7,
                                 pcontext->args_type.size() + 2 >= 8 ?
                                 pcontext->args_type.size() - 8 + 2 : 0,
                                 stack_args,
                                 pcontext->origin_call);
#else
    uint32_t java_args_offset = 2 * REG_BIT_SIZE;

#if IS_PRINT_PARAMS
    printf_java_func_args(origin_env, java_args_offset, REG_ID(0), REG_ID(1), REG_ID(2), REG_ID(3),
                          pcontext, stack_args, pcontext->args_type, pcontext->args_type.size());
#endif

    uint32_t ret = route_jni_func((uint32_t) g_fake_env, r1, r2, r3,
                                  pcontext->args_type.size() + 2 >= 4 ?
                                  pcontext->args_type.size() - 4 + 2 : 0,
                                  stack_args,
                                  pcontext->origin_call);
#endif

#if IS_PRINT_PARAMS
    logi("native call %s->%s  ret: %s", pcontext->class_name.c_str(), pcontext->method_name.c_str(),
         format_args(origin_env, pcontext->ret_type, ret).c_str());
#endif
    return ret;
}

jint hook_jni_onload(JavaVM *vm, void *reserved) {
    logi("%s", "hook native call jni_onload");
    return ((jint (*)(JavaVM *, void *)) origin_jni_onload_ptr)(g_fake_jvm, reserved);
}

jint hook_RegisterNatives(JNIEnv *env,
                          jclass c,
                          JNINativeMethod *method,
                          jint num,
                          func_type_RegisterNatives origin_call) {
    logi("%s", "hook native call RegisterNatives");
    auto *fake_method = new JNINativeMethod[num];
    for (int index = 0; index < num; ++index) {
        fake_method[index].fnPtr = method[index].fnPtr;
        fake_method[index].signature = method[index].signature;
        fake_method[index].name = method[index].name;

        auto *info = new native_method_info;
        bool is_repeat = false;
        for (const auto &item: reg_jni_func_infos) {
            if (item->addr == method[index].fnPtr) {
                is_repeat = true;
                break;
            }
        }
        if (is_repeat) {
            loge("hook_RegisterNatives parse_java_signature %s : %s is_repeat!!!",
                 method[index].name,
                 method[index].signature);
            continue;
        }
        if (!parse_java_signature(method[index].signature, info->args_type, info->ret_type)) {
            loge("hook_RegisterNatives parse_java_signature %s : %s error!!!",
                 method[index].name,
                 method[index].signature);
            continue;
        }
        info->method_name = method[index].name;
        info->addr = method[index].fnPtr;
        info->origin_call = method[index].fnPtr;
        info->class_name = get_object_class_name(env, c);
        info->available = true;
        unsigned char *shell_code = create_shellcode_hooking_func(hook_native_func, info);
        logi("hook_RegisterNatives: %s.%s : %s %lx -> %lx",
             info->class_name.c_str(),
             method[index].name,
             method[index].signature,
             (int64_t) method[index].fnPtr - (int64_t) g_tar_module->load_addr,
             (int64_t) shell_code - (int64_t) g_tar_module->load_addr);

        fake_method[index].fnPtr = shell_code;
        reg_jni_func_infos.push_back(info);
    }

    jint ret = origin_call(env, c, fake_method, num);
    delete fake_method;
    return ret;
}

void init_jni_hook(JNIEnv *env, elf_info *tar_module, const char *module_name) {
//    xhook_enable_debug(1);
    logi("%s", "to hook static jni native func");
    static_jni_func_infos = enum_static_jni_func(env, tar_module);
    for (auto &item: static_jni_func_infos) {
        if (!item.available) {
            logi("static jni native func %s->%s is not available", item.class_name.c_str(),
                 item.method_name.c_str());
//            continue;
        }
        logi("hook static jni native func %s->%s : %lx", item.class_name.c_str(),
             item.method_name.c_str(),
             (int64_t) item.addr - (int64_t) g_tar_module->load_addr);
        unsigned char *shell_code = create_shellcode_hooking_func(hook_native_func, &item);
        DobbyHook(item.addr, (dobby_dummy_func_t) shell_code,
                  (dobby_dummy_func_t *) &item.origin_call);
//        xhook_register(module_name, item.sym_name.c_str(), shell_code, &item.origin_call);
    }

    logi("%s", "to hook JNI_OnLoad, and fake java vm");
    void *jni_onload_ptr = get_jni_onload_ptr(tar_module);
    if (jni_onload_ptr) {
        DobbyHook(jni_onload_ptr, (dobby_dummy_func_t) &hook_jni_onload,
                  (dobby_dummy_func_t *) &origin_jni_onload_ptr);
//        xhook_register(module_name, "JNI_OnLoad", (void *) &hook_jni_onload,
//                       &origin_jni_onload_ptr);
    } else {
        logi("%s", "there is no JNI_OnLoad!");
    }
//    xhook_refresh(0);
}

extern "C" JNIEXPORT void JNICALL
use_file_log(const char *path) {
    log::set_file_path(path);
    log::set_adapt(log_adapt::use_file);
}

extern "C" JNIEXPORT void JNICALL
init_jni_hook(JNIEnv *env, const char *module_name) {
    logi("init jni hook! tar module %s", module_name);
    g_tar_module = fake_dlopen(module_name, 0);
    if (g_tar_module == nullptr) {
        loge("fake dlopen %s error!", module_name);
        return;
    }
    env->GetJavaVM(&g_origin_jvm);
    g_fake_env = create_hook_jni_env(env, &hook_RegisterNatives);
    g_fake_jvm = create_hook_java_vm(g_origin_jvm);

    init_jni_hook(env, g_tar_module, module_name);
}


string tat_module_name;

void *(*Libdl_dlopen)(const char *__filename, int __flag);

void *(Hook_dlopen)(const char *__filename, int __flag) {
    if (strstr(__filename, tat_module_name.c_str())) {
//        init_jni_hook()
    }
    return Libdl_dlopen(__filename, __flag);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_JniTrace_InitJniHook(JNIEnv *env, jclass clazz, jstring module_name) {
    tat_module_name = jstring2string(env, module_name);
    DobbyHook(DobbySymbolResolver("libdl.so", "dlopen"),
              (dobby_dummy_func_t) Hook_dlopen,
              (dobby_dummy_func_t *) &Libdl_dlopen);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_JniTrace_UseFileLog(JNIEnv *env, jclass clazz, jstring path) {
    use_file_log(jstring2string(env, path).c_str());
}