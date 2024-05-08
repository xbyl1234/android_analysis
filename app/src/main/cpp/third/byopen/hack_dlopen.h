#pragma once

// https://github.com/hack0z/byopen
// 这是我见过最恶心的代码之一,但是我用了

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


#ifdef __cplusplus
extern "C" {
#endif

#define check_return_val(x, v) \
    do                         \
    {                          \
        if (!(x))              \
            return (v);        \
    } while (0)
#define check_break(x) \
    {                  \
        if (!(x))      \
            break;     \
    }

#define assert_and_check_return_val(x, v) check_return_val(x, v)
#define assert_and_check_break(x) check_break(x)

typedef enum __by_dlopen_flag_e {
    _RTLD_LAZY = 1,
    _RTLD_NOW = 2
} by_dlopen_flag_e;

typedef struct _fake_dlctx_t {
    // magic, mark handle for fake dlopen
    uint32_t magic;

    // the load bias address of the dynamic library
    void *biasaddr;

    // the .dynsym and .dynstr sections
    void *dynstr;
    void *dynsym;
    int dynsym_num;

    // the .symtab and .strtab sections
    void *strtab;
    void *symtab;
    int symtab_num;

    // the file data and size
    void *filedata;
    size_t filesize;

} fake_dlctx_t, *fake_dlctx_ref_t;

void hack_linker_init();

fake_dlctx_ref_t hack_dlopen(char const *filename, int flag);

void *hack_dlsym(fake_dlctx_ref_t handle, char const *symbol);

int hack_dlclose(fake_dlctx_ref_t handle);

typedef struct _module_info_t {
    char module_path[512];
    void *module_address;
} module_info_t;

bool hack_get_module_info(const char *filename, module_info_t *info);

typedef bool(*enum_dlsym_callback)(const char *symbol, void *addr, void *ctx);

bool hack_enum_dlsym(fake_dlctx_ref_t handle, enum_dlsym_callback callback, void *ctx);

#ifdef __cplusplus
}
#endif