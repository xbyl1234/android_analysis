
#define DEBUG

#include <android/log.h>
#include <jni.h>

#define LOG_TAG "republic"
#define DEBUG
#ifdef DEBUG
#define ALOGD(fmt, args...)  do {__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args);} while(0)
#define ALOGI(fmt, args...)  do {__android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args);} while(0)
#else
#define ALOGD(fmt, args...)  do {} while(0)
#define ALOGI(fmt, args...)  do {} while(0)
#endif
#define ALOGE(fmt, args...)  do {__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##args);} while(0)

typedef union {
    JNIEnv *env;
    void *venv;
} UnionJNIEnvToVoid;

bool setApiBlacklistExemptions(JNIEnv *env) {
    jclass zygoteInitClass = env->FindClass("com/android/internal/os/ZygoteInit");
    if (zygoteInitClass == nullptr) {
        ALOGE("not found class");
        env->ExceptionClear();
        return false;
    }

    jmethodID setApiBlackListApiMethod =
            env->GetStaticMethodID(zygoteInitClass,
                                   "setApiBlacklistExemptions",
                                   "([Ljava/lang/String;)V");
    if (setApiBlackListApiMethod == nullptr) {
        env->ExceptionClear();
        setApiBlackListApiMethod =
                env->GetStaticMethodID(zygoteInitClass,
                                       "setApiDenylistExemptions",
                                       "([Ljava/lang/String;)V");
    }

    if (setApiBlackListApiMethod == nullptr) {
        ALOGE("not found method");
#if !defined(DEBUG)
        env->ExceptionClear();
#endif
        return false;
    }

    jclass stringCLass = env->FindClass("java/lang/String");

    jstring fakeStr = env->NewStringUTF("L");

    jobjectArray fakeArray = env->NewObjectArray(
            1, stringCLass, NULL);

    env->SetObjectArrayElement(fakeArray, 0, fakeStr);

    env->CallStaticVoidMethod(zygoteInitClass,
                              setApiBlackListApiMethod, fakeArray);

    env->DeleteLocalRef(fakeStr);
    env->DeleteLocalRef(fakeArray);
    ALOGD("fakeapi success!");
    return true;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv *env = NULL;

    ALOGD("JNI_OnLoad");

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_6) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed");
        goto bail;
    }
    env = uenv.env;

    if (!setApiBlacklistExemptions(env)) {
        ALOGE("failed");
        goto bail;
    }
    result = JNI_VERSION_1_6;

    bail:
    return result;
}