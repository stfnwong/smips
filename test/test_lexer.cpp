/* TEST_LEXER 
 * Unit tests for SMIPS Lexer object 
 *
 * Stefan Wong 2019
 */

#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>
#include <gtest/gtest.h>

#include "Codes.hpp"
#include "Lexer.hpp"
#include "Source.hpp"


class TestLexer : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}

    public:
        std::string test_mult_add_file = "asm/mult_add.asm";
        std::string test_for_loop_file = "asm/for_loop.asm";
        std::string test_array_file = "asm/array.asm";
        std::string test_paren_file = "asm/paren.asm";
        std::string test_psuedo_file = "asm/psuedo.asm";
};


// TODO : adjust starting address later 
SourceInfo get_mult_add_expected_source_info(void)
{
    SourceInfo info;
    TextInfo   line;

    // line 1
    // lw $t0, 4($gp)
    line.init();
    line.line_num        = 3;
    line.addr            = 0x00400000; //0x00400000;      
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "lw";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_GLOBAL;
    line.val[2]          = 4;
    line.type[2]         = SYM_LITERAL;
    info.addText(line);

    // line 2
    // mult $t0, $t0, $t0
    line.init();
    line.line_num        = 4;
    line.addr            = 0x00400001; //0x00400001;      
    line.opcode.instr    = LEX_MULT;
    line.opcode.mnemonic = "mult";
    line.val[0]         = 0;
    line.type[0]        = SYM_REG_TEMP;
    line.val[1]         = 0;
    line.type[1]        = SYM_REG_TEMP;
    line.val[2]         = 0;
    line.type[2]        = SYM_REG_TEMP;
    info.addText(line);

    // line 3
    // lw $t1, 4($gp)
    line.init();
    line.line_num        = 5;
    line.addr            = 0x00400002;  //0x00400002;      
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "lw";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_GLOBAL;
    line.val[2]          = 4;
    line.type[2]         = SYM_LITERAL;
    info.addText(line);

    // line 4
    // ori $t2, $zero, 3
    line.init();
    line.line_num        = 6;
    line.addr            = 0x00400003;  //0x00400003;      
    line.opcode.instr    = LEX_ORI;
    line.opcode.mnemonic = "ori";
    line.is_imm          = true;
    line.val[0]          = 2;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_ZERO;
    line.val[2]          = 3;
    line.type[3]         = SYM_LITERAL;
    info.addText(line);

    // line 5
    // mult $t1, $t1, $t2
    line.init();
    line.line_num        = 7;
    line.addr            = 0x00400004;  //0x00400004;      
    line.opcode.instr    = LEX_MULT;
    line.opcode.mnemonic = "mult";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 1;
    line.type[1]         = SYM_REG_TEMP;
    line.val[2]          = 2;
    line.type[3]         = SYM_REG_TEMP;
    info.addText(line);

    // line 7
    // add $t2, $t0, $t1
    line.init();
    line.line_num        = 8;
    line.addr            = 0x00400005;  //0x00400005;      
    line.opcode.instr    = LEX_ADD;
    line.opcode.mnemonic = "add";
    line.val[0]          = 2;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_TEMP;
    line.val[2]          = 1;
    line.type[3]         = SYM_REG_TEMP;
    info.addText(line);

    // line 8
    // sw $t2, 0($gp)
    line.init();
    line.line_num        = 9;
    line.addr            = 0x00400006;      //0x00400006;
    line.opcode.instr    = LEX_SW;
    line.opcode.mnemonic = "sw";
    line.val[0]          = 2;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_GLOBAL;
    info.addText(line);

    return info;
}

