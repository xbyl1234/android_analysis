#pragma once

#include <iomanip>
#include <sstream>
#include <list>
#include <sys/mman.h>

#include "../third/log2file/app_file_writer.h"
#include "../third/dobby/include/dobby.h"
#include "../third/utils/linux_helper.h"
#include "../third/utils/log.h"

#include "QBDI/Memory.hpp"
#include "QBDI/Logs.h"
#include "QBDI/Version.h"
#include "QBDI/VM.h"

using namespace QBDI;


string getPkgName();

VMAction showMemoryAccess(VM *vm, GPRState *gprState, FPRState *fprState, void *data);

VMAction showPreInstruction(VM *vm, GPRState *gprState, FPRState *fprState, void *data);

VMAction showPostInstruction(VM *vm, GPRState *gprState, FPRState *fprState, void *data);

VMAction traceCallPath(VM *vm, GPRState *gprState, FPRState *fprState, void *data);
struct Symbol {
    bool success;
    uint64_t offset;
    string moduleName;
    string symbolName;
};

enum TraceType {
    Mem = 0b0001,
    Code = 0b0010,
    Call = 0b0100
};
class MotherTrace {
    friend VMAction showMemoryAccess(VM *vm, GPRState *gprState, FPRState *fprState, void *data);

    friend VMAction showPreInstruction(VM *vm, GPRState *gprState, FPRState *fprState, void *data);

    friend VMAction showPostInstruction(VM *vm, GPRState *gprState, FPRState *fprState, void *data);
    friend VMAction traceCallPath(VM *vm, GPRState *gprState, FPRState *fprState, void *data);

public:
    MotherTrace() {
        writer.open("/data/data/" + getPkgName(), "trace");
    }

    bool enableMemoryTrace() {
        if (!vm.recordMemoryAccess(MEMORY_READ_WRITE)) {
            loge("recordMemoryAccess error!");
            return false;
        }

        uint32_t cid = vm.addMemAccessCB(MEMORY_READ_WRITE, showMemoryAccess, this);
        if (cid == INVALID_EVENTID) {
            loge("enableMemoryTrace error!");
        }
        return cid != INVALID_EVENTID;
    }

    bool enableCodeTrace() {
        uint32_t cid = vm.addCodeCB(PREINST, showPreInstruction, this);
        if (cid == INVALID_EVENTID) {
            loge("enableMemoryTrace error!");
            return false;
        }
        cid = vm.addCodeCB(POSTINST, showPostInstruction, this);
        if (cid == INVALID_EVENTID) {
            loge("enableMemoryTrace error!");
            return false;
        }
        return true;
    }

    bool enableCallTrace() {
        uint32_t cid = vm.addCodeCB(PREINST, traceCallPath, this);
        if (cid == INVALID_EVENTID) {
            loge("enableMemoryTrace error!");
            return false;
        }
        return true;
    }

    void addTargetRange(void *start, void *end) {
        range.push_back({.start=(uint64_t) start, .end=(uint64_t) end});
    }

    void addTargetModule(const string &name) {
        auto maps = get_process_maps(name);
        addTargetRange(get_module_base(maps, name), get_module_end(maps, name));
    }

    bool runTraceCode(void *start, void *stop, DobbyRegisterContext *ctx, TraceType type) {
        logi("trace start: %p", start);
        maps = get_process_maps();
        if (!preRun(ctx)) {
            return false;
        }
        if (type & TraceType::Mem) {
            enableMemoryTrace();
        }
        if (type & TraceType::Code) {
        enableCodeTrace();
        }
        if (type & TraceType::Call) {
            enableCallTrace();
        }
        stopAddr = stop;
        bool ret = vm.addInstrumentedModuleFromAddr(reinterpret_cast<rword>(start));
        if (!ret) {
            loge("addInstrumentedModuleFromAddr error");
        }
        Dl_info info{};
        if (dladdr(start, &info) != 0) {
            addTargetModule(info.dli_fname);
        } else {
            loge("not find start module");
        }
        ret = vm.call(nullptr, (uint64_t) start);
        alignedFree(fakestack);
        logi("trace end");
        return ret;
    }

private:
    VM vm;
    uint8_t *fakestack;
    app_file_writer writer;
    vector<MapsInfo> maps;
    list<uint64_t> symbolCacheOrder;
    unordered_map<uint64_t, Symbol> symbolCache;

