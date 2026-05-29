/*
 * COMMON
 * Some common constants
 *
 * Stefan Wong 2020
 */

#ifndef __COMMON_HPP
#define __COMMON_HPP


// i-instruction offsets
static constexpr int I_INSTR_OP_OFFSET = 26;
static constexpr int I_INSTR_RT_OFFSET = 16;
static constexpr int I_INSTR_RS_OFFSET = 21;
// r-instruction offsets
static constexpr int R_INSTR_OP_OFFSET = 26;
static constexpr int R_INSTR_RS_OFFSET = 21;
static constexpr int R_INSTR_RT_OFFSET = 16;
static constexpr int R_INSTR_RD_OFFSET = 11;
static constexpr int R_INSTR_SHAMT_OFFSET = 6;
// j-instruction offsets
static constexpr int J_INSTR_OP_OFFSET = 26;


// R instruction codes
#define R_SLL     0
#define R_SRL     2
#define R_SRA     3
#define R_SLLV    4
#define R_SRLV    6
#define R_SRAV    7
#define R_JR      8
#define R_JALR    9
#define R_SYSCALL 12
#define R_MFHI    16
#define R_MTHI    17
#define R_MFLO    18
#define R_MTLO    19
#define R_MULT    24
#define R_MULTU   25
#define R_DIV     26
#define R_DIVU    27
#define R_ADD     32
#define R_ADDU    33
#define R_SUB     34
#define R_SUBU    35
#define R_AND     36
#define R_OR      37
#define R_XOR     38
#define R_NOR     39
#define R_SLT     42
#define R_SLTU    43

// I-instruction codes
#define I_BEQ     4
#define I_BNE     5
#define I_BLEZ    6
#define I_BGTZ    7
#define I_ADDI    8
#define I_ADDIU   9
#define I_SLTI    10
#define I_SLTIU   11
#define I_ANDI    12
#define I_ORI     13 
#define I_XORI    14
#define I_LUI     15
#define I_LB      32
#define I_LH      33
#define I_LW      35
#define I_LBU     36
#define I_LHU     37
#define I_SB      40
#define I_SH      41
#define I_SW      43



#endif /*__COMMON_HPP*/
