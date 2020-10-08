/* TEST_LEXER 
 * Unit tests for SMIPS Lexer object 
 *
 * Stefan Wong 2019
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>

#include "Address.hpp"  // TODO : also do text offset relative to base...
#include "Codes.hpp"
#include "Lexer.hpp"
#include "Source.hpp"
#include "Register.hpp"

const std::string test_mult_add_file = "asm/mult_add.asm";
const std::string test_for_loop_file = "asm/for_loop.asm";
const std::string test_array_file    = "asm/array.asm";
const std::string test_paren_file    = "asm/paren.asm";
const std::string test_psuedo_file   = "asm/psuedo.asm";
const std::string test_instr_file    = "asm/instr_test.asm";

const bool SHOW_ALL_OUTPUT = false;
const bool GLOBAL_VERBOSE = false;


/*
 * Generate mult-add SourceInfo
 */
SourceInfo get_mult_add_expected_source_info(void)
{
    SourceInfo info;
    TextInfo   line;

    // line 1
    // lw $t0, 4($gp)
    line.init();
    line.line_num = 3;
    line.addr     = 0x00400000; 
    line.opcode   = Opcode(LEX_LW, "lw");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1]  = Argument(SYM_REGISTER, REG_GLOBAL);
    line.args[2]  = Argument(SYM_LITERAL, 4);
    info.addText(line);

    // line 2
    // mult $t0, $t0, $t0
    line.init();
    line.line_num = 4;
    line.addr     = 0x00400004; 
    line.opcode   = Opcode(LEX_MULT, "mult");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[2]  = Argument(SYM_REGISTER, REG_TEMP_0);
    info.addText(line);

    // line 3
    // lw $t1, 4($gp)
    line.init();
    line.line_num = 5;
    line.addr     = 0x00400008; 
    line.opcode   = Opcode(LEX_LW, "lw");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1]  = Argument(SYM_REGISTER, REG_GLOBAL);
    line.args[2]  = Argument(SYM_LITERAL, 4);
    info.addText(line);

    // line 4
    // ori $t2, $zero, 3
    line.init();
    line.line_num = 6;
    line.addr     = 0x0040000C;  
    line.opcode   = Opcode(LEX_ORI, "ori");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]  = Argument(SYM_LITERAL, 3);
    line.is_imm   = true;
    info.addText(line);

    // line 5
    // mult $t1, $t1, $t2
    line.init();
    line.line_num = 7;
    line.addr     = 0x00400010;  
    line.opcode   = Opcode(LEX_MULT, "mult");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[2]  = Argument(SYM_REGISTER, REG_TEMP_2);
    info.addText(line);

    // line 7
    // add $t2, $t0, $t1
    line.init();
    line.line_num = 8;
    line.addr     = 0x00400014;  
    line.opcode   = Opcode(LEX_ADD, "add");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[2]  = Argument(SYM_REGISTER, REG_TEMP_1);
    info.addText(line);

    // line 8
    // sw $t2, 0($gp)
    line.init();
    line.line_num = 9;
    line.addr     = 0x00400018;      
    line.opcode   = Opcode(LEX_SW, "sw");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1]  = Argument(SYM_REGISTER, REG_GLOBAL);
    line.args[2]  = Argument(SYM_LITERAL, 0);
    info.addText(line);

    return info;
}


/*
 * Generate SourceInfo for array program
 */
