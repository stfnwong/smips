/* 
 * TEST_PROGRAM
 * Unit tests for SMIPS Program and Instr object 
 *
 * Stefan Wong 2019
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>
// for file cleanup
//#include <filesystem>

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


/*
 * test_program_save_load()
 */
TEST_CASE("test_program_save_load", "[classic]")
{
    Program test_program;
    Program src_program;   
    int status;

    const std::string test_load_filename = "test_load.mips";

    // I supose that we need to write this to disk, then read it back, 
    // then test that the read back was correct...
    //
    // the alternative is to commit a binary to the repo, but that sees like 
    // it create a new set of problems
    src_program = assem(test_mult_add_file);
    REQUIRE(src_program.numInstrs() == 7);

    // write the test program to disk 
    src_program.save(test_load_filename);

    status = test_program.load(test_load_filename);
    REQUIRE(status == 0);
    REQUIRE(test_program.numInstrs() == src_program.numInstrs());


    std::cout << "Expected program :" << std::endl;
    std::cout << src_program.toString() << std::endl;

    std::cout << "Output program :" << std::endl;
    std::cout << test_program.toString() << std::endl;

    for(unsigned int idx = 0; idx < test_program.numInstrs(); ++idx)
    {
        Instr src_instr = src_program.getInstr(idx);
        Instr tst_instr = test_program.getInstr(idx);

        REQUIRE(src_instr == tst_instr);
    }

    //REQUIRE(std::filesystem::remove(test_load_filename) == true);
}
