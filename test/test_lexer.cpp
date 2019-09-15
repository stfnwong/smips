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


// TODO : adjust starting address later 
SourceInfo get_mult_add_expected_source_info(void)
{
    SourceInfo info;
    TextInfo   line;

    // line 1
    // lw $t0, 4($gp)
    line.init();
    line.line_num        = 3;
    line.addr            = 0x200;      
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "LW";
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
    line.addr            = 0x201;      
    line.opcode.instr    = LEX_MULT;
    line.opcode.mnemonic = "MULT";
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
    line.addr            = 0x202;      
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "LW";
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
    line.addr            = 0x203;      
    line.opcode.instr    = LEX_ORI;
    line.opcode.mnemonic = "ORI";
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
    line.addr            = 0x204;      
    line.opcode.instr    = LEX_MULT;
    line.opcode.mnemonic = "MULT";
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
    line.addr            = 0x205;      
    line.opcode.instr    = LEX_ADD;
    line.opcode.mnemonic = "ADD";
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
    line.addr            = 0x206;
    line.opcode.instr    = LEX_SW;
    line.opcode.mnemonic = "SW";
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
    line.addr            = 0x200;
    line.opcode.instr    = LEX_ADD;
    line.opcode.mnemonic = "ADD";
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
    line.addr            = 0x201;
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "LW";
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
    line.addr            = 0x202;
    line.opcode.instr    = LEX_SLL;
    line.opcode.mnemonic = "SLL";
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
    line.addr            = 0x203;
    line.opcode.instr    = LEX_ADD;
    line.opcode.mnemonic = "ADD";
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
    line.addr            = 0x204;
    line.opcode.instr    = LEX_ORI;
    line.opcode.mnemonic = "ORI";
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
    line.addr            = 0x205;
    line.opcode.instr    = LEX_SLTU;
    line.opcode.mnemonic = "SLTU";
    line.val[0]         = 3;
    line.type[0]        = SYM_REG_TEMP;
    line.val[1]         = 0;
    line.type[1]        = SYM_REG_TEMP;
    line.val[2]         = 1;
    line.type[2]        = SYM_REG_TEMP;
    line.is_label        = true;
    line.label           = "TOP";
    info.addText(line);

    // line 11
    // BEQ $t3, $zero, done
    line.init();
    line.line_num        = 11;
    line.addr            = 0x206;
    line.opcode.instr    = LEX_BEQ;
    line.opcode.mnemonic = "BEQ";
    line.val[0]          = 3;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_ZERO;
    // 3rd arg is actually converted literal
    line.type[2]         = SYM_LITERAL;
    line.val[2]          = 0x20A;
    line.is_symbol       = true;
    line.symbol          = "DONE";
    info.addText(line);

    // line 12
    // SW $t2, 28($t8)
    line.init();
    line.line_num        = 12;
    line.addr            = 0x207;
    line.opcode.instr    = LEX_SW;
    line.opcode.mnemonic = "SW";
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
    line.addr            = 0x208;
    line.opcode.instr    = LEX_ADDI;
    line.opcode.mnemonic = "SW";
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
    line.addr            = 0x209;
    line.opcode.instr    = LEX_J;
    line.opcode.mnemonic = "J";
    line.is_symbol       = true;
    line.symbol          = "TOP";
    // 3 arg is also converted literal in this case
    line.type[2]         = SYM_LITERAL;
    line.val[2]          = 0x205;
    info.addText(line);

    // DONE
    line.init();
    line.line_num        = 16;      
    line.addr            = 0x20A;
    line.is_label        = true;
    line.label           = "DONE";
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
	//data_line.init();
	//data_line.is_directive = true;
	//data_line.directive = "data";
	//info.addData(data_line);

	//// line 3
	//// list: word 3, 0, 1, 2, 6
	//data_line.init();
	//data_line.addr = 0;
	//data_line.directive = "word";
	//data_line.data = {3, 0, 1, 2, 6};
	//info.addData(data_line);


	//// line 4 
	//// char_arr: .asciiz "hello"
	//data_line.init();
	//data_line.directive = "asciiz";
	////data_line.data = {'h', 'e', 'l', 'l', 'o'}; // TODO : convert to int...
	//info.addData(data_line);
	//
	//// line 5 
	//// buffer: .space 128
	//data_line.init();
	//data_line.directive = "space";
	//data_line.data = {3, 0, 1, 2, 6};
	//info.addData(data_line);
	
	
	// line 9
	// .text
	// Starts a new text segment. There isn't any lexer output that 
	// is specifically associated with this line.
	

    // line 10
	// la $s0, list
    line.line_num        = 10;
    line.addr            = 0x200;
    line.opcode.instr    = LEX_LA;
    line.opcode.mnemonic = "LA";
    line.val[0]          = 0;
    line.type[0]         = SYM_REG_SAVED;
	line.is_symbol       = true;
	line.symbol          = "list";
    info.addText(line);

	// line 11
	// li $t0, 0
	line.init();
	line.line_num        = 11;
	line.addr            = 0x201;
	line.opcode.instr    = LEX_LI;
	line.opcode.mnemonic = "LI";
	line.val[0]          = 0;
	line.type[0]         = SYM_REG_TEMP;
	line.val[1]          = 0;
	line.val[1]          = SYM_LITERAL;
	info.addText(line);

	// line 12
	// li $t1, 5
	line.init();
	line.line_num        = 12;
	line.addr            = 0x202;
	line.opcode.instr    = LEX_LI;
	line.opcode.mnemonic = "LI";
	line.val[0]          = 1;
	line.type[0]         = SYM_REG_TEMP;
	line.val[1]          = 5;
	line.val[1]          = SYM_LITERAL;
	info.addText(line);

	// line 14 
	// loop: bgt $t0, $t1, end_loop
	line.init();
	line.line_num        = 14;
	line.addr            = 0x204;
	line.opcode.instr    = LEX_BGT;
	line.opcode.mnemonic = "BGT";
	line.val[0]          = 0;
	line.type[0]         = SYM_REG_TEMP;
	line.val[1]          = 1;
	line.val[1]          = SYM_REG_TEMP;
	line.is_symbol       = true;
	line.symbol          = "end_loop";
	info.addText(line);

	// line 17
	// lw $a0, $(s0)
	// NOTE the extra parens here.
	line.init();
	line.line_num        = 17;
	line.addr            = 0x206;
	line.opcode.instr    = LEX_LW;
	line.opcode.mnemonic = "LW";
	line.val[0]          = 0;
	line.type[0]         = SYM_REG_ARG;
	line.val[1]          = 5;
	line.val[1]          = SYM_LITERAL;
	info.addText(line);

	// line 18
	// li $v0, 1
	line.init();
	line.line_num        = 18;
	line.addr            = 0x208;
	line.opcode.instr    = LEX_LI;
	line.opcode.mnemonic = "LI";
	line.val[0]          = 0;
	line.type[0]         = SYM_REG_RET;
	line.val[1]          = 1;
	line.val[1]          = SYM_LITERAL;
	info.addText(line);

	// line 19 
	// syscall 


	// line 21 
	// addi $s0, $s0, 4
	line.init();
	line.line_num        = 21;
	line.addr            = 0x20C;
	line.opcode.instr    = LEX_ADDI;
	line.opcode.mnemonic = "ADDI";
	line.val[0]          = 0;
	line.type[0]         = SYM_REG_SAVED;
	line.val[1]          = 0;
	line.val[1]          = SYM_REG_SAVED;
	line.val[2]          = 4;
	line.type[2]         = SYM_LITERAL;
	info.addText(line);
	
	// line 22 
	// addi $t0, $t0, 1
	line.init();
	line.line_num        = 22;
	line.addr            = 0x20E;
	line.opcode.instr    = LEX_ADDI;
	line.opcode.mnemonic = "ADDI";
	line.val[0]          = 0;
	line.type[0]         = SYM_REG_TEMP;
	line.val[1]          = 0;
	line.val[1]          = SYM_REG_TEMP;
	line.val[2]          = 1;
	line.type[2]         = SYM_LITERAL;
	info.addText(line);
	

	// line 23
	// j loop
	line.init();
	line.line_num        = 23;
	line.addr            = 0x210;
	line.opcode.instr    = LEX_J;
	line.opcode.mnemonic = "J";
	line.is_symbol       = true;
	line.symbol          = "loop";
	info.addText(line);

	// line 24
	// end_loop:
	line.init();
	line.line_num        = 24;
	line.addr            = 0x212;
	line.is_label        = true;
	line.label           = "end_loop";
	info.addText(line);

	return info;
}


