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

#endif /*__COMMON_HPP*/