SourceInfo get_array_expected_source_info(void)
{
    SourceInfo info;
    TextInfo   line;
    DataInfo   data_line;

    // line 2
    // .data 
    // Starts a new data segment. There isn't any lexer output that 
    // is specifically associated with this line.

    // line 3
    // list: word 3, 0, 1, 2, 6
    data_line.init();
    data_line.addr = DATA_START_ADDR;
    data_line.label = "list";
    data_line.directive = SYM_DIR_WORD;
    data_line.data = {3, 0, 1, 2, 6};
    info.addData(data_line);


    // line 4 
    // char_arr: .asciiz "hello"
    data_line.init();
    data_line.addr = DATA_START_ADDR + (5 * 32);
    data_line.label = "char_arr";
    data_line.directive = SYM_DIR_ASCIIZ;
    data_line.data = {0x68, 0x65, 0x6C, 0x6C, 0x6F};
    info.addData(data_line);
    
    // line 5 
    // buffer: .space 128
    data_line.init();
    data_line.addr  = DATA_START_ADDR + (5 * 32) + 1;
    data_line.label = "buffer";
    data_line.directive = SYM_DIR_SPACE;
    data_line.space = 128;
    info.addData(data_line);

    // NOTE: the next data segment would start at DATA_START_ADDR + (5 * 32) + 1 + 128
    
    // line 9
    // .text
    // Starts a new text segment. There isn't any lexer output that 
    // is specifically associated with this line.

    // line 10
    // la $s0, list
    // [lui $s0, list]
    line.line_num   = 10;
    line.addr       = 0x00400000;
    line.opcode     = Opcode(LEX_LUI, "lui");
    line.args[0]    = Argument(SYM_REGISTER, REG_SAVED_0);
    line.args[1]    = Argument();
    line.args[2]    = Argument(SYM_LITERAL, (DATA_START_ADDR & 0xFFFF0000) >> 16);
    line.is_symbol  = true;
    line.is_imm     = true;
    line.upper      = true;
    line.symbol     = "list";
    info.addText(line);
    // [ori $s0 $s0, list]
    line.init();
    line.line_num   = 10;
    line.addr       = 0x00400004;
    line.opcode    = Opcode(LEX_ORI, "ori");
    line.args[0]   = Argument(SYM_REGISTER, REG_SAVED_0);
    line.args[1]   = Argument(SYM_REGISTER, REG_SAVED_0);
    line.args[2]   = Argument(SYM_LITERAL, (DATA_START_ADDR & 0x0000FFFF));
    line.is_symbol = true;
    line.is_imm    = true;
    line.lower     = true;
    line.symbol    = "list";
    info.addText(line);

    // line 11
    // li $t0, 0
    // [ori $t0, $zero, 0x0]
    line.init();
    line.line_num = 11;
    line.addr     = 0x00400008;
    line.opcode   = Opcode(LEX_ORI, "ori");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]  = Argument(SYM_LITERAL, 0);
    line.is_imm   = true;
    info.addText(line);

    // line 12
    // li $t1, 5
    // [ori $t1, $zero, 0x5]
    line.init();
    line.line_num = 12;
    line.addr     = 0x0040000C;
    line.opcode   = Opcode(LEX_ORI, "ori");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]  = Argument(SYM_LITERAL, 5);
    line.is_imm   = true;
    info.addText(line);

    // line 15 
    // loop: bgt $t0, $t1, end_loop
    // [slt $at, $t1, $t0]
    line.init();
    line.line_num = 15;
    line.addr     = 0x00400010;
    line.opcode   = Opcode(LEX_SLT, "slt");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[2]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.is_label = true;
    line.label    = "loop";
    info.addText(line);
    // [bne $at, $zero, end_loop]
    // TODO : some args are flippped to pass a test.. but actually its better if we 
    // don't need to do this because its confusing...
    line.init();
    line.line_num  = 15;
    line.addr      = 0x00400014;
    line.opcode    = Opcode(LEX_BNE, "bne");
    line.args[0]   = Argument(SYM_REGISTER, REG_AT);
    line.args[1]   = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]   = Argument(SYM_LITERAL, 0x1C);
    // the immediate here is the offset to the address of end_loop
    line.is_symbol = true;
    line.symbol    = "end_loop";
    line.is_imm    = true;
    info.addText(line);

    // line 17
    // lw $a0, $(s0)
    // NOTE the extra parens here.
    line.init();
    line.line_num = 17;
    line.addr     = 0x00400018;
    line.opcode   = Opcode(LEX_LW, "lw");
    line.args[0]  = Argument(SYM_REGISTER, REG_ARG_0);
    line.args[1]  = Argument(SYM_REGISTER, REG_SAVED_0);
    line.args[2]  = Argument(SYM_LITERAL, 0);
    info.addText(line);

    // line 18
    // li $v0, 1
    // [ori $v0, $zero,  0x01]
    line.init();
    line.line_num = 18;
    line.addr     = 0x0040001C;
    line.opcode   = Opcode(LEX_ORI, "ori");
    line.args[0]  = Argument(SYM_REGISTER, REG_RETURN_0);
    line.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]  = Argument(SYM_LITERAL, 0x1);
    line.is_imm   = true;
    info.addText(line);

    // line 19 
    // SYSCALL
    line.init();
    line.line_num        = 19;
    line.addr            = 0x00400020;
    line.opcode.instr    = LEX_SYSCALL;
    line.opcode.mnemonic = "syscall";
    info.addText(line);

    // line 21 
    // addi $s0, $s0, 4
    line.init();
    line.line_num = 21;
    line.addr     = 0x00400024;
    line.opcode   = Opcode(LEX_ADDI, "addi");
    line.args[0]  = Argument(SYM_REGISTER, REG_SAVED_0);
    line.args[1]  = Argument(SYM_REGISTER, REG_SAVED_0);
    line.args[2]  = Argument(SYM_LITERAL, 0x4);
    line.is_imm   = true;
    info.addText(line);
    
    // line 22 
    // addi $t0, $t0, 1
    line.init();
    line.line_num = 22;
    line.addr     = 0x00400028;
    line.opcode   = Opcode(LEX_ADDI, "addi");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[2]  = Argument(SYM_LITERAL, 0x1);
    line.is_imm   = true;
    info.addText(line);
    
    // line 23
    // j loop
    line.init();
    line.line_num  = 23;
    line.addr      = 0x0040002C;
    line.opcode    = Opcode(LEX_J, "j");
    line.is_symbol = true;
    line.symbol    = "loop";
    line.args[2]   = Argument(SYM_LITERAL, 0x00400010);
    info.addText(line);

    // line 25
    // end_loop:
    // NOTE: 25 due to line end 'overflow'
    line.init();
    line.line_num        = 25;
    line.addr            = 0x00400030;
    line.is_label        = true;
    line.label           = "end_loop";
    info.addText(line);

    return info;
}


