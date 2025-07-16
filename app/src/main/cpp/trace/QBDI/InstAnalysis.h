/*
 * 本文件属于 QBDI 项目的一部分。
 *
 * 版权所有 2017 - 2023 Quarkslab
 *
 * 根据 Apache 许可证，版本 2.0（以下简称“许可证”）许可；
 * 除非符合许可证，否则您不得使用此文件。
 * 您可以从以下地址获得许可证副本：
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * 除非适用法律要求或书面同意，根据许可证分发的软件按“原样”分发，
 * 无任何明示或暗示的担保或条件。
 * 有关许可证下许可的特定语言的详细信息，请参阅许可证。
 */
#ifndef QBDI_INSTANALYSIS_H_
#define QBDI_INSTANALYSIS_H_

#include <stdbool.h>

#include "Bitmask.h"
#include "Platform.h"
#include "State.h"

#ifdef __cplusplus
namespace QBDI {
#endif

/*! 寄存器操作类型（读取/写入/读写） */
    typedef enum {
        _QBDI_EI(REGISTER_UNUSED) = 0,     /*!< 未使用寄存器 */
        _QBDI_EI(REGISTER_READ) = 1,       /*!< 寄存器读取操作 */
        _QBDI_EI(REGISTER_WRITE) = 1 << 1, /*!< 寄存器写入操作 */
        _QBDI_EI(REGISTER_READ_WRITE) = 3, /*!< 寄存器读写操作 */
    } RegisterAccessType;

    _QBDI_ENABLE_BITMASK_OPERATORS(RegisterAccessType)

/*! 指令条件 */
    typedef enum {
        _QBDI_EI(CONDITION_NONE) = 0x0, /*!< 指令无条件 */
        // ConditionType ^ 0x1 反转条件，除 CONDITION_NONE 外
        _QBDI_EI(CONDITION_ALWAYS) = 0x2,       /*!< 指令始终为真 */
        _QBDI_EI(CONDITION_NEVER) = 0x3,        /*!< 指令始终为假 */
        _QBDI_EI(CONDITION_EQUALS) = 0x4,       /*!< 等于 ('==') */
        _QBDI_EI(CONDITION_NOT_EQUALS) = 0x5,   /*!< 不等于 ('!=') */
        _QBDI_EI(CONDITION_ABOVE) = 0x6,        /*!< 大于（无符号 '>'） */
        _QBDI_EI(CONDITION_BELOW_EQUALS) = 0x7, /*!< 小于等于（无符号 '<='） */
        _QBDI_EI(CONDITION_ABOVE_EQUALS) = 0x8, /*!< 大于等于（无符号 '>='） */
        _QBDI_EI(CONDITION_BELOW) = 0x9,        /*!< 小于（无符号 '<'） */
        _QBDI_EI(CONDITION_GREAT) = 0xa,        /*!< 大于（有符号 '>'） */
        _QBDI_EI(CONDITION_LESS_EQUALS) = 0xb,  /*!< 小于等于（有符号 '<='） */
        _QBDI_EI(CONDITION_GREAT_EQUALS) = 0xc, /*!< 大于等于（有符号 '>='） */
        _QBDI_EI(CONDITION_LESS) = 0xd,         /*!< 小于（有符号 '<'） */
        _QBDI_EI(CONDITION_EVEN) = 0xe,         /*!< 偶数 */
        _QBDI_EI(CONDITION_ODD) = 0xf,          /*!< 奇数 */
        _QBDI_EI(CONDITION_OVERFLOW) = 0x10,    /*!< 溢出 */
        _QBDI_EI(CONDITION_NOT_OVERFLOW) = 0x11, /*!< 无溢出 */
        _QBDI_EI(CONDITION_SIGN) = 0x12,         /*!< 符号 */
        _QBDI_EI(CONDITION_NOT_SIGN) = 0x13,     /*!< 无符号 */
    } ConditionType;

/*! 操作数类型 */
    typedef enum {
        _QBDI_EI(OPERAND_INVALID) = 0, /*!< 无效操作数 */
        _QBDI_EI(OPERAND_IMM),         /*!< 立即数操作数 */
        _QBDI_EI(OPERAND_GPR),         /*!< 寄存器操作数 */
        _QBDI_EI(OPERAND_PRED),        /*!< 谓词操作数 */
        _QBDI_EI(OPERAND_FPR),         /*!< 浮点寄存器操作数 */
        _QBDI_EI(OPERAND_SEG),         /*!< 段寄存器或不支持的寄存器操作数 */
    } OperandType;

    typedef enum {
        _QBDI_EI(OPERANDFLAG_NONE) = 0,                  /*!< 无标志 */
        _QBDI_EI(OPERANDFLAG_ADDR) = 1 << 0,             /*!< 操作数用于计算地址 */
        _QBDI_EI(OPERANDFLAG_PCREL) = 1 << 1,            /*!< 操作数值相对 PC */
        _QBDI_EI(OPERANDFLAG_UNDEFINED_EFFECT) = 1 << 2, /*!< 操作数角色未完全定义 */
        _QBDI_EI(OPERANDFLAG_IMPLICIT) = 1 << 3,         /*!< 操作数是隐式的 */
    } OperandFlag;

