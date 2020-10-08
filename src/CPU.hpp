/*
 * CPU
 * CPU internals for MIPS emulation
 * I am sort of following along from the R4300i datasheet 
 * (http://datasheets.chipdb.org/MIPS/R4300i_datasheet.pdf) 
 * when writing this, but I haven't decided that this will be
 * a proper emulation of a R4300i
 *
 * Stefan Wong 2020
 */

#ifndef __CPU_HPP
#define __CPU_HPP

#include <cstdint>


class CPU
{
    uint32_t gpr[32];
    //float    fpr[32];
    
    uint64_t pc;
    uint64_t mult_hi;
    uint64_t mult_lo;

    public:
        CPU();



};


class Coproc0
{
    uint32_t status;
    uint32_t count;

    public:
        Coproc0();

        // getters

        // setters
};



#endif /*__CPU_HPP*/
