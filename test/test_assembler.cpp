/* 
 * TEST_ASSEMBLER 
 * Unit tests for SMIPS Assembler object 
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
#include "Program.hpp"
#include "Register.hpp"
#include "Address.hpp"
// offsets, etc
#include "Common.hpp"


const bool SHOW_ALL_OUTPUT = false;
const bool GLOBAL_VERBOSE = false;


// Filenames for test 
const std::string test_mult_add_file = "asm/mult_add.asm";
const std::string test_for_loop_file = "asm/for_loop.asm";
const std::string test_array_file    = "asm/array.asm";
const std::string test_instr_file    = "asm/instr_test.asm";

/*
 * Expected assembly output for mult_add.asm
 */
Program get_mult_add_expected_program(void)
{
    Program prog;
    Instr instr;

    // LW $t0, 4($gp)
    instr.adr = 0x00400000;
    instr.ins = 35 << I_INSTR_OP_OFFSET;  // I-format opcode
    instr.ins = instr.ins | (REG_TEMP_0  << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_GLOBAL << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | 4;
    prog.add(instr);

    instr.init();
    // mul $t0, $t0, $t0
    // mult $t0, $t0
    instr.adr = 0x00400004;
    instr.ins = 0x18;
    instr.ins = instr.ins | (REG_TEMP_0 << R_INSTR_RS_OFFSET);
    instr.ins = instr.ins | (REG_TEMP_0 << R_INSTR_RT_OFFSET);
    prog.add(instr);
    // mflo $t0
    instr.adr = 0x00400008;
    instr.ins = 0x18;
    instr.ins = instr.ins | (REG_TEMP_0 << R_INSTR_RD_OFFSET);
    prog.add(instr);

    instr.init();
    // LW $t1, 4($gp)
    instr.adr = 0x0040000C;
    instr.ins = 35 << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_TEMP_1 << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_GLOBAL << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | 4;
    prog.add(instr);

    instr.init();
    // ORI $t2, $zero, 3
    instr.adr = 0x00400010;
    instr.ins = 0xD << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_TEMP_2 << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_ZERO << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | 3;
    prog.add(instr);

    instr.init();
    // MULT $t1, $t1, $t2
    instr.adr = 0x00400014;
    instr.ins = 0x18;
    instr.ins = instr.ins | (REG_TEMP_1 << R_INSTR_RD_OFFSET);
    instr.ins = instr.ins | (REG_TEMP_1 << R_INSTR_RS_OFFSET);
    instr.ins = instr.ins | (REG_TEMP_2 << R_INSTR_RT_OFFSET);
    prog.add(instr);

    instr.init();
    // ADD $t2, $t0, $t1
    instr.adr = 0x00400018;
    instr.ins = 0x20;
    instr.ins = instr.ins | (REG_TEMP_2 << R_INSTR_RD_OFFSET);
    instr.ins = instr.ins | (REG_TEMP_0  << R_INSTR_RS_OFFSET);
    instr.ins = instr.ins | (REG_TEMP_1  << R_INSTR_RT_OFFSET);
    prog.add(instr);

    instr.init();
    // SW $t2, 0($gp) 
    instr.adr = 0x0040001C;
    instr.ins = 0x2B << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_TEMP_2 << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_GLOBAL << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | 0;  // has no effect, but mirrors the instruction
    prog.add(instr);

    return prog;
}


/*
 * Test mult_add example
 */
TEST_CASE("test_asm_mult_add", "[classic]")
{
    Lexer      lexer;
    Assembler  test_asm;
    SourceInfo src_out;
    Program    prog_out;
    Program    prog_exp;

    test_asm.setVerbose(GLOBAL_VERBOSE);
    // get some source info for this program
    lexer.setVerbose(false);
    lexer.loadFile(test_mult_add_file);
    lexer.lex();

    // add source info to assembler
    src_out = lexer.getSourceInfo();
    test_asm.loadSource(src_out);
    test_asm.assemble();

    // expected program
    prog_exp =  get_mult_add_expected_program();

    // show the resulting program
    prog_out = test_asm.getProgram();
    if(SHOW_ALL_OUTPUT)
    {
        std::cout << "Expected " << prog_exp.size() << " instructions" << std::endl;
        std::cout << "Output program has " << prog_out.size() << " instructions" << std::endl;
    }
    REQUIRE(prog_exp.size() == prog_out.size());

    Instr instr_exp;
    Instr instr_out;

    for(unsigned int ins = 0; ins < prog_out.size(); ++ins)
    {
        instr_exp = prog_exp.getInstr(ins);
        instr_out = prog_out.getInstr(ins);

        if(SHOW_ALL_OUTPUT)
        {
            std::cout << "Checking instruction [" << ins+1 << 
                "/" << prog_out.size() << "]" << std::endl; 
            std::cout << "\tExpected : " << instr_exp.toString() << std::endl;
            std::cout << "\tOutput   : " << instr_out.toString() << std::endl;
        }

        REQUIRE(instr_exp == instr_out);
        if(SHOW_ALL_OUTPUT)
            std::cout << "[OK]" << std::endl;
    }
    std::cout << prog_out.toString() << std::endl;
}

/*
 * Expected assembly output for for_loop.asm
 */
Program get_for_loop_expected_program(void)
{
    Program prog;
    Instr   instr;

    // add $t0, $gp, $zero
    instr.adr = 0x00400000;
    instr.ins = 0x20;
    instr.ins = instr.ins | (REG_TEMP_0 << R_INSTR_RD_OFFSET);
    instr.ins = instr.ins | (REG_GLOBAL << R_INSTR_RS_OFFSET);
    instr.ins = instr.ins | (REG_ZERO << R_INSTR_RT_OFFSET);
    prog.add(instr);

    instr.init();
    // lw $t1, 4($gp)
    instr.adr = 0x00400004;
    instr.ins = 35 << I_INSTR_OP_OFFSET;  // I-format opcode
    instr.ins = instr.ins | (REG_TEMP_1  << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_GLOBAL << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | 4;
    prog.add(instr);

    instr.init();
    // sll $t1, $t1, 2
    instr.adr = 0x00400008;
    instr.ins = 0x0;
    instr.ins = instr.ins | (REG_TEMP_1 << R_INSTR_RD_OFFSET);
    instr.ins = instr.ins | (REG_TEMP_1 << R_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (2 << R_INSTR_SHAMT_OFFSET);
    prog.add(instr);

    instr.init();
    // add $t1, $t1, $gp
    instr.adr = 0x0040000C;
    instr.ins = 0x20;
    instr.ins = instr.ins | (REG_TEMP_1 << R_INSTR_RD_OFFSET);
    instr.ins = instr.ins | (REG_TEMP_1 << R_INSTR_RS_OFFSET);
    instr.ins = instr.ins | (REG_GLOBAL << R_INSTR_RT_OFFSET);
    prog.add(instr);

    instr.init();
    // ori $t2, $zero, 256
    instr.adr = 0x00400010;
    instr.ins = 0xD << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_TEMP_2 << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_ZERO << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | 256;
    prog.add(instr);

    instr.init();
    // top : sltu $t3, $t0, $t1
    instr.adr = 0x00400014;
    instr.ins = 43;
    instr.ins = instr.ins | (REG_TEMP_3 << R_INSTR_RD_OFFSET);
    instr.ins = instr.ins | (REG_TEMP_0 << R_INSTR_RS_OFFSET);
    instr.ins = instr.ins | (REG_TEMP_1 << R_INSTR_RT_OFFSET);
    prog.add(instr);

    instr.init();
    // beq $t3, $zero, done
    instr.adr = 0x00400018;
    instr.ins = 0x4 << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_TEMP_3 << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_ZERO << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | ((0x0040028 - 0x00400018) & 0x0000FFFF); 
    prog.add(instr);

    instr.init();
    // sw $t2 28($t0)
    instr.adr = 0x0040001C;
    instr.ins = 0x2B << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_TEMP_2 << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_TEMP_0 << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | 28;
    prog.add(instr);

    instr.init();
    // ADDI $t0, $t0, 4
    instr.adr = 0x00400020;
    instr.ins = 0x08 << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_TEMP_0 << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | (REG_TEMP_0 << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | 4;
    prog.add(instr);

    instr.init();
    // J TOP 
    instr.adr = 0x00400024;
    instr.ins = 0x02 << J_INSTR_OP_OFFSET;
    instr.ins = instr.ins | ((0x00400014 & 0x0FFFFFFC) >> 2);
    prog.add(instr);
    
    instr.init();
    // DONE (dummy instruction)
    instr.adr = 0x00400028;
    prog.add(instr);

    return prog;
}

/*
 * Test for_loop example
 */
TEST_CASE("test_for_loop", "[classic]")
{
    Lexer      lexer;
    Assembler  test_asm;
    SourceInfo src_out;
    Program    prog_out;
    Program    prog_exp;

    test_asm.setVerbose(GLOBAL_VERBOSE);
    // get some source info for this program
    lexer.setVerbose(GLOBAL_VERBOSE);
    lexer.loadFile(test_for_loop_file);
    lexer.lex();

    // add source info to assembler
    src_out = lexer.getSourceInfo();
    test_asm.loadSource(src_out);
    test_asm.assemble();

    // expected program
    prog_exp = get_for_loop_expected_program();

    // show the resulting program
    prog_out = test_asm.getProgram();
    if(SHOW_ALL_OUTPUT)
    {
        std::cout << "Expected " << prog_exp.size() << " instructions" << std::endl;
        std::cout << "Output program has " << prog_out.size() << " instructions" << std::endl;
    }
    REQUIRE(prog_exp.size() == prog_out.size());

    Instr instr_exp;
    Instr instr_out;

    std::cout << "\t           Address    Data   " << instr_exp.toString() << std::endl;
    for(unsigned int ins = 0; ins < prog_out.size(); ++ins)
    {
        instr_exp = prog_exp.getInstr(ins);
        instr_out = prog_out.getInstr(ins);
        
        if(SHOW_ALL_OUTPUT)
        {
            std::cout << "Checking instruction [" << ins+1 << 
                "/" << prog_out.size() << "]" << std::endl; 
            std::cout << src_out.getText(ins).toString() << std::endl;
            std::cout << "\tExpected : " << instr_exp.toString() << std::endl;
            std::cout << "\tOutput   : " << instr_out.toString() << std::endl;
        }
        
        REQUIRE(instr_exp ==instr_out);
        if(SHOW_ALL_OUTPUT)
            std::cout << "[OK]" << std::endl;
    }
    std::cout << prog_out.toString() << std::endl;
}

/*
 * Expected assembly output for array.asm
 */
Program get_array_expected_program(void)
{
    Program prog;
    Instr   instr;
    DataSeg data;

    // ---- Data Section ---- //
    // list: .word 3, 0, 1, 2, 6
    data.init();
    data.adr = 0x10000000;
    data.data = {0x3, 0x0, 0x1, 0x2, 0x6};
    prog.add(data);
    // char_arr: .asciiz "hello"
    data.init();
    data.adr = 0x10000000 + 0x5;
    data.data = {0x68, 0x65, 0x6C, 0x6C, 0x6F};
    prog.add(data);
    // buffer: .space 128
    data.init();
    data.adr = 0x10000000 + 0x5 + 0x5;
    for(unsigned int d = 0; d < 128; ++d)
        data.data.push_back(uint8_t(0));
    prog.add(data);

    // ---- Text Section ---- //
    
    // la $s0, list
    // Exapnds to:
    //     lui $s0, (list & 0xFFFF0000) >> 16
    //     ori $s0, $s0, (list & 0x0000FFFF)
    instr.init();
    instr.adr = 0x00400000;
    instr.ins = 0xF << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_SAVED_0 << I_INSTR_RT_OFFSET);        
    instr.ins = instr.ins | ((DATA_START_ADDR & 0xFFFF0000) >> 16);
    prog.add(instr);

    instr.init();
    instr.adr = 0x00400004;
    instr.ins = 0xD << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_SAVED_0 << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_SAVED_0 << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | (DATA_START_ADDR & 0x0000FFFF);
    prog.add(instr);

    // li $t0, 0
    // Expands to 
    //     ori $t0, $zero, 0
    instr.init();
    instr.adr = 0x00400008;
    instr.ins = 0xD << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_TEMP_0 << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_ZERO << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | (0x0);
    prog.add(instr);

    // li $t1, 5
    // Expands to 
    //     ori $t1, $zero, 5
    instr.init();
    instr.adr = 0x0040000C;
    instr.ins = 0xD << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_TEMP_1 << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_ZERO << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | 0x5;
    prog.add(instr);

    // loop: bgt $t0, $t1, end_loop
    // Expands to
    //      slt $at, $t1, $t0
    //      bne $at, $zero, end_loop
    instr.init();
    instr.adr = 0x00400010;
    instr.ins = 0x0 << R_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_AT << R_INSTR_RD_OFFSET);
    instr.ins = instr.ins | (REG_TEMP_1 << R_INSTR_RS_OFFSET);
    instr.ins = instr.ins | (REG_TEMP_0 << R_INSTR_RT_OFFSET);
    instr.ins = instr.ins | 42;
    prog.add(instr);
    instr.init();
    instr.adr = 0x00400014;
    instr.ins = 0x5 << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_AT << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_ZERO << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | 0x1C;   // offset to end_loop label
    prog.add(instr);

    // lw $a0 ($s0)
    instr.init();
    instr.adr = 0x00400018;
    instr.ins = 0x23 << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_SAVED_0 << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | (REG_ARG_0 << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | 0x0;   // no offset in this case
    prog.add(instr);

    // li $v0 1
    // Expands to
    //      ori $v0 $zero 1
    instr.init();
    instr.adr = 0x0040001C;
    instr.ins = 0xD << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_RETURN_0 << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_ZERO << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | 0x1;
    prog.add(instr);

    // syscall
    // Note yet implemented at time of writing
    instr.init();
    instr.adr = 0x00400020;
    prog.add(instr);

    // addi $s0, $s0, 4
    instr.init();
    instr.adr = 0x00400024;
    instr.ins = 0x8 << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_SAVED_0 << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_SAVED_0 << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | 0x4;
    prog.add(instr);

    // addi $t0, $t0, 1
    instr.init();
    instr.adr = 0x00400028;
    instr.ins = 0x8 << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_TEMP_0 << I_INSTR_RT_OFFSET);
    instr.ins = instr.ins | (REG_TEMP_0 << I_INSTR_RS_OFFSET);
    instr.ins = instr.ins | 0x1;
    prog.add(instr);

    // j loop
    instr.init();
    instr.adr = 0x0040002C;
    instr.ins = 0x2 << J_INSTR_OP_OFFSET;
    instr.ins = instr.ins | ((0x00400010 & 0x0FFFFFFC) >> 2);
    prog.add(instr);
    
    // end_loop
    // No instructions here, just an address
    instr.init();
    instr.adr = 0x00400030;
    prog.add(instr);

    return prog;
}

/*
 * Array and data segment test
 */
TEST_CASE("test_array", "[classic]")
{
    Lexer      lexer;
    Assembler  test_asm;
    SourceInfo src_out;
    Program    prog_out;
    Program    prog_exp;

    test_asm.setVerbose(GLOBAL_VERBOSE);
    // get some source info for this program
    lexer.setVerbose(false);
    lexer.loadFile(test_array_file);
    lexer.lex();

    // add source info to assembler
    src_out = lexer.getSourceInfo();
    test_asm.loadSource(src_out);
    test_asm.assemble();

    // expected program
    prog_exp =  get_array_expected_program();
    // show the resulting program
    prog_out = test_asm.getProgram();
    std::cout << "Expected " << prog_exp.size() << " instructions" << std::endl;
    std::cout << "Output program has " << prog_out.size() << " instructions" << std::endl;

    std::cout << "Expected " << prog_exp.dataSize() << " bytes in data segment" << std::endl;
    std::cout << "Output program has " << prog_out.dataSize() << " bytes in data segment" << std::endl;

    // Print data segment
    for(unsigned int d = 0; d < prog_out.numDataSeg(); ++d)
    {
        DataSeg seg = prog_out.getData(d);
        std::cout << "Seg " << std::setw(3) << std::dec << d << " : ";
        std::cout << seg.toString() << std::endl;
    }

    // Check data segment
    for(unsigned int d = 0; d < prog_out.numDataSeg(); ++d)
    {
        DataSeg exp_seg = prog_exp.getData(d);
        DataSeg out_seg = prog_out.getData(d);

        // Show the diff 
        if(SHOW_ALL_OUTPUT)
        {
            std::cout << exp_seg.diff(out_seg) << std::endl;
            std::cout << "Checking Seg " << std::setw(3) << std::dec << d << "/" 
               << prog_out.numDataSeg() << " : " << std::endl;
            std::cout << "[out]: " << out_seg.toString() << std::endl;
            std::cout << "[exp]: " << exp_seg.toString() << std::endl;
        }

        REQUIRE(exp_seg == out_seg);        // TODO : data segment issue....
    }

    // Check text segment
    Instr instr_exp;
    Instr instr_out;

    for(unsigned int ins = 0; ins < prog_out.size(); ++ins)
    {
        instr_exp = prog_exp.getInstr(ins);
        instr_out = prog_out.getInstr(ins);
        
        if(SHOW_ALL_OUTPUT)
        {
            std::cout << "Checking instruction [" << ins+1 << 
                "/" << prog_out.size() << "]" << std::endl; 
            std::cout << src_out.getText(ins).toString() << std::endl;
            std::cout << "\tExpected : " << instr_exp.toString() << std::endl;
            std::cout << "\tOutput   : " << instr_out.toString() << std::endl;
        }
        
        REQUIRE(instr_exp == instr_out);
    }
    //REQUIRE(prog_exp.size() == prog_out.size());
}


/*
 * Expected assembly output for instr_test.asm
 */
Program get_instr_test_expected_program(void)
{
    Program prog;
    Instr   instr;
    DataSeg data;

    // ten: .word 10
    data.init();
    data.adr = 0x10000000;
    data.data = {0xA};
    prog.add(data);

    // lui $t0 ten 
    instr.init();
    instr.adr = 0x00400000;
    instr.ins = 0xF << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_TEMP_0 < I_INSTR_RT_OFFSET);        
    instr.ins = instr.ins | (0x1000);
    prog.add(instr);

    // lui $at 4096
    instr.init();
    instr.adr = 0x00400004;
    instr.ins = 0xF << I_INSTR_OP_OFFSET;
    instr.ins = instr.ins | (REG_AT < I_INSTR_RT_OFFSET);       
    instr.ins = instr.ins | (0x1000);
    prog.add(instr);

    return prog;
}


// ======== INSTRUCTION TESTS ======== //
TEST_CASE("test_asm_instr", "[classic]")
{
    Lexer      lexer;
    Assembler  test_asm;
    SourceInfo src_out;
    Program    prog_out;
    Program    prog_exp;

    Instr      out_instr;
    Instr      exp_instr;

    test_asm.setVerbose(GLOBAL_VERBOSE);
    lexer.setVerbose(GLOBAL_VERBOSE);

    // inputs
    const std::vector<std::string> input_src = {
        "add $t2, $t0, $t1",
        "addi $t1, $t1, 1",
        "and $t1, $t2, $t3",
        "andi $t0, $t1, 255",
        "beq $a0, $s2, 1024",
        "div $t2, $s4",
        "lw $t1, 4($s4)",
        "lui $at, 4096",
        "mult $t2, $s4",
        "ori $t0, $t1, 4095",
        "sll $t0, $t1, 8",
        "xori $t2, $t0, 255"
    };
    // expected outputs
    const std::vector<Instr> exp_instrs = {
        Instr(TEXT_START_ADDR, 0x01095020),     // add $t2, $t0, $t1
        Instr(TEXT_START_ADDR, 0x21290001),     // addi $t1, $t1, 1
        Instr(TEXT_START_ADDR, 0x014B4824),     // and $t1, $t2 ,$t3
        // 0011 0001 0010 1000 0000 0000 1111 1111 
        // 0x31      0x28      0x00      0xFF
        Instr(TEXT_START_ADDR, 0x312800FF),     // andi $t0, $t1, 255
        // 0001 0000 1001 0010 0000 0100 0000 0000
        // 0x10      0x92      0x04      0x00
        Instr(TEXT_START_ADDR, 0x10920400),     // beq $a0, $s2, 1024
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
        // 0011 0101 0010 1000 0000 1111 1111 1111
        // 0x35      0x28      0x0F      0xFF
        Instr(TEXT_START_ADDR, 0x35280FFF),     // ori $t0, $t1, 4095
        Instr(TEXT_START_ADDR, 0x00094200),     // sll $t0, $t1, 255
        // 0011 1001 0010 1010 0000 0000 1111 1111
        // 0x39      0x2A      0x00      0xFF
        Instr(TEXT_START_ADDR, 0x392A00FF),     // xori $t2, $t0, 255
    };

    // Sanity check that we didn't leave any pairs unmatched 
    REQUIRE(input_src.size() == exp_instrs.size());

    for(unsigned int i = 0; i < input_src.size(); ++i)
    {
        lexer.loadSource(input_src[i]);
        lexer.lex();
        src_out = lexer.getSourceInfo();
        test_asm.loadSource(src_out);
        test_asm.assemble();

        prog_out = test_asm.getProgram();
        REQUIRE(prog_out.numInstrs() == 1);
        out_instr = prog_out.getInstr(0);

        if(out_instr != exp_instrs[i])
        {
            std::cout << "[" << input_src[i] << "]" << " assembly failed" << std::endl;
            std::cout << "[out]: " << out_instr.toString() << std::endl;
            std::cout << "[exp]: " << exp_instrs[i].toString() << std::endl;
        }

        REQUIRE(out_instr == exp_instrs[i]);
    }
}
