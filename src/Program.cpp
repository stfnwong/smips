/*
 * PROGRAM
 * Represents a binary program
 *
 * Stefan Wong 2019
 */

#include "Program.hpp"
#include <iomanip>
#include <sstream>


Instr::Instr()
{
    this->init();
}

Instr::Instr(const uint32_t adr, const uint32_t ins)
{
    this->adr = adr;
    this->ins = ins;
}

bool Instr::operator==(const Instr& that) const
{
    if(this->adr != that.adr)
        return false;
    if(this->ins != that.ins)
        return false;

    return true;
}

bool Instr::operator!=(const Instr& that) const
{
    if(this->adr == that.adr)
        return false;
    if(this->ins == that.ins)
        return false;

    return true;
}

void Instr::init(void)
{
    this->adr = 0;
    this->ins = 0;
}


std::string Instr::toString(void) const
{
    std::ostringstream oss;

    oss << "0x" << std::hex << std::setfill('0') << std::setw(8) << this->adr;
    oss << " [" << std::setw(8) << std::setfill('0') << this->ins << "] ";

    return oss.str();
}


// ======== PROGRAM ======== //
Program::Program() {} 

void Program::init(void)
{
    this->instructions.clear();
}

void Program::add(const Instr& i)
{
    this->instructions.push_back(i);
}

Instr& Program::get(const unsigned int idx) 
{
    if(idx < this->instructions.size())
        return this->instructions[idx];

    return this->null_instr;
}

void Program::writeMem(const uint32_t addr, const uint32_t val)
{
    Instr ins;
    ins.adr = addr;
    ins.ins = val;
    this->instructions.push_back(ins);
}

unsigned int Program::size(void) const
{
    return this->instructions.size();
}

std::string Program::toString(void) const
{
    std::ostringstream oss;

    for(unsigned int i = 0; i < this->instructions.size(); ++i)
    {
        oss << "Instr " << std::setw(4) << std::dec << i << 
            " : " << this->instructions[i].toString() << std::endl;
    }

    return oss.str();
}
