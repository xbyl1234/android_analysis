#pragma once

#include <jni.h>
#include <sys/types.h>
#include <unistd.h>
#include "../dlfc/dlfcn_nougat.h"

extern JavaVM *g_fake_jvm;
extern JNIEnv *g_fake_env;
extern JavaVM *g_origin_jvm;
extern elf_info *g_tar_module;

void push_origin_env(JNIEnv *origin_env);

JNIEnv *get_origin_env();