/*
 * Test mult_add example
 */
TEST_CASE("test_lex_mult_add", "[classic]")
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(GLOBAL_VERBOSE);
    test_lexer.loadFile(test_mult_add_file);
    test_lexer.lex();

    std::cout << "Running multiply-add test..." << std::endl;

    // get the source info
    expected_src_out = get_mult_add_expected_source_info();
    src_out = test_lexer.getSourceInfo();
    //REQUIRE(expected_src_out.getTextInfoSize() == src_out.getTextInfoSize());
    
    if(SHOW_ALL_OUTPUT)
    {
        std::cout << "Expected output :" << std::endl;
        std::cout << expected_src_out.toString() << std::endl << std::endl;

        std::cout << "Lexer output : " << std::endl;
        std::cout << src_out.toString() << std::endl;
    }

    // Check each line in turn
    TextInfo expected_line;
    TextInfo output_line;
    for(unsigned int line = 0; line < expected_src_out.getTextInfoSize(); ++line)
    {
        expected_line = expected_src_out.getText(line);
        output_line = src_out.getText(line);

        if(expected_line != output_line)
        {
            std::cout << "Line " << std::dec << line + 1 <<  "/" << 
                std::dec << expected_src_out.getTextInfoSize() << 
                " mismatch" << std::endl;
            std::cout << "Expecting :" << std::endl << expected_line.toString() << std::endl;
            std::cout << "Got :" << std::endl << output_line.toString() << std::endl;
            std::cout << std::endl << "    diff : " << std::endl;
            std::cout << expected_line.diff(output_line) << std::endl;
        }
        REQUIRE(expected_line == output_line);
        std::cout << "    [OK]" << std::endl;
    }
}


/*
 * Generate SourceInfo for for-loop program
 */
SourceInfo get_for_loop_expected_source_info(void)
{
    SourceInfo info;
    TextInfo   line;

    // line 4
    // ADD $t0, $gp, $zero
    line.line_num = 4;
    line.addr     = 0x00400000;
    line.opcode   = Opcode(LEX_ADD, "add");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1]  = Argument(SYM_REGISTER, REG_GLOBAL);
    line.args[2]  = Argument(SYM_REGISTER, REG_ZERO);
    info.addText(line);

    // line 5
    // LW $t1, 4($gp)
    line.init();
    line.line_num = 5;
    line.addr     = 0x00400004;
    line.opcode   = Opcode(LEX_LW, "lw");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1]  = Argument(SYM_REGISTER, REG_GLOBAL);
    line.args[2]  = Argument(SYM_LITERAL, 4);
    info.addText(line);

    // line 6
    // SLL $t1, $t1, 2
    line.init();
    line.line_num = 6;
    line.addr     = 0x00400008;
    line.opcode   = Opcode(LEX_SLL, "sll");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[2]  = Argument(SYM_LITERAL, 2);
    line.is_imm   = true;
    info.addText(line);

    // line 7
    // ADD $t1, $t1, $gp
    line.init();
    line.line_num = 7;
    line.addr     = 0x0040000C;
    line.opcode   = Opcode(LEX_ADD, "add");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[2]  = Argument(SYM_REGISTER, REG_GLOBAL);
    info.addText(line);

    // line 8
    // ORI $t2, $zero, 256
    line.init();
    line.line_num = 8;
    line.addr     = 0x00400010;
    line.opcode   = Opcode(LEX_ORI, "ori");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]  = Argument(SYM_LITERAL, 256);
    line.is_imm   = true;
    info.addText(line);

    // label gets attached to first following non-empty line
    // line 10
    // top: SLTU $t3, $t0, $t1
    line.init();
    line.line_num = 10;
    line.addr     = 0x00400014;
    line.opcode   = Opcode(LEX_SLTU, "sltu");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_3);
    line.args[1]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[2]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.is_label = true;
    line.label    = "top";
    info.addText(line);

    // line 11
    // BEQ $t3, $zero, done
    line.init();
    line.line_num  = 11;
    line.addr      = 0x00400018;
    line.opcode    = Opcode(LEX_BEQ, "beq");
    line.args[0]   = Argument(SYM_REGISTER, REG_TEMP_3);
    line.args[1]   = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]   = Argument(SYM_LITERAL, 0x00400028 - 0x00400018);
    line.is_symbol = true;
    line.symbol    = "done";
    line.is_imm    = true;
    info.addText(line);

    // line 12
    // SW $t2, 28($t0)
    line.init();
    line.line_num = 12;
    line.addr     = 0x0040001C;
    line.opcode   = Opcode(LEX_SW, "sw");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[2]  = Argument(SYM_LITERAL, 28);
    info.addText(line);

    // line 13
    // ADDI $t0, $t0, 4
    line.init();
    line.line_num = 13;
    line.addr     = 0x00400020;
    line.opcode   = Opcode(LEX_ADDI, "addi");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[2]  = Argument(SYM_LITERAL, 4);
    line.is_imm   = true;
    info.addText(line);

    // line 14
    // J TOP
    line.init();
    line.line_num  = 14;
    line.addr      = 0x00400024;
    line.opcode    = Opcode(LEX_J, "j");
    line.is_symbol = true;
    line.symbol    = "top";
    line.args[2]   = Argument(SYM_LITERAL, 0x00400014);
    info.addText(line);

    // DONE
    line.init();
    line.line_num  = 16;      
    line.addr      = 0x00400028;
    line.is_label  = true;
    line.label     = "done";
    info.addText(line);

    return info;
}


