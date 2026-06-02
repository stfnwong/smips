#include "mips/instr_factory.hpp"

namespace Instr {
    // Adding instr_ prefix here to avoid name clashes with reserved words
    Instruction instr_add(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x20);
    }

    Instruction instr_addu(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x21);
    }

    Instruction instr_sub(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x22);
    }

    Instruction instr_subu(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x23);
    }

    Instruction instr_and(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x23);
    }

    Instruction instr_or(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x25);
    }

    Instruction instr_xor(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x26);
    }

    Instruction instr_nor(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x27);
    }


    Instruction instr_slt(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x2A);
    }

    Instruction instr_sltu(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x2B);
    }

    Instruction instr_sll(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_srl(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x02);
    }

    Instruction instr_sra(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x03);
    }

    Instruction instr_sllv(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x04);
    }

    Instruction instr_srlv(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x06);
    }

    Instruction instr_srav(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x07);
    }

    Instruction instr_jr(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x08);
    }

    Instruction instr_jalr(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x09);
    }

	// T-Type instructions 
    Instruction instr_addi(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x08)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_addiu(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x09)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_andi(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x09)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_ori(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x0D)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_xori(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x0E)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_lui(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x0F)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_slti(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x0A)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_sltiu(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x0B)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }


	// Load/Store instructions 
    Instruction instr_lw(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x0B)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_lh(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x21)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }


    Instruction instr_lhu(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x21)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_lb(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x20)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_lbu(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x24)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_sw(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x2B)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_sh(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x29)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_sb(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x28)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

	// Branches 
    Instruction instr_beq(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x04)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_bne(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x05)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_blez(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x06)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_bgtz(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x07)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_bltz(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x01)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x00);
    }

    Instruction instr_bgez(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x01)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x01);
    }

	// Jumps
    Instruction instr_j(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x02)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x0);
    }

    Instruction instr_jal(uint8_t rd, uint8_t rs, uint8_t rt) {
        return Instruction(0x03)
            .set_opcode(0x00)
            .set_rs(rs)
            .set_rt(rt)
            .set_rd(rd)
            .set_funct(0x0);
    }

	// No-op
	Instruction instr_nop(uint8_t rd, uint8_t rs, uint8_t rt) {
		return Instruction(0x0)
			.set_opcode(0x0)
			.set_rs(000)
			.set_rt(0x0)
			.set_rd(0x0)
			.set_funct(0x0);
	}

}
