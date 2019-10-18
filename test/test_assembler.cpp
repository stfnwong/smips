/* TEST_ASSEMBLER 
 * Unit tests for SMIPS Assembler object 
 *
 * Stefan Wong 2019
 */

#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>
#include <gtest/gtest.h>

// unit(s) under test
#include "Lexer.hpp"
#include "Assembler.hpp"
#include "Program.hpp"


const int i_instr_op_offset = 26;
const int i_instr_rt_offset = 16;
const int i_instr_rs_offset = 21;

const int r_instr_rs_offset = 21;
const int r_instr_rt_offset = 16;
const int r_instr_rd_offset = 11;

const int j_instr_op_offset = 26;

// TODO: when starting address is sorted, update this

/*
 * Expected assembly output for mult_add.asm
 */
Program get_mult_add_expected_program(void)
{
    Program prog;
    Instr instr;

    // LW $t0, 4($gp)
    instr.adr = 0x00400000;
    instr.ins = 35 << i_instr_op_offset;  // I-format opcode
    instr.ins = instr.ins | (8  << i_instr_rt_offset);
    instr.ins = instr.ins | (28 << i_instr_rs_offset);
    instr.ins = instr.ins | 4;
    prog.add(instr);

    instr.init();
    // MULT $t0, $t0, $t0
    instr.adr = 0x00400004;
    instr.ins = 0x18;
    instr.ins = instr.ins | (8 << r_instr_rs_offset);
    instr.ins = instr.ins | (8 << r_instr_rt_offset);
    instr.ins = instr.ins | (8 << r_instr_rd_offset);
    prog.add(instr);

    instr.init();
    // LW $t1, 4($gp)
    instr.adr = 0x00400008;
    instr.ins = 35 << i_instr_op_offset;
    instr.ins = instr.ins | (9 << i_instr_rt_offset);
    instr.ins = instr.ins | (28 << i_instr_rs_offset);
    instr.ins = instr.ins | 4;
    prog.add(instr);

    instr.init();
    // ORI $t2, $zero, 3
    instr.adr = 0x0040000C;
    instr.ins = 0x0C << i_instr_op_offset;
    instr.ins = instr.ins | (10 << i_instr_rt_offset);
    instr.ins = instr.ins | (0  << i_instr_rs_offset);
    instr.ins = instr.ins | 3;
    prog.add(instr);

    instr.init();
    // MULT $t1, $t1, $t2
    instr.adr = 0x00400010;
    instr.ins = 0x18;
    instr.ins = instr.ins | (9  << r_instr_rd_offset);
    instr.ins = instr.ins | (9  << r_instr_rs_offset);
    instr.ins = instr.ins | (10 << r_instr_rt_offset);
    prog.add(instr);

    instr.init();
    // ADD $t2, $t0, $t1
    instr.adr = 0x00400014;
    instr.ins = 0x20;
    instr.ins = instr.ins | (10 << r_instr_rd_offset);
    instr.ins = instr.ins | (8  << r_instr_rs_offset);
    instr.ins = instr.ins | (9  << r_instr_rt_offset);
    prog.add(instr);

    instr.init();
    // SW $t2, 0($gp) 
    instr.adr = 0x00400018;
    instr.ins = 0x2B << i_instr_op_offset;
    instr.ins = instr.ins | (10 << i_instr_rt_offset);
    instr.ins = instr.ins | (28 << i_instr_rs_offset);
    instr.ins = instr.ins | 0;  // has no effect, but mirrors the instruction
    prog.add(instr);

    return prog;
}

/*
 * Expected assembly output for for_loop.asm
 */
