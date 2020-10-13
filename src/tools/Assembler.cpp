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
#include "Common.hpp"


// ======== Assembler ======= //
Assembler::Assembler() 
{
    this->verbose = false;
    this->num_err = 0;
    this->init_instr_to_code_map();
}

void Assembler::init_instr_to_code_map(void)
{
    // R-format instructions 
    this->instr_to_code[LEX_ADD]   = 0x20;
    this->instr_to_code[LEX_ADDU]  = 0x21;
    this->instr_to_code[LEX_AND]   = 0x24;
    this->instr_to_code[LEX_JR]    = 0x08;
    this->instr_to_code[LEX_JALR]  = 0x09;
    this->instr_to_code[LEX_JR]    = 0x08;
    this->instr_to_code[LEX_MULT]  = 0x18;
    this->instr_to_code[LEX_MULTU] = 0x19;
    this->instr_to_code[LEX_OR]    = 0x25;
    this->instr_to_code[LEX_SLL]   = 0x00;
    this->instr_to_code[LEX_SRL]   = 0x02;
    this->instr_to_code[LEX_SLT]   = 0x2A;
    this->instr_to_code[LEX_SLTU]  = 0x2B;
    this->instr_to_code[LEX_SUB]   = 0x22;
    this->instr_to_code[LEX_SUBU]  = 0x23;
    this->instr_to_code[LEX_DIV]   = 0x1A;
    this->instr_to_code[LEX_DIVU]  = 0x1B;
    this->instr_to_code[LEX_MFHI]  = 0x10;
    this->instr_to_code[LEX_MTHI]  = 0x11;
    this->instr_to_code[LEX_MFLO]  = 0x12;
    this->instr_to_code[LEX_MTLO]  = 0x13;
    this->instr_to_code[LEX_XOR]   = 0x26;

    
    // I-format instructions
    this->instr_to_code[LEX_ADDI]  = 0x08;
    this->instr_to_code[LEX_ADDIU] = 0x09;
    this->instr_to_code[LEX_BEQ]   = 0x04;
    this->instr_to_code[LEX_BNE]   = 0x05;
    this->instr_to_code[LEX_LW]    = 0x23;
    this->instr_to_code[LEX_LUI]   = 0x0F;
    this->instr_to_code[LEX_ORI]   = 0x0D;
    this->instr_to_code[LEX_SW]    = 0x2B;
    this->instr_to_code[LEX_ANDI]  = 0x0C;
    this->instr_to_code[LEX_XORI]  = 0x0E;

    // J-format instructions 
    this->instr_to_code[LEX_J]    = 0x02;
    this->instr_to_code[LEX_JAL]  = 0x03;
}

/*
 * asm_r_instr()
 * Assemble the arguments for an R-format instruction
 */
Instr Assembler::asm_r_instr(const TextInfo& l, const int n) 
{
    Instr instr;

    instr.ins = instr.ins | this->instr_to_code[l.opcode.instr];
    instr.ins = instr.ins | ((l.args[0].val & 0x1F) << R_INSTR_RD_OFFSET);
    instr.ins = instr.ins | ((l.args[1].val & 0x1F) << R_INSTR_RS_OFFSET);
    instr.ins = instr.ins | ((l.args[2].val & 0x1F) << R_INSTR_RT_OFFSET);

    return instr;
}

/*
 * asm_r_instr_rs_rt()
 * Assemble the arguments for an R-format instruction
 * which doesn't have an $rd operand
 */
Instr Assembler::asm_r_instr_rs_rt(const TextInfo& l)
{
    Instr instr;

    instr.ins = instr.ins | this->instr_to_code[l.opcode.instr];
    instr.ins = instr.ins | ((l.args[0].val & 0x1F) << R_INSTR_RS_OFFSET);
    instr.ins = instr.ins | ((l.args[1].val & 0x1F) << R_INSTR_RT_OFFSET);

    return instr;
}

/*
 * asm_r_instr_rd()
 * Assemble the arguments for an R-format instruction with only $rd
 */
Instr Assembler::asm_r_instr_rd(const TextInfo& l)
{
    Instr instr;

    instr.ins = instr.ins | this->instr_to_code[l.opcode.instr];
    instr.ins = instr.ins | ((l.args[0].val & 0x1F) << R_INSTR_RD_OFFSET);

    return instr;
}


/*
 * asm_r_instr_rs()
 * Assemble the arguments for an R-format instruction with only $rs
 */
Instr Assembler::asm_r_instr_rs(const TextInfo& l)
{
    Instr instr;

    instr.ins = instr.ins | this->instr_to_code[l.opcode.instr];
    instr.ins = instr.ins | ((l.args[0].val & 0x1F) << R_INSTR_RS_OFFSET);

    return instr;
}


/*
 * asm_r_instr_shamt()
 * Assemble the arguments for an R-format instruction which accepts 
 * a shift argument.
 */