/*
 * Test for_loop example
 */
TEST_CASE("test_for_loop", "[classic]")
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(GLOBAL_VERBOSE);
    test_lexer.loadFile(test_for_loop_file);
    test_lexer.lex();

    std::cout << "Running for-loop test..." << std::endl;

    // get the source info
    expected_src_out = get_for_loop_expected_source_info();
    src_out = test_lexer.getSourceInfo();

    if(SHOW_ALL_OUTPUT)
    {
        std::cout << "Expected output :" << std::endl;
        std::cout << expected_src_out.toString() << std::endl << std::endl;

        std::cout << "Lexer output : " << std::endl;
        std::cout << src_out.toString() << std::endl;

        // before we check each line, dump the symbol table and print
        SymbolTable sym_table = test_lexer.getSymTable();
        std::cout << "Symbol Table: " << std::endl;

        for(unsigned int sym = 0; sym < sym_table.size(); ++sym)
        {
            Symbol cur_sym = sym_table.get(sym);
            std::cout << "     " << sym << " " << 
                cur_sym.toString() << std::endl;
        }
    }

    //REQUIRE(expected_src_out.getTextInfoSize() == src_out.getNumLines());

    // Check each line in turn
    TextInfo expected_line;
    TextInfo output_line;
    
    for(unsigned int line = 0; line < expected_src_out.getTextInfoSize(); ++line)
    {
        expected_line = expected_src_out.getText(line);
        output_line = src_out.getText(line);

        if(expected_line != output_line)
        {
            std::cout << "Line " << std::dec << line+1 << "/" << 
                std::dec << expected_src_out.getTextInfoSize() <<
                "mismatch" << std::endl;
            std::cout << "Expecting :" << std::endl << expected_line.toString() << std::endl;
            std::cout << "Got :" << std::endl << output_line.toString() << std::endl;
            std::cout << "    diff : " << std::endl;
            std::cout << expected_line.diff(output_line) << std::endl;
        }
        REQUIRE(expected_line == output_line);
    }
}

/*
 * Test data_region example (arrays, etc)
 */
