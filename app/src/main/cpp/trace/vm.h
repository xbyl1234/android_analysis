#pragma once

#include <iomanip>
#include <sstream>
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

struct Symbol {
    string name;
    int64_t offset;
};

class MotherTrace {
    friend VMAction showMemoryAccess(VM *vm, GPRState *gprState, FPRState *fprState, void *data);

    friend VMAction showPreInstruction(VM *vm, GPRState *gprState, FPRState *fprState, void *data);

    friend VMAction showPostInstruction(VM *vm, GPRState *gprState, FPRState *fprState, void *data);

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


    void addTargetRange(void *start, void *end) {
        range.push_back({.start=(uint64_t) start, .end=(uint64_t) end});
    }

    void addTargetModule(const string &name) {
        auto maps = get_process_maps(name);
        addTargetRange(get_module_base(maps, name), get_module_end(maps, name));
    }

    bool runTraceCode(void *start, void *stop, DobbyRegisterContext *ctx) {
        logi("trace start: %p", start);
        maps = get_process_maps();
        if (!preRun(ctx)) {
            return false;
        }
        enableCodeTrace();
        enableMemoryTrace();
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
    unordered_map<uint64_t, Symbol> symbolCache;

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


protected:
    void *stopAddr = nullptr;

    bool getSymbolFromCache(uint64_t address, string *name, uint64_t *offset) {
        if (symbolCache.find(address) != symbolCache.end()) {
            if (symbolCache[address].offset == -1) {
                return false;
            }
            *name = symbolCache[address].name;
            *offset = symbolCache[address].offset;
            return true;
        }
        for (const auto &range: maps) {
            if (address >= (uint64_t) range.region_start && address < (uint64_t) range.region_end) {
                symbolCache[address] = {
                        .name= range.name,
                        .offset=(int64_t) address - (int64_t) range.region_start,
                };
                return true;
            }
        }
        symbolCache[address] = {.name="", .offset=-1};
        return false;
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