    static constexpr size_t MAX_CACHE_SIZE = 1000; // Maximum cache size
    static constexpr size_t PRUNE_SIZE = 200;
    struct Range {
        uint64_t start;
        uint64_t end;
    };

    vector<Range> range;

    bool checkInRange(uint64_t p) {
        for (const auto &item: range) {
            if (p >= item.start && p <= item.end) {
                return true;
            }
        }
        return false;
    }

    bool preRun(DobbyRegisterContext *ctx) {
        auto state = vm.getGPRState();
        for (int i = 0; i < 29; i++) {
            QBDI_GPR_SET(state, i, ctx->general.x[i]);
        }
        state->lr = ctx->lr;
        state->x29 = ctx->fp;
        state->sp = ctx->sp;

        long mem_size = 0x100000 * 1 * 1;
        if (!allocateVirtualStack(state, mem_size, &fakestack)) {
            loge("Failed to allocate virtual stack");
            return false;
        }
        return true;
    }

    bool getSymbolFromMaps(uint64_t address, string *moduleName, uint64_t *offset) {
        for (const auto &range: maps) {
            if (address >= (uint64_t) range.region_start && address < (uint64_t) range.region_end) {
                *moduleName = getShortModuleName(range.name);
                *offset = address - (uint64_t) range.region_start;
                return true;
            }
        }
        return false;
    }

    string getShortModuleName(const string &name) {
        int p = name.find_last_of("/");
        string result = p != -1 ? name.substr(p).c_str()
                                : name.c_str();
        return result.empty() ? name : result;
    }
protected:
    void *stopAddr = nullptr;

    bool findSymbol(uint64_t address, string *moduleName, string *symbolName, uint64_t *offset) {
        auto it = symbolCache.find(address);
        if (it != symbolCache.end()) {
            *moduleName = it->second.moduleName;
            *symbolName = it->second.symbolName;
            *offset = it->second.offset;
            return it->second.success;
            }
        Dl_info info{};
        bool suc = false;
        if (dladdr((void *) address, &info) != 0) {
            *offset = address - (uint64_t) info.dli_fbase;
            *moduleName = info.dli_fname ? getShortModuleName(info.dli_fname) : "unknown";
            *symbolName = info.dli_sname ? info.dli_sname : "";
            suc = true;
        }
        if (!suc || (*moduleName).empty()) {
            uint64_t _offset;
            string _moduleName;
            if (getSymbolFromMaps(address, &_moduleName, &_offset)) {
                if (!suc) {
                    *offset = _offset;
                }
                if ((*moduleName).empty()) {
                    *moduleName = _moduleName;
                }
                suc = true;
            } else {
                *moduleName = "unknown";
                *symbolName = "";
                *offset = 0;
            }
        }
        if (symbolCache.size() >= MAX_CACHE_SIZE) {
            for (size_t i = 0; i < PRUNE_SIZE && !symbolCacheOrder.empty(); ++i) {
                uint64_t oldAddress = symbolCacheOrder.back();
                symbolCacheOrder.pop_back();
                symbolCache.erase(oldAddress);
            }
        }
        symbolCacheOrder.push_front(address);
        symbolCache[address] = {
                .success=suc,
                .offset=*offset,
                .moduleName=*moduleName,
                .symbolName=*symbolName,
        };
        return suc;
    }

    void log2file(const char *fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        string result = xbyl::format_string(fmt, ap);
        va_end(ap);
        log2file(result);
    }

    void log2file(const string &log) {
        writer.write2file(log);
    }
};