TEST_CASE("test_array", "[classic]")
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(GLOBAL_VERBOSE);
    test_lexer.loadFile(test_array_file);
    test_lexer.lex();

    std::cout << "Running array test..." << std::endl;

    // get the source info
    expected_src_out = get_array_expected_source_info();
    src_out = test_lexer.getSourceInfo();

    if(SHOW_ALL_OUTPUT)
    {
        std::cout << "Expected output :" << std::endl;
        std::cout << expected_src_out.toString() << std::endl << std::endl;

        std::cout << "Lexer output : " << std::endl;
        std::cout << src_out.toString() << std::endl;

        // before we check each line, dump the symbol table and print
        std::cout << std::dec << src_out.getDataInfoSize() << " lines in data segment" << std::endl;
        std::cout << std::dec << src_out.getTextInfoSize() << " lines in text segment" << std::endl;

        SymbolTable sym_table = test_lexer.getSymTable();
        std::cout << "Symbol Table: " << std::endl;

        for(unsigned int sym = 0; sym < sym_table.size(); ++sym)
        {
            Symbol cur_sym = sym_table.get(sym);
            std::cout << "     " << sym << " " << 
                cur_sym.toString() << std::endl;
        }

    }

    // Check each line in turn
    TextInfo expected_line;
    TextInfo output_line;
    for(unsigned int line = 0; line < expected_src_out.getTextInfoSize(); ++line)
    {
        expected_line = expected_src_out.getText(line);
        output_line = src_out.getText(line);

        if(expected_line != output_line)
        {
            std::cout << "Line " << std::dec << line+1 << "/" << 
                expected_src_out.getTextInfoSize() << " mismatch " << std::endl;
            std::cout << "Expecting :" << std::endl << expected_line.toString() << std::endl;
            std::cout << "Got :" << std::endl << output_line.toString() << std::endl;
            std::cout << "    diff : " << std::endl;
            std::cout << expected_line.diff(output_line) << std::endl;
        }
        REQUIRE(expected_line == output_line);      
    }
    REQUIRE(expected_src_out.getTextInfoSize() == src_out.getTextInfoSize());
    REQUIRE(expected_src_out.getDataInfoSize() == src_out.getDataInfoSize());


    std::cout << "Error strings : " << std::endl << std::endl;
    std::cout << src_out.errString() << std::endl;
}

/*
 * Paren expansion expected program
 */
SourceInfo get_paren_expected_source_info(void)
{
    SourceInfo info;
    TextInfo   line;

    // line 4
    // lw $t1, ($gp)
    line.init();
    line.line_num = 4;
    line.addr     = 0x00400000;
    line.opcode   = Opcode(LEX_LW, "lw");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1]  = Argument(SYM_REGISTER, REG_GLOBAL);
    line.args[2]  = Argument(SYM_LITERAL, 0);
    info.addText(line);

    // line 5
    // lw $t2, 4($gp)
    line.init();
    line.line_num = 5;
    line.addr     = 0x00400004;
    line.opcode   = Opcode(LEX_LW, "lw");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1]  = Argument(SYM_REGISTER, REG_GLOBAL);
    line.args[2]  = Argument(SYM_LITERAL, 4);
    info.addText(line);

    // line 6
    // lw $t1, 4($gp)
    line.init();
    line.line_num = 6;
    line.addr     = 0x00400008;
    line.opcode   = Opcode(LEX_LW, "lw");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1]  = Argument(SYM_REGISTER, REG_GLOBAL);
    line.args[2]  = Argument(SYM_LITERAL, 4);
    info.addText(line);

    return info;
}


TEST_CASE("test_paren_parse", "[classic]")
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(GLOBAL_VERBOSE);
    test_lexer.loadFile(test_paren_file);
    test_lexer.lex();

    // get the source info
    expected_src_out = get_paren_expected_source_info();
    src_out = test_lexer.getSourceInfo();

    if(SHOW_ALL_OUTPUT)
    {
        std::cout << "Expected output :" << std::endl;
        std::cout << expected_src_out.toString() << std::endl << std::endl;

        std::cout << "Lexer output : " << std::endl;
        std::cout << src_out.toString() << std::endl;
    }

    // Check each line in turn
    TextInfo expected_line;
    TextInfo output_line;

    for(unsigned int line = 0; line < expected_src_out.getTextInfoSize(); ++line)
    {
        expected_line = expected_src_out.getText(line);
        output_line = src_out.getText(line);

        if(expected_line != output_line)
        {
            std::cout << "Line " << std::dec << line+1 << "/" << 
                expected_src_out.getTextInfoSize() << " mismatch" << std::endl;
            std::cout << "Expected " << std::endl << expected_line.toString() << std::endl;
            std::cout << "Got " << std::endl << output_line.toString() << std::endl;
            std::cout << "    diff : " << std::endl;
            std::cout << expected_line.diff(output_line) << std::endl;
        }
        REQUIRE(expected_line == output_line);
    }
}


/*
 * Psuedo instruction expansion
 */
