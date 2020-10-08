/*
 * STATE
 * CPU State
 *
 */

#ifndef __STATE_HPP
#define __STATE_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "DataCache.hpp"


struct State
{
    DataCache mem;      // represents memory or L1 instruction/data cache
    bool verbose;       // print info to console during execution
    
    uint32_t pc;

    uint32_t instr;     // fetch stage register
    uint8_t op_bits;
    uint8_t func;
    // register file outputs
    uint8_t rs;
    uint8_t rt;
    uint8_t rd;
    uint8_t shamt;
    int32_t imm;
    int32_t tmp;
    int32_t hi;
    int32_t lo;
    // ALU output 
    int32_t alu;
    // memory registers
    int32_t  mem_data;
    uint32_t mem_addr;

    // register file 
    int32_t reg[32];      

    private:
        void init_reg(void);
        void zero_mem(void);

    // pipeline 
    public:
        /*
         * fetch()
         * Get the next instruction from instruction memory. At the time 
         * of writing this is implemented as a single data cache, but in a 
         * more sophisticated emulation we would have seperate instruction and
         * data caches that are fed from "main" memory.
         */
        void fetch(void);
        /*
         * decode()
         * Decode instruction and split operands, immediates, etc.
         */
        void decode(void);
        /*
         * execute()
         * Push operand through datapath. Note that for now we don't explicitly 
         * emulate seperate datapaths for each instruction type (eg: each
         * instruction is just another label in a case statement, we don't have
         * a special I-instruction datapath, R-instrution datapath, etc)
         */
        void execute(void);     
        /*
         * memory()
         * Write results of any instruction that target memory
         */
        void memory(void);      
        /*
         * write_back()
         * Update the register file with instruction results
         */
        void write_back(void);  

    public:
        State();
        State(unsigned int mem_size);
        State(const State& that);

        void writeMem(const std::vector<uint8_t>& data, unsigned int offset);
        uint8_t& readMem(unsigned int idx);
        void clearMem(void);
        void tick(void);
        void reset(void);

        std::string toString(void) const;
};


#endif /*__STATE_HPP*/
