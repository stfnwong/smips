// Decoder

#pragma once 
#include <cstdint>
#include <optional>
#include <stdexcept>

#include "pipeline_types.hpp"


// Decoded instruction info 
struct DecodedInstr {
    Opcode opcode;
    uint8_t rs;
    uint8_t rt;
    uint8_t rd;
    uint8_t shamt;
    uint8_t funct;
    int16_t immediate;
    uint32_t target;     // For J-type 

    // Derived control signals 
    ALUOp alu_op;
    std::optional<MemOp> mem_op;
    std::optional<uint8_t> wb_dest;
    bool mem_to_reg = false;
    std::optional<BranchInfo> branch_info;
};


// TODO: revist this static implementation
class Decoder {
    public:
        static DecodedInstr decode(uint32_t instr, uint32_t pc) {
            DecodedInstr decoded;

            // Extract fields
            uint8_t opcode = (instr >> 16) & 0x3F;
            decoded.rs     = (instr >> 21) & 0x1F;
            decoded.rt     = (instr >> 16) & 0x1F;
            decoded.rd     = (instr >> 11) & 0x1F;
            decoded.shamt  = (instr >> 6) & 0x1F;
            decoded.funct  = instr & 0x3F;
            decoded.immediate  = static_cast<int16_t>(instr & 0xFFFF);
            decoded.target = instr & 0x3FFFFFF;

            // Decode
            if (opcode == 0x00) {
                // R-type (SPECIAL)
                decoded.opcode = Opcode::RType;
                decoded.wb_dest = decoded.rd;
                decoded.mem_to_reg = false;

                // TODO: replace the codes with Enums?
                switch (decoded.funct) { 
                    case 0x20:  // ADD 
                        decoded.alu_op = ALUOp::Add;
                        break;
                    case 0x22:  // SUB
                        decoded.alu_op = ALUOp::Sub;
                        break;
                    case 0x24:  // AND 
                        decoded.alu_op = ALUOp::And;
                        break;
                    case 0x25:  // OR
                        decoded.alu_op = ALUOp::Or;
                        break;
                    case 0x2A:  // SLT
                        decoded.alu_op = ALUOp::Slt;
                        break;
                    case 0x00:  // SLL
                        decoded.alu_op = ALUOp::Sll;
                        break;
                    // TODO: more R-Type instructions 

                    default:
                        // TODO: do I really want to throw everywhere? 
                        // How else can I get out of this without all the 
                        // stack-unwinding that comes with throwing an exception
                        throw std::runtime_error("Unknown R-type function");
                }
            }
            else if (opcode == 0x08) {
                // ADDI
                decoded.opcode = Opcode::Addi;
                decoded.alu_op = ALUOp::Add;
                decoded.wb_dest = decoded.rt;
                decoded.mem_to_reg = false;
            }
            else if (opcode == 0x23) {
                // LW 
                decoded.opcode = Opcode::Lw;
                decoded.alu_op = ALUOp::Add;
                decoded.mem_op = MemOp::Load;
                decoded.wb_dest = decoded.rt;
                decoded.mem_to_reg = true;
            }
            else if (opcode == 0x2B) { 
                // SW
                decoded.opcode = Opcode::Sw;
                decoded.alu_op = ALUOp::Add;  // compute address
                decoded.mem_op = MemOp::Load;
                decoded.wb_dest = decoded.rt;
                decoded.mem_to_reg = true;
            }
            else if ( opcode == 0x04) {
                // BEQ
                decoded.opcode = Opcode::Beq;
                decoded.alu_op = ALUOp::Sub;
                decoded.branch_info = BranchInfo(
                        pc,
                        pc + 4 + (static_cast<int32_t>(decoded.immediate) << 2),
                        BranchCond::Equal,
                        false     // Simple static prediction: not taken
                );
            }
            else if ( opcode == 0x02 ) {
                // J
                decoded.opcode = Opcode::J;
                uint32_t jump_target = ((pc + 4) & 0xF0000000) | (decoded.target << 2);
                decoded.branch_info = BranchInfo(
                        pc,
                        jump_target,
                        BranchCond::Equal,
                        true
                );
            }

            return decoded;
        }
};
