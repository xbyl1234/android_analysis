#include <jni.h>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <sys/mman.h>
#include <dlfcn.h>
#include "../third/utils/linux_helper.h"
#include "../third/utils/jni_helper.hpp"
#include "../third/utils/log.h"
#include "../third/json/json.h"
#include "dobby.h"
#include "../base/when_hook.h"
#include "../base/hook.h"
#include "../dump_so.h"

#define HookLogWitchLib(LibName, FuncName, FMT, ...)  _HookLogWitchLibWithStack(LibName,FuncName,__builtin_return_address(0),FMT,##__VA_ARGS__)
#define HookLogWithDefaultLib(FuncName, FMT, ...) HookLogWitchLib(DefaultLogTargetLib,FuncName, FMT,##__VA_ARGS__)
#define HookLog(FMT, ...) HookLogWitchLib("",__FUNCTION__, FMT, ##__VA_ARGS__)


using namespace std;

void *get_start(const string &name);

void *get_start(const vector<MapsInfo> &maps, const string &name);

#define TargetLibName   "libcompatible.so"

extern char *targetLibBase ;

void *(*org_decode_code)(void *DecodeCode, char *source, char *target, int len, int keyLen);

void *hook_decode_code(void *DecodeCode, char *source, char *target, int len, int keyLen) {
    std::mutex lock;
    lock_guard<mutex> guard(lock);
    LOGI("on decode: source: %p, target: %p, len: %d", source, target, len);
    int buffLen = len * 2 + 1;
    char *buff = new char[buffLen];
    Json::Value data;
    hex2str(source, len, buff, buffLen);
    data["type"] = keyLen;
    data["source"] = buff;
    data["source_addr"] = ((uint64_t) source - (uint64_t) targetLibBase);
    data["target_addr"] = ((uint64_t) target - (uint64_t) targetLibBase);
    data["ret_addr"] = xbyl::format_string("%p", ((uint64_t) __builtin_return_address(0) -
                                            (uint64_t) targetLibBase));

    auto result = org_decode_code(DecodeCode, source, target, len, keyLen);

    hex2str(target, len, buff, buffLen);
    data["target"] = buff;

    log2file(Json2String(data));
    delete buff;
    return result;
}

//__int64 sprintf_0(unsigned __int8 *, unsigned __int8 *, ...);;

void (*org_decode_more)(char *a1, char *a2, char *a3);

void decode_more(char *a1, char *a2, char *a3) {
    org_decode_more(a1, a2, a3);
    char *buff = new char[65];
    hex2str((char *) (a1 + 516), 32, buff, 65);
    HookLog(buff);
    delete buff;
//    LOGI((char *) (a1 + 516));
}

char *(*org_get_str)(char *a1);

char *get_str(char *a1) {
    auto ret = org_get_str(a1);
    char *buff = new char[65];
    hex2str(ret, 32, buff, 65);
    HookLog(buff);
    delete buff;
//    HookLog("%s", ret);
    return ret;
}


bool hook_log_when_init() {
    return WhenSoInitHook(TargetLibName,
                          [&](const string &path, void *addr, const string &funcType) {
                              targetLibBase = (char *) get_start(TargetLibName);
                              setLogRetOffset(targetLibBase);
                              LOGI("will hook decode code, base: %p", targetLibBase);

                              void *decode_code = ((char *) targetLibBase) + 0x198294;
//                              DobbyHook(decode_code, (dobby_dummy_func_t) hook_decode_code,
//                                        (dobby_dummy_func_t *) &org_decode_code);
                              DobbyHook(targetLibBase + 0x01951C0, (dobby_dummy_func_t) decode_more,
                                        (dobby_dummy_func_t *) &org_decode_more);
//                              DobbyHook(targetLibBase + 0x1959C4, (dobby_dummy_func_t) get_str,
//                                        (dobby_dummy_func_t *) &org_get_str);

//                              void *sprintf1 = ((char *) targetLibBase) + 0x0A53C0;
//                              DobbyHook(sprintf1, (dobby_dummy_func_t) hook_sprinf,
//                                        (dobby_dummy_func_t *) &org_hook_sprinf);
//
//                              void *sprintf2 = ((char *) targetLibBase) + 0x1949A0;
//                              DobbyHook(sprintf2, (dobby_dummy_func_t) hook_safe_sprinf,
//                                        (dobby_dummy_func_t *) &org_hook_sprinf2);

                          });
}

