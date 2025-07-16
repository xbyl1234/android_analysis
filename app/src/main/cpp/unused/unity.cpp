#include <stdio.h>
#include <thread>
#include <sys/sysinfo.h>

#include "../third/utils/linux_helper.h"
#include "../third/byopen/hack_dlopen.h"
#include "../third/utils/log.h"
#include "../third/utils/utils.h"
#include "../common_hook.h"
#include "../dump_so.h"
#include "../base/when_hook.h"
#include "../base/hook.h"

DefineHookStub(check_send_device_info, int, void *a1, void *a2, void *a3, void *a4) {
    auto r = pHook_check_send_device_info(a1, a2, a3, a4);
    logi("check_send_device_info  %d", r);
    return r;
}

DefineHookStub(MyCallObjectMethodV, void*, JNIEnv * a1, void *a2, struct _jmethodID *a3, ...) {
    logi("MyCallObjectMethodV  %p,%p,%p", a1, a2, a3);
    auto ret = pHook_MyCallObjectMethodV(a1, a2, a3);
    logi("MyCallObjectMethodV  %p", ret);
    a1->IsSameObject(static_cast<jobject>(ret), nullptr);
    return ret;
}


void hook_unity() {
    WhenSoInitHook("libunity.so", [](const string &path, void *addr, const string &funcType) {
        module_info_t info;
        hack_get_module_info("libunity.so", &info);

//        DobbyInstrument((char *) info.module_address + 0x15A644,
//                        [](void *address, DobbyRegisterContext *ctx) {
//                            log2file("on unity hook!");
//                            logi("on unity hook!");
//                        });
        InlineHookAddr(info.module_address, 0x152D8C, check_send_device_info);
    });
}