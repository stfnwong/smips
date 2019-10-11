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
uint32_t Assembler::asm_r_instr(const TextInfo& l, const int n) const
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
uint32_t Assembler::asm_i_instr(const TextInfo& l, const int n) const
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
 * add $d, $s, $t
 */
Instr Assembler::asm_add(const TextInfo& l) const
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
 * addi $t, $s, imm
 */
Instr Assembler::asm_addi(const TextInfo& l) const
{
    Instr instr;

    instr.ins = instr.ins | this->asm_i_instr(l, 3);
    instr.ins = instr.ins | (0x08 << 26);
    instr.ins = instr.ins | (l.val[2]);
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_addu()
 * R-format
 * addu $t, $s, imm
 */
Instr Assembler::asm_addu(const TextInfo& l) const
{
    Instr instr;

    instr.ins = instr.ins | this->asm_r_instr(l, 3);
    instr.ins = instr.ins | 0x21;
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_beq()
 * I-format
 * beq $s, $t, label
 */
Instr Assembler::asm_beq(const TextInfo& l) const
{
    Instr instr;
    
    instr.ins = 0x04 << this->i_instr_op_offset;
    instr.ins = instr.ins | this->asm_i_instr(l, 2);
    instr.ins = instr.ins | (l.val[2]);
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_bne()
 * I-format
 * bne $s, $t, label
 */
Instr Assembler::asm_bne(const TextInfo& l) const
{
    Instr instr;

    instr.ins = 0x05 << this->i_instr_op_offset;
    instr.ins = instr.ins | this->asm_i_instr(l, 2);
    instr.ins = instr.ins | (l.val[2]);
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_j()
 * J-format
 * j offset
 */
Instr Assembler::asm_j(const TextInfo& l) const
{
    Instr instr;

    instr.ins = 0x02 << this->j_instr_op_offset;
    instr.ins = instr.ins | l.val[2];
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_la()
 * TODO : this actually has to expand to 2 instructions...
 */
Instr Assembler::asm_la(const TextInfo& l) const
{
    Instr instr;

    return instr;
}


/*
 * asm_li()
 * TODO : this actually has to expand to 2 instructions...
 */
Instr Assembler::asm_li(const TextInfo& l) const
{
    Instr instr;

    return instr;
}

/*
 * asm_lw()
 * I-format
 * lw $t, OFFSET($s)
 */
Instr Assembler::asm_lw(const TextInfo& l) const
{
    Instr instr;

    instr.ins = 0x23 << this->i_instr_op_offset;
    instr.ins = instr.ins | this->asm_i_instr(l, 2);
    instr.ins = instr.ins | (l.val[2]);        // insert immediate
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_mult()
 * R-format
 * mult $s, $t
 */
Instr Assembler::asm_mult(const TextInfo& l) const
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
 * or $s, $s, $t
 */
Instr Assembler::asm_or(const TextInfo& l) const
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
 * ori $t, $s, imm
 */
Instr Assembler::asm_ori(const TextInfo& l) const
{
    Instr instr;

    instr.ins = instr.ins | this->asm_i_instr(l, 2);
    instr.ins = instr.ins | (l.val[2]);
    instr.ins = instr.ins | (0x0C << this->i_instr_op_offset);
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_sll()
 * R-format
 * sll $d, $t, imm
 */
Instr Assembler::asm_sll(const TextInfo& l) const
{
    Instr instr;

    instr.ins = 0x0;
    instr.ins = instr.ins | this->asm_r_instr(l, 3);
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_sltu()
 * R-format
 * sltu $d, $s, $t
 */
Instr Assembler::asm_sltu(const TextInfo& l) const
{
    Instr instr;

    instr.ins = 0x2B;
    instr.ins = instr.ins | this->asm_r_instr(l, 3);
    instr.adr = l.addr;
    return instr;
}

/*
 * asm_sub()
 * R-format
 * sub $d, $s, $t
 */
Instr Assembler::asm_sub(const TextInfo& l) const
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
 * subu $d, $s, $t
 */
Instr Assembler::asm_subu(const TextInfo& l) const
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
Instr Assembler::asm_sw(const TextInfo& l) const
{
    Instr instr;

    instr.ins = 0x2B << this->i_instr_op_offset;
    instr.ins = instr.ins | this->asm_i_instr(l, 2);
    instr.ins = instr.ins | l.val[2];       // was l.val[1]
    instr.adr = l.addr;

    return instr;
}


/*
 * AssembleLine()
 * Transform a TextInfo object into an Instr object
 */
Instr Assembler::assembleText(const TextInfo& line)
{
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

        case LEX_BEQ:
            return this->asm_beq(line);
            break;

        case LEX_J:
            return this->asm_j(line);
            break;

        // TODO : here we need to be able to insert two (or perhaps 3)
        // instructions at a time.
        case LEX_LA:
            return this->asm_la(line);
            break;

        case LEX_LI:
            return this->asm_li(line);
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

        case LEX_SLL:
            return this->asm_sll(line);
            break;

        case LEX_SLTU:
            return this->asm_sltu(line);
            break;

        case LEX_SUB:
            return this->asm_sub(line);
            break;

        case LEX_SUBU:
            return this->asm_subu(line);
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
            // TODO : I will emit NO-OPS here, which may 
            // prove later to be a bad idea
            return Instr(line.addr, 0); // emit a null instruction
    }
}


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

    this->program.init();
    for(unsigned int i = 0; i < this->source.getDataInfoSize(); ++i)
    {
        cur_data = this->source.getData(i);
        cur_seg  = this->assembleData(cur_data);
        this->program.add(cur_seg);
    }

    // TODO : assemble the text sections
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