bool dump_so_when_decode() {
    static bool hadDump = false;
    return WhenSoInitHook(TargetLibName,
                          [&](const string &path, void *addr, const string &funcType) {
                              targetLibBase = (char *) get_start(TargetLibName);
//                              void *targetAddr = ((char *) targetLibBase) + 0x000C474;  // 0D1A4
//                              void *targetAddr = ((char *) targetLibBase) + 0x000C850;  //1987A8
//                              void *targetAddr = ((char *) targetLibBase) + 0x000CC5C;  //E3E8
//                              void *targetAddr = ((char *) targetLibBase) + 0xE32DC;  //0xf04ec
//                              void *targetAddr = ((char *) targetLibBase) + 0x185B98;
                              void *targetAddr = ((char *) targetLibBase) + 0xD9458;
                              LOGI("will hook decode code, base: %p", targetLibBase);
                              DobbyInstrument(targetAddr,
                                              [](void *address, DobbyRegisterContext *ctx) {
                                                  if (!hadDump) {
                                                      hadDump = true;
                                                      dump_so("libcompatible.so",
                                                              "/data/data/com.com2usholdings.heiroflight2.android.google.global.normal");
                                                  }
                                              });
                          });
}


uint64_t (*org_check_su)();

uint64_t check_su() {
    LOGI("on hook check su be: %d", *((uint64_t *) ((char *) targetLibBase + 0x216A48)));
    uint64_t ret = org_check_su();
    LOGI("on hook check su af: %d, %d", *((uint64_t *) ((char *) targetLibBase + 0x216A48)), ret);
    return ret;
}


uint8_t nop[] = {0x1F, 0x20, 3, 0xD5,};

bool hook_sp_when_decode() {
    static bool hadHook = false;
    auto when_decode_code = [](void *address, DobbyRegisterContext *ctx) {
        LOGI("%p", ((uint64_t) ctx->lr - (uint64_t) targetLibBase));
        if (!hadHook) {
            hadHook = true;
            LOGI("on hook when_decode_code");
            LOGI("0x20DC68  %p", **(void ***) (0x20DC68 + targetLibBase + 20 * 8));


//            DobbyCodePatch(targetLibBase + 0x00FF78, nop, 4);
//            DobbyInstrument(targetLibBase + 0xFF84, [](void *address, DobbyRegisterContext *ctx) {
////            DobbyInstrument(targetLibBase + 0x0FF78, [](void *address, DobbyRegisterContext *ctx) {
//                LOGI("0xFF84 call %p", (uint64_t) ctx->general.regs.x8);
////                LOGI("x10 %d", ctx->general.regs.x10);
//            });
        }

//        DobbyInstrument(
//                ((char *) targetLibBase) + 0x00FE94,
//                [](void *address, DobbyRegisterContext *ctx) {
//                  void *chsu = ((char *) targetLibBase) + 0x005BCBC;
//                  void *chsu = ((char *) targetLibBase) + 0x0013C60;
//                  if (0x79014c1d4190eef4 == *((uint64_t *) chsu)) {
//                                       return;
//                                   }
//                    LOGI("hook check su: %p", *((uint64_t *) chsu));
//                    DobbyHook(chsu,
//                              (dobby_dummy_func_t) check_su,
//                              (dobby_dummy_func_t *) &org_check_su);
//                });
    };

    auto when_init_proc = [&](const string &path, void *addr, const string &funcType) {
        targetLibBase = (char *) get_start(TargetLibName);
        setLogRetOffset(targetLibBase);
//      void *targetAddr = ((char *) targetLibBase) + 0x000C474;  // 0xD1A4
//      void *targetAddr = ((char *) targetLibBase) + 0x000C850;  // 0x1987A8
//        void *targetAddr = ((char *) targetLibBase) + 0x000CC5C;  // 0xE3E8
        void *targetAddr = ((char *) targetLibBase) + 0x0000CC60;
//        void *targetAddr = ((char *) targetLibBase) + 0xDC17C;  // make_svc_other_D3474
        LOGI("will hook decode code, base: %p", targetLibBase);

        DobbyInstrument(targetAddr, when_decode_code);
//        DobbyInstrument(targetLibBase + 0x1959C4, [](void *address, DobbyRegisterContext *ctx) {
//            char *buff = new char[65];
//            hex2str((char *) ctx->general.regs.x0, 32, buff, 65);
//            HookLog(buff);
//            delete buff;
//            HookLog("%s", ctx->general.regs.x0);
//        });
    };

    return WhenSoInitHook(TargetLibName, when_init_proc);
}

