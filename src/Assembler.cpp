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


// Instruction Code stuff
InstrCode::InstrCode()
{
    this->code = 0;
    this->type = InstrType::INSTR_NULL;
}

InstrCode::InstrCode(const uint8_t c, const InstrType& t)
{
    this->code = c;
    this->type = t;
}

bool InstrCode::operator==(const InstrCode& that) const
{
    if(this->code != that.code)
        return false;
    if(this->type != that.type)
        return false;
    return true;
}

bool InstrCode::operator!=(const InstrCode& that) const
{
    if(this->code == that.code)
        return false;
    if(this->type == that.type)
        return false;
    return true;
}

std::string InstrCode::toString(void) const
{
    std::ostringstream oss;

    oss << std::hex << std::setw(2) << std::setfill('0') <<
        "0x" << this->code << " ";
    oss << "[";
    switch(this->type)
    {
        case InstrType::INSTR_R:
            oss << "R";
            break;
        case InstrType::INSTR_I:
            oss << "I";
            break;
        case InstrType::INSTR_J:
            oss << "J";
            break;
        default:
            oss << "NULL";
            break;
    }
    oss << " type]";

    return oss.str();
}

// Code table
InstrTable::InstrTable()
{
    // register instruction table
    this->r_instrs[0]  = InstrCode(32, InstrType::INSTR_R);  // ADD
    this->r_instrs[1]  = InstrCode(33, InstrType::INSTR_R);  // ADDU
    this->r_instrs[2]  = InstrCode(34, InstrType::INSTR_R);  // SUB
    this->r_instrs[3]  = InstrCode(35, InstrType::INSTR_R);  // SUBU
    this->r_instrs[4]  = InstrCode(36, InstrType::INSTR_R);  // AND
    this->r_instrs[5]  = InstrCode(37, InstrType::INSTR_R);  // OR
    this->r_instrs[6]  = InstrCode(39, InstrType::INSTR_R);  // NOR
    this->r_instrs[7]  = InstrCode(42, InstrType::INSTR_R);  // SLT
    this->r_instrs[8]  = InstrCode(43, InstrType::INSTR_R);  // SLTU
    this->r_instrs[9]  = InstrCode(0 , InstrType::INSTR_R);  // SLL
    this->r_instrs[10] = InstrCode(2 , InstrType::INSTR_R);  // SRL
    this->r_instrs[11] = InstrCode(8 , InstrType::INSTR_R);  // JR

    // immediate instruction table
    this->i_instrs[0]  = InstrCode(4,  InstrType::INSTR_I);  // BEQ
    this->i_instrs[1]  = InstrCode(5 , InstrType::INSTR_I);  // BNE
    this->i_instrs[2]  = InstrCode(8 , InstrType::INSTR_I);  // ADDI
    this->i_instrs[3]  = InstrCode(9 , InstrType::INSTR_I);  // ADDIU
    this->i_instrs[4]  = InstrCode(12, InstrType::INSTR_I);  // ANDI
    this->i_instrs[5]  = InstrCode(13, InstrType::INSTR_I);  // ORI
    this->i_instrs[6]  = InstrCode(10, InstrType::INSTR_I);  // SLTI
    this->i_instrs[7]  = InstrCode(11, InstrType::INSTR_I);  // SLTIU
    this->i_instrs[8]  = InstrCode(15, InstrType::INSTR_I);  // LUI
    this->i_instrs[9]  = instrcode(35, instrtype::instr_i);  // lw
    this->i_instrs[10] = instrcode(43, instrtype::instr_i);  // sw

    // jump instructions
    this->j_instrs[0]  = InstrCode(2, InstrType::INSTR_J);  // J
    this->j_instrs[1]  = InstrCode(3, InstrType::INSTR_J);  // JAL
}


// ======== Assembler ======= //
Assembler::Assembler() 
{
    this->verbose = false;
    this->num_err = 0;
}


int Assembler::reg_lookup(const TokenType& type, const int val) const
{
    switch(type)
    {
        case SYM_REG_GLOBAL:
            break;

        default:
            if(this->verbose)
            {
                std::cout << "[" << __func__ << "] " << type << 
                    " not a valid register token type" << std::endl;
            }
            return -1;
    }

    // if we somehow get here its obviously a problem
    return -1;
}


// ==== Instruction Assembly ==== //

void Assembler::asm_add(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0x20;
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

    instr.ins = 0x18;
    instr.ins = instr.ins | (l.args[0] << 21);
    instr.ins = instr.ins | (l.args[1] << 16);
    instr.adr = l.addr;
    this->program.add(instr);

}

void Assembler::asm_or(const LineInfo& l)
{
    Instr instr;

    instr.ins = 0x25;
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

void Assembler::asm_sub(const LineInfo& l)
{

}

void Assembler::asm_subu(const LineInfo& l)
{

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
