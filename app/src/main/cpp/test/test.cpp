#include <string>

#include "../third/utils/linux_helper.h"
#include "../third/utils/log.h"
#include "../third/json/json.h"

using namespace std;

extern "C"
JNIEXPORT void JNICALL
Java_com_android_analyse_Test_Test(JNIEnv *env, jclass clazz) {
    logi("test start");
    logi("test end");
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    logi("test load");
    return JNI_VERSION_1_6;
}