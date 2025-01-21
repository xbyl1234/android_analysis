#include <jni.h>
#include "third/utils/log.h"
#include "third/utils/jni_helper.hpp"
#include "third/utils/linux_helper.h"
#include "third/byopen/hack_dlopen.h"
#include "jni_trace/parse_java_sig.h"
#include "unwindstack/LocalUnwinder.h"
#include "dump_so.h"

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    return JNI_VERSION_1_6;
}