Instr Assembler::asm_r_instr_shamt(const TextInfo& l, const int n)
{
    Instr instr;

    instr.ins = instr.ins | this->instr_to_code[l.opcode.instr];
    instr.ins = instr.ins | ((l.args[0].val & 0x1F) << R_INSTR_RD_OFFSET);
    instr.ins = instr.ins | ((l.args[1].val & 0x1F) << R_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (l.args[2].val & 0x1F) << R_INSTR_SHAMT_OFFSET;

    return instr;
}

/*
 * asm_i_instr()
 * Assemble the arguments for an I-format instruction
 */
Instr Assembler::asm_i_instr(const TextInfo& l, const int n) 
{
    Instr instr;

    instr.ins = instr.ins | (this->instr_to_code[l.opcode.instr] << I_INSTR_OP_OFFSET);
    instr.ins = instr.ins | ((l.args[0].val & 0x1F) << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | ((l.args[1].val & 0x1F) << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | (l.args[2].val & 0xFFFF); 

    return instr;
}

/*
 * asm_i_instr_rt()
 * Assemble the arguments for an I-format instruction that 
 * only accepts $rt and imm
 */
Instr Assembler::asm_i_instr_rt(const TextInfo& l)
{
    Instr instr;

    instr.ins = instr.ins | (this->instr_to_code[l.opcode.instr] << I_INSTR_OP_OFFSET);
    instr.ins = instr.ins | ((l.args[0].val & 0x1F) << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (l.args[1].val & 0xFFFF);           // TODO : check if this should be index 1 or 2

    return instr;
}

/*
 * asm_i_instr_branch()
 * Assemble the arguments for an I-format branch instruction 
 * 
 */
Instr Assembler::asm_i_instr_branch(const TextInfo& l)
{
    Instr instr;


    return instr;
}

/*
 * asm_j_instr()
 * Assemble the arguments for a J-format instruction
 */
Instr Assembler::asm_j_instr(const TextInfo& l) 
{
    Instr instr;

    instr.ins = instr.ins | (this->instr_to_code[l.opcode.instr] << J_INSTR_OP_OFFSET);
    instr.ins = instr.ins | ((l.args[0].val & 0x0FFFFFFC) >> 2);

    return instr;
}

// ==== Instruction Assembly ==== //

/*
 * AssembleLine()
 * Transform a TextInfo object into an Instr object
 */
Instr Assembler::assembleText(const TextInfo& line)
{
    Instr instr;

    if(this->verbose)
    {
        std::cout << "[" << __func__ << "] assembling " << line.opcode.toString() 
            << " which has code [" << std::hex << unsigned(this->instr_to_code[line.opcode.instr])
            << "]" << std::endl;
        std::cout << "[" << __func__ << "] " << line.toInstrString() << std::endl;
    }

    switch(line.opcode.instr)
    {
        // R-format instructions
        case LEX_ADD:
        case LEX_ADDU:
        case LEX_AND:
        case LEX_JR:
        case LEX_OR:
        case LEX_SLT:
        case LEX_SLTU:
        case LEX_SUB:
        case LEX_SUBU:
        case LEX_XOR:
            instr = this->asm_r_instr(line, 3);
            break;

        case LEX_SLL:
        case LEX_SRL:
            instr = this->asm_r_instr_shamt(line, 3);
            break;

        case LEX_DIV:
        case LEX_DIVU:
        case LEX_MULT:
        case LEX_MULTU:
            instr = this->asm_r_instr_rs_rt(line);
            break;

        case LEX_MFHI:
        case LEX_MFLO:
            instr = this->asm_r_instr_rd(line);
            break;

        case LEX_MTHI:
        case LEX_MTLO:
            instr = this->asm_r_instr_rs(line);
            break;

        // I-format instructions 
        case LEX_ADDI:
        case LEX_ADDIU:
        case LEX_ANDI:
        case LEX_BEQ:
        case LEX_BNE:
        case LEX_LW:
        case LEX_ORI:
        case LEX_XORI:
        case LEX_SW:
            instr = this->asm_i_instr(line, 3);
            break;

        case LEX_LUI:
            instr = this->asm_i_instr_rt(line);
            break;

        // J-format instructions 
        case LEX_J:
        case LEX_JAL:
            instr = this->asm_j_instr(line);
            break;

        default:
            // If we can't work out what we are assembling then emit a noop
            if(this->verbose)
            {
                std::cout << "[" << __func__ << "] (line " << 
                    std::dec << line.line_num << 
                    ") unknown opcode " << line.opcode.toString() << 
                    " inserting NOOP" << std::endl;
            }
            return Instr(line.addr, 0); 
    }
    instr.adr = line.addr;

    if(this->verbose)
        std::cout << "[" << __func__ << "] output instr: " << instr.toString() << std::endl;

    return instr;
}

// ======== DATA SEGMENT ASSEMBLY ======== //

/*
 * assembleData()
 */
DataSeg Assembler::assembleData(const DataInfo& data)
{
    DataSeg seg;

    switch(data.directive)
    {
        case SYM_DIR_NONE:
            if(this->verbose)
                std::cout << "[" << __func__ << "] got NONE directive" << std::endl;
            break;

        case SYM_DIR_ASCIIZ:
        case SYM_DIR_BYTE:
        case SYM_DIR_WORD:
            seg.adr = data.addr;
            seg.data = data.data;

        case SYM_DIR_SPACE:
            seg.adr = data.addr;
            for(unsigned int i = 0; i < data.space; ++i)
                seg.data.push_back(0);
            break;

        default:
            break;
    }

    return seg;
}


/*
 * assemble()
 * Peform assembly on each line in turn
 */
void Assembler::assemble(void)
{
    TextInfo cur_line;
    DataInfo cur_data;
    Instr    cur_instr;
    DataSeg  cur_seg;

    if(this->source.getNumLines() == 0)
    {
        if(this->verbose)
            std::cout << "[" << __func__ << "] no lines in source" << std::endl;
        return;
    }

    // assemble the data section
    this->program.init();
    for(unsigned int i = 0; i < this->source.getDataInfoSize(); ++i)
    {
        cur_data = this->source.getData(i);
        cur_seg  = this->assembleData(cur_data);
        this->program.add(cur_seg);
    }

    // assemble the text sections
    for(unsigned int i = 0; i < this->source.getTextInfoSize(); ++i)
    {
        cur_line = this->source.getText(i);
        cur_instr = this->assembleText(cur_line);
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
