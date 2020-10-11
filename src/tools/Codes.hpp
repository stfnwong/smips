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

    LEX_ADD, LEX_ADDU, LEX_ADDI, LEX_ADDIU, 
    LEX_AND, LEX_ANDI, LEX_DIV, LEX_DIVU, LEX_MUL,
    LEX_MULT, LEX_MULTU, LEX_NOR, LEX_OR,
    LEX_ORI, LEX_SUB, LEX_SUBU, LEX_XOR, LEX_XORI,
    LEX_SLL, LEX_SLLV, LEX_SRL, LEX_SRV,        // shifts
    // manipulate hi, lo registers 
    LEX_MFHI, LEX_MTHI, LEX_MFLO, LEX_MTLO,
    // comparison instructions 
    LEX_SLT, LEX_SLTU, LEX_SLTI, LEX_SLTIU,
    // load
    LEX_LB, LEX_LBU, LEX_LH, LEX_LHU, 
    LEX_LW, LEX_LI, LEX_LA, LEX_LUI,
    // store
    LEX_SB, LEX_SH, LEX_SW, 
    // jumps
    LEX_J, LEX_JAL, LEX_JALR, LEX_JR,
    // branches
    LEX_BEQ, LEX_BLEZ, LEX_BNE, LEX_BGTZ,
    // branch psuedo instructions 
    LEX_BGT, LEX_BLT, LEX_BGE, LEX_BLE, 
    LEX_BGTU, LEX_BEQZ, 
    // syscall
    LEX_SYSCALL     // not strictly an instruction
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



// MIPS Opcode list 
const Opcode lex_instr_codes[] = {
    Opcode(LEX_NULL,  "\0"),
    // arith / logic
    Opcode(LEX_ADD,   "add"),
    Opcode(LEX_ADDU,  "addu"),
    Opcode(LEX_ADDI,  "addi"),
    Opcode(LEX_ADDIU, "addiu"),
    Opcode(LEX_AND,   "and"),
    Opcode(LEX_ANDI,  "andi"),
    Opcode(LEX_DIV,   "div"),
    Opcode(LEX_DIVU,  "divu"),
    Opcode(LEX_MUL,   "mul"),
    Opcode(LEX_MULT,  "mult"),
    Opcode(LEX_MULTU, "multu"),
    Opcode(LEX_NOR,   "nor"),
    Opcode(LEX_OR,    "or"),
    Opcode(LEX_ORI,   "ori"),
    Opcode(LEX_SLL,   "sll"),
    Opcode(LEX_SLLV,  "sllv"),
    Opcode(LEX_SRL,   "srl"),
    Opcode(LEX_SRV,   "srv"),
    Opcode(LEX_XOR,   "xor"),
    Opcode(LEX_XORI,  "xori"),
    // hi/lo register 
    Opcode(LEX_MFHI,  "mfhi"),
    Opcode(LEX_MTHI,  "mthi"),
    Opcode(LEX_MFLO,  "mflo"),
    Opcode(LEX_MTLO,  "mtlo"),
    // comparison
    Opcode(LEX_SLT,   "slt"),
    Opcode(LEX_SLTU,  "sltu"),
    Opcode(LEX_SLTI,  "slti"),
    Opcode(LEX_SLTIU, "sltiu"),
    // loads 
    Opcode(LEX_LB,    "lb"),
    Opcode(LEX_LBU,   "lbu"),
    Opcode(LEX_LH,    "lh"),
    Opcode(LEX_LHU,   "lhu"),
    Opcode(LEX_LW,    "lw"),
    Opcode(LEX_LI,    "li"),
	Opcode(LEX_LA,    "la"),
    Opcode(LEX_LUI,   "lui"),
    // store
    Opcode(LEX_SH,    "sh"),
    Opcode(LEX_SW,    "sw"),
    // jump
    Opcode(LEX_J,     "j"),
    Opcode(LEX_JAL,   "jal"),
    Opcode(LEX_JALR,  "jalr"),
    Opcode(LEX_JR,    "jr"),
    // branch
    Opcode(LEX_BEQ,   "beq"),
	Opcode(LEX_BLT,   "blt"),
	Opcode(LEX_BGT,   "bgt"),
	Opcode(LEX_BGE,   "bge"),
	Opcode(LEX_BLE,   "ble"),
	Opcode(LEX_BGTU,  "bgtu"),
	Opcode(LEX_BEQZ,  "beqz"),
	Opcode(LEX_BEQ,   "beq"),
    Opcode(LEX_BNE,   "bne"),
    Opcode(LEX_BGTZ,  "bgtz"),
    Opcode(LEX_BLEZ,  "blez"),
    // syscall
    Opcode(LEX_SYSCALL, "syscall"),
};


// Assembly directives
const Opcode lex_directive_codes[] = {
    Opcode(LEX_ALIGN,     ".align"),
    Opcode(LEX_ASCIIZ,    ".asciiz"),
    Opcode(LEX_BYTE,      ".byte"),
    Opcode(LEX_DATA,      ".data"),
    Opcode(LEX_DOUBLE,    ".double"),
    Opcode(LEX_END_MACRO, ".end_macro"),
    Opcode(LEX_FLOAT,     ".float"),
    Opcode(LEX_GLOBAL,    ".global"),
    Opcode(LEX_HALF,      ".half"),
    Opcode(LEX_INCLUDE,   ".include"),
    Opcode(LEX_MACRO,     ".macro"),
    Opcode(LEX_SPACE,     ".space"),
    Opcode(LEX_TEXT,      ".text"),
    Opcode(LEX_WORD,      ".word")
};

// Psuedo Ops
const Opcode lex_psuedo_ops[] = {
    Opcode(LEX_BLT,   "blt"),
    Opcode(LEX_BGT,   "bgt"),
    Opcode(LEX_BGE,   "bge"),
    Opcode(LEX_BLE,   "ble"),
    Opcode(LEX_LA,    "la"),
    Opcode(LEX_LI,    "li"),
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
