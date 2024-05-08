#include <jni.h>
#include <mutex>
#include <map>
#include "../utils/log.h"
#include "global_code.h"

using std::mutex;
using std::map;
using std::pair;

JavaVM *g_fake_jvm = nullptr;
JNIEnv *g_fake_env = nullptr;

JavaVM *g_origin_jvm = nullptr;

map<pid_t, JNIEnv *> g_map_jnievn;
mutex g_jnienv_lock;

void push_origin_env(JNIEnv *origin_env) {
    pid_t tid = gettid();
    g_jnienv_lock.lock();
    if (g_map_jnievn.find(tid) != g_map_jnievn.end()) {
//        logi("push_origin_env tid had in g_map_jnievn! tid %d origin_env %p", tid, origin_env);
    }
    g_map_jnievn.insert(pair<pid_t, JNIEnv *>(tid, origin_env));
    g_jnienv_lock.unlock();
}

JNIEnv *get_origin_env() {
    pid_t tid = gettid();
    g_jnienv_lock.lock();
    auto ret = g_map_jnievn.find(tid);
    g_jnienv_lock.unlock();

    if (ret == g_map_jnievn.end()) {
        logi("get_origin_env not found!!! tid %d", tid);
        return nullptr;
    }
    return ret->second;
}