Program get_for_loop_expected_program(void)
{
    Program prog;
    Instr   instr;

    // ADD $t0, $gp, $zero
    instr.adr = 0x00400000;
    instr.ins = 0x20;
    instr.ins = instr.ins | (8 << r_instr_rd_offset);
    instr.ins = instr.ins | (28 << r_instr_rs_offset);
    instr.ins = instr.ins | (0 << r_instr_rt_offset);
    prog.add(instr);

    instr.init();
    // LW $t1, 4($gp)
    instr.adr = 0x00400004;
    instr.ins = 35 << i_instr_op_offset;  // I-format opcode
    instr.ins = instr.ins | (9  << i_instr_rt_offset);
    instr.ins = instr.ins | (28 << i_instr_rs_offset);
    instr.ins = instr.ins | 4;
    prog.add(instr);

    instr.init();
    // SLL $t1, $t1, 2
    instr.adr = 0x00400008;
    instr.ins = 0x0;
    instr.ins = instr.ins | (9 << r_instr_rd_offset);
    instr.ins = instr.ins | (9 << r_instr_rs_offset);
    instr.ins = instr.ins | (2 << r_instr_rt_offset);
    prog.add(instr);

    instr.init();
    // ADD $t1, $t1, $gp
    instr.adr = 0x0040000C;
    instr.ins = 0x20;
    instr.ins = instr.ins | (9 << r_instr_rd_offset);
    instr.ins = instr.ins | (9 << r_instr_rs_offset);
    instr.ins = instr.ins | (28 << r_instr_rt_offset);
    prog.add(instr);

    instr.init();
    // ORI $t2, $zero, 256
    instr.adr = 0x00400010;
    instr.ins = 0x0C << i_instr_op_offset;
    instr.ins = instr.ins | (10 << i_instr_rt_offset);
    instr.ins = instr.ins | (0 << i_instr_rs_offset);
    instr.ins = instr.ins | (256);
    prog.add(instr);

    instr.init();
    // TOP : SLTU $t3, $t0, $t1
    instr.adr = 0x00400014;
    instr.ins = 43;
    instr.ins = instr.ins | (11 << r_instr_rd_offset);
    instr.ins = instr.ins | (8 << r_instr_rs_offset);
    instr.ins = instr.ins | (9 << r_instr_rt_offset);
    prog.add(instr);

    instr.init();
    // BEQ $t3, $zero, DONE
    instr.adr = 0x00400018;
    instr.ins = 0x4 << i_instr_op_offset;
    instr.ins = instr.ins | (11 << i_instr_rt_offset);
    instr.ins = instr.ins | (0 << i_instr_rs_offset);
    instr.ins = instr.ins | (0x0040028 & 0x0000FFFF); 
    prog.add(instr);

    instr.init();
    // SW $t2 28($t0)
    instr.adr = 0x0040001C;
    instr.ins = 0x2B << i_instr_op_offset;
    instr.ins = instr.ins | (10 << i_instr_rt_offset);
    instr.ins = instr.ins | (8 << i_instr_rs_offset);
    instr.ins = instr.ins | 28;
    prog.add(instr);

    instr.init();
    // ADDI $t0, $t0, 4
    instr.adr = 0x00400020;
    instr.ins = 0x08 << i_instr_op_offset;
    instr.ins = instr.ins | (8 << i_instr_rs_offset);
    instr.ins = instr.ins | (8 << i_instr_rt_offset);
    instr.ins = instr.ins | 4;
    prog.add(instr);

    instr.init();
    // J TOP 
    instr.adr = 0x00400024;
    instr.ins = 0x02 << j_instr_op_offset;
    instr.ins = instr.ins | 0x00400014;
    prog.add(instr);
    
    instr.init();
    // DONE (dummy instruction)
    instr.adr = 0x00400028;
    prog.add(instr);

    return prog;
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
    
    // ---- Text Section ---- //
    
    // la $s0, list
    // NOTE: <la $s0, list> expands to
    // lui $at, list
    // ori $s0, $at, displ
    instr.init();
    instr.adr = 0x00400000;

    // li $t0, 0
    // NOTE; <li $t0, 0> translates to
    // lui $at, 0
    // ori $t0, $at, 0


    return prog;
}


class TestAssembler : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}

    public:
        std::string test_mult_add_file = "asm/mult_add.asm";
        std::string test_for_loop_file = "asm/for_loop.asm";
        std::string test_array_file = "asm/array.asm";
};



