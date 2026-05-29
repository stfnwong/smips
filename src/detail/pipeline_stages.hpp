#pragma once 

#include "memory.hpp"
#include "pipeline_types.hpp"
#include "register_file.hpp"
#include "decoder.hpp"



class FetchStage {
    private:
        const Memory&  i_mem;
        HazardSignals* hazard;
        IfIdLatch*     output;
        uint32_t       pc;

    public:
        FetchStage(const Memory& mem, HazardSignals* hazard, IfIdLatch* output)
            : i_mem(mem), hazard(hazard), output(output), pc(0) {}

        void tick(void) {
            if (this->hazard->flush_if) { 
                output->invalidate();
                return;
            }

            if (this->hazard->stall_if)
                return;

            this->output->valid = true;
            this->output->pc    = this->pc;
            this->output->instruction = this->i_mem.read_word(this->pc);
            this->pc += 4;
        }
};



class DecodeStage {
    private:
        const RegisterFile& reg_file;
        const IfIfLatch&    input;
        IdExLatch*          output;
        HazardSignals*      hazard;

    public:
        DecodeStage(
            const RegisterFile& rf,
            const IdIfLatch& inp,
            IdExLatch* out,
            HazardSignals* haz
        ) : reg_file(rf), input(inp), output(out), hazard(haz) {}

        void tick(void) {
            if (!this->input.valid || this->hazard->stall_id || this->hazard->flush_id) {
                this->output.invalidate();
                return;
            }

            // Decode the instruction 
            DecodeInstr decoded = Decoder::decode(this->input->instruction, this->input->pc);

            // Read registers (speculative) 
            uint32_t rs_val = this->reg_file.read(decoded.rs);
            uint32_t rt_val = this->reg_file.read(decoded.rt);

            // Populate ID/EX latch
            this->output.valid       = true;
            this->output.pc          = this->input.pc;
            this->output.opcode      = decoded.opcode;
            this->output.rs_val      = rs_val;
            this->output.rt_val      = rt_val;
            this->output.rs_addr     = decoded.rs;
            this->output.rt_addr     = decoded.rt;
            this->output.rd_addr     = decoded.rd;
            this->output.immediate   = decoded.immediate;
            this->output.alu_op      = decoded.alu_op;
            this->output.mem_op      = decoded.mem_op;
            this->output.wb_dest     = decoded.wb_dest;
            this->output.mem_to_reg  = decoded.mem_to_reg;
            this->output.branch_info = decoded.branch_info;
        }
};
