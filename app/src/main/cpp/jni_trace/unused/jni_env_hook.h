#pragma once

#include <jni.h>

using func_type_RegisterNatives = jint (*)(JNIEnv *, jclass, JNINativeMethod *, jint);
using func_type_call_back_RegisterNatives = jint (*)(JNIEnv *,
                                                     jclass,
                                                     JNINativeMethod *,
                                                     jint,
                                                     func_type_RegisterNatives);

JNIEnv *
create_hook_jni_env(JNIEnv *tar_env, func_type_call_back_RegisterNatives call_back);

void init_global_hook_jni_env(JNIEnv *tar_env, func_type_call_back_RegisterNatives call_back);

using func_type_GetEnv = jint (*)(JavaVM *, void **, jint);

JavaVM *create_hook_java_vm(JavaVM *jvm);

void init_hook_java_vm(JavaVM *jvm);