/*
 * Test mult_add example
 */
TEST_F(TestAssembler, test_asm_mult_add)
{
    Lexer      lexer;
    Assembler  test_asm;
    SourceInfo src_out;
    Program    prog_out;
    Program    prog_exp;

    test_asm.setVerbose(true);
    // get some source info for this program
    lexer.setVerbose(false);
    lexer.loadFile(this->test_mult_add_file);
    lexer.lex();

    // add source info to assembler
    src_out = lexer.getSourceInfo();
    test_asm.loadSource(src_out);
    test_asm.assemble();

    // expected program
    prog_exp =  get_mult_add_expected_program();

    // show the resulting program
    prog_out = test_asm.getProgram();
    std::cout << "Expected " << prog_exp.size() << " instructions" << std::endl;
    std::cout << "Output program has " << prog_out.size() << " instructions" << std::endl;
    ASSERT_EQ(prog_exp.size(), prog_out.size());

    Instr instr_exp;
    Instr instr_out;

    std::cout << "\t           Address    Data   " << instr_exp.toString() << std::endl;
    for(unsigned int ins = 0; ins < prog_out.size(); ++ins)
    {
        instr_exp = prog_exp.getInstr(ins);
        instr_out = prog_out.getInstr(ins);
        std::cout << "Checking instruction [" << ins+1 << 
            "/" << prog_out.size() << "]" << std::endl; 
        
        std::cout << "\tExpected : " << instr_exp.toString() << std::endl;
        std::cout << "\tOutput   : " << instr_out.toString() << std::endl;
        
        ASSERT_EQ(instr_exp, instr_out);
        std::cout << "[OK]" << std::endl;
    }
    std::cout << prog_out.toString() << std::endl;
}


/*
 * Test for_loop example
 */
TEST_F(TestAssembler, test_for_loop)
{
    Lexer      lexer;
    Assembler  test_asm;
    SourceInfo src_out;
    Program    prog_out;
    Program    prog_exp;

    test_asm.setVerbose(true);
    // get some source info for this program
    lexer.setVerbose(false);
    lexer.loadFile(this->test_for_loop_file);
    lexer.lex();

    // add source info to assembler
    src_out = lexer.getSourceInfo();
    test_asm.loadSource(src_out);
    test_asm.assemble();

    // expected program
    prog_exp =  get_for_loop_expected_program();

    // show the resulting program
    prog_out = test_asm.getProgram();
    std::cout << "Expected " << prog_exp.size() << " instructions" << std::endl;
    std::cout << "Output program has " << prog_out.size() << " instructions" << std::endl;
    ASSERT_EQ(prog_exp.size(), prog_out.size());

    Instr instr_exp;
    Instr instr_out;

    std::cout << "\t           Address    Data   " << instr_exp.toString() << std::endl;
    for(unsigned int ins = 0; ins < prog_out.size(); ++ins)
    {
        instr_exp = prog_exp.getInstr(ins);
        instr_out = prog_out.getInstr(ins);
        std::cout << "Checking instruction [" << ins+1 << 
            "/" << prog_out.size() << "]" << std::endl; 
        
        std::cout << src_out.getText(ins).toString() << std::endl;

        std::cout << "\tExpected : " << instr_exp.toString() << std::endl;
        std::cout << "\tOutput   : " << instr_out.toString() << std::endl;
        
        ASSERT_EQ(instr_exp, instr_out);
        std::cout << "[OK]" << std::endl;
    }
    std::cout << prog_out.toString() << std::endl;
}


/*
 * Array and data segment test
 */
TEST_F(TestAssembler, test_array)
{
    Lexer      lexer;
    Assembler  test_asm;
    SourceInfo src_out;
    Program    prog_out;
    Program    prog_exp;

    test_asm.setVerbose(true);
    // get some source info for this program
    lexer.setVerbose(false);
    lexer.loadFile(this->test_array_file);
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
    //ASSERT_EQ(prog_exp.size(), prog_out.size());

}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
