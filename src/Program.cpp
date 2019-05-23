/*
 * PROGRAM
 * Represents a binary program
 *
 * Stefan Wong 2019
 */

#include "Program.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
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


// disk operations
int Program::save(const std::string& filename) 
{
    uint32_t N;
    std::ofstream outfile;

    try{
        outfile.open(filename, std::ios::binary);
    }
    catch(std::ios_base::failure& e) {
        std::cerr << "[" << __func__ << "] " << e.what() << std::endl;
        return -1;
    }

    N = (uint32_t) this->instructions.size();
    outfile.write(reinterpret_cast<char*>(&N), sizeof(uint32_t));

    outfile.write(
            reinterpret_cast<char*>(&this->instructions[0].adr),
            sizeof(uint32_t)
    );
    for(unsigned int idx = 0; idx < this->instructions.size(); ++idx)
    {
        outfile.write(
                reinterpret_cast<char*>(&this->instructions[idx].ins),
                sizeof(uint32_t)
        );
    }
    outfile.close();

    return 0;
}

int Program::load(const std::string& filename) 
{
    std::ifstream infile;
    uint32_t num_records;
    uint32_t addr;

    this->instructions.clear();

    try {
        infile.open(filename, std::ios::binary);
    }
    catch(std::ios_base::failure& e) {
        std::cerr << "[" << __func__ << "] " << e.what() << std::endl;
    }

    // find how many records are in the file
    infile.read(reinterpret_cast<char*>(&num_records), sizeof(uint32_t));
    if(num_records == 0)
    {
        std::cerr << "[" << __func__ << "] no records in file " << 
            filename << std::endl;
    }

    // load the first address pointer 
    infile.read(reinterpret_cast<char*>(&addr), sizeof(uint32_t));

    Instr instr;
    for(unsigned int idx = 0; idx < num_records; ++idx)
    {
        infile.read(
                reinterpret_cast<char*>(&instr.ins), 
                sizeof(uint32_t)
        );
        instr.adr = addr;
        addr++;
        this->instructions.push_back(instr);
    }
    infile.close();

    return 0;
}
