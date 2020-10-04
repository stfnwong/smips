/*
 * STATE
 * CPU State
 *
 */

#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "Common.hpp"
#include "Register.hpp"
#include "State.hpp"

// ======== DATA CACHE ======== //

DataCache::DataCache()
{
    this->mem = new uint8_t[SMIPS_MEM_SIZE];
    this->mem_size = SMIPS_MEM_SIZE;
}

DataCache::DataCache(unsigned int size)
{
    this->mem = new uint8_t[size];
    this->mem_size = size;
}

DataCache::DataCache(const DataCache& that)
{
    this->mem_size = that.mem_size;
    this->mem = new uint8_t[this->mem_size];
    
    std::memcpy(this->mem, that.mem, sizeof(uint8_t) * this->mem_size);
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
State::State() : mem(SMIPS_MEM_SIZE), verbose(false)
{
    this->init_reg();
}

State::State(unsigned int mem_size) : mem(mem_size), verbose(false)
{
    this->init_reg();
}

/*
 * copy ctor
 */
State::State(const State& that)
{
    this->pc      = that.pc;
    this->instr   = that.instr;
    this->op_bits = that.op_bits;
    this->func    = that.func;
    this->rs      = that.rs;
    this->rt      = that.rt;
    this->rd      = that.rd;
    this->shamt   = that.shamt;
    this->imm     = that.imm;
    this->tmp     = that.tmp;
    this->hi      = that.hi;
    this->lo      = that.lo;

    this->mem_data = that.mem_data;
    this->mem_addr = that.mem_addr;

    for(int i = 0; i < 32; ++i)
        this->reg[i] = that.reg[i];

    this->mem = that.mem;
}

/*
 * init_reg()
 */
void State::init_reg(void)
{
    this->pc       = 0;       // TODO : should actually be START_ADDR
    // pipeline reg
    this->instr    = 0;
    this->op_bits  = 0;
    this->func     = 0;
    this->rs       = 0;
    this->rt       = 0;
    this->rd       = 0;
    this->shamt    = 0;
    this->imm      = 0;
    this->tmp      = 0;
    this->hi       = 0;
    this->lo       = 0;
    this->mem_data = 0;
    this->mem_addr = 0;

    // register file reg
    for(int i = 0; i < 32; ++i)
        this->reg[i] = 0;
}

/*
 * zero_mem()
 */
void State::zero_mem(void)
{
    for(int i = 0; i < SMIPS_MEM_SIZE; ++i)
        this->mem[i] = 0;
}

// ================ PIPELINE ================ //
/*
 * fetch()
 */
void State::fetch(void)
{
    this->instr = 0x0;
    this->instr = this->instr | (this->mem[this->pc+0] << 24);
    this->instr = this->instr | (this->mem[this->pc+1] << 16);
    this->instr = this->instr | (this->mem[this->pc+2] << 8);
    this->instr = this->instr | (this->mem[this->pc+3] << 0);
    this->pc += 4;
}

/*
 * decode()
 */
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
            }
            break;

        // J-instruction
        case 0x2:
        case 0x3:
            {
                this->imm = (this->instr & 0x03FFFFFF);
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

/*
 * execute()
 */
void State::execute(void)
{
    int64_t t;
    // R-instructions
    // TODO ; this should be implemented so that the memory stage can be 
    // seperated out...
    if(this->op_bits == 0x0)
    {
        switch(this->func)
        {
            case R_SLL:     // R[$rd] <- R[$rt] << shamt
                this->alu = this->reg[this->rt] << this->shamt;
                break;

            case R_SRL:     // R[$rd] <- R[$rt] >> shamt
                this->alu = this->reg[this->rt] >> this->shamt;
                break;

            case R_SRA:     // R[$rd] <- R[$st] >> shamt (signed)
                this->alu = unsigned(this->reg[this->rt] >> this->shamt);
                break;

            case R_SLLV:    // R[$rd] <- R[$rt] << R[$rs]
                this->alu = (this->reg[this->rt] << this->reg[this->rs]);
                break;

            case R_SRLV:    // R[$rd] <- R[$rt] >> R[$rs]
                this->alu = unsigned(this->reg[this->rt] >> this->reg[this->rs]);
                break;

            case R_SRAV:    // R[$rd] <- R[$rt] >> R[$rs]   (signed)
                this->alu = (this->reg[this->rt] >> this->reg[this->rs]);
                break;

            case R_JR:      // PC <- R[$rs]
                this->pc = this->reg[this->rs];
                break;

            case R_JALR:    // 
                this->tmp = this->reg[this->rs];
                this->alu = this->pc + 8;
                this->pc = this->tmp;
                break;

            case R_SYSCALL:
                std::cout << "[" << __func__ << "] SYSCALL not yet implemented" << std::endl;
                break;

            case R_MFHI:    // R[$rd] <- HI
                this->alu = this->hi;
                break;

            case R_MTHI:    // HI <- R[$rs]
                this->hi = this->reg[this->rs];
                break;

            case R_MFLO:    // R[$rd] <- LO
                this->alu = this->lo;
                break;

            case R_MTLO:    // LO <- R[$rs]
                this->lo = this->reg[this->rs];
                break;

            case R_MULT:    // {HI, LO} <- R[$rs] * R[$rt]
                t = this->reg[this->rs] * this->reg[this->rt];
                this->hi = (t & 0xFFFFFFFF00000000) >> 32;
                this->lo = (t & 0x00000000FFFFFFFF);
                break;

            case R_MULTU:   // {HI, LO} <- R[$rs] * R[$rt]
                t = unsigned(this->reg[this->rs]) * unsigned(this->reg[this->rt]);
                this->hi = (t & 0xFFFFFFFF00000000) >> 32;
                this->lo = (t & 0x00000000FFFFFFFF);
                break;

            case R_DIV:     // LO <- R[$rs] / R[$rt], HI <- R[$rs] % R[$rt] (signed)
                this->lo = this->reg[this->rs] / this->reg[this->rt];
                this->hi = this->reg[this->rs] % this->reg[this->rt];
                break;

            case R_DIVU:    // LO <- R[$rs] / R[$rt], HI <- R[$rs] % R[$rt] (unsigned)
                this->lo = unsigned(this->reg[this->rs]) / unsigned(this->reg[this->rt]);
                this->hi = unsigned(this->reg[this->rs]) % unsigned(this->reg[this->rt]);
                break;

            case R_ADD:     // R[$rd] <- R[$rs] + R[$rt]    (signed)
                this->alu = this->reg[this->rs] + this->reg[this->rt];
                break;

            case R_ADDU:    // R[$rd] <- R[$rs] + R[$rt] (unsigned)
                this->alu = unsigned(this->reg[this->rs]) + unsigned(this->reg[this->rt]);
                break;

            case R_SUB:     // R[$rd] <- R[$rs] - R[$rt]    (signed)
                this->alu = this->reg[this->rs] - this->reg[this->rt];
                break;

            case R_SUBU:    // R[$rd] <- R[$rs] - R[$rt]    (unsigned)
                this->alu = unsigned(this->reg[this->rs]) - unsigned(this->reg[this->rt]);
                break;

            case R_AND:     // R[$rd] <- R[$rs] & R[$rt]
                this->alu = this->reg[this->rs] & this->reg[this->rt];
                break;

            case R_OR:      // R[$rd] <- R[$rs] | R[$rt]
                this->alu = this->reg[this->rs] | this->reg[this->rt];
                break;

            case R_XOR:     // R[$rd] <- R[$rs] ^ R[$rt]
                this->alu = this->reg[this->rs] ^ this->reg[this->rt];
                break;

            case R_NOR:     // R[$rd] <- !(R[$rs] | R[$rt])
                this->alu = !(this->reg[this->rs] | this->reg[this->rt]);
                break;

            case R_SLT:     // R[$rd] <- R[$rs] < R{$rt]
                this->alu = (this->reg[this->rs] < this->reg[this->rt]) ? 1 : 0;
                break;

            case R_SLTU:     // R[$rd] <- R[$rs] < R{$rt] (unsigned)
                this->alu = (unsigned(this->reg[this->rs]) < unsigned(this->reg[this->rt])) ? 1 : 0;
                break;


            default:
                break;      // No-op
        }
    }

    // J-instructions 
    // TODO: have a decode table or something rather than testing here...
    else if(this->op_bits == 0x2 || this->op_bits == 0x3)
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
        switch(this->op_bits)
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

            case I_LUI: // R[$rt] <- {imm, 0x0000}
                this->reg[this->rt] = (this->imm << 16);
                break;

            default:        // Noop
                break;
        }
    }

}

