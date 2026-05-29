#include <cstdint>
#include <optional>

#include "mips/instruction.hpp"

// TODO: once the build setup works this can probably become header only

// Instruction methods 
uint32_t  Instruction::bits(void) const { return this->bits_; }

void Instruction::set_pc(uint32_t pc) { this->pc_ = pc; }

std::optional<uint32_t> Instruction::pc(void) const { return this->pc_; }

const InstrMetadata* Instruction::metadata(void) const { 
    return this->meta.has_value() ? &meta.value() : nullptr;
}

void Instruction::set_metadata(const InstrMetadata& m) { 
    this->meta = m;
}


uint8_t Instruction::opcode(void) const {
    return (this->bits_ >> 26) & 0x3F;
}

uint8_t Instruction::rs(void) const {
    return (this->bits_ >> 21) & 0x1F;
}

uint8_t Instruction::rt(void) const {
    return (this->bits_ >> 16) & 0x1F;
}

uint8_t Instruction::rd(void) const {
    return (this->bits_ >> 11) & 0x1F;
}

uint8_t Instruction::shamt(void) const {
    return (this->bits_ >> 6) & 0x1F;
}

uint8_t Instruction::funct(void) const {
    return this->bits_ & 0x3F;
}

int32_t Instruction::imm_signed(void) const {
    return static_cast<int16_t>(this->bits_ & 0xFFFF);
}

uint32_t Instruction::imm_unsigned(void) const {
    return this->bits_ & 0xFFFF;
}

uint32_t Instruction::target(void) const {
    return this->bits_ & 0x3FFFFFF;
}

std::optional<uint32_t> Instruction::branch_target(void) const {
    if (!this->pc_.has_value())
        return std::nullopt;
    // PC-relative: (PC + 4) + (sign_extend(imm) << 2)
    return ((this->pc_.value() + 4) & 0xF0000000) | (this->target() << 2);
}

std::optional<uint32_t> Instruction::jump_target(void) const {
    if (!this->pc_.has_value())
        return std::nullopt;

    // Absolute: {(PC + 4[31:28], target, 2'b00}
    return ((this->pc_.value() + 4) & 0xF0000000) | (this->target() << 2);
}

// Field mutation 
Instruction& Instruction::set_opcode(uint8_t op) {
    this->bits_ = (this->bits_ & 0x00FFFFFF) | ((op & 0x3F) << 26);
    return *this;
}

Instruction& Instruction::set_rs(uint8_t rs) { 
    this->bits_ = (this->bits_ & 0xFC1FFFFF) | ((rs & 0x1F) << 21);
    return *this;
}

Instruction& Instruction::set_rt(uint8_t rt) { 
    this->bits_ = (this->bits_ & 0xFFE0FFFF) | ((rt & 0x1F) << 16);
    return *this;
}

Instruction& Instruction::set_rd(uint8_t rd) { 
    this->bits_ = (this->bits_ & 0xFFFF07FF) | ((rd & 0x1F) << 11);
    return *this;
}

Instruction& Instruction::set_shamt(uint8_t shamt) {
    this->bits_ = (this->bits_ & 0xFFFFF83F) | ((shamt & 0x1F) << 6);
    return *this;
}

Instruction& Instruction::set_funct(uint8_t funct) {
    this->bits_ = (this->bits_ & 0xFFFFFFC0) | (funct & 0x3F);
    return *this;
}

Instruction& Instruction::set_immediate(int16_t imm) {
    this->bits_ = (this->bits_ & 0xFFFF0000) | (static_cast<uint16_t>(imm) & 0xFFFF);
    return *this;
}

Instruction& Instruction::set_target(uint32_t target) { 
    this->bits_ = (this->bits_ & 0xFC000000) | (target & 0x3FFFFFFF);
    return *this;
}

// Format detection 
InstrFormat Instruction::detect_format(void) const {
    uint8_t op = this->opcode();
    if (op == 0x00)
        return InstrFormat::RType;  // SPECIAL 
    if (op == 0x02 || op == 0x03) 
        return InstrFormat::JType;

    return InstrFormat::IType;    // TODO:: is this all other types?
}

// Comparison 
bool Instruction::operator==(const Instruction& other) const {
    return this->bits_ == other.bits_;
}

bool Instruction::operator!=(const Instruction& other) const {
    return this->bits_ != other.bits_;
}

std::string Instruction::to_hex(void) const {
    char buf[16];
    snprintf(buf, sizeof(buf), "0x%08X", this->bits_);

    return std::string(buf);
}
