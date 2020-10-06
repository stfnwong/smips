/*
 * CODES
 * Lexer instruction codes
 *
 * Stefan Wong 2019
 */

#include <sstream>
#include <iomanip>
#include <iostream>
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
    this->i_instrs[9]  = InstrCode(35, InstrType::INSTR_I);  // LW
    this->i_instrs[10] = InstrCode(43, InstrType::INSTR_I);  // SW

    // jump instructions
    this->j_instrs[0]  = InstrCode(2, InstrType::INSTR_J);  // J
    this->j_instrs[1]  = InstrCode(3, InstrType::INSTR_J);  // JAL
}
