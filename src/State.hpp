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

// R instruction codes
#define R_SLL     0
#define R_SRL     2
#define R_SRA     3
#define R_SLLV    4
#define R_SRLV    6
#define R_SRAV    7
#define R_JR      8
#define R_JALR    9
#define R_SYSCALL 12
#define R_MFHI    16
#define R_MTHI    17
#define R_MFLO    18
#define R_MTLO    19
#define R_MULT    24
#define R_MULTU   25
#define R_DIV     26
#define R_DIVU    27
#define R_ADD     32
#define R_ADDU    33
#define R_SUB     34
#define R_SUBU    35
#define R_AND     36
#define R_OR      37
#define R_XOR     38
#define R_NOR     39
#define R_SLT     42
#define R_SLTU    43

// I-instructions 
#define I_BEQ     4
#define I_BNE     5
#define I_BLEZ    6
#define I_BGTZ    7
#define I_ADDI    8
#define I_ADDIU   9
#define I_SLTI    10
#define I_SLTIU   11
#define I_ANDI    12
#define I_ORI     13 
#define I_XORI    14
#define I_LUI     15
#define I_LB      32
#define I_LH      33
#define I_LW      35
#define I_LBU     36
#define I_LHU     37
#define I_SB      40
#define I_SH      41
#define I_SW      43


#define SMIPS_MEM_SIZE 0x1000       // TODO: boost this up later


class DataCache
{
    private:
        uint8_t* mem;
        unsigned int mem_size;

    public:
        DataCache();
        DataCache(unsigned int size);
        DataCache(const DataCache& that);
        ~DataCache();
        unsigned int size(void) const;
        void clear(void);       // write all zeros (slow!)

        uint8_t &operator[](const int i);  
        int32_t readWord(const int i) const;     // read 4-bytes starting from i;;
};


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
