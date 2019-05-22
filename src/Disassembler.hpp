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


class Disassembler
{
    private:
        bool verbose;

    private:
        Program    program;
        SourceInfo source;
        LineInfo   cur_line;
        uint32_t   cur_instr;

    // instruction disassembly
    private:
        void dis_add(void);


    public:
        Disassembler();


        void disassemble(void);
        int  load(const std::string& filename);

        void setVerbose(const bool v);
        bool getVerbose(void) const;
};

#endif /*__DISASSEMBLER_HPP*/
