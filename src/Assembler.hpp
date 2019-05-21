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

// Could have a lookup table here for machine codes..

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


    // instruction assembly
    private:
        void asm_add(const LineInfo& l);
        void asm_addi(const LineInfo& l);
        void asm_addu(const LineInfo& l);
        void asm_lw(const LineInfo& l);
        void asm_mult(const LineInfo& l);
        void asm_or(const LineInfo& l);
        void asm_ori(const LineInfo& l);
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
