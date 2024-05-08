#pragma once

#define Symbol(lib, x)   (decltype(lib##_##x))DobbySymbolResolver(#lib".so",#x)
#define Symbol2(type, lib, name)   (decltype(type))DobbySymbolResolver(lib,name)
#define IsBit64  __x86_64__ ||  __arm64__  || __aarch64__
#define IsBit32  __i386__ ||  __arm__
#define IsArm64  __arm64__  || __aarch64__
#define IsArm32  __arm__
#define IsX86_64  __x86_64__
#define IsX86_32  __i386__
#define IsX86 __x86_64__||__i386__
#define IsArm __arm__||__arm64__||__aarch64__

#define InlineHook(lib, name, exportName)   { \
                                    void* tmp_##name = DobbySymbolResolver(lib,#exportName); \
                                    if(tmp_##name == nullptr){                         \
                                        loge("DobbySymbolResolver %s -> %s failed!!!",lib,#name);    \
                                    }    \
                                    int tmp2_##name = DobbyHook(tmp_##name,(dobby_dummy_func_t) Hook_##name,(dobby_dummy_func_t*) &p##name); \
                                    if(tmp2_##name== -1) {   \
                                      loge("DobbyHook %s -> %s failed!!!",lib,#name);       \
                                    }   \
                                }