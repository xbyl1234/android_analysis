#include <mutex>
#include "../third/utils/macro_helper.h"
#include "../third/utils/log.h"
#include "../third/byopen/hack_dlopen.h"
#include "hook.h"
#include "when_hook.h"

using namespace std;

static bool hadInit = false;
static vector<pair<string, WhenHookCallBack>> callBack;
static mutex callBackLock;

void callCallBack(const char *filename, const vector<pair<string, WhenHookCallBack>> &cbs) {
    for (const auto &item: cbs) {
        if (strstr(filename, item.first.c_str())) {
            logi("loaded %s", filename);
            item.second(filename, nullptr, "");
        }
    }
}

DefineHookStub(android_dlopen_ext, void *, const char *filename, int flag, const void *extinfo) {
    void *ret = pHook_android_dlopen_ext(filename, flag, extinfo);
    callCallBack(filename, callBack);
    return ret;
}

void WhenSoLoaded(const string &libName, WhenHookCallBack _callBack) {
    callBackLock.lock();
    if (!hadInit) {
        InlineHookSymbol("libc.so", android_dlopen_ext, Hook_android_dlopen_ext);
        hadInit = true;
    }
    callBack.push_back(make_pair(libName, _callBack));
    callBackLock.unlock();
}