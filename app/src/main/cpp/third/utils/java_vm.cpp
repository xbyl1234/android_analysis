//#include <jni.h>
//#include "../dlfc/dlfcn_nougat.h"
//
//JNIEnv *g_tls_jnienv = nullptr;
//
//JNIEnv *get_jni_env() {
//    if (!g_tls_jnienv) {
//        elf_info *dlctx = fake_dlopen("libandroid_runtime.so", 0);
//        if (dlctx) {
//            typedef JNIEnv *(*getJNIEnv_t)();
//            getJNIEnv_t getJNIEnv = (getJNIEnv_t) fake_dlsym(dlctx,
//                                                             "_ZN7android14AndroidRuntime9getJNIEnvEv");
//            if (getJNIEnv)
//                g_tls_jnienv = getJNIEnv();
//            fake_dlclose(dlctx);
//        }
//    }
//    return g_tls_jnienv;
//}