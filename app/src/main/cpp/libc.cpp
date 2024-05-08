#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <jni.h>

#include "third/utils/log.h"
#include "third/utils/macro_helper.h"
#include "third/utils/linux_helper.h"

#include "base/hook.h"
#include "base/when_hook.h"
#include "bionic.h"

//vector<HookBaseInfo *> LibcHooks;
//
//#define DefineLogHook(Lib, Func, Ret, ...)  _DefineHook(&LibcHooks,Lib, #Func,Func,  Ret, ##__VA_ARGS__)
//#define DefaultLogTargetLib ""
//
//#define HookLogWitchLib(LibName, FuncName, FMT, ...)  _HookLogWitchLibWithStack(LibName,FuncName,__builtin_return_address(0),FMT,##__VA_ARGS__)
//#define HookLogWithDefaultLib(FuncName, FMT, ...) HookLogWitchLib(DefaultLogTargetLib,FuncName, FMT,##__VA_ARGS__)
//#define HookLog(FMT, ...) HookLogWitchLib("",__FUNCTION__, FMT, ##__VA_ARGS__)
//
//
//void *selfModuleStart;
//void *selfModuleEnd;
//void *targetModuleStart;
//void *targetModuleEnd;
//
//#define InSelfModule (GetStack(0) >= selfModuleStart && GetStack(0) <= selfModuleEnd)
//#define InNotTargetModule !(GetStack(0) >= targetModuleStart && GetStack(0) <= targetModuleEnd)
//#define ShouldPassLog (InSelfModule||InNotTargetModule)
//
//#if IsBit64
//
//DefineLogHook("libc.so", __statfs, int, const char *path, struct statfs *buf) {
//    if (ShouldPassLog) {
//        return hookInfo___statfs(path, buf);
//    }
//    HookLogWithDefaultLib("statfs", "%s", path);
//    return hookInfo___statfs(path, buf);
//}
//
//DefineLogHook("libc.so", __fstatfs, int, int fd, struct statfs *buf) {
//    if (ShouldPassLog) {
//        return hookInfo___fstatfs(fd, buf);
//    }
//    HookLogWithDefaultLib("fstatfs", "%s", Fd2Path(fd).c_str());
//    return hookInfo___fstatfs(fd, buf);
//}
//
//#else
////InlineHook("libc.so", statfs, __statfs64);
////InlineHook("libc.so", fstatfs, __fstatfs64);
//#endif
//
//DefineLogHook("libc.so", popen, FILE*, const char *command, const char *mode) {
//    if (ShouldPassLog) {
//        return hookInfo_popen(command, mode);
//    }
//    HookLogWithDefaultLib("popen", "%s", command);
//    return hookInfo_popen(command, mode);
//}
//
//DefineLogHook("libc.so", faccessat, int, int dirfd, const char *path, int mode, int flags) {
//    if (ShouldPassLog) {
//        return hookInfo_faccessat(dirfd, path, mode, flags);
//    }
//    HookLogWithDefaultLib("faccessat", "%s", path);
//    return hookInfo_faccessat(dirfd, path, mode, flags);
//}
//
//DefineLogHook("libc.so", __openat, int, int fd, const char *pathname, int flags, mode_t modes) {
//    if (ShouldPassLog) {
//        return hookInfo___openat(fd, pathname, flags, modes);
//    }
//    HookLogWithDefaultLib("open", "%s", pathname);
//    return hookInfo___openat(fd, pathname, flags, modes);
//}
//
//DefineLogHook("libc.so", strlen, size_t, const char *s) {
//    if (ShouldPassLog) {
//        return xbyl::strlen(s);
//    }
//    HookLogWithDefaultLib("strlen", "%s", s);
//    return xbyl::strlen(s);
//}
//
//DefineLogHook("libc.so", strcpy, char *, char *dest, char *src) {
//    if (ShouldPassLog) {
//        return xbyl::strcpy(dest, src);
//    }
////    HookLogWithDefaultLib("strcpy", "%s", src);
//    return xbyl::strcpy(dest, src);
//}
//
////DefineLogHook("libc.so", free, int, char *dest, char *src) {
////    if (ShouldPassLog) {
////        return hookInfo_free(dest, src);
////    }
////    HookLogWithDefaultLib("free", "%s", src);
////    return hookInfo_free(dest, src);
////}
//
//const string selfModuleName = "libanalyse.so";
//const string targetModuleName = "libunity.so";
//
//void InitLibcHook() {
//    WhenSoInitHook(targetModuleName, [](const string &path, void *addr, const string &funcType) {
//        logi("InitLibcHook on load!");
//        MapsHelper mapsHelper;
//        mapsHelper.refresh();
//        selfModuleStart = mapsHelper.get_module_base(selfModuleName);
//        selfModuleEnd = mapsHelper.get_module_end(selfModuleName);
//        targetModuleStart = mapsHelper.get_module_base(targetModuleName);
//        targetModuleEnd = mapsHelper.get_module_end(targetModuleName);
//        if (selfModuleStart == nullptr ||
//            selfModuleEnd == nullptr ||
//            targetModuleStart == nullptr ||
//            targetModuleEnd == nullptr) {
//            logi("InitLibcHook get module error!");
//            return;
//        }
//
//        setLogRetOffset(targetModuleStart);
//        for (HookBaseInfo *item: LibcHooks) {
//            LOGI("analyse start hook: %s , is success: %d", item->targetSymbol.c_str(),
//                 item->hook());
//        }
//    });
//}