void watch_thread() {
    int *magisk = (int *) (targetLibBase + 0x216A48);
    char **some_flags = (char **) (targetLibBase + 0x20D840);
    char **pcheck_status_flags = (char **) (targetLibBase + 0x20DA78);
    char **xposed_flags2 = (char **) (targetLibBase + 0x20DC10);
    int **xposed_flags1 = (int **) (targetLibBase + 0x20E020);
    char *frida_flag = (char *) (targetLibBase + 0x20DA28);
    char **call_addr = (char **) (targetLibBase + 0x213AC8);
    char *code = (char *) (targetLibBase + 0x5D028);

//    uint64_t *qword_214EF8 = (uint64_t *) (targetLibBase + 0x214EF8);
//    uint64_t *qword_20DBB0 = (uint64_t *) (targetLibBase + 0x20DBB0);
//        LOGI("0xFF84 call: %p",
//             *qword_214EF8 + *(uint64_t *) (*qword_20DBB0 & 0xFFFFFFFFFFFFFFFELL));

    while (true) {
        if (*magisk != 0) {
            LOGI("magisk: %d", *magisk);
//            *magisk = 0;
        }
        if (*frida_flag != 0) {
            LOGI("frida_flag: %d", *frida_flag);
//            *frida_flag = 23;
        }
        if (*some_flags != nullptr &&
            **some_flags != 0) {
            LOGI("some_flags: %d", **some_flags);
//            **some_flags = 0;
        }
        if (*pcheck_status_flags != nullptr &&
            **pcheck_status_flags != 0) {
            LOGI("pcheck_status_flags: %d", **pcheck_status_flags);
        }
        if (*xposed_flags2 != nullptr &&
            **xposed_flags2 != 0) {
            LOGI("xposed_flags2: %d", **xposed_flags2);
        }
        if (*xposed_flags1 != nullptr &&
            **xposed_flags1 != 0) {
            LOGI("xposed_flags1: %d", **xposed_flags1);
        }
        if (*call_addr != nullptr) {
            if (*(char **) (*call_addr + 232) != nullptr) {
                LOGI("call_addr 232: %p", *(char **) (*call_addr + 232));
            }
        }
        sleep(1);
    }
}

void watch_value() {
    WhenSoInitHook(TargetLibName, [&](const string &path, void *addr, const string &funcType) {
        targetLibBase = (char *) get_start(TargetLibName);
        LOGI("will watch_value base: %p", targetLibBase);
        thread *mytobj = new thread(watch_thread);
    });
}

void *(*org_get_map_info)(void *maps, void *out);

void *get_map_info(void *maps, void *out) {

    auto ret = org_get_map_info(maps, out);

    HookLog("maps: %p , %s", *((void **) out), (char *) (((char *) out) + 0x39));

    return ret;
}

void *(*org_sub_180AA4)(void *a1);

void *sub_180AA4(void *a1) {
    HookLog("end check !");
    return org_sub_180AA4(a1);
}

int (*org_check_xposed_bridge_jar)();

int check_xposed_bridge_jar() {
    int ret = org_check_xposed_bridge_jar();
    LOGI("check_xposed_bridge_jar: %d", ret);
    return ret;
}

void (*org_check_swtich_errro)(uint64_t result, uint64_t a2);

void check_swtich_errro(uint64_t result, uint64_t a2) {
    LOGI("check_swtich_errro: %d", result);
    return;
}

int (*org_check_some_art)();

int check_some_art() {
    int ret = org_check_some_art();
    int **some_flag = (int **) (targetLibBase + 0x20D810);
    if (*some_flag != nullptr) {
        LOGI("check_some_art: 0x20D810 %d", **some_flag);
    }
    LOGI("check_some_art ret: %d", ret);
    return ret;
}

int (*org_check_art)(void *a1);

int check_art(void *a1) {
    int ret = org_check_art(a1);
    LOGI("check_art: %d", ret);
    return ret;
}

