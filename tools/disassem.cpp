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
    bool verbose;

    public:
        DisOpts()
        {
            infile = "\0";
            outfile = "\0";
            verbose = false;
        }
};



int main(int argc, char *argv[])
{
    DisOpts dis_opts;
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
                dis_opts.verbose = true;
                break;
                
            case 'i':
                dis_opts.infile = std::string(optarg);
                break;

            case 'o':
                dis_opts.outfile = std::string(optarg);
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
    if(dis_opts.infile == "\0")
    {
        std::cerr << "Invalid input filename" << std::endl;
        std::cerr << "Use -i <filename> to specify" << std::endl;
        exit(-1);
    }
    if(dis_opts.outfile == "\0")
    {
        std::cerr << "Invalid output filename" << std::endl;
        std::cerr << "Use -o <filename> to specify" << std::endl;
        exit(-1);
    }

    Disassembler disassem;

    status = diassem.load(dis_opts.infile);
    if(status < 0)
    {
        std::cerr << "Failed to load input file [" << 
            dis_opts.infile << "]" << std::endl;
        exit(-1);
    }

    std::cout << "Here is where we would do the actual disassembly" << std::endl;

    return 0;
}