SourceInfo get_for_loop_expected_source_info(void)
{
    SourceInfo info;
    TextInfo   line;

    // line 4
    // ADD $t0, $gp, $zero
    line.line_num        = 4;
    line.addr            = 0x00400000;
    line.opcode.instr    = LEX_ADD;
    line.opcode.mnemonic = "add";
    line.val[0]         = 0;
    line.type[0]        = SYM_REG_TEMP;
    line.val[1]         = 0;
    line.type[1]        = SYM_REG_GLOBAL;
    line.type[2]        = SYM_REG_ZERO;
    info.addText(line);

    // line 5
    // LW $t1, 4($gp)
    line.init();
    line.line_num        = 5;
    line.addr            = 0x00400001;
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "lw";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_GLOBAL;
    // offset
    line.val[2]          = 4;
    line.type[2]         = SYM_LITERAL;
    info.addText(line);

    // line 6
    // SLL $t1, $t1, 2
    line.init();
    line.line_num        = 6;
    line.addr            = 0x00400002;
    line.opcode.instr    = LEX_SLL;
    line.opcode.mnemonic = "sll";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 1;
    line.type[1]         = SYM_REG_TEMP;
    line.val[2]          = 2;
    line.type[2]         = SYM_LITERAL;
    line.is_imm          = true;
    info.addText(line);

    // line 7
    // ADD $t1, $t1, $gp
    line.init();
    line.line_num        = 7;
    line.addr            = 0x00400003;
    line.opcode.instr    = LEX_ADD;
    line.opcode.mnemonic = "add";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 1;
    line.type[1]         = SYM_REG_TEMP;
    line.val[2]          = 0;
    line.type[2]         = SYM_REG_GLOBAL;
    info.addText(line);

    // line 8
    // ORI $t2, $zero, 256
    line.init();
    line.line_num        = 8;
    line.addr            = 0x00400004;
    line.opcode.instr    = LEX_ORI;
    line.opcode.mnemonic = "ori";
    line.val[0]          = 2;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_ZERO;
    line.val[2]          = 256;
    line.type[2]         = SYM_LITERAL;
    line.is_imm          = true;
    info.addText(line);

    // label gets attached to first following non-empty line
    // line 10
    // top: SLTU $t3, $t0, $t1
    line.init();
    line.line_num        = 10;
    line.addr            = 0x00400005;
    line.opcode.instr    = LEX_SLTU;
    line.opcode.mnemonic = "sltu";
    line.val[0]         = 3;
    line.type[0]        = SYM_REG_TEMP;
    line.val[1]         = 0;
    line.type[1]        = SYM_REG_TEMP;
    line.val[2]         = 1;
    line.type[2]        = SYM_REG_TEMP;
    line.is_label        = true;
    line.label           = "top";
    info.addText(line);

    // line 11
    // BEQ $t3, $zero, done
    line.init();
    line.line_num        = 11;
    line.addr            = 0x00400006;
    line.opcode.instr    = LEX_BEQ;
    line.opcode.mnemonic = "beq";
    line.val[0]          = 3;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_ZERO;
    // 3rd arg is actually converted literal
    line.type[2]         = SYM_LITERAL;
    line.val[2]          = 0x0040000A;
    line.is_symbol       = true;
    line.symbol          = "done";
    info.addText(line);

    // line 12
    // SW $t2, 28($t8)
    line.init();
    line.line_num        = 12;
    line.addr            = 0x00400007;
    line.opcode.instr    = LEX_SW;
    line.opcode.mnemonic = "sw";
    line.val[0]          = 2;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_TEMP;
    line.val[2]          = 28;
    line.type[2]         = SYM_LITERAL;
    info.addText(line);

    // line 13
    // ADDI $t0, $t0, 4
    line.init();
    line.line_num        = 13;
    line.addr            = 0x00400008;
    line.opcode.instr    = LEX_ADDI;
    line.opcode.mnemonic = "sw";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_TEMP;
    line.val[2]          = 4;
    line.type[2]         = SYM_LITERAL;
    line.is_imm          = true;
    info.addText(line);

    // line 14
    // J TOP
    line.init();
    line.line_num        = 14;
    line.addr            = 0x00400009;
    line.opcode.instr    = LEX_J;
    line.opcode.mnemonic = "j";
    line.is_symbol       = true;
    line.symbol          = "top";
    // 3 arg is also converted literal in this case
    line.type[2]         = SYM_LITERAL;
    line.val[2]          = 0x00400005;
    info.addText(line);

    // DONE
    line.init();
    line.line_num        = 16;      
    line.addr            = 0x0040000A;
    line.is_label        = true;
    line.label           = "done";
    info.addText(line);

    return info;
}

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
    data_line.addr = 0;
    data_line.label = "list";
    data_line.directive = SYM_DIR_WORD;
    data_line.data = {3, 0, 1, 2, 6};
    info.addData(data_line);


    // line 4 
    // char_arr: .asciiz "hello"
    data_line.init();
    data_line.addr = 4;
    data_line.label = "char_arr";
    data_line.directive = SYM_DIR_ASCIIZ;
    data_line.data = {0x68, 0x65, 0x6C, 0x6C, 0x6F};
    info.addData(data_line);
    
    // line 5 
    // buffer: .space 128
    data_line.init();
    data_line.label = "buffer";
    data_line.directive = SYM_DIR_SPACE;
    data_line.space = 128;
    info.addData(data_line);
    
    // line 9
    // .text
    // Starts a new text segment. There isn't any lexer output that 
    // is specifically associated with this line.

    // line 10
    // la $s0, list
    line.line_num        = 10;
    line.addr            = 0x00400000;
    line.opcode.instr    = LEX_LA;
    line.opcode.mnemonic = "la";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_SAVED;
    line.is_symbol       = true;
    line.symbol          = "list";
    info.addText(line);

    // line 11
    // li $t0, 0
    line.init();
    line.line_num        = 11;
    line.addr            = 0x00400001;
    line.opcode.instr    = LEX_LI;
    line.opcode.mnemonic = "li";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_LITERAL;
    line.is_imm          = true;
    info.addText(line);

    // line 12
    // li $t1, 5
    line.init();
    line.line_num        = 12;
    line.addr            = 0x00400002;
    line.opcode.instr    = LEX_LI;
    line.opcode.mnemonic = "li";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 5;
    line.type[1]         = SYM_LITERAL;
    line.is_imm          = true;
    info.addText(line);

    // line 15 
    // loop: bgt $t0, $t1, end_loop
    line.init();
    line.line_num        = 15;
    line.addr            = 0x00400003;
    line.opcode.instr    = LEX_BGT;
    line.opcode.mnemonic = "bgt";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 1;
    line.type[1]          = SYM_REG_TEMP;
    // the immediate here is the address of end_loop
    line.val[2]          = 0x0040000A;
    line.type[2]         = SYM_LITERAL;
    line.is_symbol       = true;
    line.symbol          = "end_loop";
    line.is_label        = true;
    line.label           = "loop";
    info.addText(line);

    // line 17
    // lw $a0, $(s0)
    // NOTE the extra parens here.
    line.init();
    line.line_num        = 17;
    line.addr            = 0x00400004;
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "lw";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_ARG;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_SAVED;
    info.addText(line);

    // line 18
    // li $v0, 1
    line.init();
    line.line_num        = 18;
    line.addr            = 0x00400005;
    line.opcode.instr    = LEX_LI;
    line.opcode.mnemonic = "li";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_RET;
    line.val[1]          = 1;
    line.type[1]         = SYM_LITERAL;
    line.is_imm          = true;
    info.addText(line);

    // line 19 
    line.init();
    line.line_num        = 19;
    line.addr            = 0x00400006;
    line.opcode.instr    = LEX_SYSCALL;
    line.opcode.mnemonic = "syscall";
    info.addText(line);

    // line 21 
    // addi $s0, $s0, 4
    line.init();
    line.line_num        = 21;
    line.addr            = 0x00400007;
    line.opcode.instr    = LEX_ADDI;
    line.opcode.mnemonic = "addi";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_SAVED;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_SAVED;
    line.val[2]          = 4;
    line.type[2]         = SYM_LITERAL;
    line.is_imm          = true;
    info.addText(line);
    
    // line 22 
    // addi $t0, $t0, 1
    line.init();
    line.line_num        = 22;
    line.addr            = 0x00400008;
    line.opcode.instr    = LEX_ADDI;
    line.opcode.mnemonic = "addi";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_TEMP;
    line.val[2]          = 1;
    line.type[2]         = SYM_LITERAL;
    line.is_imm          = true;
    info.addText(line);
    

    // line 23
    // j loop
    line.init();
    line.line_num        = 23;
    line.addr            = 0x00400009;
    line.opcode.instr    = LEX_J;
    line.opcode.mnemonic = "j";
    line.is_symbol       = true;
    line.symbol          = "loop";
    line.val[2]          = 0x00400003;
    line.type[2]         = SYM_LITERAL;
    info.addText(line);

    // line 25
    // end_loop:
    // NOTE: 25 due to line end 'overflow'
    line.init();
    line.line_num        = 25;
    line.addr            = 0x0040000A;
    line.is_label        = true;
    line.label           = "end_loop";
    info.addText(line);

    return info;
}



