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
        // NOTE: really the only reason that we have an object here is
        // so that we can carry this map around without having to
        // statically initialize it. If not for that, these could just 
        // be regular functions.
        //
        // TODO : consider making these regular functions.
        RegisterMap rmap;

    private:
        TextInfo dis_i_instr(uint32_t instr, uint32_t addr);
        TextInfo dis_r_instr(uint32_t instr, uint32_t addr);
        TextInfo dis_j_instr(uint32_t instr, uint32_t addr);

    public:
        Disassembler();

        // TODO : these can probably be const functions
        TextInfo dis_instr(uint32_t instr, uint32_t addr);
        SourceInfo dis_program(const Program& program);

        void setVerbose(const bool v);
        bool getVerbose(void) const;
};

#endif /*__DISASSEMBLER_HPP*/
