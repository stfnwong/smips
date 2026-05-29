/*
 * DISASSEMBLER
 * Reverse a SMIPS binary into a SourceInfo
 *
 * Stefan Wong 2019
 */

#ifndef __DISASSEMBLER_HPP
#define __DISASSEMBLER_HPP

#include <string>

#include "Program.hpp"
#include "Source.hpp"
#include "Register.hpp"


TextInfo dis_i_instr(uint32_t instr, uint32_t addr);
TextInfo dis_r_instr(uint32_t instr, uint32_t addr);
TextInfo dis_j_instr(uint32_t instr, uint32_t addr);

TextInfo dis_instr(uint32_t instr, uint32_t addr);
SourceInfo dis_program(const Program& program);


#endif /*__DISASSEMBLER_HPP*/
