/*
 * ASSEM
 * Assembler tool for SMIPS
 *
 * Stefan Wong 2019
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <getopt.h>

#include "Lexer.hpp"
#include "Source.hpp"
#include "Assembler.hpp"

struct AsmOpts
{
    std::string infile;
    std::string outfile;
    bool verbose;

    public:
        AsmOpts()
        {
            infile = "\0";
            outfile = "\0";
            verbose = false;
        }
};

int main(int argc, char *argv[])
{
    AsmOpts asm_opts;
    const char* const short_opts = "vhi:o:";
    const option long_opts[] = {};
    int argn = 0;
    int status;

    // get args
    while(1) 
    {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);
        if(opt == -1)
            break;

        switch(opt)
        {
            case 'v':
                asm_opts.verbose = true;
                break;
                
            case 'i':
                asm_opts.infile = std::string(optarg);
                break;

            case 'o':
                asm_opts.outfile = std::string(optarg);
                break;

            default:
                std::cerr << "Uknown option " << std::string(optarg) << 
                    "(arg " << argn << ")" << std::endl;
                exit(-1);
                break;
        }
        argn++;
    }

    // check that what we got was valid
    if(asm_opts.infile == "\0")
    {
        std::cerr << "Invalid input filename" << std::endl;
        std::cerr << "Use -i <filename> to specify" << std::endl;
        exit(-1);
    }
    if(asm_opts.outfile == "\0")
    {
        std::cerr << "Invalid output filename" << std::endl;
        std::cerr << "Use -o <filename> to specify" << std::endl;
        exit(-1);
    }

    // Get a Lexer and assembler
    Lexer lexer;
    SourceInfo lexed_source;
    Assembler assembler;
    Program program;

    lexer.setVerbose(asm_opts.verbose);
    status = lexer.loadFile(asm_opts.infile);
    if(status < 0)
    {
        std::cerr << "Failed to load input file [" << 
            asm_opts.infile << "]" << std::endl;
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
    assembler.loadSource(lexed_source);
    assembler.assemble();
    
    // Save the program to disk
    program.setVerbose(asm_opts.verbose);
    program = assembler.getProgram();
    status = program.save(asm_opts.outfile);
    if(status < 0)
    {
        std::cerr << "[ERROR] writing program to file [" <<
            asm_opts.outfile << "]" << std::endl;
        exit(-1);
    }



    return 0;
}
