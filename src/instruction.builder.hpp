// Fluent builder for assembly-like construction

#include "mips/instruction.hpp"
#include "instruction_db.hpp"


class InstructionBuilder {
    private:
        Instruction instr;

    // TODO: these don't need to be static..
    public:
        // R-Type: op, rd, rs, rt
        static Instruction r_rtype(
            const std::string& mnemonic,
            uint8_t rd,
            uint8_t rs,
            uint8_t rt
        ) {
            const auto* meta = instruction_db().lookup(mnemonic);
            if ( !meta || meta->format != InstrFormat::RType) {
                throw std::runtime_error("Invalid R-Type instruction");
            }

            Instruction instr;
            instr.set_opcode(0x00);   // R-Type always has opcode 0
            instr.set_rs(rs);
            instr.set_rt(rt);
            instr.set_rd(rd);
            instr.set_shamt(0);
            instr.set_funct(
                instruction_db().lookup(mnemomnic)->mnemonic == mnemonic ?
                0 :   // TODO: need to extract funct from meta
                0
            );

            // TODO: need to extract funct from database
            for ( uint8_t f = 0; f < 64; ++f) {
                const auto* test_meta = instruction_db().lookup(0x00, f);
                if ( test_meta && test_meta->mnemonic == mnemonic ) {
                    instr.set_funct(f);
                    break;
                }
            }

            instr.set_metadata(*meta);

            return instr;
        }


};

