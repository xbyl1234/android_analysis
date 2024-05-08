#pragma once

#include <jni.h>
#include "../dlfc/dlfcn_nougat.h"

//void init_jni_hook(JNIEnv *env, elf_info *tar_module);

extern "C" JNIEXPORT void JNICALL
init_jni_hook(JNIEnv *env, const char *module_name);