/*
 * Test mult_add example
 */
TEST_F(TestLexer, test_lex_mult_add)
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(false);
    test_lexer.setExpandPsuedo(true);
    test_lexer.loadFile(this->test_mult_add_file);
    test_lexer.lex();

    // get the source info
    expected_src_out = get_mult_add_expected_source_info();
    std::cout << "Expected output :" << std::endl;
    std::cout << expected_src_out.toString() << std::endl << std::endl;

    src_out = test_lexer.getSourceInfo();
    std::cout << "Lexer output : " << std::endl;
    std::cout << src_out.toString() << std::endl;
    ASSERT_EQ(expected_src_out.getTextInfoSize(), src_out.getTextInfoSize());

    // Check each line in turn
    TextInfo expected_line;
    TextInfo output_line;
    for(unsigned int line = 0; line < expected_src_out.getTextInfoSize(); ++line)
    {
        expected_line = expected_src_out.getText(line);
        output_line = src_out.getText(line);
        std::cout << "Checking line " << std::dec << line+1 << "/" << 
            std::dec << expected_src_out.getTextInfoSize();

        if(expected_line != output_line)
        {
            std::cout << std::endl << "    diff : " << std::endl;
            std::cout << expected_line.diff(output_line) << std::endl;
        }
        ASSERT_EQ(expected_line, output_line);
        std::cout << "    [OK]" << std::endl;
    }
}


