/*
 * ASSEMBLER
 * SMIPS Assembler object
 * 
 * Stefan Wong 2019
 */

#ifndef __ASSEMBLER_HPP
#define __ASSEMBLER_HPP

#include <string>
#include <unordered_map>
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

        // R-format is 
        // OP rd, rs, rt
        // I-format is 
        // OP rt, IMM(rs)
        // except for BEQ and BNE, which are 
        // OP rs, rt ,IMM
        // 
        // Note that for an instruction with OP rs, IMM(rs) format, the lexer 
        // LineInfo order will be 
        // 0 -> rt 
        // 1 -> rs
        // 2 -> imm

    // lexer output
    private:
        SourceInfo source;

    // program binary
    private:
        Program program;
        Object  object;

    private:
        std::unordered_map<int, uint8_t> instr_to_code;
        void init_instr_to_code_map(void);

    // assemble by instruction type
    private:
        Instr asm_r_instr(const TextInfo& l, const int n);
        Instr asm_r_instr_rs_rt(const TextInfo& l);
        Instr asm_r_instr_rd(const TextInfo& l);
        Instr asm_r_instr_rs(const TextInfo& l);
        Instr asm_r_instr_shamt(const TextInfo& l, const int n);
        Instr asm_i_instr(const TextInfo& l, const int n);
        Instr asm_i_instr_rt(const TextInfo& l);
        Instr asm_i_instr_branch(const TextInfo& l);
        Instr asm_j_instr(const TextInfo& l) ;

    // disable copy and move construction, 
    private:
        Assembler(const Assembler& that) = delete;
        Assembler& operator=(const Assembler& assem) = delete;

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
