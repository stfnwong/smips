/*
 * CODES
 * Lexer instruction codes
 *
 * Stefan Wong 2019
 */


#ifndef __CODES_HPP
#define __CODES_HPP

#include <array>
#include <string>
#include "Opcode.hpp"

// Opcodes used by Lexer
typedef enum instr_code 
{
    LEX_NULL,
    // arith / logic 
    LEX_ADD, LEX_ADDU, LEX_ADDI, LEX_ADDIU, 
    LEX_AND, LEX_ANDI, LEX_DIV, LEX_DIVU,
    LEX_MULT, LEX_MULTU, LEX_NOR, LEX_OR,
    LEX_ORI, LEX_SUB, LEX_SUBU,
    LEX_SLL, LEX_SLLV, LEX_SRL, LEX_SRV,        // shifts
    // comparison instructions 
    LEX_SLT, LEX_SLTU, LEX_SLTI, LEX_SLTIU,
    // load
    LEX_LB, LEX_LBU, LEX_LH, LEX_LHU, 
    LEX_LW, LEX_LI, LEX_LA,
    // store
    LEX_SB, LEX_SH, LEX_SW, 
    // jumps
    LEX_J, LEX_JAL, LEX_JALR, LEX_JR,
    // branches
    LEX_BEQ, LEX_BGT, LEX_BGTZ, LEX_BLEZ, LEX_BNE
} instr_code;


// Directive codes used by Lexer
typedef enum directive_code
{
    LEX_ALIGN,     
    LEX_ASCIIZ,    
    LEX_BYTE,      
    LEX_DATA,      
    LEX_DOUBLE,    
    LEX_END_MACRO, 
    LEX_FLOAT,     
    LEX_GLOBAL,    
    LEX_HALF,      
    LEX_INCLUDE,   
    LEX_MACRO,     
    LEX_SPACE,     
    LEX_TEXT,      
    LEX_WORD,      
} directive_code;


// TODO : are syscalls really that different that they need 
// their own special datatype (the answer may be yes)
typedef enum sys_code
{
    LEX_SYSCALL
} sys_code;

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
    Opcode(LEX_SLL,   "SLL"),
    Opcode(LEX_SLLV,  "SLLV"),
    Opcode(LEX_SRL,   "SRL"),
    Opcode(LEX_SRV,   "SRV"),
    // comparison
    Opcode(LEX_SLT,   "SLT"),
    Opcode(LEX_SLTU,  "SLTU"),
    Opcode(LEX_SLTI,  "SLTI"),
    Opcode(LEX_SLTIU, "SLTIU"),
    // loads 
    Opcode(LEX_LB,    "LB"),
    Opcode(LEX_LBU,   "LBU"),
    Opcode(LEX_LH,    "LH"),
    Opcode(LEX_LHU,   "LHU"),
    Opcode(LEX_LW,    "LW"),
    Opcode(LEX_LI,    "LI"),
	Opcode(LEX_LA,    "LA"),
    // store
    Opcode(LEX_SH,    "SH"),
    Opcode(LEX_SW,    "SW"),
    // jump
    Opcode(LEX_J,     "J"),
    Opcode(LEX_JAL,   "JAL"),
    Opcode(LEX_JALR,  "JALR"),
    Opcode(LEX_JR,    "JR"),
    // branch
    Opcode(LEX_BEQ,   "BEQ"),
	Opcode(LEX_BGT,   "BGT"),
    Opcode(LEX_BGTZ,  "BGTZ"),
    Opcode(LEX_BLEZ,  "BLEZ"),
    Opcode(LEX_BNE,   "BNE"),
};


// Assembly directives
const Opcode lex_directive_codes[] = {
    Opcode(LEX_ALIGN,     ".ALIGN"),
    Opcode(LEX_ASCIIZ,    ".ASCIIZ"),
    Opcode(LEX_BYTE,      ".BYTE"),
    Opcode(LEX_DATA,      ".DATA"),
    Opcode(LEX_DOUBLE,    ".DOUBLE"),
    Opcode(LEX_END_MACRO, ".END_MACRO"),
    Opcode(LEX_FLOAT,     ".FLOAT"),
    Opcode(LEX_GLOBAL,    ".GLOBAL"),
    Opcode(LEX_HALF,      ".HALF"),
    Opcode(LEX_INCLUDE,   ".INCLUDE"),
    Opcode(LEX_MACRO,     ".MACRO"),
    Opcode(LEX_SPACE,     ".SPACE"),
    Opcode(LEX_TEXT,      ".TEXT"),
    Opcode(LEX_WORD,      ".WORD")
};


// Assembler side instruction codes
enum class InstrType : uint8_t
{
    INSTR_NULL,     // NULL instruction
    INSTR_R,        // register instruction
    INSTR_I,        // immediate instruction
    INSTR_J,        // jump instruction
};


// R-instruction table index
typedef enum asm_r_instr_map {
    ASM_ADD, ASM_ADDU, ASM_SUBU, ASM_AND, 
    ASM_OR, ASM_NOR, ASM_SLT, ASM_SLTU, 
    ASM_SLL, ASM_SRL, ASM_JR
} asm_r_instr_map; 

typedef enum asm_i_instr_map {
    ASM_BEQ, ASM_BNE, ASM_ADDI, ASM_ADDIU,
    ASM_ANDI, ASM_ORI, ASM_SLTI, ASM_SLTIU,
    ASM_LUI, ASM_LW, ASM_SW
} asm_i_instr_map; 

typedef enum asm_j_instr_map {
    ASM_J, ASM_JAL
} asm_j_instr_map; 


/*
 * InstrCode
 * Holds a single instruction code used by the assembler
 */
struct InstrCode
{
    uint8_t   code;
    InstrType type;

    public:
        InstrCode();
        InstrCode(const uint8_t c, const InstrType& t);
        bool operator==(const InstrCode& that) const;
        bool operator!=(const InstrCode& that) const;
        std::string toString(void) const;
};

/*
 * InstrTable
 * Table holding actual opcode for all instructions
 */
class InstrTable
{
    private:
        std::array<InstrCode, 12> r_instrs;
        std::array<InstrCode, 11> i_instrs;
        std::array<InstrCode, 2>  j_instrs;
        InstrCode null_instr;

    public:
        InstrTable();
};


#endif /*__CODES_HPP*/
