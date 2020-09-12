/* 
 * TEST_PROGRAM
 * Unit tests for SMIPS Progran and Instr object 
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
#include "Program.hpp"
#include "Assembler.hpp"
#include "Lexer.hpp"


const std::string test_mult_add_file = "asm/mult_add.asm";

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


TEST_CASE("test_program_load", "[classic]")
{
    Program test_program;   // TODO : going to use seperate program object here 
    int status;

    test_program = assem(test_mult_add_file);
    // I supose that we need to write this to disk, then read it back, 
    // then test that the read back was correct...
    //
    // the alternative is to commit a binary to the repo, but that sees like 
    // it create a new set of problems


    REQUIRE(test_program.numInstrs() == 7);
}