class TestLexer : public ::testing::Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}

    public:
        std::string test_mult_add_file = "asm/mult_add.asm";
        std::string test_for_loop_file = "asm/for_loop.asm";
        std::string test_array_file = "asm/array.asm";
        std::string test_paren_file = "asm/paren.asm";
};


/*
 * Test mult_add example
 */
TEST_F(TestLexer, test_lex_mult_add)
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(false);
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
 * Test data_region example
 */
TEST_F(TestLexer, test_array)
{
    Lexer test_lexer;
    SourceInfo src_out;
    SourceInfo expected_src_out;

    test_lexer.setVerbose(true);
    test_lexer.loadFile(this->test_array_file);
    test_lexer.lex();

    // get the source info
    expected_src_out = get_array_expected_source_info();
	std::cout << "Expected output :" << std::endl;
	std::cout << expected_src_out.toString() << std::endl << std::endl;

    src_out = test_lexer.getSourceInfo();
    std::cout << "Lexer output : " << std::endl;
    std::cout << src_out.toString() << std::endl;

    expected_src_out = get_for_loop_expected_source_info();
    // before we check each line, dump the symbol table and print
    SymbolTable sym_table = test_lexer.getSymTable();
    std::cout << "Symbol Table: " << std::endl;

    for(unsigned int sym = 0; sym < sym_table.size(); ++sym)
    {
        Symbol cur_sym = sym_table.get(sym);
        std::cout << "     " << sym << " " << 
            cur_sym.toString() << std::endl;
    }

    // TODO : Make an expected source output for the assertion check
}


SourceInfo get_paren_expected_source_info(void)
{
	SourceInfo info;
    TextInfo   line;

    // line 4
    // lw $t1, ($gp)
    line.init();
    line.line_num        = 4;
    line.addr            = 0x200;
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "LW";
    line.val[0]          = 1;
    line.type[0]         = SYM_REG_TEMP;
    line.val[1]          = 0;
    line.type[1]         = SYM_REG_GLOBAL;
    info.addText(line);

    // line 5
    // lw $t1, 4($gp)
    line.init();
    line.line_num        = 5;
    line.addr            = 0x201;
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "LW";
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
    line.addr            = 0x202;
    line.opcode.instr    = LEX_LW;
    line.opcode.mnemonic = "LW";
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


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
