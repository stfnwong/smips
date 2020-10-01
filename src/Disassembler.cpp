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
TextInfo dis_i_instr(uint32_t instr, uint32_t addr)
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
    ti.args[0] = Argument(SYM_REGISTER, (instr & (0x1F << 16)) >> 16);       // rs
    ti.args[1] = Argument(SYM_REGISTER, (instr & (0x1F << 21)) >> 21);       // rt
    ti.args[2] = Argument(SYM_LITERAL, (instr & 0xFFFF));

    return ti;
}

/*
 * disassemble a single R-instruction
 */
TextInfo dis_r_instr(uint32_t instr, uint32_t addr)
{
    TextInfo ti;
    uint8_t func_bits = (instr & 0x3F);

    ti.addr = addr;
    switch(func_bits)
    {
        case 0x20:
            ti.opcode = Opcode(LEX_ADD, "add");
            break;
        case 0x21:
            ti.opcode = Opcode(LEX_ADDU, "addu");
            break;
        case 0x22:
            ti.opcode = Opcode(LEX_SUB, "sub");
            break;
        case 0x23:
            ti.opcode = Opcode(LEX_SUBU, "subu");
            break;
        case 0x24:
            ti.opcode = Opcode(LEX_AND, "and");
            break;
        case 0x25:
            ti.opcode = Opcode(LEX_OR, "or");
            break;
        case 0x27:
            ti.opcode = Opcode(LEX_NOR, "nor");
            break;
        case 0x2A:
            ti.opcode = Opcode(LEX_SLT, "slt");
            break;
        case 0x2B:
            ti.opcode = Opcode(LEX_SLTU, "sltu");
            break;
        case 0x0:
            ti.opcode = Opcode(LEX_SLL, "sll");
            break;
        case 0x2:
            ti.opcode = Opcode(LEX_SRL, "srl");
            break;
        case 0x4:
            ti.opcode = Opcode(LEX_JR, "jr");
            break;
        case 0x18:
            ti.opcode = Opcode(LEX_MULT, "mult");
            break;
        default:
            std::cerr << "[" << __func__ << "] unknown R-Instruction with func bits 0x" << std::hex
                << std::setw(2) << std::setfill('0') << func_bits << std::endl;
            break;
    }

    // arguments 
    ti.args[1] = Argument(SYM_REGISTER, (instr & (0x1F << 21)) >> 21);       // rs 
    ti.args[0] = Argument(SYM_REGISTER, (instr & (0x1F << 11)) >> 11);       // rd
    if(ti.opcode.instr == LEX_SLL || ti.opcode.instr == LEX_SRL)
        ti.args[2] = Argument(SYM_LITERAL, (instr & (0x1F << 6)) >> 6);      // rt
    else
        ti.args[2] = Argument(SYM_REGISTER, (instr & (0x1F << 16)) >> 16);   // rt 

    return ti;
}

/*
 * disassemble a single J-instruction
 */
TextInfo dis_j_instr(uint32_t instr, uint32_t addr)
{
    TextInfo ti;
    uint8_t op_bits;

    ti.addr = addr;
    op_bits = (instr & 0xFC000000) >> 26;

    std::cout << "[" << __func__ << "] instr :" << std::hex 
        << instr << std::endl;
    std::cout << "[" << __func__ << "] j op bits 0x" << std::hex 
        << unsigned(op_bits) << std::endl;

    if(op_bits == 0x2)
        ti.opcode = Opcode(LEX_J, "j");
    else
        ti.opcode = Opcode(LEX_JAL, "jal");

    // this ensures the output is such that if we put it back in
    // the lexer we'd be able to assemble the same program again
    ti.args[2] = Argument(SYM_LITERAL, (instr & 0x02FFFFFF) << 2);

    return ti;
}



// instruction disassembly
/*
 * dis_instr()
 * Disassemble a single instruction
 */
TextInfo dis_instr(uint32_t instr, uint32_t addr)
{
    uint8_t op_bits;

    // TODO : add disassembly for no-op
    if(instr == 0x00000000)
    {
        TextInfo noop;
        noop.addr = addr;

        return noop;
    }

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
 * dis_program()
 */
SourceInfo dis_program(const Program& program)
{
    SourceInfo source;

    // TODO: ideally we would also be able to handle the data segment 
    // of the program as well
    for(unsigned int idx = 0; idx < program.numInstrs(); ++idx)
    {
        Instr cur_instr = program.getInstr(idx);
        TextInfo cur_line = dis_instr(cur_instr.ins, cur_instr.adr);
        source.addText(cur_line);
    }

    return source;
}
