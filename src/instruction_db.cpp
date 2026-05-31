#include "instruction_db.hpp"



const InstrMetadata* InstructionDB::lookup(uint8_t opcode, uint8_t funct) const {
    InstrId id{ opcode, funct };
    auto it = this->db_.find(id);
    
    return it != this->db_.end() ? &it->second : nullptr;
}

const InstrMetadata* InstructionDB::lookup(const Instruction& instr) const {
    return this->lookup(instr.opcode(), instr.funct());
}

const InstrMetadata* InstructionDB::lookup(const std::string& mnemonic) const {
    auto it = this->mnemonic_to_id.find(mnemonic);
    if ( it == this->mnemonic_to_id.end() )
        return nullptr;

    return this->lookup(it->second.opcode, it->second.funct);
}


std::vector<std::string> InstructionDB::all_mnemonics(void) const {
    std::vector<std::string> result;
    for (const auto& pair: this->mnemonic_to_id ) {
        result.push_back(pair.first);
    }

    return result;
}



void InstructionDB::add_instr(
    const std::string& mnemonic,
    uint8_t opcode,
    uint8_t funct,
    InstrFormat format,
    const std::string& description,
    bool is_branch,
    bool is_jump,
    bool is_load ,
    bool is_store,
    bool uses_immediate ,
    uint8_t latency
) {
    InstrId id{opcode, funct};
    InstrMetadata meta{
        mnemonic,
        format,
        is_branch,
        is_jump,
        is_load,
        is_store,
        uses_immediate,
        is_branch || is_jump,  // has delay slot 
        latency,
        description
    };

    this->db_[id] = meta;
    this->mnemonic_to_id[mnemonic] = id;
}


void InstructionDB::init_mips_instrs(void) {
    // R-type instructions (opcode = 0x00, differentiated by funct)
    this->add_instr("add",  0x00, 0x20, InstrFormat::RType, "Add");
    this->add_instr("addu", 0x00, 0x21, InstrFormat::RType, "Add unsigned");
    this->add_instr("sub",  0x00, 0x22, InstrFormat::RType, "Substract");
    this->add_instr("subu", 0x00, 0x23, InstrFormat::RType, "Substract unsigned");
    this->add_instr("and",  0x00, 0x24, InstrFormat::RType, "Bitwise AND");
    this->add_instr("or",   0x00, 0x25, InstrFormat::RType, "Bitwise OR");
    this->add_instr("xor",  0x00, 0x26, InstrFormat::RType, "Bitwise XOR");
    this->add_instr("nor",  0x00, 0x27, InstrFormat::RType, "Bitwise NOR");
    this->add_instr("slt",  0x00, 0x2A, InstrFormat::RType, "Set less than");
    this->add_instr("sltu", 0x00, 0x2B, InstrFormat::RType, "Set less than unsigned");
    this->add_instr("sll",  0x00, 0x00, InstrFormat::RType, "Shift left logical");
    this->add_instr("srl",  0x00, 0x02, InstrFormat::RType, "Shift right logical");
    this->add_instr("sra",  0x00, 0x03, InstrFormat::RType, "Shift right arithmetic");
    this->add_instr("sllv", 0x00, 0x04, InstrFormat::RType, "Shift left logical variable");
    this->add_instr("srlv", 0x00, 0x06, InstrFormat::RType, "Shift right logical variable");
    this->add_instr("srav", 0x00, 0x07, InstrFormat::RType, "Shift right arithmetic variable");
    this->add_instr("jr",   0x00, 0x08, InstrFormat::RType, "Jump register", false, true);
    this->add_instr("jalr", 0x00, 0x09, InstrFormat::RType, "Jump and link register", false, true);

    // I-type instructions 
    this->add_instr("addi",  0x08, 0x00, InstrFormat::IType, "Add immediate", false, false, false, false, true);
    this->add_instr("addiu", 0x09, 0x00, InstrFormat::IType, "Add immediate unsigned", false, false, false, false, true);
    this->add_instr("andi",  0x0C, 0x00, InstrFormat::IType, "AND immediate unsigned", false, false, false, false, true);
    this->add_instr("ori",   0x0D, 0x00, InstrFormat::IType, "OR immediate unsigned", false, false, false, false, true);
    this->add_instr("xori",  0x0E, 0x00, InstrFormat::IType, "XOR immediate unsigned", false, false, false, false, true);
    this->add_instr("lui",   0x0F, 0x00, InstrFormat::IType, "Load upper immediate", false, false, false, false, true);
    this->add_instr("slti",  0x0A, 0x00, InstrFormat::IType, "Set less than immediate", false, false, false, false, true);
    this->add_instr("sltiu", 0x0B, 0x00, InstrFormat::IType, "Set less than immediate unsigned", false, false, false, false, true);

    // Load/Store 
    this->add_instr("lw",  0x23, 0x00, InstrFormat::IType, "Load word", false, false, true, false, false, 2);
    this->add_instr("lh",  0x21, 0x00, InstrFormat::IType, "Load halfword", false, false, true, false, false, 2);
    this->add_instr("lhu", 0x21, 0x00, InstrFormat::IType, "Load halfword unsigned", false, false, true, false, false, 2);
    this->add_instr("lb",  0x20, 0x00, InstrFormat::IType, "Load byte", false, false, true, false, false, 2);
    this->add_instr("lbu", 0x24, 0x00, InstrFormat::IType, "Load byte unsigned", false, false, true, false, false, 2);
    this->add_instr("sw",  0x2B, 0x00, InstrFormat::IType, "Store word", false, false, false, true);
    this->add_instr("sh",  0x29, 0x00, InstrFormat::IType, "Store half word", false, false, false, true);
    this->add_instr("sb",  0x28, 0x00, InstrFormat::IType, "Store byte", false, false, false, true);

    // Branches
    this->add_instr("beq",  0x04, 0x00, InstrFormat::IType, "Branch if equal", true);
    this->add_instr("bne",  0x05, 0x00, InstrFormat::IType, "Branch if not equal", true);
    this->add_instr("blez", 0x06, 0x00, InstrFormat::IType, "Branch if less than or equal zero", true);
    this->add_instr("bgtz", 0x07, 0x00, InstrFormat::IType, "Branch if greater than zero", true);
    this->add_instr("bltz", 0x01, 0x00, InstrFormat::IType, "Branch if less than zero", true);
    this->add_instr("bgez", 0x01, 0x01, InstrFormat::IType, "Branch if greater or equal zero", true);

    // Jumps 
    this->add_instr("j",   0x02, 0x00, InstrFormat::JType, "Jump", false, true);
    this->add_instr("jal", 0x03, 0x00, InstrFormat::JType, "Jump and link", false, true);
}
