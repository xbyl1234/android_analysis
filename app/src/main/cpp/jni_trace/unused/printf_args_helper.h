#pragma once

#include <vector>
#include <stdint.h>

#include "../route_func/route_func.h"

using std::vector;
using std::string;

#if defined(__arm64__) || defined(__aarch64__)

class printf_args_helper {
public:
    template<class T>
    static inline T get_stack_args(int stack_offset, void *stack_args) {
        return *(T *) ((char *) stack_args + stack_offset);
    };

    template<class T>
    static inline T
    get_args(int offset, int64_t x0, int64_t x1, int64_t x2, int64_t x3, int64_t x4, int64_t x5,
             int64_t x6, int64_t x7, void *pcontext, void *stack_args) {
        if (offset >= REG_COUNT * REG_BIT_SIZE) {
            return get_stack_args<T>(offset - REG_COUNT * REG_BIT_SIZE, stack_args);
        }
        switch (offset / REG_BIT_SIZE) {
            case 0:
                return *((T *) &REG_ID(0));
            case 1:
                return *((T *) &REG_ID(1));
            case 2:
                return *((T *) &REG_ID(2));
            case 3:
                return *((T *) &REG_ID(3));
            case 4:
                return *((T *) &REG_ID(4));
            case 5:
                return *((T *) &REG_ID(5));
            case 6:
                return *((T *) &REG_ID(6));
            case 7:
                return *((T *) &REG_ID(7));
        }
    }
};

void
printf_java_func_args_end_with_array(JNIEnv *env, uint64_t offset, int64_t x0, int64_t x1,
                                     int64_t x2, int64_t x3, int64_t x4, int64_t x5, int64_t x6,
                                     int64_t x7, void *pcontext, void *stack_args,
                                     const vector<string> &args_type);

void
printf_java_func_args(JNIEnv *env, uint64_t &offset, int64_t x0, int64_t x1, int64_t x2, int64_t x3,
                      int64_t x4, int64_t x5, int64_t x6, int64_t x7, void *pcontext,
                      void *stack_args, const vector<string> &args_type, int64_t args_count);

#else

class printf_args_helper {
public:
    template<class T>
    static inline T get_stack_args(uint32_t stack_offset, void *stack_args) {
        return *(T *) ((char *) stack_args + stack_offset);
    };

    template<class T>
    static inline T
    get_args(uint32_t offset, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3,
             void *pcontext, void *stack_args) {
        if (offset >= REG_COUNT * REG_BIT_SIZE) {
            return get_stack_args<T>(offset - REG_COUNT * REG_BIT_SIZE, stack_args);
        }
        switch (offset / REG_BIT_SIZE) {
            case 0:
                return *((T *) &REG_ID(0));
            case 1:
                return *((T *) &REG_ID(1));
            case 2:
                return *((T *) &REG_ID(2));
            case 3:
                return *((T *) &REG_ID(3));
        }
    }
};

void
printf_java_func_args_end_with_array(JNIEnv *env, uint32_t offset, uint32_t r0, uint32_t r1,
                                     uint32_t r2, uint32_t r3, void *pcontext, void *stack_args,
                                     const vector <string> &args_type);

void
printf_java_func_args(JNIEnv *env, uint32_t &offset, uint32_t r0, uint32_t r1, uint32_t r2,
                      uint32_t r3, void *pcontext, void *stack_args,
                      const vector <string> &args_type, int32_t args_count);

#endif