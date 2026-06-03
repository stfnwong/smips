/*
 * ASSEM
 * Assembler tool for SMIPS
 *
 * Stefan Wong 2019
 */

#include <iostream>
#include <string>
#include <getopt.h>
#include <CLI/CLI.hpp>

#include "mips/Lexer.hpp"
#include "mips/Source.hpp"
#include "mips/Program.hpp"
#include "mips/assembler.hpp"

struct AsmOpts
{
    std::string infile;
    std::string outfile;
    std::string literal;
    bool verbose;

    public:
        AsmOpts() : infile("\0"), 
                    outfile("\0"),
                    literal("\0"),
                    verbose(false) {} 
};

int main(int argc, char *argv[])
{
    AsmOpts asm_opts;

    CLI::App app{"SMIPS disassembler"};

    std::string inp_file;
    std::string out_file;

    bool verbose = false;

    int status;

    app.add_option("-o,--output", out_file, "Output file");
    app.add_option("-v,--verbose", verbose, "verbose");
    app.add_option("input", inp_file, "Input binary")->required();

    CLI11_PARSE(app, argc, argv);

    // get args
    // Get a Lexer and assembler
    Lexer lexer;
    SourceInfo lexed_source;
    Assembler assembler;
    Program program;

    lexer.setVerbose(asm_opts.verbose);

    if(asm_opts.literal != "\0")
    {
        lexer.loadSource(asm_opts.literal);
    }
    // check that what we got was valid
    else if(asm_opts.infile != "\0")
    {
        status = lexer.loadFile(asm_opts.infile);
        if(status < 0)
        {
            std::cerr << "Failed to load input file [" << 
                asm_opts.infile << "]" << std::endl;
            exit(-1);
        }
    }
    else
    {
        std::cerr << "Invalid input filename" << std::endl;
        std::cerr << "Use -i <filename> to specify" << std::endl;
        exit(-1);
    }

    lexer.lex();
    lexed_source = lexer.getSourceInfo();

    // Run through source and print error strings
    // TODO : need to handle data sections as well..
    TextInfo line;
    for(unsigned int idx = 0; idx < lexed_source.getNumLines(); ++idx)
    {
        line = lexed_source.getText(idx);
        if(line.error)
        {
            std::cerr << "[ERROR] line " << line.line_num << std::endl;
            std::cerr << line.errstr << std::endl;
        }
    }

    // After (possibly) printing error strings, exit
    if(lexed_source.hasError())
        exit(-1);

    if(asm_opts.verbose)
        std::cout << lexed_source.toString() << std::endl;

    // Now assemble the source files 
    //assembler.loadSource(lexed_source);
    //assembler.assemble();
    
    // Save the program to disk
    //program = assembler.getProgram();

    //if(asm_opts.outfile != "\0")
    //{
    //    status = program.save(asm_opts.outfile);
    //    if(status < 0)
    //    {
    //        std::cerr << "[ERROR] writing program to file [" <<
    //            asm_opts.outfile << "]" << std::endl;
    //        exit(-1);
    //    }
    //}
    //else
    //{
    //    std::cout << program.toString() << std::endl;
    //}

    return 0;
}
