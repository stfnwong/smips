/*
 * ASSEMBLER
 * SMIPS Assembler object
 * 
 * Stefan Wong 2019
 */

#include <sstream>
#include <iomanip>
#include <iostream>
#include "Assembler.hpp"
#include "Lexer.hpp"
#include "Codes.hpp"


// ======== Assembler ======= //
Assembler::Assembler() 
{
    this->verbose = false;
    this->num_err = 0;
}

int Assembler::arg2Offset(const TokenType& type, const int val) const
{
    int offset;

    switch(type)
    {
        case SYM_REG_ZERO:
            return 0;

        case SYM_REG_AT:
            return 1;

        case SYM_REG_RET:
            if((2 + val) > 3)
                return this->ARG_INVALID_OFFSET;
            return 2 + val;

        case SYM_REG_ARG:
            if(4 + val > 7)
                return this->ARG_INVALID_OFFSET;
            return 4 + val;

        case SYM_REG_TEMP:
            offset = 8 + val;
            if(offset > 7 && offset < 16)
                return offset;
            if(offset > 23 && offset < 26)
                return offset;
            return this->ARG_INVALID_OFFSET;

        case SYM_REG_SAVED:
            if(16 + val > 23)
                return this->ARG_INVALID_OFFSET;
            return 16 + val;

        case SYM_REG_GLOBAL:
            return 28;

        case SYM_REG_STACK:
            return 29;

        case SYM_REG_FRAME:
            return 30;

        case SYM_LITERAL:
            return val;

        default:
            std::cout << "[" << __func__  << "] not a register type" << std::endl;
            return this->ARG_INVALID_OFFSET;
    }

    // getting here is an error 
    return this->ARG_INVALID_OFFSET;
}

uint32_t Assembler::asm_r_instr(const LineInfo& l, const int n) const
{
    uint32_t instr = 0;
    int reg;

    for(int i = 0; i < n; ++i)
    {
        reg   = this->arg2Offset(l.types[i], l.args[i]);
        instr = instr | (reg << this->r_instr_offsets[i]);
    }

    return instr;
}

uint32_t Assembler::asm_i_instr(const LineInfo& l, const int n) const
{
    uint32_t instr = 0;
    int reg;

    for(int i = 0; i < n; ++i)
    {
        reg = this->arg2Offset(l.types[i], l.args[i]);
        instr = instr | (reg << this->i_instr_offsets[i]);
    }

    return instr;
}


// ==== Instruction Assembly ==== //

void Assembler::asm_add(const LineInfo& l)
{
    Instr instr;

    instr.ins = instr.ins | this->asm_r_instr(l, 3);
    instr.ins = instr.ins | 0x20;
    instr.adr = l.addr;
    this->program.add(instr);
}

void Assembler::asm_addi(const LineInfo& l)
{
    Instr instr;

    instr.ins = instr.ins | this->asm_i_instr(l, 3);
    instr.ins = instr.ins | (0x08 << 26);
    instr.adr = l.addr;
    this->program.add(instr);
}

void Assembler::asm_addu(const LineInfo& l)
{
    Instr instr;

    instr.ins = instr.ins | this->asm_r_instr(l, 3);
    instr.ins = instr.ins | 0x21;
    instr.adr = l.addr;
    this->program.add(instr);
}

void Assembler::asm_lw(const LineInfo& l)
{
    // I format
    Instr instr;

    instr.ins = instr.ins | this->asm_i_instr(l, 2);
    instr.ins = instr.ins | (0x23 << 26);
    instr.adr = l.addr;
    this->program.add(instr);
}

void Assembler::asm_mult(const LineInfo& l)
{
    Instr instr;

    instr.ins = instr.ins | this->asm_r_instr(l, 2);
    instr.ins = instr.ins | 0x18;
    instr.adr = l.addr;
    this->program.add(instr);

}

void Assembler::asm_or(const LineInfo& l)
{
    Instr instr;

    instr.ins = instr.ins | this->asm_r_instr(l, 3);
    instr.ins = instr.ins | 0x25;
    instr.adr = l.addr;
    this->program.add(instr);
}

void Assembler::asm_ori(const LineInfo& l)
{
    Instr instr;

    instr.ins = instr.ins | this->asm_i_instr(l, 3);
    instr.ins = instr.ins | (0x0C << 26);
    instr.adr = l.addr;
    this->program.add(instr);
}

void Assembler::asm_sub(const LineInfo& l)
{
    Instr instr;

    instr.ins = instr.ins | this->asm_r_instr(l, 3);
    instr.ins = instr.ins | 0x22;
    instr.adr = l.addr;
    this->program.add(instr);
}

void Assembler::asm_subu(const LineInfo& l)
{
    Instr instr;

    instr.ins = instr.ins | this->asm_r_instr(l, 3);
    instr.ins = instr.ins | 0x23;
    instr.adr = l.addr;
    this->program.add(instr);
}

void Assembler::asm_sw(const LineInfo& l)
{
    // I format
    Instr instr;

    instr.ins = 0x2B << 26;
    instr.ins = instr.ins | (l.args[0] << 21);
    instr.adr = l.addr;
    this->program.add(instr);
}


void Assembler::assemble(void)
{
    LineInfo cur_line;

    if(this->source.getNumLines() == 0)
    {
        if(this->verbose)
            std::cout << "[" << __func__ << "] no lines in source" << std::endl;
        return;
    }

    this->program.init();

    for(unsigned int i = 0; i < this->source.getNumLines(); ++i)
    {
        cur_line = this->source.get(i);

        switch(cur_line.opcode.instr)
        {
            case LEX_ADD:
                this->asm_add(cur_line);
                break;

            case LEX_ADDI:
                this->asm_addi(cur_line);
                break;

            case LEX_ADDU:
                this->asm_addu(cur_line);
                break;

            case LEX_LW:
                this->asm_lw(cur_line);
                break;

            case LEX_MULT:
                this->asm_mult(cur_line);
                break;

            case LEX_OR:
                this->asm_or(cur_line);
                break;

            case LEX_ORI:
                this->asm_ori(cur_line);
                break;

            case LEX_SW:
                this->asm_sw(cur_line);
                break;

            default:
                if(this->verbose)
                {
                    std::cout << "[" << __func__ << "] (line " << 
                        std::dec << cur_line.line_num << 
                        ") unknown opcode " << cur_line.opcode.toString() << std::endl;
                }
                this->num_err += 1;

                break;
        }
    }

}

void Assembler::setVerbose(const bool v)
{
    this->verbose = v;
}

bool Assembler::getVerbose(void) const
{
    return this->verbose;
}

int Assembler::getNumErr(void) const
{
    return this->num_err;
}

void Assembler::loadSource(const SourceInfo& s)
{
    this->source = s;
}

Program Assembler::getProgram(void) const
{
    return this->program;
}
