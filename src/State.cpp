/*
 * STATE
 * CPU State
 *
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include "State.hpp"


State::State()
{
    this->init_reg();
}

State::State(const State& that)
{
    this->pc = that.pc;
    this->addr = that.addr;
    this->instr = that.instr;
    this->id_ex = that.id_ex;
    this->ex_mem = that.ex_mem;
    this->mem_wb = that.mem_wb;

    for(int i = 0; i < 32; ++i)
        this->reg[i] = that.reg[i];
}

void State::init_reg(void)
{
    this->pc = 0;       // TODO : should actually be START_ADDR
    this->addr = 0;

    // pipeline reg
    this->instr = 0;
    this->id_ex = 0;
    this->ex_mem = 0;
    this->mem_wb = 0;
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
    this->instr = this->mem[this->pc];
    this->pc += 4;
}

void State::decode(void)
{
    uint8_t rs;
    uint8_t rt;
    uint8_t rd;
    uint8_t shamt;
    uint8_t op_bits;

    op_bits = (this->instr & 0xFC000000) >> 26;
    // R-instruction
    if(op_bits == 0x0)
    {
        rs = (this->instr & 0x03E00000) >> 21;
        rt = (this->instr & 0x001F0000) >> 16;
        rd = (this->instr & 0x0000F700) >> 11;
        shamt = (this->instr & 0x000003E0) >> 6;

        std::cout << "[" << __func__ << "] rs    : " << unsigned(rs) << std::endl;
        std::cout << "[" << __func__ << "] rt    : " << unsigned(rt) << std::endl;
        std::cout << "[" << __func__ << "] rd    : " << unsigned(rd) << std::endl;
        std::cout << "[" << __func__ << "] shamt : " << unsigned(shamt) << std::endl;
    }

    //switch(op_bits)
    //{
    //    // J-instructions
    //    case 0x2:
    //    case 0x3:
    //        //return dis_j_instr(instr, addr);
    //        break;
    //        
    //    // R-instructions 
    //    case 0x0:
    //        //return dis_r_instr(instr, addr);
    //        break;

    //    // I-instructions
    //    default:
    //        //return dis_i_instr(instr, addr);
    //        break;
    //}
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
