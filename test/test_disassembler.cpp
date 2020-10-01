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
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1] = Argument(SYM_REGISTER, REG_GLOBAL);
    line.args[2] = Argument(SYM_LITERAL, 4);
    line.is_imm  = true;
    info.addText(line);

    // mult $t0, $t0, $t0
    line.init();
    line.addr = 0x00400004;
    line.opcode = Opcode(LEX_MULT, "mult");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1] = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[2] = Argument(SYM_REGISTER, REG_TEMP_0);
    info.addText(line);

    // lw $t1 4($gp)
    line.init();
    line.addr = 0x00400008;
    line.opcode = Opcode(LEX_LW, "lw");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1] = Argument(SYM_REGISTER, REG_GLOBAL);
    line.args[2] = Argument(SYM_LITERAL, 4);
    line.is_imm = true;
    info.addText(line);

    // ori $t2, $zero, 3
    line.init();
    line.addr = 0x0040000C;
    line.opcode = Opcode(LEX_ORI, "ori");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1] = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2] = Argument(SYM_LITERAL, 3);
    line.is_imm = true;
    info.addText(line);

    // mult $t1, $t1, $t2
    line.init();
    line.addr = 0x00400010;
    line.opcode = Opcode(LEX_MULT, "mult");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1] = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[2] = Argument(SYM_REGISTER, REG_TEMP_2);
    info.addText(line);

    // add $t2, $t0, $t1
    line.init();
    line.addr = 0x00400014;
    line.opcode = Opcode(LEX_ADD, "add");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1] = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[2] = Argument(SYM_REGISTER, REG_TEMP_1);
    info.addText(line);

    // sw $t2, $0($gp)
    line.init();
    line.addr = 0x00400018;
    line.opcode = Opcode(LEX_SW, "sw");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1] = Argument(SYM_REGISTER, REG_GLOBAL);
    line.args[2] = Argument(SYM_LITERAL, 0);
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

    Instr cur_instr;
    for(unsigned int idx = 0; idx < test_program.numInstrs(); ++idx)
    {
        cur_instr = test_program.getInstr(idx);
        TextInfo dis_out = dis_instr(cur_instr.ins, cur_instr.adr);
        TextInfo exp_out = expected_out.getText(idx);

        REQUIRE(exp_out == dis_out);
    }
}



SourceInfo get_for_loop_expected_dis(void)
{
    SourceInfo info;
    TextInfo line;

    // add $t0, $gp, $zero 
    line.addr = 0x00400000;
    line.opcode = Opcode(LEX_ADD, "add");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1] = Argument(SYM_REGISTER, REG_GLOBAL);
    line.args[2] = Argument(SYM_REGISTER, REG_ZERO);
    info.addText(line);

    // lw $t1, 4($gp)
    line.init();
    line.addr = 0x00400004;
    line.opcode = Opcode(LEX_LW, "lw");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1] = Argument(SYM_REGISTER, REG_GLOBAL);
    line.args[2] = Argument(SYM_LITERAL, 4);
    line.is_imm = true;
    info.addText(line);

    // sll $t1, $t1, 2
    line.init();
    line.addr = 0x00400008;
    line.opcode = Opcode(LEX_SLL, "sll");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1] = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[2] = Argument(SYM_LITERAL, 2);
    info.addText(line);
    
    // add $t1, $t1, $gp
    line.init();
    line.addr = 0x0040000C;
    line.opcode = Opcode(LEX_ADD, "add");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1] = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[2] = Argument(SYM_REGISTER, REG_GLOBAL);
    info.addText(line);

    // ori $t2, $zero, 256
    line.init();
    line.addr = 0x00400010;
    line.opcode = Opcode(LEX_ORI, "ori");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1] = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2] = Argument(SYM_LITERAL, 256);
    line.is_imm  = true;
    info.addText(line);

    // sltu $t3, $t0, $t1
    line.init();
    line.addr = 0x00400014;
    line.opcode = Opcode(LEX_SLTU, "sltu");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_3);
    line.args[1] = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[2] = Argument(SYM_REGISTER, REG_TEMP_1);
    info.addText(line);

    // beq $t3, $zero, done 
    line.init();
    line.addr = 0x00400018;
    line.opcode = Opcode(LEX_BEQ, "beq");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_3);
    line.args[1] = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2] = Argument(SYM_LITERAL, 0x00400028 - 0x00400018);
    line.is_imm  = true;
    info.addText(line);

    // sw $t2, 28($t0)
    line.init();
    line.addr = 0x0040001C;
    line.opcode = Opcode(LEX_SW, "sw");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1] = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[2] = Argument(SYM_LITERAL, 28);
    line.is_imm  = true;
    info.addText(line);

    // addi $t0, $t0 ,4
    line.init();
    line.addr = 0x00400020;
    line.opcode = Opcode(LEX_ADDI, "addi");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1] = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[2] = Argument(SYM_LITERAL, 4);
    line.is_imm  = true;
    info.addText(line);

    // j top
    line.init();
    line.addr = 0x00400024;
    line.opcode = Opcode(LEX_J, "j");
    line.args[2]   = Argument(SYM_LITERAL, 0x00400014);
    info.addText(line);

    // dummy instr
    line.init();
    line.addr      = 0x00400028;
    //line.is_label  = true;
    //line.label     = "done";
    info.addText(line);

    return info;
}


TEST_CASE("test_dis_for_loop", "[classic]")
{
    Program test_program;   
    const std::string test_for_loop_file = "asm/for_loop.asm";

    SourceInfo expected_out = get_for_loop_expected_dis(); 

    test_program = assem(test_for_loop_file);
    int status = test_program.save("bin/test_for_loop.bin");
    REQUIRE(status == 0);
    REQUIRE(test_program.numInstrs() == expected_out.getTextInfoSize());

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

        if(exp_out != dis_out)
        {
            std::cout << "Line " << std::dec << idx + 1 <<  "/" << 
                std::dec << expected_out.getTextInfoSize() << 
                " mismatch" << std::endl;
            std::cout << std::endl << "    diff : " << std::endl;
            std::cout << exp_out.diff(dis_out) << std::endl;
        }
        REQUIRE(exp_out == dis_out);
        std::cout << "    [OK]" << std::endl;
    }
}
