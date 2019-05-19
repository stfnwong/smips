/*
 * OPCODE
 * Opcodes for SMIPS
 *
 * Stefan Wong 2019
 */

#ifndef __OPCODE_HPP
#define __OPCODE_HPP

#include <cstdint>
#include <string>
#include <vector>

/*
 * Opcode 
 * Represents a single Opcode from the MIPS aseembly language
 */
struct Opcode
{
    uint32_t    instr;
    std::string mnemonic;

    public:
        Opcode();
        Opcode(const uint32_t c, const std::string& m);
        void init(void);
        std::string toString(void) const;
};


/*
 * OpcodeTable
 * Hold a collection of Opcodes
 */
class OpcodeTable
{
    private:
        std::vector<Opcode> op_list;
        Opcode null_op;

    public:
        OpcodeTable();
        ~OpcodeTable();
        void         add(const Opcode& op);
        Opcode&      get(const std::string& mnemonic);
        Opcode&      get(const uint32_t opcode);
        Opcode&      getIdx(const int idx);
        unsigned int size(void) const;
        void         init(void);
};


#endif /*__OPCODE_HPP*/
