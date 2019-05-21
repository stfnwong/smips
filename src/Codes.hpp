/*
 * CODES
 * Lexer instruction codes
 *
 * Stefan Wong 2019
 */


#ifndef __CODES_HPP
#define __CODES_HPP

#include "Opcode.hpp"

// Opcodes used by Lexer
typedef enum instr_code 
{
    LEX_NULL,
    // arith / logic 
    LEX_ADD, LEX_ADDU, LEX_ADDI, LEX_ADDIU, 
    LEX_AND, LEX_ANDI, LEX_DIV, LEX_DIVU,
    LEX_MULT, LEX_MULTU, LEX_NOR, LEX_OR,
    LEX_ORI, 
    // load
    LEX_LB, LEX_LBU, LEX_LH, LEX_LHU, 
    LEX_LW, LEX_LI,
    // store
    LEX_SB, LEX_SH, LEX_SW,
    // jumps
    LEX_J, LEX_JAL, LEX_JALR, LEX_JR,
} instr_code;

// MIPS Opcode list 
const Opcode lex_instr_codes[] = {
    Opcode(LEX_NULL,  "\0"),
    // arith / logic
    Opcode(LEX_ADD,   "ADD"),
    Opcode(LEX_ADDU,  "ADDU"),
    Opcode(LEX_ADDI,  "ADDI"),
    Opcode(LEX_ADDIU, "ADDIU"),
    Opcode(LEX_AND,   "AND"),
    Opcode(LEX_ANDI,  "ANDI"),
    Opcode(LEX_DIV,   "DIV"),
    Opcode(LEX_DIVU,  "DIVU"),
    Opcode(LEX_MULT,  "MULT"),
    Opcode(LEX_MULTU, "MULTU"),
    Opcode(LEX_NOR,   "NOR"),
    Opcode(LEX_OR,    "OR"),
    Opcode(LEX_ORI,   "ORI"),
    // loads 
    Opcode(LEX_LB,    "LB"),
    Opcode(LEX_LBU,   "LBU"),
    Opcode(LEX_LH,    "LH"),
    Opcode(LEX_LHU,   "LHU"),
    Opcode(LEX_LW,    "LW"),
    Opcode(LEX_LI,    "LI"),
    // store
    Opcode(LEX_SH,    "SH"),
    Opcode(LEX_SW,    "SW"),
    // jump
    Opcode(LEX_J,     "J"),
    Opcode(LEX_JAL,   "JAL"),
    Opcode(LEX_JALR,  "JALR"),
    Opcode(LEX_JR,    "JR"),
};

#endif /*__CODES_HPP*/
