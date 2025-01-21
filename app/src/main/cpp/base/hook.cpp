#include <jni.h>
#include <dlfcn.h>
#include <fstream>
#include <mutex>
#include <asm-generic/mman.h>

#include "../third/utils/log.h"
#include "../third/utils/utils.h"
#include "../third/utils/jni_helper.hpp"

#include "hook.h"

#define HOOKLOG2FILE

using namespace std;
static char *retOffsetBase = nullptr;


void setLogRetOffset(void *offset) {
    retOffsetBase = (char *) offset;
}

void
_HookLogWitchLibWithStack(const string &libName, const string &funcName, void *ret, const char *fmt,
                          ...) {
    string logPerf;
    if (!libName.empty()) {
        Dl_info info{};
        if (dladdr(ret, &info) != 0 && string(info.dli_fname).find(libName) == -1) {
            return;
        }
    }

    logPerf += format_string("func %s, ret: %p: ",
                             funcName.c_str(),
                             ((char *) ret) - retOffsetBase);
    va_list ap;
    va_start(ap, fmt);
    string log = logPerf + format_string(fmt, ap);

#ifdef HOOKLOG2FILE
    log2file(log);
#else
    LOGI("%s", log.c_str());
#endif
}

bool _resolve(SymbolInfo *item, const char *symbol, void *addr) {
    if (item->isReg && !regex_search(symbol, regex(item->sym))) {
        return false;
    }
    if (item->sym != symbol) {
        return false;
    }
    item->target = addr;
    return true;
}

//bool _resolve(SymbolInfo &item, const char *symbol, void *addr) {
//    DobbySymbolResolver(libName.c_str(), targetSymbol.c_str());
//    hack_dlsym(targetHandle, this->targetSymbol.c_str());
//}

bool resolve(fake_dlctx_ref_t handle, vector<SymbolInfo*> *symbols) {
    struct Ctx {
        vector<SymbolInfo*> *symbols;
        int count = 0;
    } ctx;
    ctx.symbols = symbols;

    hack_enum_dlsym(
            handle, [](const char *symbol, void *addr, void *ctx) -> bool {
                Ctx *c = (Ctx *) ctx;
                for (auto &item: *c->symbols) {
                    if (_resolve(item, symbol, addr)) {
                        logi("resolved %s", item->sym.c_str());
                        c->count++;
                        break;
                    }
                }
                return c->count != c->symbols->size();
            },
            &ctx);
    return ctx.count == symbols->size();
}

bool hookAll(vector<SymbolInfo> *symbols) {
    for (auto item: *symbols) {
        auto result = DobbyHook(item.target, (dobby_dummy_func_t) item.stub,
                                (dobby_dummy_func_t *) item.org);
        if (result == RS_FAILED) {
            loge("hook error %s target: %p", item.sym.c_str(), item.target);
            return false;
        } else {
            logi("hook jni: %s", item.sym.c_str());
        }
    }
    return true;
}


string stack2str(const vector<Stack> &stack) {
    string result;
    for (const Stack &item: stack) {
        result += format_string("%s:%p", item.name.c_str(), item.offset);
    }
    return result;
}
