/*
 * DISASSEMBLER
 * Reverse a SMIPS binary into a SourceInfo
 *
 * Stefan Wong 2019
 */

#include <iostream>
#include <iomanip>
#include "Codes.hpp"
#include "Disassembler.hpp"


/*
 * disassemble a single I-instruction
 */
TextInfo Disassembler::dis_i_instr(uint32_t instr, uint32_t addr)
{
    TextInfo ti;

    ti.addr = addr;
    ti.is_imm = true;
    uint8_t op_bits = (instr & 0xFC000000) >> 26;
    switch(op_bits)
    {
        case 4:
            ti.opcode = Opcode(LEX_BEQ, "beq");
            break;
        case 5:
            ti.opcode = Opcode(LEX_BNE, "bne");
            break;
        case 8:
            ti.opcode = Opcode(LEX_ADDI, "addi");
            break;
        case 9:
            ti.opcode = Opcode(LEX_ADDIU, "addiu");
            break;
        case 12:
            ti.opcode = Opcode(LEX_ANDI, "andi");
            break;
        case 13:
            ti.opcode = Opcode(LEX_ORI, "ori");
            break;
        case 10:
            ti.opcode = Opcode(LEX_SLTI, "slti");
            break;
        case 11:
            ti.opcode = Opcode(LEX_SLTIU, "sltiu");
            break;
        case 15:
            ti.opcode = Opcode(LEX_LUI, "lui");
            break;
        case 35:
            ti.opcode = Opcode(LEX_LW, "lw");
            break;
        case 43:
            ti.opcode = Opcode(LEX_SW, "sw");
            break;
        default:
            std::cerr << "[" << __func__ << " unknown I-instruction with op bits 0x" 
                << std::hex << std::setw(2) << std::setfill('0') 
                << op_bits << std::endl;
    }

    // arguments 
    ti.val[0] = (instr & (0x1F << 21));
    ti.val[1] = (instr & (0x1F << 16));
    ti.val[2] = (instr & 0xFFFF);
    ti.type[0] = SYM_REGISTER;
    ti.type[1] = SYM_REGISTER;
    ti.type[2] = SYM_LITERAL;

    return ti;
}

/*
 * disassemble a single R-instruction
 */
TextInfo Disassembler::dis_r_instr(uint32_t instr, uint32_t addr)
{
    TextInfo ti;
    uint8_t func_bits = (instr & 0x3F);

    ti.addr = addr;
    switch(func_bits)
    {
        case 32:
            ti.opcode = Opcode(LEX_ADD, "add");
            break;
        case 33:
            ti.opcode = Opcode(LEX_ADDU, "addu");
            break;
        case 34:
            ti.opcode = Opcode(LEX_SUB, "sub");
            break;
        case 35:
            ti.opcode = Opcode(LEX_SUBU, "subu");
            break;
        case 36:
            ti.opcode = Opcode(LEX_AND, "and");
            break;
        case 37:
            ti.opcode = Opcode(LEX_OR, "or");
            break;
        case 39:
            ti.opcode = Opcode(LEX_NOR, "nor");
            break;
        case 42:
            ti.opcode = Opcode(LEX_SLT, "slt");
            break;
        case 43:
            ti.opcode = Opcode(LEX_SLTU, "sltu");
            break;
        case 0:
            ti.opcode = Opcode(LEX_SLL, "sll");
            break;
        case 2:
            ti.opcode = Opcode(LEX_SRL, "srl");
            break;
        case 4:
            ti.opcode = Opcode(LEX_JR, "jr");
            break;
    }
    
    // arguments 
    ti.val[0] = (instr & (0x1F << 26));
    ti.val[1] = (instr & (0x1F << 21));
    if(ti.opcode.instr == LEX_SLL || ti.opcode.instr == LEX_SRL)
        ti.val[2] = 10;
    else
        ti.val[2] = (instr & (0x1F << 16));
    ti.type[0] = SYM_REGISTER;
    ti.type[1] = SYM_REGISTER;
    ti.type[2] = SYM_REGISTER;

    return ti;
}

/*
 * disassemble a single J-instruction
 */
TextInfo Disassembler::dis_j_instr(uint32_t instr, uint32_t addr)
{
    TextInfo ti;

    ti.addr = addr;


    return ti;
}

Disassembler::Disassembler()
{
    this->verbose = false;
}


// instruction disassembly
/*
 * dis_instr()
 * Disassemble a single instruction
 */
TextInfo Disassembler::dis_instr(uint32_t instr, uint32_t addr)
{
    uint8_t op_bits;

    op_bits = (instr & 0xFC000000) >> 26;
    switch(op_bits)
    {
        case 0x2:
        case 0x3:
            return dis_j_instr(instr, addr);
            
        case 0x0:
            return dis_r_instr(instr, addr);

        default:
            return dis_i_instr(instr, addr);
    }

    // if we somehow get here then just return a black TextInfo
    return TextInfo();
}

/*
 * diassemble()
 * Disassemble an entire program (eg: from disk)
 */
void Disassembler::disassemble(void)
{
    unsigned int i;
    //uint32_t instr_mask;
    Instr instr;


    for(i = 0; i < this->program.size(); ++i)
    {
    }
}



int Disassembler::load(const std::string& filename)
{
    return this->program.load(filename);
}




void Disassembler::setVerbose(const bool v)
{
    this->verbose = v;
}

bool Disassembler::getVerbose(void) const
{
    return this->verbose;
}
