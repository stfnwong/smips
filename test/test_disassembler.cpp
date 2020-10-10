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

/*
 * for_loop expected output
 */
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

    // noop instr
    line.init();
    line.addr      = 0x00400028;
    info.addText(line);

    return info;
}


TEST_CASE("test_dis_for_loop", "[classic]")
{
    Program test_program;   
    const std::string test_for_loop_file = "asm/for_loop.asm";

    SourceInfo expected_out = get_for_loop_expected_dis(); 
    test_program = assem(test_for_loop_file);
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


SourceInfo get_dis_instr_expected_out(void)
{
    SourceInfo info;
    TextInfo line;

    // add $t2, $t0, $t1
    line.init();
    line.addr = TEXT_START_ADDR;
    line.opcode = Opcode(LEX_ADD, "add");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1] = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[2] = Argument(SYM_REGISTER, REG_TEMP_1);
    info.addText(line);
    // addi $t1, $t1, 1
    line.init();
    line.addr = TEXT_START_ADDR;
    line.opcode = Opcode(LEX_ADDI, "addi");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1] = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[2] = Argument(SYM_LITERAL, 1);
    line.is_imm = true;
    info.addText(line);
    // and $t1, $t2, $t3
    line.init();
    line.addr = TEXT_START_ADDR;
    line.opcode = Opcode(LEX_AND, "and");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1] = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[2] = Argument(SYM_REGISTER, REG_TEMP_3);
    info.addText(line);
    // andi $t0, $t1, 255
    line.init();
    line.addr = TEXT_START_ADDR;
    line.opcode = Opcode(LEX_ANDI, "andi");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1] = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[2] = Argument(SYM_LITERAL, 255);
    line.is_imm = true;
    info.addText(line);
    // div $t2, $s4
    line.init();
    line.addr = TEXT_START_ADDR;
    line.opcode = Opcode(LEX_DIV, "div");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1] = Argument(SYM_REGISTER, REG_SAVED_4);
    info.addText(line);
    // lw $t1, 4($s4)
    line.init();
    line.addr = TEXT_START_ADDR;
    line.opcode = Opcode(LEX_LW, "lw");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1] = Argument(SYM_REGISTER, REG_SAVED_4);
    line.args[2] = Argument(SYM_LITERAL, 4);
    line.is_imm = true;
    info.addText(line);
    // lui $at, 4096
    line.init();
    line.addr = TEXT_START_ADDR;
    line.opcode = Opcode(LEX_LUI, "lui");
    line.args[0] = Argument(SYM_REGISTER, REG_AT);
    line.args[1] = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2] = Argument(SYM_LITERAL, 4096);
    line.is_imm = true;
    info.addText(line);
    // mult $t2, $s4 
    line.init();
    line.addr = TEXT_START_ADDR;
    line.opcode = Opcode(LEX_MULT, "mult");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1] = Argument(SYM_REGISTER, REG_SAVED_4);
    info.addText(line);

    // sll $t0, $t1, 8
    line.init();
    line.addr = TEXT_START_ADDR;
    line.opcode = Opcode(LEX_SLL, "sll");
    line.args[0] = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1] = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[2] = Argument(SYM_LITERAL, 8);
    info.addText(line);

    return info;
}


TEST_CASE("test_dis_instr", "[classic]")
{
    // Instructions to disassemble
    const std::vector<Instr> inp_instr = {
        Instr(TEXT_START_ADDR, 0x01095020),     // add $t2, $t0, $t1
        Instr(TEXT_START_ADDR, 0x21290001),     // addi $t1, $t1, 1
        Instr(TEXT_START_ADDR, 0x014B4824),     // and $t1, $t2 ,$t3
        // 0011 0001 0010 1000 0000 0000 1111 1111 
        // 0x31      0x28      0x00      0xFF
        Instr(TEXT_START_ADDR, 0x312800FF),     // andi $t0, $t1, 255
        // 0000 0001 0101 0100 0000 0000 0001 1010
        // 0x01      0x54      0x00      0x1A
        Instr(TEXT_START_ADDR, 0x0154001A),     // div $t2, $s4
        // 1000 1110 1000 1001 0000 0000 0000 0100
        // 0x8E      0x89      0x00      0x04
        Instr(TEXT_START_ADDR, 0x8E890004),     // lw $t1 4($s4)
        // 0011 1100 0000 0001 0001 0000 0000 0000
        // 0x3C      0x01      0x10      0x00
        Instr(TEXT_START_ADDR, 0x3C011000),     // lui $at 4096
        Instr(TEXT_START_ADDR, 0x01540018),     // mult $t2, $s4
        Instr(TEXT_START_ADDR, 0x00094200),     // sll $t0, $t1, 8
    };

    SourceInfo expected_out = get_dis_instr_expected_out();
    REQUIRE(expected_out.getTextInfoSize() == inp_instr.size());

    for(unsigned int idx = 0; idx < expected_out.getTextInfoSize(); ++idx)
    {
        TextInfo dis_out = dis_instr(inp_instr[idx].ins, inp_instr[idx].adr);
        TextInfo exp_out = expected_out.getText(idx);
        std::cout << "Checking [" << exp_out.toInstrString() << "]" << std::endl;

        //std::cout << "Line " << std::dec << idx << " expected: " << std::endl;
        //std::cout << exp_out.toString() << std::endl;

        //std::cout << "Got :" << std::endl;
        //std::cout << dis_out.toString() << std::endl;

        if(exp_out != dis_out)
        {
            std::cout << "Line " << std::dec << idx + 1 <<  "/" << 
                std::dec << expected_out.getTextInfoSize() << 
                " mismatch" << std::endl;
            std::cout << std::endl << "    diff : " << std::endl;
            std::cout << exp_out.diff(dis_out) << std::endl;
            std::cout << "Input was [0x" << std::hex << std::setw(8) 
                << std::setfill('0') << inp_instr[idx].ins << "]" << std::endl;
            std::cout << "Output is :" << std::endl;
            std::cout << dis_out.toString() << std::endl;
        }
        REQUIRE(exp_out == dis_out);
        //std::cout << "    [OK]" << std::endl;
    }
}
