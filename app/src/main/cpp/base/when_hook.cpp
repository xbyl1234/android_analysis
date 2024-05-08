#include <mutex>
#include "../third/utils/macro_helper.h"
#include "../third/utils/log.h"
#include "../third/json/json.h"
#include "../third/byopen/hack_dlopen.h"
#include "hook.h"
#include "when_hook.h"

using namespace std;

static string targetLibName;
static WhenHookCallBack callBack;
static bool hadCall = false;
static bool hadHook = false;
static int apiLeave;


int
do_linker_log(int priority, const char *format, va_list args) {
    if (hadCall) {
        return 0;
    }
    if (strlen(format) < 5) {
        return 0;
    }
    if (strstr(format, "c-tor") && !strstr(format, "Done")) {
        va_list cp_va;
        va_copy(cp_va, args);
//        LOGI("%s", format_string(format, cp_va).c_str());
        char *type = va_arg(args, char*);
        void *addr = va_arg(args, void*);
        char *path = va_arg(args, char*);
        if (path == nullptr || strlen(path) < targetLibName.size()) {
            return 0;
        }
        if (strstr(path, targetLibName.c_str())) {
            hadCall = true;
            logi("WhenSoInitHook on %s init", targetLibName.c_str());
            callBack(path, addr, type);
        }
    } else {
//        LOGI("%s", format_string(format, args).c_str());
    }
    return 0;
}

int linker_logi(int priority, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = do_linker_log(priority, format, args);
    va_end(args);
    return result;
}

int async_safe_format_log(int priority, const char *tag, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = do_linker_log(priority, format, args);
    va_end(args);
    return result;
}

bool HookConstructor() {
#if IsBit64
    string linker = "linker64";
#else
    string linker="linker";
#endif

    auto linkerHandle = hack_dlopen(linker.c_str(), 0);

    struct Ctx {
        int *g_ld_debug_verbosity = nullptr;
        void *linker_logi = nullptr;
        void *async_safe_format_log = nullptr;
    } ctx;

    hack_enum_dlsym(linkerHandle, [](const char *symbol, void *addr, void *ctx) -> bool {
                        if (strstr(symbol, "call_function")) {
                        } else if (strstr(symbol, "g_ld_debug_verbosity")) {
                            ((Ctx *) ctx)->g_ld_debug_verbosity = (int *) addr;
                        } else if (strstr(symbol, "linker_logi")) {
                            ((Ctx *) ctx)->linker_logi = addr;
                        } else if (strstr(symbol, "async_safe_format_log") &&
                                   !strstr(symbol, "va_list")) {
                            ((Ctx *) ctx)->async_safe_format_log = addr;
                        }
                        return true;
                    },
                    &ctx);
    if (ctx.g_ld_debug_verbosity == nullptr ||
        (ctx.linker_logi == nullptr && ctx.async_safe_format_log == nullptr)) {
        LOGI("dlsym error: %p, %p, %p", ctx.g_ld_debug_verbosity, ctx.linker_logi,
             ctx.async_safe_format_log);
        return false;
    }
    *ctx.g_ld_debug_verbosity = 2;

    LOGI("dlsym: %p, %p, %p %d",
         (int64_t) ctx.g_ld_debug_verbosity - (int64_t) linkerHandle->biasaddr,
         (int64_t) ctx.linker_logi - (int64_t) linkerHandle->biasaddr,
         (int64_t) ctx.async_safe_format_log - (int64_t) linkerHandle->biasaddr,
         *ctx.g_ld_debug_verbosity);
    hack_dlclose(linkerHandle);

    void *org;
    if (ctx.linker_logi != nullptr) {
        DobbyHook(ctx.linker_logi,
                  (dobby_dummy_func_t) linker_logi,
                  (dobby_dummy_func_t *) &org);
    }
    if (ctx.async_safe_format_log != nullptr) {
        DobbyHook(ctx.async_safe_format_log,
                  (dobby_dummy_func_t) async_safe_format_log,
                  (dobby_dummy_func_t *) &org);
    }
    LOGI("HookConstructor finish");
    return true;
}

//DefineSelfHook("libdl.so", dlopen, void*, const char *filename, int flag) {
//    LOGI("load: %s", filename);
//    if (strstr(filename, targetLibName.c_str())) {
//        HookConstructor();
//    }
//    return hookInfo_dlopen(filename, flag);
//}
//void* android_dlopen_ext(const char* filename, int flag, const android_dlextinfo* extinfo)

bool WhenSoInitHook(const string &libName, WhenHookCallBack _callBack) {
    if (hadHook) {
        return true;
    }
    hadHook = true;
    targetLibName = libName;
    callBack = _callBack;
    apiLeave = android_get_device_api_level();
    HookConstructor();
//    return hookInfo_dlopen.hook();
    return true;
}

void TestWhenHook() {
    WhenSoInitHook("libtest.so", [](const string &path, void *addr, const string &funcType) {
        LOGI("call %p, func: %s, path: %s", addr, funcType.c_str(), path.c_str());
    });
}


