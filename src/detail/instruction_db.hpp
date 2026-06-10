// Maintain an internal database of instructions

#pragma once
#include "mips/instruction.hpp"
#include <vector>
#include <unordered_map>


// Unique identifier for an instruction
struct InstrId { 
    uint8_t opcode;
    uint8_t funct;    // Need this for R-type opcodes only

    bool operator==(const InstrId& other) const {
        return (this->opcode == other.opcode) && (this->funct == other.funct);
    }

};

// Hash function 
namespace std {
    template<>
        struct hash<InstrId> {
            size_t operator()(const InstrId& id) const {
                return (static_cast<size_t>(id.opcode) << 8) | id.funct;
            }
        };
}

// Centralised instruction database 
class InstructionDB {
    private:
        std::unordered_map<InstrId, InstrMetadata> db_;
        std::unordered_map<std::string, InstrId> mnemonic_to_id;
            
    public:
        InstructionDB() {
            // Populate database 
            this->init_mips_instrs();
        }

        // Lookup by opcode / funct 
        const InstrMetadata* lookup(uint8_t opcode, uint8_t funct = 0) const;
        const InstrMetadata* lookup(const Instruction& instr) const;
        const InstrMetadata* lookup(const std::string& mnemonic) const;
        std::vector<std::string> all_mnemonics(void) const;

    private:
        void add_instr(
            const std::string& mnemonic,
            uint8_t opcode,
            uint8_t funct,
            InstrFormat format,
            const std::string& description = "",
            bool is_branch = false,
            bool is_jump = false,
            bool is_load = false,
            bool is_store = false,
            bool uses_immediate = false,
            uint8_t latency = 1
        ); 

        void init_mips_instrs(void);

};


// Create a global instance of the database 
inline InstructionDB& instruction_db(void) {
    static InstructionDB db;
    return db;
}