/*
 * Test for_loop example
 */
TEST_F(TestLexer, test_for_loop)
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(true);
    test_lexer.loadFile(this->test_for_loop_file);
    test_lexer.lex();

    // get the source info
    expected_src_out = get_for_loop_expected_source_info();
    std::cout << "Expected output :" << std::endl;
    std::cout << expected_src_out.toString() << std::endl << std::endl;

    src_out = test_lexer.getSourceInfo();

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

    //ASSERT_EQ(expected_src_out.getTextInfoSize(), src_out.getNumLines());

    // Check each line in turn
    TextInfo expected_line;
    TextInfo output_line;
    for(unsigned int line = 0; line < expected_src_out.getTextInfoSize(); ++line)
    {
        expected_line = expected_src_out.getText(line);
        output_line = src_out.getText(line);
        std::cout << "Checking line " << std::dec << line+1 << "/" << 
            std::dec << expected_src_out.getTextInfoSize() << std::endl;

        if(expected_line != output_line)
        {
            std::cout << "    diff : " << std::endl;
            std::cout << expected_line.diff(output_line) << std::endl;
        }
        ASSERT_EQ(expected_line, output_line);
    }
}

/*
 * Test data_region example (arrays, etc)
 */
TEST_F(TestLexer, test_array)
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(true);
    // for this test, just leave the psuedo instructions in place
    test_lexer.setExpandPsuedo(false);          
    test_lexer.loadFile(this->test_array_file);
    test_lexer.lex();

    // get the source info
    expected_src_out = get_array_expected_source_info();
    std::cout << "Expected output :" << std::endl;
    std::cout << expected_src_out.toString() << std::endl << std::endl;

    src_out = test_lexer.getSourceInfo();
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

    // Check each line in turn
    TextInfo expected_line;
    TextInfo output_line;
    for(unsigned int line = 0; line < expected_src_out.getTextInfoSize(); ++line)
    {
        expected_line = expected_src_out.getText(line);
        output_line = src_out.getText(line);
        std::cout << "Checking line " << std::dec << line+1 << "/" << 
            std::dec << expected_src_out.getTextInfoSize() << std::endl;

        if(expected_line != output_line)
        {
            std::cout << "    diff : " << std::endl;
            std::cout << expected_line.diff(output_line) << std::endl;
        }
        ASSERT_EQ(expected_line, output_line);
    }

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
    line.line_num        = 4;
    line.addr            = 0x00400000;
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "lw";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_GLOBAL;
    info.addText(line);

    // line 5
    // lw $t1, 4($gp)
    line.init();
    line.line_num        = 5;
    line.addr            = 0x00400001;
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "lw";
    line.val[0]          = 2;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_GLOBAL;
    line.val[2]          = 4;
    line.type[2]         = SYM_LITERAL;
    info.addText(line);

    // line 6
    // lw $t1, 4($gp)
    line.init();
    line.line_num        = 6;
    line.addr            = 0x00400002;
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "lw";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_GLOBAL;
    line.val[2]          = 4;
    line.type[2]         = SYM_LITERAL;
    info.addText(line);

    return info;
}