SourceInfo get_psuedo_instr_source_info(void)
{
    SourceInfo info;
    TextInfo   line;
    DataInfo   data_line;

    // line 8
    // bgt $s0, $t1 8
    //
    // we expect this to generate two instructions, 
    // slt $at, $t1, $s0
    line.init();
    line.line_num = 8;
    line.addr     = 0x00400000;
    line.opcode   = Opcode(LEX_SLT, "slt");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[2]  = Argument(SYM_REGISTER, REG_SAVED_0);
    info.addText(line);
    // bne $at, $zero, 8
    // NOTE : args flipped for assembler
    line.init();
    line.line_num = 8;
    line.addr     = 0x00400004;
    line.opcode   = Opcode(LEX_BNE, "bne");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]  = Argument(SYM_LITERAL, 8);
    line.is_imm   = true;
    info.addText(line);

    // li $t2, 5
    // 16-bit immediate, therefore we can transform to just ori
    // TODO : check what type[1] should be.... The literal could go
    // in slot 1, its only supposed to go into slot 2 when there is a 
    // offset in front of a register..
    line.init();
    line.line_num = 9;
    line.addr     = 0x00400008;
    line.opcode   = Opcode(LEX_ORI, "ori");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_2);
    line.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]  = Argument(SYM_LITERAL, 5);
    line.is_imm   = true;
    info.addText(line);

    // li $t1, 75000
    // 32-bit immedaite, therefore we need two instructions
    // lui $t0,  75000 & (0xFFFF0000)
    line.init();
    line.line_num = 10;
    line.addr     = 0x0040000C;
    line.opcode   = Opcode(LEX_LUI, "lui");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1]  = Argument(SYM_NONE, 0);
    line.args[2]  = Argument(SYM_LITERAL, (75000 & 0xFFFF0000) >> 16);
    line.is_imm   = true;
    line.upper    = true;
    info.addText(line);
    // ori $t0, $t0, 75000 & (0x0000FFFF)
    line.init();
    line.line_num = 10;
    line.addr     = 0x00400010;
    line.opcode   = Opcode(LEX_ORI, "ori");
    line.args[0]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1]  = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[2]  = Argument(SYM_LITERAL, 75000 & 0x0000FFFF);
    line.is_imm   = true;
    line.lower    = true;
    info.addText(line);

    // la $t1, arr
    // lui $t1, DATA_START_ADDR & (0xFFFF0000)
    line.init();
    line.line_num  = 11;
    line.addr      = 0x00400014;
    line.opcode    = Opcode(LEX_LUI, "lui");
    line.args[0]   = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1]   = Argument(SYM_NONE, 0);
    line.args[2]   = Argument(SYM_LITERAL, (DATA_START_ADDR & 0xFFFF0000) >> 16);
    line.upper     = true;
    line.is_imm    = true;
    line.is_symbol = true;
    line.symbol    = "arr";
    info.addText(line);

    // ori  $t1, $t1, DATA_START_ADDR & (0x0000FFFF)
    line.init();
    line.line_num  = 11;
    line.addr      = 0x00400018;
    line.opcode    = Opcode(LEX_ORI, "ori");
    line.args[0]   = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[1]   = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[2]   = Argument(SYM_LITERAL, DATA_START_ADDR & 0x0000FFFF);
    line.is_imm    = true;
    line.is_symbol = true;
    line.lower     = true;
    line.symbol    = "arr";
    info.addText(line);

    // branch_label: bgt $s1 $t1 arr
    // slt $at $t1 $s1
    line.init();
    line.line_num  = 15;
    line.addr      = 0x0040001C;
    line.opcode    = Opcode(LEX_SLT, "slt");
    line.args[0]   = Argument(SYM_REGISTER, REG_AT);
    line.args[1]   = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[2]   = Argument(SYM_REGISTER, REG_SAVED_1);
    line.is_label  = true;
    line.label     = "branch_label";
    info.addText(line);

    // bne $at $zero 0x2 
    // NOTE : args flipped for assembler
    line.init();
    line.line_num = 15;
    line.addr     = 0x00400020;
    line.opcode   = Opcode(LEX_BNE, "bne");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]  = Argument(SYM_LITERAL, 0x2);
    line.is_imm   = true;
    info.addText(line);

    // blt $s0 $t0 0x20
    // slt $at $s0 $t0
    line.init();
    line.line_num = 16;
    line.addr     = 0x00400024;
    line.opcode   = Opcode(LEX_SLT, "slt");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_REGISTER, REG_SAVED_0);
    line.args[2]  = Argument(SYM_REGISTER, REG_TEMP_0);
    info.addText(line);
    // bne $at $zero 0x20
    // NOTE : args flipped for assembler
    line.init();
    line.line_num = 16;
    line.addr     = 0x00400028;
    line.opcode   = Opcode(LEX_BNE, "bne");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]  = Argument(SYM_LITERAL, 0x20);
    line.is_imm   = true;
    info.addText(line);

    // blt $s0 $t2 0x40
    // slt $at $s0 $t2 
    line.init();
    line.line_num = 17;
    line.addr     = 0x0040002C;
    line.opcode   = Opcode(LEX_SLT, "slt");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_REGISTER, REG_SAVED_0);
    line.args[2]  = Argument(SYM_REGISTER, REG_TEMP_2);
    info.addText(line);

    // bne $at $zero 0x40
    // Flipped args for assembler
    line.init();
    line.line_num = 17;
    line.addr     = 0x00400030;
    line.opcode   = Opcode(LEX_BNE, "bne");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]  = Argument(SYM_LITERAL, 0x40);
    line.is_imm          = true;
    info.addText(line);

    // bge $s1 $t0 4
    // slt $at $s1 $t0
    line.init();
    line.line_num = 20;
    line.addr     = 0x00400034;
    line.opcode   = Opcode(LEX_SLT, "slt");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_REGISTER, REG_SAVED_1);
    line.args[2]  = Argument(SYM_REGISTER, REG_TEMP_0);
    info.addText(line);
    // beq $at $zero 0x4
    line.init();
    line.line_num = 20;
    line.addr     = 0x00400038;
    line.opcode   = Opcode(LEX_BEQ, "beq");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]  = Argument(SYM_LITERAL, 0x4);
    line.is_imm          = true;
    info.addText(line);

    // ble $s3 $t1 10
    // slt $at $t1 $s3
    line.init();
    line.line_num = 21;
    line.addr     = 0x0040003C;
    line.opcode   = Opcode(LEX_SLT, "slt");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_REGISTER, REG_TEMP_1);
    line.args[2]  = Argument(SYM_REGISTER, REG_SAVED_3);
    info.addText(line);
    // beq $at $zero 0xA
    line.init();
    line.line_num = 21;
    line.addr     = 0x00400040;
    line.opcode   = Opcode(LEX_BEQ, "beq");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]  = Argument(SYM_LITERAL, 0xA);
    line.is_imm   = true;
    info.addText(line);

    // bgtu $s0 $t5 10
    // sltu $at $t5 $s0
    line.init();
    line.line_num = 22;
    line.addr     = 0x00400044;
    line.opcode   = Opcode(LEX_SLTU, "sltu");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_REGISTER, REG_TEMP_5);
    line.args[2]  = Argument(SYM_REGISTER, REG_SAVED_0);
    info.addText(line);
    // bne $at $zero 0xA
    // NOTE : args flipped for assembler
    line.init();
    line.line_num = 22;
    line.addr     = 0x00400048;
    line.opcode   = Opcode(LEX_BNE, "bne");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]  = Argument(SYM_LITERAL, 0xA);
    line.is_imm   = true;
    info.addText(line);

    // beqz $s2 500
    // beq $s2 $zero 500
    line.init();
    line.line_num = 23;
    line.addr     = 0x0040004C;
    line.opcode   = Opcode(LEX_BEQ, "beq");
    line.args[0]  = Argument(SYM_REGISTER, REG_SAVED_2);
    line.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
    line.args[2]  = Argument(SYM_LITERAL, 500);
    line.is_imm   = true;
    info.addText(line);

    // arr: .word 3
    data_line.init();
    data_line.line_num  = 5;
    data_line.addr      = DATA_START_ADDR;
    data_line.label     = "arr";
    data_line.directive = SYM_DIR_WORD;
    data_line.data      = {3};
    data_line.is_label  = true;
    info.addData(data_line);

    return info;
}


