/*
 * DATA
 * Represents the data region in memory 
 *
 * Stefan Wong 2019
 */

#ifndef __DATA_HPP
#define __DATA_HPP

#include <cstdint>
#include <vector>


// TODO : note that there are alot of structures that look like this 
// in the codebase. 
struct MemData
{
    uint32_t addr;
    uint32_t data;

    public:
        MemData();
        MemData(uint32_t a, uint32_t d);

        void init(void);
        bool operator==(const MemData& that) const;
        bool operator!=(const MemData& that) const;
};


// TODO: this is meant to be for the lexer. The assembler should 
// transform each of these into element in the a DataRegion
struct MemLine
{
    uint32_t start_addr;
    std::vector<uint32_t> data_words;

    public:
        MemLine();
        MemLine(const uint32_t start);
        
        void init(void);
};


class DataRegion
{
    private:
        std::vector<MemData> data_elements;


    public:
        DataRegion();
        void init(void);
};



#endif /*__DATA_HPP*/
