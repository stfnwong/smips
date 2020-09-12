/*
 * DISASSEMBLER
 * Reverse a SMIPS binary into a SourceInfo
 *
 * Stefan Wong 2019
 */

#ifndef __DISASSEMBLER_HPP
#define __DISASSEMBLER_HPP

#include <string>

#include "Program.hpp"
#include "Source.hpp"
#include "Register.hpp"




class Disassembler
{
    private:
        bool verbose;

    private:
        RegisterMap rmap;

    private:
        TextInfo dis_i_instr(uint32_t instr, uint32_t addr);
        TextInfo dis_r_instr(uint32_t instr, uint32_t addr);
        TextInfo dis_j_instr(uint32_t instr, uint32_t addr);

    private:
        Program    program;
        SourceInfo source;
        TextInfo   cur_line;

    public:
        Disassembler();
        TextInfo dis_instr(uint32_t instr, uint32_t addr);

        void disassemble(void);
        // TODO : we can always have a seperate Program obeject.
        // If we wamt to hold project state here then this makes sense,
        int  load(const std::string& filename);

        void setVerbose(const bool v);
        bool getVerbose(void) const;
};

#endif /*__DISASSEMBLER_HPP*/