TEST_F(TestLexer, test_paren_parse)
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(true);
    test_lexer.loadFile(this->test_paren_file);
    test_lexer.lex();

    // get the source info
    expected_src_out = get_paren_expected_source_info();
    std::cout << "Expected output :" << std::endl;
    std::cout << expected_src_out.toString() << std::endl << std::endl;

    src_out = test_lexer.getSourceInfo();
    std::cout << "Lexer output : " << std::endl;
    std::cout << src_out.toString() << std::endl;

    // Check each line in turn
    TextInfo expected_line;
    TextInfo output_line;

    for(unsigned int line = 0; line < expected_src_out.getTextInfoSize(); ++line)
    {
        expected_line = expected_src_out.getText(line);
        output_line = src_out.getText(line);
        std::cout << "Checking line " << std::dec << line+1 << "/" << 
            std::dec << expected_src_out.getTextInfoSize() << std::endl;

        if(expected_line != output_line)
        {
            std::cout << "    diff : " << std::endl;
            std::cout << expected_line.diff(output_line) << std::endl;
        }
        ASSERT_EQ(expected_line, output_line);
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
    line.line_num        = 8;
    line.addr            = 0x00400000;
    line.opcode.instr    = LEX_SLT;
    line.opcode.mnemonic = "slt";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_AT;
    line.val[1]          = 1;
    line.type[1]         = SYM_REG_TEMP;
    line.val[2]          = 0;
    line.type[2]         = SYM_REG_SAVED;
    info.addText(line);
    // bne $at, $zero, 8
    line.init();
    line.line_num        = 8;
    line.addr            = 0x00400001;
    line.opcode.instr    = LEX_BNE;
    line.opcode.mnemonic = "bne";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_AT;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_ZERO;
    line.val[2]          = 8;
    line.type[2]         = SYM_LITERAL;
    info.addText(line);

    // li $t2, 5
    // 16-bit immediate, therefore we can transform to just ori
    line.init();
    line.line_num        = 9;
    line.addr            = 0x00400002;
    line.opcode.instr    = LEX_ORI;
    line.opcode.mnemonic = "ori";
    line.val[0]          = 2;
    line.type[0]         = SYM_REG_TEMP;
    line.type[1]         = SYM_REG_ZERO;
    line.val[2]          = 5;
    line.type[2]         = SYM_LITERAL;
    line.is_imm          = true;
    info.addText(line);

    // li $t1, 75000
    // 32-bit immedaite, therefore we need two instructions
    // lui $t0,  7500 & (0xFFFF0000)
    line.init();
    line.line_num        = 10;
    line.addr            = 0x00400003;
    line.opcode.instr    = LEX_LUI;
    line.opcode.mnemonic = "lui";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 75000 & (0xFFFF0000);
    line.type[1]         = SYM_LITERAL;
    line.is_imm          = true;
    line.upper           = true;
    info.addText(line);

    // ori $t0, $t0, 7500 & (0x0000FFFF)
    line.init();
    line.line_num        = 10;
    line.addr            = 0x00400004;
    line.opcode.instr    = LEX_ORI;
    line.opcode.mnemonic = "ori";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_TEMP;
    line.val[2]          = 75000 & (0x0000FFFF);
    line.type[2]         = SYM_LITERAL;
    line.is_imm          = true;
    info.addText(line);

    // la $t1, arr
    // lui $t1, 0x00400007 & (0xFFFF0000)
    line.init();
    line.line_num        = 11;
    line.addr            = 0x00400005;
    line.opcode.instr    = LEX_LUI;
    line.opcode.mnemonic = "lui";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0x10000000 & 0xFFFF0000;
    line.type[1]         = SYM_LITERAL;
    line.upper           = true;
    line.is_imm          = true;
    line.is_symbol       = true;
    line.symbol          = "arr";
    info.addText(line);

    // ori  $t1, $t1, 0x00400007 & (0x0000FFFF)
    line.init();
    line.line_num        = 11;
    line.addr            = 0x00400006;
    line.opcode.instr    = LEX_ORI;
    line.opcode.mnemonic = "ori";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 1;
    line.type[1]         = SYM_REG_TEMP;
    line.val[2]          = 0x10000000 & 0x0000FFFF;
    line.type[2]         = SYM_LITERAL;
    line.is_imm          = true;
    line.is_symbol       = true;
    line.lower           = true;
    line.symbol          = "arr";
    info.addText(line);

    // arr: .word 3
    // TODO: actual label address should be 0x02 (which needs to change
    // to fit memory map)
    data_line.init();
    data_line.line_num  = 5;
    data_line.addr      = 0x10000000;
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
TEST_F(TestLexer, test_psuedo_instr)
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(true);
    test_lexer.loadFile(this->test_psuedo_file);
    test_lexer.lex();

    // get the source info
    expected_src_out = get_psuedo_instr_source_info();
    std::cout << "Expected output :" << std::endl;
    std::cout << expected_src_out.toString() << std::endl << std::endl;

    src_out = test_lexer.getSourceInfo();
    std::cout << "Lexer output : " << std::endl;
    std::cout << src_out.toString() << std::endl;

    // Check each line in turn
    TextInfo expected_line;
    TextInfo output_line;

    std::cout << src_out.getTextInfoSize() << " lines in output text segment" << std::endl;
    std::cout << src_out.getDataInfoSize() << " lines in output data segment" << std::endl;

    SymbolTable sym_table = test_lexer.getSymTable();
    std::cout << "Symbol Table: " << std::endl;

    for(unsigned int sym = 0; sym < sym_table.size(); ++sym)
    {
        Symbol cur_sym = sym_table.get(sym);
        std::cout << "     " << sym << " " << 
            cur_sym.toString() << std::endl;
    }

    for(unsigned int line = 0; line < expected_src_out.getTextInfoSize(); ++line)
    {
        expected_line = expected_src_out.getText(line);
        output_line = src_out.getText(line);
        std::cout << "Checking line " << std::dec << line+1 << "/" << 
            std::dec << expected_src_out.getTextInfoSize() << std::endl;

        if(expected_line != output_line)
        {
            std::cout << "    diff : " << std::endl;
            std::cout << expected_line.diff(output_line) << std::endl;
        }
        ASSERT_EQ(expected_line, output_line);
    }

    // Also check that we have the corect number of text and data segments 
    ASSERT_EQ(1, src_out.getDataInfoSize());
    ASSERT_EQ(7, src_out.getTextInfoSize());
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
