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

#define FUNC_ADD 32
#define FUNC_ADDU 33
#define FUNC_SUB 34
#define FUNC_SUBU 35
#define FUNC_AND 36
#define FUNC_OR 37

#define SMIPS_MEM_SIZE 0x1000       // TODO: boost this up later

struct State
{
    uint32_t pc;
    uint32_t addr;

    uint8_t mem[SMIPS_MEM_SIZE];    // TODO: this would be cache in a modern processor...

    // reg for 4 stage pipeline
    uint32_t instr;     // fetch stage register
    uint8_t op_bits;
    uint8_t func;
    // register file outputs
    uint8_t rs;
    uint8_t rt;
    uint8_t rd;
    uint8_t shamt;
    uint32_t imm;

    // register file 
    uint32_t reg[32];

    private:
        void init_reg(void);
        void zero_mem(void);

    // pipeline 
    public:
        void fetch(void);
        void decode(void);
        void execute(void);

    public:
        State();
        State(const State& that);

        void loadMem(const std::vector<uint8_t>& data, unsigned int offset);
        void tick(void);

        std::string toString(void) const;
};


#endif /*__STATE_HPP*/