    _QBDI_ENABLE_BITMASK_OPERATORS(OperandFlag)

/*! 包含 VM 提供的操作数分析结果的结构体 */
    typedef struct {
        // 通用字段
        OperandType type; /*!< 操作数类型 */
        OperandFlag flag; /*!< 操作数标志 */
        sword value;      /*!< 操作数值（如果为立即数），或寄存器 ID */
        uint8_t size;     /*!< 操作数大小（以字节为单位） */
        // 寄存器特定字段
        uint8_t regOff;      /*!< 子寄存器在寄存器中的偏移（以位为单位） */
        int16_t regCtxIdx;   /*!< VM 状态中的寄存器索引（如果未知则小于 0） */
        const char *regName; /*!< 寄存器名称 */
        RegisterAccessType regAccess; /*!< 寄存器访问类型（读取、写入、读写） */
    } OperandAnalysis;

/*! 指令分析类型 */
    typedef enum {
        _QBDI_EI(ANALYSIS_INSTRUCTION) = 1,      /*!< 指令分析（地址、助记符等） */
        _QBDI_EI(ANALYSIS_DISASSEMBLY) = 1 << 1, /*!< 指令反汇编 */
        _QBDI_EI(ANALYSIS_OPERANDS) = 1 << 2,    /*!< 指令操作数分析 */
        _QBDI_EI(ANALYSIS_SYMBOL) = 1 << 3,      /*!< 指令符号 */
    } AnalysisType;

    _QBDI_ENABLE_BITMASK_OPERATORS(AnalysisType)

/*! 包含 VM 提供的指令分析结果的结构体 */
    typedef struct {
        // ANALYSIS_INSTRUCTION
        const char *mnemonic;   /*!< LLVM 助记符
                           * （注意：如果未使用 ANALYSIS_INSTRUCTION，则为 NULL）
                           */
        rword address;          /*!< 指令地址 */
        uint32_t instSize;      /*!< 指令大小（以字节为单位） */
        CPUMode cpuMode;        /*!< 指令 CPU 模式 */
        bool affectControlFlow; /*!< 如果指令影响控制流，则为 true */
        bool isBranch;          /*!< 如果指令像“跳转”一样，则为 true */
        bool isCall;            /*!< 如果指令像“调用”一样，则为 true */
        bool isReturn;          /*!< 如果指令像“返回”一样，则为 true */
        bool isCompare;         /*!< 如果指令是比较指令，则为 true */
        bool isPredicable;      /*!< 如果指令包含谓词（即条件），则为 true */
        bool isMoveImm;         /*!< 如果此指令是立即数移动（包括条件移动），则为 true */
        bool mayLoad;       /*!< 如果 QBDI 检测到此指令为加载指令，则为 true */
        bool mayStore;      /*!< 如果 QBDI 检测到此指令为存储指令，则为 true */
        uint32_t loadSize;  /*!< 预期读取访问的大小，
                       * 如果无法确定大小，则 mayLoad 可能为 true 且值为 0
                       */
        uint32_t storeSize; /*!< 预期写入访问的大小，
                       * 如果无法确定大小，则 mayStore 可能为 true 且值为 0
                       */
        ConditionType condition; /*!< 与指令关联的条件 */
        bool mayLoad_LLVM;       /*!< 0.7.1 版本中的 mayLoad */
        bool mayStore_LLVM;      /*!< 0.7.1 版本中的 mayStore */
        // ANALYSIS_DISASSEMBLY
        char *disassembly; /*!< 指令反汇编
                      * （注意：如果未使用 ANALYSIS_DISASSEMBLY，则为 NULL）
                      */
        // ANALYSIS_OPERANDS
        RegisterAccessType flagsAccess; /*!< 标志访问类型（无访问、读取、写入、读写）
                                   * （注意：如果未使用 ANALYSIS_OPERANDS，则为 REGISTER_UNUSED）
                                   */
        uint8_t numOperands;            /*!< 指令使用的操作数数量 */
        OperandAnalysis *operands;      /*!< 包含 VM 提供的操作数分析结果的结构体
                                   * （注意：如果未使用 ANALYSIS_OPERANDS，则为 NULL）
                                   */
        // ANALYSIS_SYMBOL
        const char *symbol;    /*!< 指令符号
                          * （注意：如果未使用 ANALYSIS_SYMBOL 或未找到符号，则为 NULL）
                          */
        uint32_t symbolOffset; /*!< 指令符号偏移 */
        const char *module;    /*!< 指令模块名称
                          * （注意：如果未使用 ANALYSIS_SYMBOL 或未找到模块，则为 NULL）
                          */
        // INTERNAL
        uint32_t analysisType; /*!< 内部：指令分析类型
                          * （不建议使用）
                          */
    } InstAnalysis;

#ifdef __cplusplus
}
#endif

#endif // QBDI_INSTANALYSIS_H_
