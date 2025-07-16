#include <jni.h>
#include "third/utils/log.h"
#include "third/utils/jni_helper.hpp"
#include "third/utils/linux_helper.h"
#include "third/byopen/hack_dlopen.h"
#include "jni_trace/parse_java_sig.h"
#include "dump_so.h"
#include "global/global.h"
#include "base/when_hook.h"
#include "base/hook.h"
#include "trace/vm.h"

#include "third/dobby/include/dobby.h"


extern "C" JNIEXPORT void JNICALL
init_jni_hook(JNIEnv *env, const char *pkg_name);

static bool CheckAllowModule(const vector<Stack> &frame, const string &str) {
    for (const auto &item: frame) {
        if (item.name.find(str) != -1) {
            return true;
        }
    }
//    for (const auto &item: frame) {
//        logi("check false: %s", item.name.c_str());
//    }
    return false;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_android_analyse_hook_Native_initNative(JNIEnv *env, jclass clazz, jstring pkg_name) {
    setPkgName(lsplant::JUTFString(env, pkg_name).get());
    LOGI("analyse inject pid: %d, %s", getpid(), getPkgName().c_str());
}


module_info_t info;

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
//    dump_so_delay("libjiagu_sdk_caNVksxYProtected.so", 10);
    WhenSoInitHook("libjiagu_sdk_caNVksxYProtected.so",
                   [](const string &path, void *addr, const string &funcType) {
                       logi("libjiagu_sdk_caNVksxYProtected load");
//                       dump_mprotect([](void *addr, int size, int prot, const string &name) {
//                           if ((uint64_t) addr == 0xc8000 || (uint64_t) addr == 0x1f6000) {
//                               dump_so("libjiagu_sdk_caNVksxYProtected.so",
//                                       "/data/data/" + getPkgName());
//                           }
//                       });
                       hack_get_module_info("libjiagu_sdk_caNVksxYProtected.so", &info);
                       DobbyInstrument((void *) ((uint64_t) info.module_address + 0x2CBC),
                                       [](void *address, DobbyRegisterContext *ctx) {
                                           logi("init array call finish");
                                           DobbyDestroy(address);
                                           DobbyInstrument(
                                                   (void *) ((uint64_t) info.module_address +
                                                             0x00104EC),
                                                   [](void *address, DobbyRegisterContext *ctx) {
                                                       logi("will trace");
                                                       DobbyDestroy(address);
                                                       MotherTrace *trace = new MotherTrace();
                                                       trace->runTraceCode(address,
                                                                           (void *) (
                                                                                   (uint64_t) info.module_address +
                                                                                   0x1061C), ctx);
                                                   });
                                       });
                   });
//    WhenSoInitHook("libtest.so",
//                   [](const string &path, void *addr, const string &funcType) {
//                       logi("libtest load");
//                       hack_get_module_info("libtest.so", &info);
//                       DobbyInstrument(
//                               DobbySymbolResolver("libtest.so",
//                                                   "Java_com_android_analyse_Test_Test"),
//                               [](void *address, DobbyRegisterContext *ctx) {
//                                   logi("init array call finish");
//                                   DobbyDestroy(address);
//                                   logi("will trace");
//                                   MotherTrace *trace = new MotherTrace();
//                                   trace->runTraceCode(address, nullptr, ctx);
////                                    test_qbdi(address, ctx);
//                               });
//                   });
    return JNI_VERSION_1_6;
}


////
//DefineHookStub(waitpid, pid_t, pid_t pid, int *status, int options) {
//    logi("waitpid");
//    if (!CheckAllowModule(GetStack1(), "libx69737.so")) {
//        logi("pass waitpid");
//        return pHook_waitpid(pid, status, options);
//    }
//    logi("proc: %d, before waitpid: %d, status: %d, opt: %d", getpid(), pid, status, options);
//    auto result = pHook_waitpid(pid, status, options);
//    logi("proc: %d, after waitpid: %d, status: %d, opt: %d", getpid(), pid, status, options);
//    return result;
//}
//
//DefineHookStub(fork, pid_t, void) {
//    logi("fork");
//    if (!CheckAllowModule(GetStack1(), "libx69737.so")) {
//        logi("pass fork");
//        return pHook_fork();
//    }
//    logi("proc: %d, before fork: %d", getpid(), getpid());
//    auto pid = pHook_fork();
//    return pid;
//}
//
//DefineHookStub(flock, int, int fd, int op) {
//    logi("flock");
//    if (!CheckAllowModule(GetStack1(), "libx69737.so")) {
//        logi("pass flock");
//        return pHook_flock(fd, op);
//    }
//    logi("proc: %d, before flock fd: %d, op: %d", getpid(), fd, op);
//    auto result = pHook_flock(fd, op);
//    logi("proc: %d, before flock fd: %d, op: %d", getpid(), fd, op);
//    sleep(3);
//    return result;
//}
//
//DefineHookStub(__openat, int64_t, int a1, const char *a2, int a3) {
//    logi("open %s", a2);
//    return pHook___openat(a1, a2, a3);
//}
//
//DefineHookStub(__faccessat, int64_t, int a1, const char *a2, int a3, int a4) {
//    logi("faccessat %s", a2);
//    return pHook___faccessat(a1, a2, a3, a4);
//}
//
//DefineHookStub(decode_str, char*, const char *a1, int a2) {
//    auto r = pHook_decode_str(a1, a2);
//    log2file("decode_str %s -> %s", a1, r);
//    return r;
//}
//
////    InlineHookSymbol("libc.so", waitpid, Hook_waitpid);
////    InlineHookSymbol("libc.so", flock, Hook_flock);
////    InlineHookSymbol("libc.so", fork, Hook_fork);
//
//#include <cstdio>
////int remove(const char *pathname);
//DefineHookStub(remove, int, const char *pathname) {
//    logi("remove %s", pathname);
//    rename(pathname, (string(pathname) + "_delete").c_str());
//    return 0;
//}
//
//DefineHookStub(_exit, void, int) {
//    logi("exit %d %s", getpid(), stack2str(GetStack01()).c_str());
//    pHook__exit(0);
//}
//
//DefineHookStub(_Exit, void, int) {
//    logi("_Exit %d %s", getpid(), stack2str(GetStack01()).c_str());
//    pHook__Exit(0);
//}
//
//DefineHookStub(syscall, uint64_t, uint64_t call, ...) {
//    va_list args;
//    va_start(args, call);
//    uint64_t result = pHook_syscall(call, args);
//    va_end(args);
//    if (call != 98) {
//        logi("syscall %d %d %s", call, result, stack2str(GetStack01()).c_str());
//    }
//    return result;
//}