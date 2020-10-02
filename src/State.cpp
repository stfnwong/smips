/*
 * STATE
 * CPU State
 *
 */

#include <iostream>
#include <iomanip>
#include <sstream>

#include "State.hpp"
#include "Common.hpp"


State::State()
{
    this->init_reg();
}

State::State(const State& that)
{
    this->pc      = that.pc;
    this->addr    = that.addr;
    this->instr   = that.instr;
    this->op_bits = that.op_bits;
    this->func    = that.func;
    this->rs      = that.rs;
    this->rt      = that.rt;
    this->rd      = that.rd;
    this->shamt   = that.shamt;
    this->imm     = that.imm;

    for(int i = 0; i < 32; ++i)
        this->reg[i] = that.reg[i];
}

void State::init_reg(void)
{
    this->pc      = 0;       // TODO : should actually be START_ADDR
    this->addr    = 0;
    // pipeline reg
    this->instr   = 0;
    this->op_bits = 0;
    this->func    = 0;
    this->rs      = 0;
    this->rt      = 0;
    this->rd      = 0;
    this->shamt   = 0;
    this->imm     = 0;
    // register file reg
    for(int i = 0; i < 32; ++i)
        this->reg[i] = 0;
}

void State::zero_mem(void)
{
    for(int i = 0; i < SMIPS_MEM_SIZE; ++i)
        this->mem[i] = 0;
}

// ================ PIPELINE ================ //
void State::fetch(void)
{
    this->instr = 0x0;
    this->instr = this->instr | (this->mem[this->pc+0] << 24);
    this->instr = this->instr | (this->mem[this->pc+1] << 16);
    this->instr = this->instr | (this->mem[this->pc+2] << 8);
    this->instr = this->instr | (this->mem[this->pc+3] << 0);
    this->pc += 4;

    // TODO : debug, remove 
    //std::cout << "[" << __func__ << "] instr: 0x" << std::hex 
    //    << std::setw(8) << std::setfill('0') << this->instr << std::endl;
}

void State::decode(void)
{
    this->op_bits = (this->instr & 0xFC000000) >> 26;
    // R-instruction
    switch(this->op_bits)
    {
        // R-instruction
        case 0x0:
            {
                this->rs = (this->instr & 0x03E00000) >> R_INSTR_RS_OFFSET;
                this->rt = (this->instr & 0x001F0000) >> R_INSTR_RT_OFFSET;
                this->rd = (this->instr & 0x0000F700) >> R_INSTR_RD_OFFSET;
                this->shamt = (this->instr & 0x000003E0) >> R_INSTR_SHAMT_OFFSET;
                this->func = (this->instr & 0x0000001F);

                std::cout << "[" << __func__ << "] rs    : " << std::dec << unsigned(this->rs) << std::endl;
                std::cout << "[" << __func__ << "] rt    : " << std::dec << unsigned(this->rt) << std::endl;
                std::cout << "[" << __func__ << "] rd    : " << std::dec << unsigned(this->rd) << std::endl;
                std::cout << "[" << __func__ << "] shamt : " << std::dec << unsigned(this->shamt) << std::endl;
            }
            break;

        // J-instruction
        case 0x2:
        case 0x3:
            {
                this->imm = (this->instr & 0x3FFFFFFF) << 2;    
            }
            break;

        // I-instructions
        default:
            {
                this->rs = (this->instr & 0x03E00000) >> I_INSTR_RS_OFFSET;
                this->rt = (this->instr & 0x001F0000) >> I_INSTR_RT_OFFSET;
                this->imm = (this->instr & 0x0000FFFF);
            }
            break;
    }
}

void State::execute(void)
{
    // R-instructions
    if(this->op_bits == 0x0)
    {
        switch(this->func)
        {
            case FUNC_ADD:
                this->reg[this->rd] = this->reg[this->rs] + this->reg[this->rt];
                break;

            case FUNC_ADDU:
                this->reg[this->rd] = unsigned(this->reg[this->rs]) + unsigned(this->reg[this->rt]);
                break;

            default:
                break;      // No-op
        }
    }
    // J-instructions 
    else if(this->op_bits > 0)
    {
        switch(this->op_bits)
        {
            case 0x2:   // j
                break;
            case 0x3:   // jal
                break;
        }
    }
    // I-instructions
    else
    {
        switch(this->func)
        {
            case 4:     // beq
                break;
        }
    }

}

// ================ MEMORY ================ //
void State::loadMem(const std::vector<uint8_t>& data, unsigned int offset)
{
    if(offset + data.size() > SMIPS_MEM_SIZE)
        return;

    for(unsigned int i = 0; i < data.size(); ++i)
        this->mem[offset + i] = data[i];
}

void State::tick(void)
{
    // run the pipeline for one cycle
    this->fetch();
    this->decode();
}


std::string State::toString(void) const
{
    std::ostringstream oss;

    oss << "---------------------------------------------------------------------" << std::endl;
    oss << "PC         Addr       Instr   " << std::endl;
    oss << "0x" << std::hex << std::setw(8) << std::setfill('0') << this->pc << " ";
    oss << "0x" << std::hex << std::setw(8) << std::setfill('0') << this->addr << " ";
    oss << "0x" << std::hex << std::setw(8) << std::setfill('0') << this->instr << " ";
   

    return oss.str();
}
