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
#include "Common.hpp"


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
        case I_BEQ:
            ti.opcode = Opcode(LEX_BEQ, "beq");
            break;
        case I_BNE:
            ti.opcode = Opcode(LEX_BNE, "bne");
            break;
        case I_ADDI:
            ti.opcode = Opcode(LEX_ADDI, "addi");
            break;
        case I_ADDIU:
            ti.opcode = Opcode(LEX_ADDIU, "addiu");
            break;
        case I_ANDI:
            ti.opcode = Opcode(LEX_ANDI, "andi");
            break;
        case I_ORI:
            ti.opcode = Opcode(LEX_ORI, "ori");
            break;
        case I_SLTI:
            ti.opcode = Opcode(LEX_SLTI, "slti");
            break;
        case I_SLTIU:
            ti.opcode = Opcode(LEX_SLTIU, "sltiu");
            break;
        case I_LUI:
            ti.opcode = Opcode(LEX_LUI, "lui");
            break;
        case I_LW:
            ti.opcode = Opcode(LEX_LW, "lw");
            break;
        case I_SW:
            ti.opcode = Opcode(LEX_SW, "sw");
            break;
        case I_XORI:
            ti.opcode = Opcode(LEX_XORI, "xori");
            break;
        default:
            std::cerr << "[" << __func__ << " unknown I-instruction with op bits 0x" 
                << std::hex << std::setw(2) << std::setfill('0') 
                << op_bits << std::endl;
    }

    switch(ti.opcode.instr)
    {
        case LEX_BEQ:
        case LEX_BNE:
            // we have to flip branch args
            ti.args[1] = Argument(SYM_REGISTER, (instr & (0x1F << 16)) >> 16);       // rs
            ti.args[0] = Argument(SYM_REGISTER, (instr & (0x1F << 21)) >> 21);       // rt
            ti.args[2] = Argument(SYM_LITERAL, (instr & 0xFFFF));   // imm
            break;

        default:
            ti.args[0] = Argument(SYM_REGISTER, (instr & (0x1F << 16)) >> 16);       // rs
            ti.args[1] = Argument(SYM_REGISTER, (instr & (0x1F << 21)) >> 21);       // rt
            ti.args[2] = Argument(SYM_LITERAL, (instr & 0xFFFF));   // imm
            break;
    }

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
        case R_ADD:
            ti.opcode = Opcode(LEX_ADD, "add");
            break;
        case R_ADDU:
            ti.opcode = Opcode(LEX_ADDU, "addu");
            break;
        case R_SUB:
            ti.opcode = Opcode(LEX_SUB, "sub");
            break;
        case R_SUBU:
            ti.opcode = Opcode(LEX_SUBU, "subu");
            break;
        case R_AND:
            ti.opcode = Opcode(LEX_AND, "and");
            break;
        case R_OR:
            ti.opcode = Opcode(LEX_OR, "or");
            break;
        case R_NOR:
            ti.opcode = Opcode(LEX_NOR, "nor");
            break;
        case R_SLT:
            ti.opcode = Opcode(LEX_SLT, "slt");
            break;
        case R_SLTU:
            ti.opcode = Opcode(LEX_SLTU, "sltu");
            break;
        case R_SLL:
            ti.opcode = Opcode(LEX_SLL, "sll");
            break;
        case R_SRL:
            ti.opcode = Opcode(LEX_SRL, "srl");
            break;
        case R_JR:
            ti.opcode = Opcode(LEX_JR, "jr");
            break;
        case R_MULT:
            ti.opcode = Opcode(LEX_MULT, "mult");
            break;
        case R_MULTU:
            ti.opcode = Opcode(LEX_MULTU, "multu");
            break;
        case R_DIV:
            ti.opcode = Opcode(LEX_DIV, "div");
            break;
        case R_DIVU:
            ti.opcode = Opcode(LEX_DIVU, "divu");
            break;
        case R_MFHI:
            ti.opcode = Opcode(LEX_MFHI, "mfhi");
            break;
        case R_MTHI:
            ti.opcode = Opcode(LEX_MTHI, "mthi");
            break;
        case R_MFLO:
            ti.opcode = Opcode(LEX_MFLO, "mflo");
            break;
        case R_MTLO:
            ti.opcode = Opcode(LEX_MTLO, "mtlo");
            break;

        default:
            std::cerr << "[" << __func__ << "] unknown R-Instruction with func bits 0x" << std::hex
                << std::setw(2) << std::setfill('0') << unsigned(func_bits) << std::endl;
            break;
    }

    // which operands?
    switch(func_bits)
    {
        case R_SLL:   
        case R_SRL:   
            ti.args[1] = Argument(SYM_REGISTER, (instr & (0x1F << 16)) >> 16);   // rt 
            ti.args[0] = Argument(SYM_REGISTER, (instr & (0x1F << 11)) >> 11);   // rd
            ti.args[2] = Argument(SYM_LITERAL, (instr & (0x1F << 6)) >> 6);      // shamt
            break;

        case R_DIV:    
        case R_DIVU:   
        case R_MULT:   
        case R_MULTU:  
            ti.args[0] = Argument(SYM_REGISTER, (instr & (0x1F << 21)) >> 21);  // rs 
            ti.args[1] = Argument(SYM_REGISTER, (instr & (0x1F << 16)) >> 16);  // rt
            break;

        case R_MFHI:  
        case R_MFLO:  
            ti.args[0] = Argument(SYM_REGISTER, (instr & (0x1F << 11)) >> 11); // rd 
            break;

        case R_MTHI:  
        case R_MTLO:  
            ti.args[0] = Argument(SYM_REGISTER, (instr & (0x1F << 21)) >> 21); // rs
            break;

        default:
            // arguments 
            ti.args[0] = Argument(SYM_REGISTER, (instr & (0x1F << 11)) >> 11);   // rd
            ti.args[1] = Argument(SYM_REGISTER, (instr & (0x1F << 21)) >> 21);   // rs 
            ti.args[2] = Argument(SYM_REGISTER, (instr & (0x1F << 16)) >> 16);   // rt 
            break;
    }

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

    if(op_bits == 0x2)
        ti.opcode = Opcode(LEX_J, "j");
    else if(op_bits == 0x3)
        ti.opcode = Opcode(LEX_JAL, "jal");
    else 
        return ti;      // Invalid/unsupported J instruction

    // this ensures the output is such that if we put it back in
    // the lexer we'd be able to assemble the same program again
    ti.args[0] = Argument(SYM_LITERAL, (instr & 0x02FFFFFF) << 2);
    ti.is_imm = true;

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

    // if we somehow get here then just return a blank TextInfo
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
