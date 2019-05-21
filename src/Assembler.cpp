/*
 * ASSEMBLER
 * SMIPS Assembler object
 * 
 * Stefan Wong 2019
 */

#include <iostream>
#include "Assembler.hpp"
#include "Lexer.hpp"
#include "Codes.hpp"

Assembler::Assembler() 
{
    this->verbose = false;
    this->num_err = 0;
}






// ==== Instruction Assembly ==== //

void Assembler::asm_add(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0x20 << 26;
    instr.ins = instr.ins | (l.args[0] << 21);
    instr.ins = instr.ins | (l.args[1] << 16);
    instr.ins = instr.ins | (l.args[2] << 11);
    instr.adr = l.addr;
    this->program.add(instr);
}

void Assembler::asm_addi(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0x08 << 26;
    instr.ins = instr.ins | (l.args[0] << 21);
    instr.ins = instr.ins | (l.args[1] << 16);
    instr.ins = instr.ins | (l.args[2]);
    instr.adr = l.addr;
    this->program.add(instr);
}

void Assembler::asm_addu(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0x21 << 26;
    instr.ins = instr.ins | (l.args[0] << 21);
    instr.ins = instr.ins | (l.args[1] << 16);
    instr.ins = instr.ins | (l.args[2] << 11);
    instr.adr = l.addr;
    this->program.add(instr);
}

void Assembler::asm_lw(const LineInfo& l)
{
    // R format
    Instr instr;

    instr.ins = 0x23 << 26;
    instr.ins = instr.ins | (l.args[0] << 21);
    instr.ins = instr.ins | (l.args[1] << 16);
    instr.adr = l.addr;
    this->program.add(instr);
}

void Assembler::asm_mult(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0x18 << 26;
    instr.ins = instr.ins | (l.args[0] << 21);
    instr.ins = instr.ins | (l.args[1] << 16);
    instr.adr = l.addr;
    this->program.add(instr);

}

void Assembler::asm_or(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0x25 << 26;
    instr.ins = instr.ins | (l.args[0] << 21);
    instr.ins = instr.ins | (l.args[1] << 16);
    instr.ins = instr.ins | (l.args[2] << 11);
    instr.adr = l.addr;
    this->program.add(instr);
}

void Assembler::asm_ori(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0x0C << 26;
    instr.ins = instr.ins | (l.args[0] << 21);
    instr.ins = instr.ins | (l.args[1] << 16);
    instr.ins = instr.ins | (l.args[2]);
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
