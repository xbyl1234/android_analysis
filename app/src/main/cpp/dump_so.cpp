#include <jni.h>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <regex>
#include "third/utils/linux_helper.h"
#include "third/utils/jni_helper.hpp"
#include "third/utils/log.h"
#include "third/json/json.h"
#include "third/dobby/include/dobby.h"
#include "base/when_hook.h"
#include "base/hook.h"
#include "global/global.h"
using namespace std;

void *get_start(const vector<MapsInfo> &maps, const string &name) {
    void *start = (void *) -1;
    regex regLibPath(name);
    for (const auto &item: maps) {
//        LOGI("%s %d", item.path.c_str(), regex_search(item.path, regLibPath));
        if (!regex_search(item.path, regLibPath)) {
            continue;
        }
        if ((uint64_t) start > (uint64_t) item.region_start) {
            start = item.region_start;
        }
    }
    return start;
}

void *get_start(const string &name) {
    MapsHelper maps;
    if (maps.refresh(name) == 0) {
        LOGI("open maps error!");
        return nullptr;
    }
    return get_start(maps.mapsInfo, name);
}

bool dump_so(const string &libName, const string &save_path_dir) {
    LOGI("start dump_so %s to %s", libName.c_str(), save_path_dir.c_str());
    MapsHelper maps;
    if (maps.refresh(libName) == 0) {
        LOGI("dump_so open maps error!");
        return false;
    }

    ofstream ofs;
    ofs.open(format_string("%s/%d_%p_%s",
                           save_path_dir.c_str(),
                           getpid(),
                           maps.get_module_base(libName),
                           libName.c_str()),
             ios::out | ios::binary);
    if (!ofs.is_open()) {
        LOGI("dump_so open write file error!");
        return false;
    }

    for (const auto &item: maps.mapsInfo) {
        LOGI("dump_so write offset: %p, start: %p, end: %p, perm: %s, path: %s", item.region_offset,
             item.region_start, item.region_end, item.permissions.c_str(), item.path.c_str());

        ofs.seekp((uint64_t) item.region_offset);
        ofs.write((char *) item.region_start,
                  (uint64_t) item.region_end - (uint64_t) item.region_start);
    }

    ofs.close();

    return true;
}

static bool hadDump = false;

jint (*org_RegisterNatives)(JNIEnv *env, jclass java_class, const JNINativeMethod *methods,
                            jint method_count);

static jint
RegisterNatives(JNIEnv *env, jclass java_class, const JNINativeMethod *methods, jint method_count) {
    if (!hadDump) {
        for (int i = 0; i < method_count; ++i) {
            if (strstr(methods[i].name, "IiIiiIiIiI") == nullptr) {
                auto *thd = new thread([]() {
                    sleep(6);
                    dump_so("libcompatible.so",
                            "/data/data/com.com2usholdings.heiroflight2.android.google.global.normal");
                });
                hadDump = true;
                break;
            }
        }
    }
    return org_RegisterNatives(env, java_class, methods, method_count);
}

bool dump_so_when_register_natives() {
    auto target = DobbySymbolResolver("libart.so",
                                      "_ZN3art3JNI15RegisterNativesEP7_JNIEnvP7_jclassPK15JNINativeMethodi");
    DobbyHook(target, (dobby_dummy_func_t) &RegisterNatives,
              (dobby_dummy_func_t *) &org_RegisterNatives);
    return true;
}

bool dump_so_when_init(const string &targetLibName) {
    return WhenSoInitHook(targetLibName,
                          [=](const string &path, void *addr, const string &funcType) {
                              dump_so(targetLibName, "/data/data/" + getPkgName());
                          });
}

bool dump_so_delay(const string &targetLibName, int sleepTime) {
    return WhenSoInitHook(targetLibName,
                          [=](const string &path, void *addr, const string &funcType) {
                              auto *thd = new thread([&]() {
                                  sleep(sleepTime);
                                  dump_so(targetLibName,
                                          "/data/data/" + getPkgName());
                              });
                          });
}


static string gDumpSoName;

DefineHookStub(JNI_OnLoad, jint, JavaVM *vm, void *reserved) {
    auto ret = pHook_JNI_OnLoad(vm, reserved);
    dump_so(gDumpSoName, "/data/data/" + getPkgName());
    return ret;
}

bool dump_so_when_after_jni_load(const string &targetLibName) {
    return WhenSoInitHook(targetLibName,
                          [&](const string &path, void *addr, const string &funcType) {

                          });
}