/*
 * DISASSEMBLER
 * Reverse a SMIPS binary into a SourceInfo
 *
 * Stefan Wong 2019
 */

#include <iostream>
#include "Disassembler.hpp"


Disassembler::Disassembler()
{
    this->verbose = false;
}


// instruction disassembly



void Disassembler::disassemble(void)
{
    unsigned int i;
    //uint32_t instr_mask;
    Instr instr;


    for(i = 0; i < this->program.size(); ++i)
    {
        std::cout << "[" << __func__ << "] would diassemble instr " << i << " here" << std::endl;
    }
}



int Disassembler::load(const std::string& filename)
{
    return this->program.load(filename);
}




void Disassembler::setVerbose(const bool v)
{
    this->verbose = v;
}

bool Disassembler::getVerbose(void) const
{
    return this->verbose;
}
