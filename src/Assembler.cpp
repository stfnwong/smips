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
    this->init_instr_to_code_map();
}

void Assembler::init_instr_to_code_map(void)
{
    // R-format instructions 
    this->instr_to_code[LEX_ADD]  = 0x20;
    this->instr_to_code[LEX_ADDU] = 0x21;
    this->instr_to_code[LEX_AND]  = 0x24;
    this->instr_to_code[LEX_JR]   = 0x08;
    this->instr_to_code[LEX_MULT] = 0x18;
    this->instr_to_code[LEX_OR]   = 0x25;
    this->instr_to_code[LEX_SLL]  = 0x00;
    this->instr_to_code[LEX_SLT]  = 0x2A;
    this->instr_to_code[LEX_SLTU] = 0x2B;
    this->instr_to_code[LEX_SUB]  = 0x22;
    this->instr_to_code[LEX_SUBU] = 0x23;
    
    // I-format instructions
    this->instr_to_code[LEX_ADDI]  = 0x08;
    this->instr_to_code[LEX_ADDIU] = 0x09;
    this->instr_to_code[LEX_BEQ]   = 0x04;
    this->instr_to_code[LEX_BNE]   = 0x05;
    this->instr_to_code[LEX_LW]    = 0x23;
    this->instr_to_code[LEX_LUI]   = 0x0F;
    this->instr_to_code[LEX_ORI]   = 0x0D;
    this->instr_to_code[LEX_SW]    = 0x2B;

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
    for(int i = 0; i < n; ++i)
    {
        std::cout << "[" << __func__ << "] instr : " << l.opcode.toString() 
            << " arg " << std::dec << i << " val : " << l.args[i].val 
            << " (offsetting by " << std::dec << unsigned(this->r_instr_offsets[i]) << ")" << std::endl;
        instr.ins = instr.ins | ((l.args[i].val & 0xFF) << this->r_instr_offsets[i]);
    }

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
    for(int i = 0; i < n; ++i)
    {
        std::cout << "[" << __func__ << "] instr : " << l.opcode.toString() 
            << " arg " << std::dec << i << " val : " << l.args[i].val 
            << " (offsetting by " << std::dec << unsigned(this->r_instr_offsets[i]) << ")" << std::endl;

        if(i == 2)
            instr.ins = instr.ins | (l.args[i].val & 0xFF) << 6;
        else
            instr.ins = instr.ins | ((l.args[i].val & 0xFF) << this->r_instr_offsets[i]);
    }
    instr.ins = instr.ins | this->instr_to_code[l.opcode.instr];

    return instr;
}

/*
 * asm_i_instr()
 * Assemble the arguments for an I-format instruction
 */
Instr Assembler::asm_i_instr(const TextInfo& l, const int n) 
{
    Instr instr;

    instr.ins = instr.ins | (this->instr_to_code[l.opcode.instr] << this->i_instr_op_offset);
    for(int i = 0; i < n; ++i)
    {
        std::cout << "[" << __func__ << "] instr : " << l.opcode.toString() 
            << " arg " << std::dec << i << " val : " << l.args[i].val 
            << " (offsetting by " << std::dec << unsigned(this->i_instr_offsets[i]) << ")" << std::endl;
        instr.ins = instr.ins | ((l.args[i].val & 0xFFFF) << this->i_instr_offsets[i]);
    }

    return instr;
}


/*
 * asm_j_instr()
 * Assemble the arguments for a J-format instruction
 */
Instr Assembler::asm_j_instr(const TextInfo& l) 
{
    Instr instr;

    instr.ins = instr.ins | (this->instr_to_code[l.opcode.instr] << this->j_instr_op_offset);
    instr.ins = instr.ins | ((l.args[2].val & 0x0FFFFFFC) >> 2);
    std::cout << "[" << __func__ << "] set J instr val to " << instr.toString() << std::endl;

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

    std::cout << "[" << __func__ << "] assembling " << line.opcode.toString() 
        << " which has code [" << std::hex << unsigned(this->instr_to_code[line.opcode.instr])
        << "]" << std::endl;

    std::cout << "[" << __func__ << "] " << line.toInstrString() << std::endl;

    switch(line.opcode.instr)
    {
        // No-op
        //case LEX_NULL:      
        //    instr = Instr(line.addr, 0x00000000);
        //    break;

        // R-format instructions
        case LEX_ADD:
        case LEX_ADDU:
        case LEX_AND:
        case LEX_JR:
        case LEX_MULT:
        case LEX_OR:
        case LEX_SLT:
        case LEX_SLTU:
        case LEX_SUB:
        case LEX_SUBU:
            instr = this->asm_r_instr(line, 3);
            break;

        case LEX_SLL:
        case LEX_SRL:
            instr = this->asm_r_instr_shamt(line, 3);
            break;

        // I-format instructions 
        case LEX_ADDI:
        case LEX_ADDIU:
        case LEX_BEQ:
        case LEX_BNE:
        case LEX_LW:
        case LEX_LUI:
        case LEX_ORI:
        case LEX_SW:
            instr = this->asm_i_instr(line, 3);
            break;

        // J-format instructions 
        case LEX_J:
        case LEX_JAL:
            instr = this->asm_j_instr(line);
            break;

        default:
            // If we can't work out what to place here then emit a noop
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
