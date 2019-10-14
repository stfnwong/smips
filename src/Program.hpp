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


/*
 * Instr
 * Represents a single assembly instruction.
 * (machine word + address)
 */
struct Instr
{
    uint32_t adr;
    uint32_t ins;

    public:
        Instr();
        Instr(const uint32_t adr, const uint32_t ins);
        Instr(const Instr& that);

        // TODO: will the default move constructor be sufficient here?
        bool operator==(const Instr& that) const;
        bool operator!=(const Instr& that) const;
        Instr& operator=(const Instr& that);
        void init(void);
        std::string toString(void) const;
};

// NOTE: There are a number of 'psuedo instructions' that need to be expanded, probably by the Lexer


/*
 * DataSeg
 */
struct DataSeg
{
    uint32_t adr;           // start address, we work out offsets from here
    std::vector<uint8_t> data;

    public:
        DataSeg();
        DataSeg(uint32_t adr, const std::vector<uint8_t> d);

        void init(void);

        DataSeg& operator=(const DataSeg& that);
        bool operator==(const DataSeg& that) const;
        bool operator!=(const DataSeg& that) const;

        std::string toString(void) const;
};


/*
 * Program
 * Represents a series of Instr objects
 */
class Program
{
    private:
        std::vector <DataSeg> data_segments;
        std::vector <Instr> instructions;
        Instr null_instr;
    // TODO : does it make sense to have a copy constructor for Program objects?
    private:

    public:
        Program();

        /*
         * init()
         * Reset the program object
         */
        void init(void);
        /*
         * add()
         * Adds a new instruction object to the end of the instruction list
         */
        void add(const Instr& i);
        /*
         * add()
         * Adds a new DataSeg object to the end of the data_segments list
         */
        void add(const DataSeg& d);
        /*
         * get()
         * Returns the Nth instruction object from the instruction list
         */
        Instr& get(const unsigned int idx);
        /*
         * writeMem()
         * Write a word directly to a specific memory location
         */
        void writeMem(const uint32_t addr, const uint32_t val);
        unsigned int size(void) const;
        std::string toString(void) const;

        // disk operations
        int save(const std::string& filename);
        int load(const std::string& filename);
};


#endif /*__PROGRAM_HPP*/
