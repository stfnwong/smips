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
#include "Object.hpp"


/*
 * Assembler
 * Transform a SourceInfo object into a Program object
 */
class Assembler
{
    // TODO ; move to global space in header?
    private:
        bool verbose;
        int  num_err;
        // instruction arg offsets
        const uint8_t j_instr_op_offset = 26;
        const uint8_t i_instr_op_offset = 26;
        const uint8_t r_instr_op_offset = 26;

        const uint8_t r_instr_offsets[3] = {
            11,     // rd offset
            21,     // rs offset 
            16      // rt offset
        };
        // TODO : this order fails for BNE...
        const uint8_t i_instr_offsets[3] = {
            16,     // rt   (dest))
            21,     // rs   (base)
            0       // imm
        };
        const int ARG_INVALID_OFFSET  = -1;

    // lexer output
    private:
        SourceInfo source;

    // program binary
    private:
        Program program;
        Object  object;

    // assemble by instruction type
    private:
        uint32_t asm_r_instr(const TextInfo& l, const int n) const;
        uint32_t asm_i_instr(const TextInfo& l, const int n) const;
        uint32_t asm_j_instr(const TextInfo& l) const;

    // data region assembly
    private:
        //DataSeg asm_asciiz(const DataInfo& d);
        //DataSeg asm_byte(const DataInfo& d);
        //DataSeg asm_char(const DataInfo& d);
        //DataSeg asm_half(const DataInfo& d);
        //DataSeg asm_space(const DataInfo& d);
        //DataSeg asm_word(const DataInfo& d);

    // text region/instruction assembly
    private:
        Instr asm_add(const TextInfo& l) const;
        Instr asm_addi(const TextInfo& l) const;
        Instr asm_addu(const TextInfo& l) const;
        Instr asm_beq(const TextInfo& l) const;
        Instr asm_bne(const TextInfo& l) const;
        Instr asm_j(const TextInfo& l) const;
        Instr asm_lui(const TextInfo& l) const;
        Instr asm_lw(const TextInfo& l) const;
        Instr asm_mult(const TextInfo& l) const;
        Instr asm_or(const TextInfo& l) const;
        Instr asm_ori(const TextInfo& l) const;
        Instr asm_sll(const TextInfo& l) const;
        Instr asm_slt(const TextInfo& l) const;
        Instr asm_sltu(const TextInfo& l) const;
        Instr asm_sub(const TextInfo& l) const;
        Instr asm_subu(const TextInfo& l) const;
        Instr asm_sw(const TextInfo& l) const;

    // disable copy and move construction, 
    private:
        Assembler(const Assembler& that) = delete;

    public:
        Assembler();

        void    assemble(void);
        Instr   assembleText(const TextInfo& line);
        DataSeg assembleData(const DataInfo& data);

        void    setVerbose(const bool v);
        bool    getVerbose(void) const;
        int     getNumErr(void) const;
        void    loadSource(const SourceInfo& s);
        Program getProgram(void) const;
};


#endif /*__ASSEMBLER_HPP*/