/*
 * Psuedo instruction expansion test
 */
TEST_CASE("test_psuedo_instr", "[classic]")
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(GLOBAL_VERBOSE);
    test_lexer.loadFile(test_psuedo_file);
    test_lexer.lex();

    // get the source info
    expected_src_out = get_psuedo_instr_source_info();
    src_out = test_lexer.getSourceInfo();

    if(SHOW_ALL_OUTPUT)
    {
        std::cout << "Expected output :" << std::endl;
        std::cout << expected_src_out.toString() << std::endl << std::endl;

        std::cout << "Lexer output : " << std::endl;
        std::cout << src_out.toString() << std::endl;

        SymbolTable sym_table = test_lexer.getSymTable();
        std::cout << "Symbol Table: " << std::endl;

        for(unsigned int sym = 0; sym < sym_table.size(); ++sym)
        {
            Symbol cur_sym = sym_table.get(sym);
            std::cout << "     " << sym << " " << 
                cur_sym.toString() << std::endl;
        }
    }
    // Check each line in turn
    TextInfo expected_line;
    TextInfo output_line;

    std::cout << src_out.getTextInfoSize() << " lines in output text segment" << std::endl;
    std::cout << src_out.getDataInfoSize() << " lines in output data segment" << std::endl;

    for(unsigned int line = 0; line < expected_src_out.getTextInfoSize(); ++line)
    {
        expected_line = expected_src_out.getText(line);
        output_line = src_out.getText(line);

        if(expected_line != output_line)
        {
            std::cout << "Line " << std::dec << line+1 << "/" <<
                expected_src_out.getTextInfoSize() << " mismatch " << std::endl;
            std::cout << "Expected " << std::endl << expected_line.toString() << std::endl;
            std::cout << "Got " << std::endl << output_line.toString() << std::endl;
            std::cout << "    diff : " << std::endl;
            std::cout << expected_line.diff(output_line) << std::endl;
        }
        REQUIRE(expected_line == output_line);      
    }

    // Also check that we have the corect number of text and data segments 
    REQUIRE(1 == src_out.getDataInfoSize());
    REQUIRE(20 == src_out.getTextInfoSize());
}


