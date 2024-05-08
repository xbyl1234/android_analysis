#include "third/utils/log.h"
#include "third/utils/utils.h"
#include "base/hook.h"
#include "log2file/app_file_writer.h"

char *targetLibBase = nullptr;


int (*org_hook_sprinf)(const char *dest, const char *format, ...);

int hook_sprinf(char *dest, const char *format, ...) {
    LOGI("on sprintf %s", format);
    va_list va;
    va_start(va, format);
    string result = format_string(format, va);
    va_end(va);
    ::strcpy(dest, result.c_str());

    log2file(result.c_str());
    return result.length();
}

int (*org_hook_safe_sprinf)(const char *dest, size_t maxlen, const char *format, ...);

int hook_safe_sprinf(char *dest, size_t maxlen, const char *format, ...) {
    LOGI("on sprintf2 %s", format);
    va_list va;
    va_start(va, format);
    string result = format_string(format, va);
    va_end(va);
    ::strcpy(dest, result.c_str());

    log2file(result.c_str());
    return result.length();
}
