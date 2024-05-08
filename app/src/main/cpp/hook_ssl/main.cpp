//
// Created by Administrator on 2021/8/12 0012.
//
#include <jni.h>
#include <string.h>

#include "../utils/log.h"
#include "../dobby/include/dobby.h"

using namespace std;

class file_writer {
    std::mutex lock;
    FILE *file = nullptr;
public:
    file_writer(string path, string mod) {
        file = fopen(path.c_str(), mod.c_str());
        if (file == nullptr) {
            loge("open file %s error!", path.c_str());
        }
    }

    bool is_open() {
        return file != nullptr;
    }

    void write(string data) {
        write(data.c_str());
    }

    void write(const char *data) {
        if (file == nullptr) {
            return;
        }
        lock.lock();
        fwrite(data, strlen(data), 1, file);
        fflush(file);
        lock.unlock();
    }
};

file_writer *ssl_key_log_file;
file_writer *packet_file;
string packet_name;

typedef void (*SSL_CTX_keylog_cb_func)(const void *ssl, const char *line);

void (*SSL_CTX_set_keylog_callback)(void *ctx, SSL_CTX_keylog_cb_func cb);

void *(*SSL_CTX_new)(const void *meth);

void keylog_callback(const void *ssl, const char *line) {
    logd("hook ssl %s", line);
    ssl_key_log_file->write(line);
    ssl_key_log_file->write("\n");
}

void *hook_SSL_CTX_new(const void *meth) {
    logd("on hook_SSL_CTX_new");
    void *ctx = SSL_CTX_new(meth);
    SSL_CTX_set_keylog_callback(ctx, keylog_callback);
    return ctx;
}

void hook_ssl(const char *path) {
    logd("find libssl.so!");
    string lp = "/storage/emulated/0/Android/data/" + packet_name + "/ssl.log";
    ssl_key_log_file = new file_writer(lp.c_str(), "a+");

    auto elf = fake_dlopen(path, 0);
    if (elf == nullptr) {
        logd("dlopen error: %s", path);
        return;
    }
    void *pSSL_CTX_new = fake_dlsym(elf, "SSL_CTX_new");
    if (pSSL_CTX_new == nullptr) {
        return;
    }
    logd("find SSL_CTX_new: %s", path);
    SSL_CTX_set_keylog_callback = (void (*)(void *ctx,
                                            SSL_CTX_keylog_cb_func cb)) fake_dlsym(elf,
                                                                                   "SSL_CTX_set_keylog_callback");
    if (SSL_CTX_set_keylog_callback == nullptr) {
        return;
    }
    logd("find SSL_CTX_set_keylog_callback: %s", path);
    DobbyHook((void *) pSSL_CTX_new, (dobby_dummy_func_t) hook_SSL_CTX_new,
              (dobby_dummy_func_t *) &SSL_CTX_new);
    fake_dlclose(elf);
}

bool do_hook(const char *path) {
//    logd("emu: %s", path);
    if (strstr(path, "libssl.so")) {
//        hook_ssl(path);
        return false;
    }
    return false;
}

//FunctionInlineReplaceRouting
void *(*android_dlopen_ext)(const char *filename, int flags, const void *extinfo);

void *hook_android_dlopen_ext(const char *filename, int flags, const void *extinfo) {
    void *ret = android_dlopen_ext(filename, flags, extinfo);
    do_hook(filename);
    return ret;
}


extern "C"
JNIEXPORT void unsslpinning(char *log) {
    fake_enumerate_module(do_hook);
}

extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    log::set_adapt(log_adapt::use_adb);
    logd("onload in!");
    JNIEnv *env;
    vm->GetEnv((void **) (&env), JNI_VERSION_1_4);
    if (env != nullptr) {
        jstring pkn = get_package_name(env);
        if (pkn != nullptr) {
            packet_name = jstring2str(env, pkn);
        } else {
            loge("get package name error!");
        }
        fake_enumerate_module(do_hook);
    } else {
        loge("get env error!");
    }

    return JNI_VERSION_1_4;
}

//    auto libc_elf = fake_dlopen("/system/bin/linker64", 0);
//    android_dlopen_ext = (void *(*)(const char *filename, int flags,
//                                    const void *extinfo)) fake_dlsym(libc_elf,
//                                                                     "android_dlopen_ext");
//    if (android_dlopen_ext == nullptr) {
//        loge("find android_dlopen_ext error!");
//        return;
//    }
//    DobbyHook((void *) android_dlopen_ext, (dobby_dummy_func_t) hook_android_dlopen_ext,
//              (dobby_dummy_func_t *) &android_dlopen_ext);