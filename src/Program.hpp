/*
 * PROGRAM
 * Represents a binary program
 *
 * Stefan Wong 2019
 */

#ifndef __PROGRAM_HPP
#define __PROGRAM_HPP


#include <cstdint>
#include <string>
#include <vector>


struct Instr
{
    uint32_t adr;
    uint32_t ins;

    public:
        Instr();
        Instr(const uint32_t adr, const uint32_t ins);

        bool operator==(const Instr& that) const;
        bool operator!=(const Instr& that) const;
        void init(void);
        std::string toString(void) const;
};



class Program
{
    private:
        std::vector <Instr> instructions;
        Instr null_instr;


    public:
        Program();

        void init(void);
        void add(const Instr& i);
        Instr& get(const unsigned int idx);
        void writeMem(const uint32_t addr, const uint32_t val);

        unsigned int size(void) const;
        std::string toString(void) const;
};


#endif /*__PROGRAM_HPP*/