bool SvcReadFile(const string &path, char **buffer, int *size) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        return false;
    }
    int cur = 0;
    int _size = 1024;
    *buffer = (char *) malloc(_size + 1);
    int readSize;
    while ((readSize = read(fd, &(*buffer)[cur], _size - cur)) > 0) {
        cur += readSize;
        if (cur >= _size) {
            _size *= 2;
            *buffer = (char *) realloc(*buffer, _size + 1);
        }
    }
    close(fd);
    *size = cur;
    (*buffer)[cur] = 0;
    return true;
}

int (*org_call_some_check)();

int call_some_check() {
    char *buff;
    int size;
    SvcReadFile("/proc/self/mounts", &buff, &size);
    auto sp = string_split(buff, "\n");
    for (auto &item: sp) {
        LOGI("mount:  %s", item.c_str());
    }

    int *magisk = (int *) (targetLibBase + 0x216A48);
    LOGI("call_some_check before magisk: %d", *magisk);
    int ret = org_call_some_check();
    LOGI("call_some_check ret %d, magisk: %d", ret, *magisk);
    *magisk = 0;
    return ret;
}

string which_module(void *addr) {
    MapsHelper maps;
    maps.refresh();
    for (auto &item: maps.mapsInfo) {
        if (addr >= item.region_start && addr < item.region_end) {
            return item.path;
        }
    }
    return "";
}


void magisk_check() {
//    static int fileName =
    WhenSoInitHook(TargetLibName, [&](const string &path, void *addr, const string &funcType) {
        targetLibBase = (char *) get_start(TargetLibName);
        setLogRetOffset(targetLibBase);
        LOGI("magisk_check");

        thread *mytobj = new thread(watch_thread);
//        DobbyHook(targetLibBase + 0x19396C, (dobby_dummy_func_t) check_xposed_bridge_jar,
//                  (dobby_dummy_func_t *) &org_check_xposed_bridge_jar);
        DobbyHook(targetLibBase + 0x190484, (dobby_dummy_func_t) check_swtich_errro,
                  (dobby_dummy_func_t *) &org_check_swtich_errro);
        DobbyHook(targetLibBase + 0x193420, (dobby_dummy_func_t) check_some_art,
                  (dobby_dummy_func_t *) &org_check_some_art);
        DobbyHook(targetLibBase + 0xEB608, (dobby_dummy_func_t) check_art,
                  (dobby_dummy_func_t *) &org_check_art);
        DobbyHook(targetLibBase + 0x13C60, (dobby_dummy_func_t) call_some_check,
                  (dobby_dummy_func_t *) &org_call_some_check);

//        DobbyHook(targetLibBase + 0x180BAC, (dobby_dummy_func_t) get_map_info,
//                  (dobby_dummy_func_t *) &org_get_map_info);

//        DobbyHook(targetLibBase + 0x1959C4, (dobby_dummy_func_t) sub_180AA4,
//                  (dobby_dummy_func_t *) &org_sub_180AA4);

//        7ff4f55000-7ff5754000
//        DobbyInstrument(targetLibBase + 0x180AA4, [](void *address, DobbyRegisterContext *ctx) {
////            HookLog("end check %d", ctx->general.regs.x23);
//            if (ctx->general.regs.x23 == 512) {
//                string appPath = "/data/data/com.com2usholdings.heiroflight2.android.google.global.normal/stack";
//                ofstream ofs;
//                ofs.open(appPath, ios::out | ios::binary);
//                if (!ofs.is_open()) {
//                    LOGI("dump_so open write file error!");
//                    return;
//                }
//                ofs.write((char *) 0x7ff4f55000, 0x7ff5754000 - 0x7ff4f55000);
//                ofs.close();
//                LOGI("dump_so !");
//            }
//        });


//        DobbyInstrument(targetLibBase + 0x19364C, [](void *address, DobbyRegisterContext *ctx) {
//            LOGI("which_module %s", which_module((void *) ctx->general.regs.x8).c_str());
//        });

//        DobbyInstrument(targetLibBase + 0x1936D4, [](void *address, DobbyRegisterContext *ctx) {
////            JNIEnv *env = (JNIEnv *) ctx->general.regs.x1;
////            void **pfunc = (void **) &(env)->functions->reserved0;
//            void **pfunc = (void **)ctx->general.regs.x25;
//            LOGI("jni  %p ", pfunc);
//            for (int i = 0; i < sizeof(JNINativeInterface) / sizeof(void *); i++) {
//                LOGI("jni env func index: %d, %p ", i, pfunc[i]);
//            }
//        });

    });
}


