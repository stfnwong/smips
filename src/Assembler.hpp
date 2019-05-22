/*
 * ASSEMBLER
 * SMIPS Assembler object
 * 
 * Stefan Wong 2019
 */

#ifndef __ASSEMBLER_HPP
#define __ASSEMBLER_HPP

#include <array>
#include <string>
#include "Program.hpp"
#include "Source.hpp"

// Could have a lookup table here for machine codes..
enum class InstrType : uint8_t
{
    INSTR_NULL,     // NULL instruction
    INSTR_R,        // register instruction
    INSTR_I,        // immediate instruction
    INSTR_J,        // jump instruction
};


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


/*
 * Assembler
 * Transform a SourceInfo object into a Program object
 */
class Assembler
{
    private:
        bool verbose;
        int  num_err;
    // lexer output
    private:
        SourceInfo source;

    // program binary
    private:
        Program program;


    // argument conversion
    private:
        uint8_t get_rfunct(void);
        uint8_t get_iopcode(void);
        int  reg_lookup(const TokenType& type, const int val) const;

    // instruction assembly
    private:
        void asm_add(const LineInfo& l);
        void asm_addi(const LineInfo& l);
        void asm_addu(const LineInfo& l);
        void asm_lw(const LineInfo& l);
        void asm_mult(const LineInfo& l);
        void asm_or(const LineInfo& l);
        void asm_ori(const LineInfo& l);
        void asm_sub(const LineInfo& l);
        void asm_subu(const LineInfo& l);
        void asm_sw(const LineInfo& l);

    public:
        Assembler();
        //int  readFile(const std::string& filename);

        void assemble(void);

        void setVerbose(const bool v);
        bool getVerbose(void) const;
        int  getNumErr(void) const;
        void loadSource(const SourceInfo& s);
        Program getProgram(void) const;
};


#endif /*__ASSEMBLER_HPP*/
