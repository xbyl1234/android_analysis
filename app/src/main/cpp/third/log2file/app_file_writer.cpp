#include "app_file_writer.h"

app_file_writer logWriter;

string getPkgName();
string get_extern_data_path() {
    return "/sdcard/Android/data/" + getPkgName();
}

string get_data_path() {
    return "/data/data/" + getPkgName();
}

void log2file(const string &log) {
    if (!logWriter.is_open()) {
        logWriter.open(get_data_path(), "log");
    }
    logWriter.write2file(log);
}

void log2file(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    string result = xbyl::format_string(fmt, ap);
    va_end(ap);
    log2file(result);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_android_analyse_hook_Native_nativeLog(JNIEnv *env, jclass clazz, jint level, jstring msg) {
    log2file(string(lsplant::JUTFString(env, msg).get()));
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_android_analyse_hook_Native_nativeOpenFile(JNIEnv *env, jclass clazz, jstring name) {
    return (int64_t) new app_file_writer(get_extern_data_path(), lsplant::JUTFString(env, name));
}

extern "C"
JNIEXPORT void JNICALL
Java_com_android_analyse_hook_Native_nativeWrite__JLjava_lang_String_2(JNIEnv *env, jclass clazz,
                                                                       jlong handle, jstring data) {
    auto obj = (app_file_writer *) handle;
    obj->write2file(lsplant::JUTFString(env, data));
}
