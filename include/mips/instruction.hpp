#pragma once
#include <cstdint>
#include <string>
#include <optional>



// Instruction formats 
enum class InstrFormat {
    RType,    // Register op:6rt:5shamt:5
    IType,    // op:6rt:5
    JType     // op:6
};


// Metadata about instruction properties 
struct InstrMetadata { 
    std::string mnemonic;    // eg: "add", "lw", "beq", etc
    InstrFormat format;
	uint8_t opcode;
	uint8_t funct;
    bool is_branch = false;
    bool is_jump = false;
    bool is_load = false;
    bool is_store = false;
    bool uses_immediate = false;
    bool has_delay_slot = false;
    uint8_t latency = 1;
    std::string description;

    // For testing/validation 
    std::optional<std::string> operand_pattern;   // eg: "rd", "rt"
};



class Instruction {
    private:
        uint32_t bits_;
        std::optional<InstrMetadata> meta_;
        std::optional<uint32_t> pc_;

    public:
        explicit Instruction(uint32_t b = 0) : bits_(b) {} 

        Instruction(uint32_t b, const InstrMetadata& meta) :
            bits_(b), meta_(meta) {}

        // Raw access 
        uint32_t  bits(void) const;
        //uint32_t& bits(void) const;

        // PC associatiojn (for branches, jumps, disassembly)
        void set_pc(uint32_t pc);
        std::optional<uint32_t> pc(void) const;

        // Metadata access 
        const InstrMetadata* metadata(void) const;
        void set_metadata(const InstrMetadata& m);

        // Field extraction - this is format agnostic so its always safe to call
        uint8_t opcode(void) const;
        uint8_t rs(void) const;
        uint8_t rt(void) const;
        uint8_t rd(void) const;
        uint8_t shamt(void) const;
        uint8_t funct(void) const;

        // Sign-extended immediate 
        int32_t imm_signed(void) const;

        // Zero-extended immediate 
        uint32_t imm_unsigned(void) const;

        // Jump target (26-bit)
        uint32_t target(void) const;

        // Compute branch/jummp addresses
        std::optional<uint32_t> branch_target(void) const;
        std::optional<uint32_t> jump_target(void) const;

        // Field mutation 
        Instruction& set_opcode(uint8_t op);
        Instruction& set_rs(uint8_t rs);
        Instruction& set_rt(uint8_t rt);
        Instruction& set_rd(uint8_t rd);
        Instruction& set_shamt(uint8_t shamt);
        Instruction& set_funct(uint8_t funct);
        Instruction& set_immediate(int16_t imm);
        Instruction& set_target(uint32_t target);

        // Format detection 
        InstrFormat detect_format(void) const;

        // Comparison 
        bool operator==(const Instruction& other) const;
        bool operator!=(const Instruction& other) const;

        // String representation
        std::string to_hex(void) const;

};


