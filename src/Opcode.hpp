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

struct Opcode
{
    uint32_t instr;
    std::string mnemonic;

    public:
        Opcode();
        Opcode(const uint32_t c, const std::string& m);
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
        void add(const Opcode& op);
        Opcode& get(const std::string& mnemonic);
        Opcode& get(const uint32_t opcode);
        Opcode& getIdx(const int idx);
        unsigned int size(void) const;

};


#endif /*__OPCODE_HPP*/
