#include <jni.h>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/mman.h>

#include "third/utils/linux_helper.h"
#include "third/utils/jni_helper.hpp"
#include "third/utils/log.h"
#include "third/json/json.h"
#include "third/dobby/include/dobby.h"
#include "base/when_hook.h"
#include "base/hook.h"
#include "global/global.h"

#include "dump_so.h"

using namespace std;

void *get_start(const vector<MapsInfo> &maps, const string &name) {
    void *start = (void *) -1;
    for (const auto &item: maps) {
        if (item.path.find(name) == string::npos) {
            continue;
        }
        if ((uint64_t) start > (uint64_t) item.region_start) {
            start = item.region_start;
        }
    }
    return start;
}

void *get_start(const string &name) {
    auto maps = get_process_maps(name);
    if (maps.size() == 0) {
        LOGI("open maps error!");
        return nullptr;
    }
    return get_start(maps, name);
}

bool dump_so(const string &libName, const string &save_path_dir) {
    LOGI("start dump_so %s to %s", libName.c_str(), save_path_dir.c_str());
    static int idx = 0;
    auto maps = get_process_maps(libName);
    if (maps.size() == 0) {
        LOGI("dump_so open maps error!");
        return false;
    }

    ofstream ofs;
    ofs.open(xbyl::format_string("%s/%d_%d_%p_%s",
                                 save_path_dir.c_str(),
                                 idx++,
                                 getpid(),
                                 get_module_base(maps, libName),
                                 libName.c_str()),
             ios::out | ios::binary);
    if (!ofs.is_open()) {
        LOGI("dump_so open write file error!");
        return false;
    }

    for (const auto &item: maps) {
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

bool dump_so_delay_after_loaded(const string &targetLibName, int sleepTime) {
    return WhenSoInitHook(targetLibName,
                          [=](const string &path, void *addr, const string &funcType) {
                              logi("dump_so_delay on callback");
                              auto *thd = new thread([&]() {
                                  sleep(sleepTime);
                                  dump_so(targetLibName,
                                          "/data/data/" + getPkgName());
                              });
                              thd->join();
                          });
}

int suspend_threads() {
    pid_t my_pid = getpid(); // 当前进程 ID
    pid_t my_tid = gettid(); // 当前线程 ID
    char path[256];
    DIR *dir;
    struct dirent *entry;

    // 打开 /proc/[pid]/task 目录
    snprintf(path, sizeof(path), "/proc/%d/task", my_pid);
    dir = opendir(path);
    if (!dir) {
        loge("Failed to open %s: %s\n", path, strerror(errno));
        return -1;
    }

    while ((entry = readdir(dir))) {
        if (entry->d_type != DT_DIR || strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        pid_t tid = atoi(entry->d_name);
        if (tid == my_tid)
            continue;

        if (pthread_kill(tid, SIGSTOP) == -1) {
            loge("Failed to suspend thread %d: %s\n", tid, strerror(errno));
            continue;
        }
        logi("Suspended thread %d\n", tid);
    }

    closedir(dir);
    return 0;
}

bool dump_so_delay(const string &targetLibName, int sleepTime) {
    logi("dump_so_delay on callback");
    (new thread([=]() {
        sleep(sleepTime);
//        suspend_threads();
        dump_so(targetLibName,
                "/data/data/" + getPkgName());
    }))->detach();
    return true;
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

static callback mprotect_cb;

DefineHookStub(mprotect, int, void *addr, size_t size, int prot) {
    string logs;
    Dl_info info{};
    if (dladdr(addr, &info) != 0) {
        logs = xbyl::format_string("mprotect %p:%s, ",
                                   (void *) ((uint64_t) addr - (uint64_t) info.dli_fbase),
                                   info.dli_fname);
    } else {
        logs = xbyl::format_string("mprotect %p, ", addr);
    }
    logs += xbyl::format_string("size %d, prot: %d,  ", size, prot);
    auto stack = GetStackInfo();
    for (const auto &item: stack) {
        logs += xbyl::format_string("%s:%p,", item.name.c_str(), item.offset);
    }
    logi(logs.c_str());
    mprotect_cb((void *) ((uint64_t) addr - (uint64_t) info.dli_fbase), size, prot,
                info.dli_fname == nullptr ? "" : info.dli_fname);
    return pHook_mprotect(addr, size, prot);
}

void dump_mprotect(callback cb) {
    mprotect_cb = cb;
    auto target = DobbySymbolResolver("libc.so", "mprotect");
    DobbyHook(target, (dobby_dummy_func_t) &Hook_mprotect,
              (dobby_dummy_func_t *) &pHook_mprotect);
}
