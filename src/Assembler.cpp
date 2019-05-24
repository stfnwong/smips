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

/*
 * val2Offset()
 * Convert register value to a memory offset
 */
int Assembler::val2Offset(const TokenType& type, const int val) const
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

/*
 * asm_r_instr()
 * Assemble the arguments for an R-format instruction
 */
uint32_t Assembler::asm_r_instr(const LineInfo& l, const int n) const
{
    uint32_t instr = 0;
    int reg;

    for(int i = 0; i < n; ++i)
    {
        reg   = this->val2Offset(l.type[i], l.val[i]);
        instr = instr | (reg << this->r_instr_offsets[i]);
    }

    return instr;
}

/*
 * asm_i_instr()
 * Assemble the arguments for an I-format instruction
 */
uint32_t Assembler::asm_i_instr(const LineInfo& l, const int n) const
{
    uint32_t instr = 0;
    int reg;

    for(int i = 0; i < n; ++i)
    {
        reg = this->val2Offset(l.type[i], l.val[i]);
        instr = instr | (reg << this->i_instr_offsets[i]);
    }

    return instr;
}

// ==== Instruction Assembly ==== //

/*
 * asm_add()
 * R-format
 */
Instr Assembler::asm_add(const LineInfo& l) const
{
    Instr instr;

    instr.ins = instr.ins | this->asm_r_instr(l, 3);
    instr.ins = instr.ins | 0x20;
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_addi()
 * I-format
 */
Instr Assembler::asm_addi(const LineInfo& l) const
{
    Instr instr;

    instr.ins = instr.ins | this->asm_i_instr(l, 3);
    instr.ins = instr.ins | (0x08 << 26);
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_addu()
 * R-format
 */
Instr Assembler::asm_addu(const LineInfo& l) const
{
    Instr instr;

    instr.ins = instr.ins | this->asm_r_instr(l, 3);
    instr.ins = instr.ins | 0x21;
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_lw()
 * I-format
 */
Instr Assembler::asm_lw(const LineInfo& l) const
{
    // I format
    Instr instr;

    instr.ins = instr.ins | this->asm_i_instr(l, 2);
    instr.ins = instr.ins | (l.val[1]);        // insert immediate
    instr.ins = instr.ins | (0x23 << this->i_instr_op_offset);
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_mult()
 * R-format
 */
Instr Assembler::asm_mult(const LineInfo& l) const
{
    Instr instr;

    instr.ins = instr.ins | this->asm_r_instr(l, 3);
    instr.ins = instr.ins | 0x18;
    instr.adr = l.addr;
    return instr;

}

/*
 * asm_or()
 * R-format
 */
Instr Assembler::asm_or(const LineInfo& l) const
{
    Instr instr;

    instr.ins = instr.ins | this->asm_r_instr(l, 3);
    instr.ins = instr.ins | 0x25;
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_ori()
 * I-format
 */
Instr Assembler::asm_ori(const LineInfo& l) const
{
    Instr instr;

    instr.ins = instr.ins | this->asm_i_instr(l, 2);
    instr.ins = instr.ins | (l.val[2]);
    instr.ins = instr.ins | (0x0C << this->i_instr_op_offset);
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_sub()
 * R-format
 */
Instr Assembler::asm_sub(const LineInfo& l) const
{
    Instr instr;

    instr.ins = instr.ins | this->asm_r_instr(l, 3);
    instr.ins = instr.ins | 0x22;
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_subu()
 * R-format
 */
Instr Assembler::asm_subu(const LineInfo& l) const
{
    Instr instr;

    instr.ins = instr.ins | this->asm_r_instr(l, 3);
    instr.ins = instr.ins | 0x23;
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_sw()
 * I-format
 */
Instr Assembler::asm_sw(const LineInfo& l) const
{
    Instr instr;

    instr.ins = 0x2B << this->i_instr_op_offset;
    instr.ins = instr.ins | this->asm_i_instr(l, 2);
    instr.ins = instr.ins | (l.val[1]);
    instr.adr = l.addr;

    return instr;
}


/*
 * AssembleLine()
 * Transform a LineInfo object into an Instr object
 */
Instr Assembler::assembleLine(const LineInfo& line)
{
    // TODO : debug, remove
    std::cout << std::endl;
    std::cout << "[" << __func__ << "] assembling line " << std::endl;
    std::cout << line.toString() << std::endl;

    switch(line.opcode.instr)
    {
        case LEX_ADD:
            return this->asm_add(line);
            break;

        case LEX_ADDI:
            return this->asm_addi(line);
            break;

        case LEX_ADDU:
            return this->asm_addu(line);
            break;

        case LEX_LW:
            return this->asm_lw(line);
            break;

        case LEX_MULT:
            return this->asm_mult(line);
            break;

        case LEX_OR:
            return this->asm_or(line);
            break;

        case LEX_ORI:
            return this->asm_ori(line);
            break;

        case LEX_SW:
            return this->asm_sw(line);
            break;

        default:
            if(this->verbose)
            {
                std::cout << "[" << __func__ << "] (line " << 
                    std::dec << line.line_num << 
                    ") unknown opcode " << line.opcode.toString() << std::endl;
            }
            return Instr(0, 0); // emit a null instruction
    }

}

// TODO : make an AssembleLine() and call in a loop?
void Assembler::assemble(void)
{
    LineInfo cur_line;
    Instr    cur_instr;

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
        cur_instr = this->assembleLine(cur_line);
        this->program.add(cur_instr);
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
