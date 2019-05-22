/*
 * ASSEMBLER
 * SMIPS Assembler object
 * 
 * Stefan Wong 2019
 */

#ifndef __ASSEMBLER_HPP
#define __ASSEMBLER_HPP

#include <string>
#include "Program.hpp"
#include "Source.hpp"

constexpr int REG_INVALID_OFFSET  = -1;

// Convert a register type and  value to an offset in memory
int reg2Offset(const TokenType& type, const int val);


/*
 * Assembler
 * Transform a SourceInfo object into a Program object
 */
class Assembler
{
    private:
        bool verbose;
        int  num_err;
        const uint8_t r_instr_offsets[3] = {
            11,         // rd offset
            21,         // rs offset 
            16          // rt offset
        };
        const uint8_t i_instr_offsets[3] = {
            16,     // rt   (dest))
            21,     // rs   (base)
            0       // imm
        };
        //const uint8_t r_instr_rs_offset = 21;
        //const uint8_t r_instr_rt_offset = 16;
        //const uint8_t r_instr_rd_offset = 11;


    // lexer output
    private:
        SourceInfo source;

    // program binary
    private:
        Program program;

    // assemble by instruction type
    private:
        uint32_t asm_r_instr(const LineInfo& l, const int n) const;
        uint32_t asm_i_instr(const LineInfo& l, const int n) const;
        uint32_t asm_j_instr(const LineInfo& l) const;

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