/*
 * memory()
 */
void State::memory(void)
{
    // R-type instructions do nothing during this cycle
    switch(this->op_bits)
    {
        case I_SB:
            this->mem[this->reg[this->rs + (this->imm & 0xFFFF)]] = this->reg[this->rt] & 0xFF; 
            break;

        case I_SH:
            this->mem[this->reg[this->rs + (this->imm & 0xFFFF)]] = this->reg[this->rt] & 0xFF;
            this->mem[this->reg[this->rs + (this->imm & 0xFFFF)+1]] = ((this->reg[this->rt] & 0xFF00) >> 8);
            break;

        case I_SW:
            // TODO : check endianness here...
            this->mem[this->reg[this->rs + (this->imm & 0xFFFF)]] = this->reg[this->rt] & 0xFF;
            this->mem[this->reg[this->rs + (this->imm & 0xFFFF)+1]] = ((this->reg[this->rt] & 0xFF00) >> 8);
            this->mem[this->reg[this->rs + (this->imm & 0xFFFF)+2]] = ((this->reg[this->rt] & 0xFF0000) >> 16);
            this->mem[this->reg[this->rs + (this->imm & 0xFFFF)+3]] = ((this->reg[this->rt] & 0xFF000000) >> 24);
            break;

        case I_LB:  // sign extend to 8 bits
            this->reg[this->rt] = (this->mem[this->reg[this->rs] + (this->imm & 0x0000FFFF)]) & 0xFF;
            break;

        case I_LH:  // sign extend to 16 bits
            this->reg[this->rt] = (this->mem[this->reg[this->rs] + (this->imm & 0x0000FFFF)]) & 0xFFFF;
            break;

        case I_LW:  // R[$rt] <- Mem4b(R[$rs] + imm16)
            // TODO: we can almost certainly implement a faster version of this instruction
            this->reg[this->rt] = 0;
            this->reg[this->rt] |= (this->mem[this->reg[this->rs] + (this->imm & 0xFFF) + 0] << 24);
            this->reg[this->rt] |= (this->mem[this->reg[this->rs] + (this->imm & 0xFFF) + 1] << 16);
            this->reg[this->rt] |= (this->mem[this->reg[this->rs] + (this->imm & 0xFFF) + 2] << 8);
            this->reg[this->rt] |= (this->mem[this->reg[this->rs] + (this->imm & 0xFFF) + 3] << 0);
            break;

        default:
            std::cout << "[" << __func__ << "] no operation for op_bits: 0x" 
                << std::hex << std::setw(5) << std::setfill('0') 
                << this->op_bits << std::endl;
            break;
    }
}

