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

string checkNameLength(const string &name) {
    return name.length() > 30 ? name.substr(0, 20) + "..." + name.substr(name.length() - 7) : name;
}
VMAction traceCallPath(VM *vm, GPRState *gprState, FPRState *fprState, void *data) {
    auto thiz = (MotherTrace *) data;
    const InstAnalysis *instAnalysis = vm->getInstAnalysis(
            ANALYSIS_INSTRUCTION | ANALYSIS_SYMBOL | ANALYSIS_DISASSEMBLY | ANALYSIS_OPERANDS);
    if (!thiz->checkInRange(instAnalysis->address)) {
        return VMAction::CONTINUE;
    }
    static int depth = 0;
    string fromModuleName, fromSymbolName, toModuleName, toSymbolName;
    uint64_t fromOffset = 0, toOffset = 0, targetAddress = 0;
    if (instAnalysis->isCall) {
        string regName;
        for (int i = 0; i < instAnalysis->numOperands; ++i) {
            auto op = instAnalysis->operands[i];
            if (op.type == OPERAND_IMM) {
                targetAddress = instAnalysis->address + (op.value << 2);
                regName = "im";
                break;
            } else if (op.type == OPERAND_GPR && op.regName != string("LR")) {
                targetAddress = QBDI_GPR_GET(gprState, op.regCtxIdx);
                regName = op.regName;
                break;
            }
        }
        thiz->findSymbol(targetAddress, &toModuleName, &toSymbolName, &toOffset);
        thiz->findSymbol(instAnalysis->address, &fromModuleName, &fromSymbolName, &fromOffset);
        thiz->log2file("call:%s:%s:%d:%s:%s:%p:%p:%s:%s:%p:%p",
                       thiz->checkInRange(targetAddress) ? "inside" : "outside",
                       regName.c_str(),
                       depth,
                       fromModuleName.c_str(),
                       fromSymbolName.c_str(),
                       instAnalysis->address,
                       fromOffset,
                       toModuleName.c_str(),
                       toSymbolName.c_str(),
                       targetAddress,
                       toOffset);
        if (thiz->checkInRange(targetAddress)) {
            depth++;
        }
    } else if (instAnalysis->isReturn) {
        targetAddress = QBDI_GPR_GET(gprState, REG_LR);
        thiz->findSymbol(targetAddress, &toModuleName, &toSymbolName, &toOffset);
        thiz->findSymbol(instAnalysis->address, &fromModuleName, &fromSymbolName, &fromOffset);
        thiz->log2file("ret:%d:%s:%s:%p:%p:%s:%s:%p:%p",
                       depth,
                       fromModuleName.c_str(),
                       fromSymbolName.c_str(),
                       instAnalysis->address,
                       fromOffset,
                       toModuleName.c_str(),
                       toSymbolName.c_str(),
                       targetAddress,
                       toOffset);
        depth--;
    }
    if (instAnalysis->address == (uint64_t) thiz->stopAddr) {
        thiz->log2file("Execution stopped at 0x%lx\n", instAnalysis->address);
        return VMAction::STOP;
    }
    return VMAction::CONTINUE;
}
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

    thiz->findSymbol(instAnalysis->address, &moduleName, &symbolName, &offset);
    symbolName = checkNameLength(symbolName);
    moduleName = checkNameLength(moduleName);

    // 省略过长符号名

    // 格式化指令信息，地址和反汇编左对齐
    std::string instInfo = xbyl::format_string("[0x%-16lx] %30s[0x%-10lx] %-30s:\t\t\t\t\t %s",
                                               instAnalysis->address,
                                               moduleName.c_str(),
                                               offset,
                                               symbolName.c_str(),
                                               instAnalysis->disassembly);

    string callLikeInfo;
    if (instAnalysis->isBranch || instAnalysis->isCall) {
        for (int i = 0; i < instAnalysis->numOperands; ++i) {
            auto op = instAnalysis->operands[i];
            string regName, targetSymbol, targetModule;
            uint64_t targetOffset, targetAddress = 0;;
            if (op.type == OPERAND_GPR) {
                regName = op.regName;
                if (regName == "LR") {
                    continue;
                }
                targetAddress = QBDI_GPR_GET(gprState, op.regCtxIdx);
            } else {
                continue;
            }
            thiz->findSymbol(targetAddress, &targetModule, &targetSymbol, &targetOffset);
            targetModule = checkNameLength(targetModule);
            targetSymbol = checkNameLength(targetSymbol);
            callLikeInfo += xbyl::format_string("%s:[%s:%s][0x%x]\t",
                                                regName.c_str(),
                                                targetModule.c_str(),
                                                targetSymbol.c_str(),
                                                targetOffset);
        }
    }
    thiz->log2file("Instruction: %s\t\t%s\n", instInfo.c_str(), callLikeInfo.c_str());

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
