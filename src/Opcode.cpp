/*
 * OPCODE
 * Opcodes for SMIPS
 *
 * Stefan Wong 2019
 */

#include "Opcode.hpp"

Opcode::Opcode()
{
    this->instr = 0;
    this->mnemonic = "\0";
}

Opcode::Opcode(const uint32_t c, const std::string& m)
{
    this->instr = c;
    this->mnemonic = m;
}


// ========= OPCODE TABLE ======== //
OpcodeTable::OpcodeTable() {} 

OpcodeTable::~OpcodeTable() {} 

void OpcodeTable::add(const Opcode& op)
{
    this->op_list.push_back(op);
}

Opcode& OpcodeTable::get(const std::string& mnemonic) 
{
    for(unsigned int o = 0; o < this->op_list.size(); ++o)
    {
        if(mnemonic == this->op_list[o].mnemonic)
            return this->op_list[o];
    }

    return this->null_op;
}

Opcode& OpcodeTable::get(const uint32_t opcode) 
{
    for(unsigned int o = 0; o < this->op_list.size(); ++o)
    {
        if(opcode == this->op_list[o].instr)
            return this->op_list[o];
    }

    return this->null_op;
}

Opcode& OpcodeTable::getIdx(const int idx)
{
    if(idx < (int) this->op_list.size())
        return this->op_list[idx];
    return this->null_op;
}

unsigned int OpcodeTable::size(void) const
{
    return this->op_list.size();
}
