#include <cstdint>
#include "mips/instruction.hpp"


namespace Instr {
    // Adding instr_ prefix here to avoid name clashes with reserved words

    // R-Type
    Instruction instr_add(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_addu(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_sub(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_subu(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_and(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_or(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_xor(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_nor(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_slt(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_sltu(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_sll(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_srl(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_sra(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_sllv(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_srlv(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_srav(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_jr(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_jalr(uint8_t rd, uint8_t rs, uint8_t rt);

    // I-Type
    Instruction instr_addi(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_addiu(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_andi(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_ori(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_xori(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_lui(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_slti(uint8_t rd, uint8_t rs, uint8_t rt);
    Instruction instr_sltiu(uint8_t rd, uint8_t rs, uint8_t rt);

	// Load/Store instructions 
	Instruction instr_lw(uint8_t rd, uint8_t rs, uint8_t rt);
	Instruction instr_lh(uint8_t rd, uint8_t rs, uint8_t rt);
	Instruction instr_lhu(uint8_t rd, uint8_t rs, uint8_t rt);
	Instruction instr_lb(uint8_t rd, uint8_t rs, uint8_t rt);
	Instruction instr_lbu(uint8_t rd, uint8_t rs, uint8_t rt);
	Instruction instr_sw(uint8_t rd, uint8_t rs, uint8_t rt);
	Instruction instr_sh(uint8_t rd, uint8_t rs, uint8_t rt);
	Instruction instr_sb(uint8_t rd, uint8_t rs, uint8_t rt);

	// Branches
	Instruction instr_beq(uint8_t rd, uint8_t rs, uint8_t rt);
	Instruction instr_bne(uint8_t rd, uint8_t rs, uint8_t rt);
	Instruction instr_blez(uint8_t rd, uint8_t rs, uint8_t rt);
	Instruction instr_bgtz(uint8_t rd, uint8_t rs, uint8_t rt);
	Instruction instr_bltz(uint8_t rd, uint8_t rs, uint8_t rt);
	Instruction instr_bgez(uint8_t rd, uint8_t rs, uint8_t rt);

	// Jumps 
	Instruction instr_j(uint8_t rd, uint8_t rs, uint8_t rt);
	Instruction instr_jal(uint8_t rd, uint8_t rs, uint8_t rt);

	// No-op
	Instruction instr_nop(void);
	//Instruction instr_nop(uint8_t rd, uint8_t rs, uint8_t rt);
}
