/*
 * STATE
 * CPU State
 *
 */

#include <iostream>
#include <iomanip>
#include <sstream>

#include "Common.hpp"
#include "Register.hpp"
#include "State.hpp"


DataCache::DataCache(unsigned int size)
{
    this->mem = new uint8_t[size];
    this->mem_size = size;
}

DataCache::~DataCache()
{
    delete[] this->mem;
}

/*
 * size()
 */
unsigned int DataCache::size(void) const
{
    return this->mem_size;
}

/*
 * clear()
 */
void DataCache::clear(void)
{
    for(unsigned int i = 0; i < this->mem_size; ++i)
        this->mem[i] = 0;
}

/*
 * []
 */
uint8_t& DataCache::operator[](const int i)
{
    return this->mem[i];
}



// ======== STATE ======== //
State::State()
{
    this->verbose = false;
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
                this->rs    = (this->instr & 0x03E00000) >> R_INSTR_RS_OFFSET;
                this->rt    = (this->instr & 0x001F0000) >> R_INSTR_RT_OFFSET;
                this->rd    = (this->instr & 0x0000F700) >> R_INSTR_RD_OFFSET;
                this->shamt = (this->instr & 0x000003E0) >> R_INSTR_SHAMT_OFFSET;
                this->func  = (this->instr & 0x0000003F);

                std::cout << "[" << __func__ << "] func: " << unsigned(this->func) << std::endl;
            }
            break;

        // J-instruction
        case 0x2:
        case 0x3:
            {
                this->imm = (this->instr & 0x3FFFFFFF);
            }
            break;

        // I-instructions
        default:
            {
                this->rs  = (this->instr & 0x03E00000) >> I_INSTR_RS_OFFSET;
                this->rt  = (this->instr & 0x001F0000) >> I_INSTR_RT_OFFSET;
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
            case R_SLL:     // R[$rd] <- R[$rt] << shamt
                this->reg[this->rd] = this->reg[this->rt] << this->shamt;
                break;

            case R_SRL:     // R[$rd] <- R[$rt] >> shamt
                this->reg[this->rd] = this->reg[this->rt] >> this->shamt;
                break;

            case R_SRA:     // R[$rd] <- R[$st] >> shamt (signed)
                this->reg[this->rd] = unsigned(this->reg[this->rt] >> this->shamt);

            case R_ADD:     // R[$rd] <- R[$rs] + R[$rt]
                this->reg[this->rd] = this->reg[this->rs] + this->reg[this->rt];
                break;

            case R_ADDU: // R[$rd] <- R[$rs] + R[$rt] (unsigned)
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
                this->pc += (this->imm << 2);
                break;
            case 0x3:   // jal
                this->reg[REG_RETURN] = this->pc;
                this->pc += (this->imm << 2);
                break;
        }
    }
    // I-instructions
    else
    {
        switch(this->func)
        {
            case I_BEQ:     
                if(this->reg[this->rs] == this->reg[this->rt])
                    this->pc += (this->imm << 2) + 4;
                break;

            case I_BNE:
                if(this->reg[this->rs] != this->reg[this->rt])
                    this->pc += (this->imm << 2) + 4;
                break;

            case I_BLEZ:
                if(this->reg[this->rs] <= 0)
                    this->pc += (this->imm << 2) + 4;
                break;

            case I_BGTZ:
                if(this->reg[this->rs] > 0)
                    this->pc += (this->imm << 2) + 4;
                break;

            case I_ADDI:    // R[$rt] <- R[$rs] + imm16s
                this->reg[this->rt] = this->reg[this->rs] + this->imm;
                break;

            case I_ADDIU:   /// R[$rt] <- R[$rs] + imm16u
                this->reg[this->rt] = unsigned(this->reg[this->rs] + this->imm);
                break;

            case I_SLTI:
                if(this->reg[this->rs] < this->imm)
                    this->reg[this->rt] = 1;
                else 
                    this->reg[this->rt] = 0;
                break;

            case I_SLTIU:
                if(unsigned(this->reg[this->rs]) < unsigned(this->imm))
                    this->reg[this->rt] = 1;
                else 
                    this->reg[this->rt] = 0;
                break;

            case I_ANDI:
                this->reg[this->rt] = this->reg[this->rs] & this->imm;
                break;
                
            case I_ORI:
                this->reg[this->rt] = this->reg[this->rs] | this->imm;
                break;

            case I_XORI:
                this->reg[this->rt] = this->reg[this->rs] ^ this->imm;
                break;

            case I_LUI:
                this->reg[this->rt] = (this->imm << 16);
                break;

            case I_LB:  // sign extend to 8 bits
                this->reg[this->rt] = (this->mem[this->reg[this->rs] + (this->imm & 0x0000FFFF)]) & 0xFF;
                break;

            case I_LH:  // sign extend to 16 bits
                this->reg[this->rt] = (this->mem[this->reg[this->rs] + (this->imm & 0x0000FFFF)]) & 0xFFFF;
                break;

            case I_LW:  // get 4 bytes starting at $s + i
                this->reg[this->rt] = this->mem[this->reg[this->rs]] + (this->imm & 0xFFFF);
                break;

            default:        // Noop
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
