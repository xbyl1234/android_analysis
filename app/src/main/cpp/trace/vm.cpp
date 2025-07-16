#include <unordered_map>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <sstream>
#include <string>
#include <fstream>
#include <sys/mman.h>
#include "../third/utils/utils.h"
#include "vm.h"
#include "assert.h"

using namespace std;
using namespace QBDI;

VMAction showPostInstruction(VM *vm, GPRState *gprState, FPRState *fprState, void *data) {
    auto thiz = (MotherTrace *) data;
    const InstAnalysis *instAnalysis = vm->getInstAnalysis(
            ANALYSIS_INSTRUCTION | ANALYSIS_SYMBOL | ANALYSIS_DISASSEMBLY | ANALYSIS_OPERANDS);
    if (!thiz->checkInRange(instAnalysis->address)) {
        return VMAction::CONTINUE;
    }
    std::string regOutput, memOutput;
    for (int i = 0; i < instAnalysis->numOperands; ++i) {
        auto op = instAnalysis->operands[i];
        if (op.regAccess == REGISTER_WRITE || op.regAccess == REGISTER_READ_WRITE) {
            if (op.regCtxIdx != -1 && op.type == OPERAND_GPR) {
                uint64_t regValue = QBDI_GPR_GET(gprState, op.regCtxIdx);
                regOutput += xbyl::format_string("  %s=0x%-16lx", op.regName, regValue);
                if (check_mem((void *) regValue)) {
                    const uint8_t *dataPtr = reinterpret_cast<const uint8_t *>(regValue);
                    uint8_t buffer[256] = {0};
                    if (safe_read_memory(regValue, buffer, 256)) {
                        memOutput += is_ascii_string(buffer, 256)
                                     ? xbyl::format_string("    Strings: %s\n", buffer)
                                     : xbyl::format_string("    Hexdump %s @ 0x%lx:\n%s\n",
                                                           op.regName, regValue,
                                                           hexdump_memory(buffer, 32,
                                                                          regValue).c_str());
                    } else {
                        memOutput += xbyl::format_string("    Invalid memory @ 0x%lx\n", regValue);
                    }
                }
            }
        }
    }

    if (!regOutput.empty() || !memOutput.empty()) {
        thiz->log2file("Write Registers:\n%s\n%s", regOutput.c_str(), memOutput.c_str());
    }
    if (instAnalysis->address == (uint64_t) thiz->stopAddr) {
        thiz->log2file("Execution stopped at 0x%lx\n", instAnalysis->address);
        return VMAction::STOP;
    }
    return VMAction::CONTINUE;
}

VMAction showPreInstruction(VM *vm, GPRState *gprState, FPRState *fprState, void *data) {
    auto thiz = (MotherTrace *) data;
    const InstAnalysis *instAnalysis = vm->getInstAnalysis(
            ANALYSIS_INSTRUCTION | ANALYSIS_SYMBOL | ANALYSIS_DISASSEMBLY | ANALYSIS_OPERANDS);
    if (!thiz->checkInRange(instAnalysis->address)) {
        return VMAction::CONTINUE;
    }
    std::string symbolName;
    std::string moduleName;
    uint64_t offset = 0;

    if (instAnalysis->symbol != nullptr) {
        symbolName = instAnalysis->symbol;
    }

    Dl_info info{};
    if (dladdr((void *) instAnalysis->address, &info) != 0) {
        offset = instAnalysis->address - (uint64_t) info.dli_fbase;
        if (info.dli_fname) {
            moduleName = info.dli_fname;
        }
        if (symbolName.empty() && info.dli_sname) {
            symbolName = info.dli_sname;
        }
    } else if (!thiz->getSymbolFromCache(instAnalysis->address, &moduleName, &offset)) {
        moduleName = "unknown";
    }

    // 省略过长符号名
    if (symbolName.length() > 30) {
        symbolName = symbolName.substr(0, 20) + "..." + symbolName.substr(symbolName.length() - 7);
    }
    if (moduleName.length() > 30) {
        moduleName = moduleName.substr(0, 20) + "..." + moduleName.substr(moduleName.length() - 7);
    }

    // 格式化指令信息，地址和反汇编左对齐
    std::string instInfo = xbyl::format_string("[0x%-16lx] %30s[0x%-10lx] %-30s:\t\t\t\t\t %s",
                                               instAnalysis->address,
                                               moduleName.c_str(),
                                               offset,
                                               symbolName.c_str(),
                                               instAnalysis->disassembly);

    thiz->log2file("Instruction: %s\n", instInfo.c_str());

    // 记录读取的寄存器状态
    std::string regOutput;
    for (int i = 0; i < instAnalysis->numOperands; ++i) {
        auto op = instAnalysis->operands[i];
        if (op.regAccess == REGISTER_READ || op.regAccess == REGISTER_READ_WRITE) {
            if (op.regCtxIdx != -1 && op.type == OPERAND_GPR) {
                regOutput += xbyl::format_string("  %s=0x%-16lx", op.regName,
                                                 QBDI_GPR_GET(gprState, op.regCtxIdx));
            }
        }
    }
    if (!regOutput.empty()) {
        thiz->log2file("Read Registers:\n%s\n", regOutput.c_str());
    }
    return VMAction::CONTINUE;
}

VMAction showMemoryAccess(VM *vm, GPRState *gprState, FPRState *fprState, void *data) {
    auto thiz = (MotherTrace *) data;
    const InstAnalysis *instAnalysis = vm->getInstAnalysis(ANALYSIS_INSTRUCTION);
    if (!thiz->checkInRange(instAnalysis->address)) {
        return VMAction::CONTINUE;
    }

    std::string memOutput;
    for (const auto &acc: vm->getInstMemoryAccess()) {
        const char *type = acc.type == MEMORY_READ ? "R" : acc.type == MEMORY_WRITE ? "W" : "RW";
        memOutput += xbyl::format_string("  %s [0x%lx] size=%d value=0x%lx\n",
                                         type, acc.accessAddress, acc.size, acc.value);
    }
    if (!memOutput.empty()) {
        thiz->log2file("Memory Access:\n%s", memOutput.c_str());
    }
    return VMAction::CONTINUE;
}
