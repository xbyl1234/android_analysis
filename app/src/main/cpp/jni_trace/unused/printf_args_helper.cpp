#include <string>

#include "../utils/log.h"
#include "printf_java_args.h"
#include "printf_args_helper.h"
#include "../route_func/route_func.h"
#include "../utils/jni_helper.h"
#include "enum_jni_static_method.h"


#if defined(__arm64__) || defined(__aarch64__)


void
printf_java_func_args(JNIEnv *env, uint64_t &offset, int64_t x0, int64_t x1, int64_t x2, int64_t x3,
                      int64_t x4, int64_t x5, int64_t x6, int64_t x7, void *pcontext,
                      void *stack_args, const vector<string> &args_type, int64_t args_count) {
    if (clean_exception(env)) {
        auto *context = (native_method_info *) pcontext;
        loge("printf_java_func_args has pending exception at %s.%s!", context->class_name.c_str(),
             context->method_name.c_str());
        return;
    }

    if (args_count <= 0) {
        return;
    }
    int64_t args_index = offset / REG_BIT_SIZE;
    for (int for_index = 0; for_index < args_count; for_index++) {
        int args_bit_size = get_args_size(args_type[for_index]);
        uint64_t args_value;
        switch (args_bit_size) {
            case 4:
                args_value = printf_args_helper::get_args<uint32_t>(offset, x0, x1, x2, x3, x4, x5,
                                                                    x6, x7,
                                                                    pcontext, stack_args);
//                if (args_index < REG_COUNT) {
                offset += 8;
//                } else {
//                    offset += 4;
//                }
                break;
            case 8:
                args_value = printf_args_helper::get_args<uint64_t>(offset, x0, x1, x2, x3, x4, x5,
                                                                    x6, x7,
                                                                    pcontext, stack_args);
                offset += 8;
                break;
            default:
                args_value = printf_args_helper::get_args<uint32_t>(offset, x0, x1, x2, x3, x4, x5,
                                                                    x6, x7,
                                                                    pcontext, stack_args);
//                if (args_index < REG_COUNT) {
                offset += 8;
//                } else {
//                    offset += 4;
//                }
                break;
        }
        args_index += 1;
        logd("                       args type: %s , value: %s", args_type[for_index].c_str(),
             format_args(env, args_type[for_index], args_value).c_str());
    }
}

void
printf_java_func_args_end_with_array(JNIEnv *env, uint64_t offset, int64_t x0, int64_t x1,
                                     int64_t x2, int64_t x3, int64_t x4, int64_t x5, int64_t x6,
                                     int64_t x7, void *pcontext, void *stack_args,
                                     const vector<string> &args_type) {
    if (clean_exception(env)) {
        auto *context = (native_method_info *) pcontext;
        loge("printf_java_func_args has pending exception at %s.%s!", context->class_name.c_str(),
             context->method_name.c_str());
        return;
    }

    if (args_type.size() == 0) {
        return;
    }
//    printf_java_func_args(env, offset, x0, x1, x2, x3, x4, x5, x6, x7, pcontext, stack_args,
//                          args_type, args_type.size() - 1);

    void *pva = printf_args_helper::get_args<void *>(offset, x0, x1, x2, x3, x4, x5, x6, x7,
                                                     pcontext,
                                                     stack_args);
    char va_buf[0x20];
    memcpy(va_buf, pva, 0x20);
    va_list *pva_start = (va_list *) va_buf;
    for (const auto &item: args_type) {
        int args_bit_size = get_args_size(item);
        uint64_t args_value;
        switch (args_bit_size) {
            case 4:
                args_value = va_arg(*pva_start, uint32_t);
                break;
            case 8:
                args_value = va_arg(*pva_start, uint64_t);
                break;
            default:
                args_value = va_arg(*pva_start, uint32_t);
                break;
        }
        logd("                       args: %s",
             format_args(env, item, args_value).c_str());
    }
}

//
//void
//printf_java_func_args_end_with_array(JNIEnv *env, uint64_t offset, int64_t x0, int64_t x1,
//                                     int64_t x2, int64_t x3, int64_t x4, int64_t x5, int64_t x6,
//                                     int64_t x7, void *pcontext, void *stack_args,
//                                     const vector<string> &args_type) {
//    if (args_type.size() == 0) {
//        return;
//    }
//    printf_java_func_args(env, offset, x0, x1, x2, x3, x4, x5, x6, x7, pcontext, stack_args,
//                          args_type, args_type.size() - 1);
//
//    jvalue *value = printf_args_helper::get_args<jvalue *>(offset, x0, x1, x2, x3, x4, x5, x6, x7,
//                                                           pcontext,
//                                                           stack_args);
//    for (int index = 0; index < args_type.size(); index++) {
//        int args_bit_size = get_args_size(args_type[index]);
//        uint64_t args_value;
//        switch (args_bit_size) {
//            case 4:
////                args_value = value[index].i;
////                break;
//            case 8:
//                args_value = (uint64_t) value[index].d;
//                break;
//            default:
//                args_value = value[index].i;
//                break;
//        }
//
//        logd("                       args type: %s , value: %s", args_type[index].c_str(),
//             format_args(env, args_type[index], args_value).c_str());
//    }
//}
#else

void
printf_java_func_args(JNIEnv *env, uint32_t &offset, uint32_t r0, uint32_t r1, uint32_t r2,
                      uint32_t r3, void *pcontext, void *stack_args,
                      const vector<string> &args_type, int32_t args_count) {
    if (args_count <= 0) {
        return;
    }
    int32_t args_index = offset / REG_BIT_SIZE;
    for (int for_index = 0; for_index < args_count; for_index++) {
        int args_bit_size = get_args_size(args_type[for_index]);
        uint64_t args_value;
        switch (args_bit_size) {
            case 4:
                args_value = printf_args_helper::get_args<uint32_t>(offset, r0, r1, r2, r3,
                                                                    pcontext, stack_args);
                offset += 4;
                break;
            case 8:
                args_value = printf_args_helper::get_args<uint64_t>(offset, r0, r1, r2, r3,
                                                                    pcontext, stack_args);
                offset += 8;
                break;
            default:
                args_value = printf_args_helper::get_args<uint32_t>(offset, r0, r1, r2, r3,
                                                                    pcontext, stack_args);
                offset += 4;
                break;
        }
        args_index += 1;
        logd("                       args: %s",
             format_args(env, args_type[for_index], args_value).c_str());
    }
}

void
printf_java_func_args_end_with_array(JNIEnv *env, uint32_t offset, uint32_t r0, uint32_t r1,
                                     uint32_t r2, uint32_t r3, void *pcontext, void *stack_args,
                                     const vector<string> &args_type) {
    if (args_type.empty()) {
        return;
    }
    printf_java_func_args(env, offset, r0, r1, r2, r3, pcontext, stack_args, args_type,
                          args_type.size() - 1);

    jvalue *value = printf_args_helper::get_args<jvalue *>(offset, r0, r1, r2, r3, pcontext,
                                                           stack_args);
    for (int index = 0; index < args_type.size(); index++) {
        int args_bit_size = get_args_size(args_type[index]);
        uint64_t args_value;
        switch (args_bit_size) {
            case 4:
                args_value = value[index].i;
                break;
            case 8:
                args_value = (uint64_t) value[index].d;
                break;
            default:
                args_value = value[index].i;
                break;
        }
        logd("                       args: %s",
             format_args(env, args_type[index], args_value).c_str());
    }
}
#endif