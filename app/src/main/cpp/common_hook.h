#pragma once


extern int (*org_hook_sprinf)(const char *dest, const char *format, ...);

int hook_safe_sprinf(char *dest, size_t maxlen, const char *format, ...);

extern int (*org_hook_safe_sprinf)(const char *dest, size_t maxlen, const char *format, ...);

int hook_sprinf(char *dest, const char *format, ...);