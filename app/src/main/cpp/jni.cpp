#include <jni.h>
#include <thread>

#include "global/global.h"
#include "base/when_hook.h"
#include "third/utils/jni_helper.hpp"
#include "third/utils/linux_helper.h"
#include "third/utils/log.h"

void find_native_method_addr(JNIEnv *env, jobject targetClz);

extern "C"
JNIEXPORT void JNICALL
Java_com_android_analyse_hook_Native_initNative(JNIEnv *env, jclass clazz, jstring pkg_name) {
    setPkgName(lsplant::JUTFString(env, pkg_name).get());
    LOGI("analyse inject pid: %d, %s", getpid(), getPkgName().c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_android_analyse_hook_Native_find_1jni_1native_1addr(JNIEnv *env, jclass clazz,
                                                             jobjectArray clzs) {
    logi("start find native addr");
    JavaVM *jvm;
    env->GetJavaVM(&jvm);
    jobjectArray gClzs = (jobjectArray) env->NewGlobalRef(clzs);
    WhenSoInitHook("l41739d0d",
                   [jvm, gClzs](const string &path, void *addr, const string &funcType) {
                       (new std::thread([=]() {
                           sleep(3);
                           logi("on find native addr");
                           JNIEnv *env = nullptr;
                           jint result = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
                           if (result == JNI_EDETACHED) {
                               result = jvm->AttachCurrentThread(&env, nullptr);
                               if (result != JNI_OK) {
                                   loge("Failed to attach thread");
                                   return;
                               }
                           } else if (result != JNI_OK) {
                               loge("Failed to get JNIEnv");
                               return;
                           }

                           jsize length = env->GetArrayLength(gClzs);
                           for (int i = 0; i < length; ++i) {
                               jobject clz = env->GetObjectArrayElement(gClzs, i);
                               find_native_method_addr(env, clz);
                               env->DeleteLocalRef(clz);
                           }
                           env->DeleteGlobalRef(gClzs);
                           jvm->DetachCurrentThread();
                       }))->detach();
                   });
}