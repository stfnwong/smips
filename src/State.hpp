/*
 * STATE
 * CPU State
 *
 */


#ifndef __STATE_HPP
#define __STATE_HPP

#include <cstdint>
#include <string>
#include <vector>


#define SMIPS_MEM_SIZE 0x1000       // TODO: boost this up later

struct State
{
    uint32_t pc;
    uint32_t addr;

    uint8_t mem[SMIPS_MEM_SIZE];    // TODO: this would be cache in a modern processor...

    // reg for 4 stage pipeline
    uint32_t instr;
    uint32_t id_ex;
    uint32_t ex_mem;
    uint32_t mem_wb;
    // register file 
    uint32_t reg[32];

    private:
        void init_reg(void);
        void zero_mem(void);

    // pipeline 
    public:
        void fetch(void);
        void decode(void);

    public:
        State();
        State(const State& that);

        void loadMem(const std::vector<uint8_t>& data, unsigned int offset);
        void tick(void);

        std::string toString(void) const;
};


#endif /*__STATE_HPP*/
