/* 
 * TEST_DISASSEMBLER
 * Unit tests for SMIPS Disassembler object 
 *
 * Stefan Wong 2019
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>

// unit(s) under test
#include "Lexer.hpp"
#include "Assembler.hpp"
#include "Disassembler.hpp"
#include "Program.hpp"
#include "Register.hpp"
#include "Address.hpp"
#include "Opcode.hpp"
#include "Source.hpp"

// offsets, etc
#include "Common.hpp"
#include "Codes.hpp"

// Filenames for test 
const std::string test_mult_add_file = "asm/mult_add.asm";
const std::string test_for_loop_file = "asm/for_loop.asm";
const std::string test_array_file    = "asm/array.asm";
const std::string test_instr_file    = "asm/instr_test.asm";


// Assembly helper function 
Program assem(const std::string& filename)
{
    Lexer lexer;
    Assembler assem;

    lexer.loadFile(filename);
    lexer.lex();

    assem.loadSource(lexer.getSourceInfo());
    assem.assemble();

    return assem.getProgram();
}


SourceInfo get_mult_add_expected_dis(void)
{
    SourceInfo info;
    TextInfo line;

    // lw $t0, 4($gp)
    line.init();
    line.addr = 0x00400000;
    line.opcode = Opcode(LEX_LW, "lw");
    line.val[0] = REG_TEMP_0;
    line.val[1] = REG_GLOBAL;
    line.val[2] = 4;
    line.type[0] = SYM_REGISTER;
    line.type[1] = SYM_REGISTER;
    line.type[2] = SYM_LITERAL;
    line.is_imm  = true;
    info.addText(line);

    // mult $t0, $t0, $t0
    line.init();
    line.addr = 0x00400004;
    line.opcode = Opcode(LEX_MULT, "mult");
    line.val[0] = REG_TEMP_0;
    line.val[1] = REG_TEMP_0;
    line.val[2] = REG_TEMP_0;
    line.type[0] = SYM_REGISTER;
    line.type[1] = SYM_REGISTER;
    line.type[2] = SYM_REGISTER;
    info.addText(line);

    // lw $t1 4($gp)
    line.init();
    line.addr = 0x00400008;
    line.opcode = Opcode(LEX_LW, "lw");
    line.val[0] = REG_TEMP_1;
    line.val[1] = REG_GLOBAL;
    line.val[2] = 4;
    line.type[0] = SYM_REGISTER;
    line.type[1] = SYM_REGISTER;
    line.type[2] = SYM_LITERAL;
    line.is_imm = true;
    info.addText(line);

    // ori $t2, $zero, 3
    line.init();
    line.addr = 0x0040000C;
    line.opcode = Opcode(LEX_ORI, "ori");
    line.val[0] = REG_TEMP_2;
    line.val[1] = REG_ZERO;
    line.val[2] = 3;
    line.type[0] = SYM_REGISTER;
    line.type[1] = SYM_REGISTER;
    line.type[2] = SYM_LITERAL;
    line.is_imm = true;
    info.addText(line);

    // mult $t1, $t1, $t2
    line.init();
    line.addr = 0x00400010;
    line.opcode = Opcode(LEX_MULT, "mult");
    line.val[0] = REG_TEMP_1;
    line.val[1] = REG_TEMP_1;
    line.val[2] = REG_TEMP_2;
    line.type[0] = SYM_REGISTER;
    line.type[1] = SYM_REGISTER;
    line.type[2] = SYM_REGISTER;
    info.addText(line);

    // add $t2, $t0, $t1
    line.init();
    line.addr = 0x00400014;
    line.opcode = Opcode(LEX_ADD, "add");
    line.val[0] = REG_TEMP_2;
    line.val[1] = REG_TEMP_0;
    line.val[2] = REG_TEMP_1;
    line.type[0] = SYM_REGISTER;
    line.type[1] = SYM_REGISTER;
    line.type[2] = SYM_REGISTER;
    info.addText(line);

    // sw $t2, $0($gp)
    line.init();
    line.addr = 0x00400018;
    line.opcode = Opcode(LEX_SW, "sw");
    line.val[0] = REG_TEMP_2;
    line.val[1] = REG_GLOBAL;
    line.type[0] = SYM_REGISTER;
    line.type[1] = SYM_REGISTER;
    line.is_imm = true;
    info.addText(line);

    return info;
}



/*
 * Test mult_add example
 */
TEST_CASE("test_dis_mult_add", "[classic]")
{
    Program test_program;   

    SourceInfo expected_out = get_mult_add_expected_dis(); 

    test_program = assem(test_mult_add_file);
    REQUIRE(test_program.numInstrs() == expected_out.getTextInfoSize());

    std::cout << "[" << __func__ << "] dis output : " << std::endl;
    // disassemble the binary
    Instr cur_instr;
    for(unsigned int idx = 0; idx < test_program.numInstrs(); ++idx)
    {
        cur_instr = test_program.getInstr(idx);
        TextInfo dis_out = dis_instr(cur_instr.ins, cur_instr.adr);
        TextInfo exp_out = expected_out.getText(idx);

        std::cout << "Line " << std::dec << idx << " expected: " << std::endl;
        std::cout << exp_out.toString() << std::endl;

        std::cout << "Got :" << std::endl;
        std::cout << dis_out.toString() << std::endl;

        REQUIRE(exp_out == dis_out);
    }

    // Now check each instruction
}
