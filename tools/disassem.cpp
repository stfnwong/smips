/*
 * DISASSEM
 * Disassembler tool for SMIPS
 *
 * Stefan Wong 2019
 */


#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <getopt.h>

#include "Disassembler.hpp"

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
    DisOpts dis_opts;
    const char* const short_opts = "avhi:o:l:";
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
            case 'v':   // verbose mode
                dis_opts.verbose = true;
                break;

            case 'a':   // print addresses
                dis_opts.print_adr = true;
                break;
                
            case 'i':   // input file 
                dis_opts.infile = std::string(optarg);
                break;

            case 'o':   // output file
                dis_opts.outfile = std::string(optarg);
                break;

            case 'l':
                dis_opts.literal = std::string(optarg);
                break;

            default:
                std::cerr << "Uknown option " << std::string(optarg) << 
                    "(arg " << argn << ")" << std::endl;
                exit(-1);
                break;
        }
        argn++;
    }

    if(dis_opts.literal != "\0")
    {
        uint32_t instr_literal = std::stoi(dis_opts.literal, nullptr, 16);
        TextInfo dis_out = dis_instr(instr_literal, 0);

        if(dis_opts.print_adr)
        {
            std::cout << "[0x" << std::hex << std::setw(8) << std::setfill('0')
                << dis_out.addr << "] ";
        }
        std::cout << dis_out.toInstrString() << std::endl;

        return 0;
    }
    // check that what we got was valid
    else if(dis_opts.infile == "\0")
    {
        std::cerr << "Invalid input filename" << std::endl;
        std::cerr << "Use -i <filename> to specify" << std::endl;
        exit(-1);
    }
    //if(dis_opts.outfile == "\0")
    //{
    //    std::cerr << "Invalid output filename" << std::endl;
    //    std::cerr << "Use -o <filename> to specify" << std::endl;
    //    exit(-1);
    //}

    Program prog;

    // TODO; this is broken
    status = prog.load(dis_opts.infile);
    if(status < 0)
    {
        std::cerr << "Failed to load input file [" << 
            dis_opts.infile << "]" << std::endl;
        exit(-1);
    }

    if(dis_opts.verbose)
    {
        std::cout << "Found " << prog.numDataSeg() << " data segments in program [" 
            << dis_opts.infile << "]" << std::endl;
        std::cout << "Found " << prog.numInstrs() << " instructions in program [" 
            << dis_opts.infile << "]" << std::endl;
        std::cout << std::endl;
    }

    // Dump representation to console
    // TODO: data segment
    for(unsigned int idx = 0; idx < prog.numInstrs(); ++idx)
    {
        Instr cur_instr = prog.getInstr(idx);
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
