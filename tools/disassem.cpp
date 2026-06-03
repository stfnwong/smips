/*
 * DISASSEM
 * Disassembler tool for SMIPS
 *
 * Stefan Wong 2019
 */


#include <iostream>
#include <iomanip>
#include <string>
#include <CLI/CLI.hpp>

#include "mips/Disassembler.hpp"


// TODO : this could possibly become common with AsmOpts
struct DisOpts
{
    std::string infile;
    std::string outfile;
    std::string literal;
    bool verbose;
    bool print_adr;

    public:
        DisOpts() : infile("\0"), 
                    outfile("\0"), 
                    literal("\0"), 
                    verbose(false), 
                    print_adr(false) {}
};



int main(int argc, char *argv[])
{
    CLI::App app{"SMIPS disassembler"};

    std::string inp_file;
    std::string out_file;

    bool verbose = false;

    DisOpts dis_opts;
    int status;

    app.add_option("-o,--output", out_file, "Output file");
    app.add_option("-v,--verbose", verbose, "verbose");
    app.add_option("input", inp_file, "Input binary")->required();

    CLI11_PARSE(app, argc, argv);

    Program prog;
    status = prog.load(inp_file);
    if(status < 0)
    {
        std::cerr << "Failed to load input file [" << 
            dis_opts.infile << "]" << std::endl;
        exit(-1);
    }

    if(dis_opts.verbose)
    {
        std::cout << "Found " << prog.numDataSeg() << " data segments in program [" 
            << inp_file << "]" << std::endl;
        std::cout << "Found " << prog.numInstrs() << " instructions in program [" 
            << inp_file << "]" << std::endl;
        std::cout << std::endl;
    }

    // Dump representation to console
    // TODO: data segment
    for(unsigned int idx = 0; idx < prog.numInstrs(); ++idx)
    {
        OldInstr cur_instr = prog.getInstr(idx);
        TextInfo dis_out = dis_instr(cur_instr.ins, cur_instr.adr);

        if(dis_opts.print_adr)
        {
            std::cout << "[0x" << std::hex << std::setw(8) << std::setfill('0')
                << dis_out.addr << "] ";
        }
        std::cout << dis_out.toInstrString() << std::endl;
    }

    return 0;
}