/*
 * write_back()
 */
void State::write_back(void)
{
    // R-instructions
    if(this->op_bits == 0)
    {
        switch(this->func)
        {
            case R_SYSCALL:
                break;

            default:
                this->reg[this->rd] = this->alu;    
                break;
        }

    }
    else if(this->op_bits == 0x2 || this->op_bits == 0x3)
    {
        std::cout << "[" << __func__ << "] TODO: J instruction write back here" << std::endl;
    }
    // I-instructions
    else
    {
        switch(this->func)
        {
            case I_SB:
            case I_SH:
            case I_SW:
                break;

            default:
                this->reg[this->rt] = alu;
                break;
        }
    }
}


// ================ MEMORY ================ //
void State::writeMem(const std::vector<uint8_t>& data, unsigned int offset)
{
    if(offset + data.size() > SMIPS_MEM_SIZE)
        return;

    for(unsigned int i = 0; i < data.size(); ++i)
        this->mem[offset + i] = data[i];
}

uint8_t& State::readMem(unsigned int idx) 
{
    return this->mem[idx];
}

/*
 * clearMem()
 */
void State::clearMem(void)
{
    this->zero_mem();
}

/*
 * tick()
 */
void State::tick(void)
{
    // run the pipeline for one cycle
    this->fetch();
    this->decode();
    this->execute();
    // TODO: write back?
}

/*
 * reset()
 */
void State::reset(void)
{
    this->init_reg();
}



std::string State::toString(void) const
{
    std::ostringstream oss;

    oss << "---------------------------------------------------------------------" << std::endl;
    oss << "PC         Instr   " << std::endl;
    oss << "0x" << std::hex << std::setw(8) << std::setfill('0') << this->pc << " ";
    oss << "0x" << std::hex << std::setw(8) << std::setfill('0') << this->instr << " ";
   

    return oss.str();
}