/*
 *  All instruction test 
 */
SourceInfo get_instr_test_source_info(void)
{
    SourceInfo info;
    TextInfo   line;
    DataInfo   data_line;

    // .data ten: word 10
    data_line.init();
    data_line.addr  = DATA_START_ADDR;
    data_line.label = "ten";
    data_line.directive = SYM_DIR_WORD;
    data_line.data = {10};
    info.addData(data_line);

    // lui $t0 ten
    line.init();
    line.line_num  = 9;
    line.addr      = 0x00400000;
    line.opcode    = Opcode(LEX_LUI, "lui");
    line.args[0]   = Argument(SYM_REGISTER, REG_TEMP_0);
    line.args[1]   = Argument(SYM_NONE, 0);
    line.args[2]   = Argument(SYM_LITERAL, (DATA_START_ADDR & 0xFFFF0000) >> 16); 
    line.is_imm    = true;
    line.is_symbol = true;
    line.upper     = true;
    line.symbol    = "ten";
    info.addText(line);

    // lui $at 4096
    line.init();
    line.line_num = 10;
    line.addr     = 0x00400004;
    line.opcode   = Opcode(LEX_LUI, "lui");
    line.args[0]  = Argument(SYM_REGISTER, REG_AT);
    line.args[1]  = Argument(SYM_NONE, 0);
    line.args[2]  = Argument(SYM_LITERAL, 4096);
    line.is_imm   = true;
    line.upper    = true;
    info.addText(line);

    return info;
}


// TODO : finish this test once the section selection for symbols is implemented
/*
 * All instruction test
 */
TEST_CASE("test_instr", "[classic]")
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(GLOBAL_VERBOSE);
    test_lexer.loadFile(test_instr_file);
    test_lexer.lex();

    // get the source info
    expected_src_out = get_instr_test_source_info();
    src_out = test_lexer.getSourceInfo();

    if(SHOW_ALL_OUTPUT)
    {
        std::cout << "Expected output :" << std::endl;
        std::cout << expected_src_out.toString() << std::endl << std::endl;

        std::cout << "Lexer output : " << std::endl;
        std::cout << src_out.toString() << std::endl;

        SymbolTable sym_table = test_lexer.getSymTable();
        std::cout << "Symbol Table: " << std::endl;

        for(unsigned int sym = 0; sym < sym_table.size(); ++sym)
        {
            Symbol cur_sym = sym_table.get(sym);
            std::cout << "     " << sym << " " << 
                cur_sym.toString() << std::endl;
        }
    }
    // Check each line in turn
    TextInfo expected_line;
    TextInfo output_line;

    std::cout << src_out.getTextInfoSize() << " lines in output text segment" << std::endl;
    std::cout << src_out.getDataInfoSize() << " lines in output data segment" << std::endl;

    for(unsigned int line = 0; line < expected_src_out.getTextInfoSize(); ++line)
    {
        expected_line = expected_src_out.getText(line);
        output_line = src_out.getText(line);

        if(expected_line != output_line)
        {
            std::cout << "Line " << std::dec << line+1 << "/" <<
                expected_src_out.getTextInfoSize() << " mismatch " << std::endl;
            std::cout << "Expected " << std::endl << expected_line.toString() << std::endl;
            std::cout << "Got " << std::endl << output_line.toString() << std::endl;
            std::cout << "    diff : " << std::endl;
            std::cout << expected_line.diff(output_line) << std::endl;
        }
        REQUIRE(expected_line == output_line);      
    }
}

TEST_CASE("test_lexer_load_source", "[classic]")
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(true);
    const std::string test_src = "sll $t0, $t0, 2\nsll $t1, $t1, 5\n";

    test_lexer.loadSource(test_src);
    std::cout << "Text in lexer is : " << std::endl;
    std::cout << test_lexer.getText() << std::endl;
    test_lexer.lex();

    src_out = test_lexer.getSourceInfo();
    std::cout << src_out.getTextInfoSize() << "line in text segment " << std::endl;
    
    std::cout << src_out.toString() << std::endl;
}
