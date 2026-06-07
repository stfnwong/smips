#include <cstdint>
#include <stdexcept>

#include "mips/instruction.hpp"
#include "instruction_db.hpp"



class InstructionBuilder {
	private:
		uint32_t bits = 0;

	public:
		// Start from scratch 
		InstructionBuilder() = default;

		// Start from existing instruction 
		static InstructionBuilder from_bits(uint32_t bits) { 
			InstructionBuilder b;
			b.bits = bits;
			return b;
		}
		InstructionBuilder& opcode(uint8_t op) {
			this->bits = (this->bits & ~(0x3F << 26)) | ((op & 0x3F) << 26);
			return *this;
		}
		
		InstructionBuilder& rs(uint8_t r) {
			this->bits = (this->bits & ~(0x1F << 21)) | ((r & 0x1F) << 21);
			return *this;
		}
		
		InstructionBuilder& rt(uint8_t r) {
			this->bits = (this->bits & ~(0x1F << 16)) | ((r & 0x1F) << 16);
			return *this;
		}
		
		InstructionBuilder& rd(uint8_t r) {
			this->bits = (this->bits & ~(0x1F << 11)) | ((r & 0x1F) << 11);
			return *this;
		}
		
		InstructionBuilder& shamt(uint8_t s) {
			this->bits = (this->bits & ~(0x1F << 6)) | ((s & 0x1F) << 6);
			return *this;
		}
		
		InstructionBuilder& funct(uint8_t f) {
			this->bits = (this->bits & ~0x3F) | (f & 0x3F);
			return *this;
		}
		
		InstructionBuilder& immediate(int16_t imm) {
			this->bits = (this->bits & ~0xFFFF) | static_cast<uint16_t>(imm);
			return *this;
		}
		
		InstructionBuilder& target(uint32_t t) {
			this->bits = (this->bits & ~0x3FFFFFF) | (t & 0x3FFFFFF);
			return *this;
		}
		
		// Bit-level mutation (for fuzzing)
		InstructionBuilder& flip_bit(uint8_t pos) {
			this->bits ^= (1u << pos);
			return *this;
		}
		
		InstructionBuilder& set_bits(uint32_t mask, uint32_t value) {
			this->bits = (this->bits & ~mask) | (value & mask);
			return *this;
		}

		// Terminal operation: consume the builder, produce an Instruction
		Instruction build() const {
			return Instruction(this->bits);
		}
		
		// Implicit conversion for convenience
		explicit operator Instruction() const { return build(); }

		/// Field setters 
		static InstructionBuilder r_type(const std::string& mnemonic, uint8_t rd, uint8_t rs, uint8_t rt) {
			const auto* meta = instruction_db().lookup(mnemonic);
			if( !meta || meta->format != InstrFormat::RType) {
				throw std::runtime_error("Not an R-Type instruction: " + mnemonic);
			}

			return InstructionBuilder{}
				.opcode(0x00)
				.rs(rs)
				.rt(rt)
				.rd(rd)
				.shamt(0)
				.funct(meta->funct);
		}

		static InstructionBuilder i_type(const std::string& mnemonic, uint8_t rt, uint8_t rs, int16_t imm) {
			const auto* meta = instruction_db().lookup(mnemonic);
			if( !meta || meta->format != InstrFormat::IType ){
				throw std::runtime_error("Not an I-Type instruction: " + mnemonic);
			}

			return InstructionBuilder{}
				.opcode(meta->opcode)
				.rs(rs)
				.rt(rt)
				.immediate(imm);
		}

		static InstructionBuilder j_type(const std::string& mnemonic, uint32_t target_addr) {
			const auto* meta = instruction_db().lookup(mnemonic);
			if( !meta || meta->format != InstrFormat::JType ) {
				throw std::runtime_error("Not a J-Type instruction: " + mnemonic);
			}

			return InstructionBuilder{}
				.opcode(meta->opcode)
				.target(target_addr >> 2);
		}
};
