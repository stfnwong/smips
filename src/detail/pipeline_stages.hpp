#pragma once 

#include "mips/register_file.hpp"
#include "pipeline_types.hpp"
#include "memory.hpp"



// ==== FetchStage
class FetchStage {
    private:
        const Memory&  i_mem;
        IfIdLatch&     output;
        HazardSignals& hazard;
        uint32_t       pc;

    public:
        FetchStage(const Memory& mem, IfIdLatch& output, HazardSignals& haz)
            : i_mem(mem), output(output), hazard(haz), pc(0) {}

        void tick(void);
};



// ==== DecodeStage
class DecodeStage {
    private:
        const RegisterFile& reg_file;
        const IfIdLatch&    input;
        IdExLatch&          output;
        HazardSignals&      hazard;   // TODO: come back and use a ref here rather than a raw pointer

    public:
        DecodeStage(
            const RegisterFile& rf,
            const IfIdLatch& inp,
            IdExLatch& out,
            HazardSignals& haz
        ) : reg_file(rf), input(inp), output(out), hazard(haz) {}

        void tick(void);
};



// ==== ExecuteStage
class ExecuteStage {
	private:
		const IdExLatch& input;
		ExMemLatch&      output;
		HazardSignals&   hazard;
		ForwardingPaths& forward;

	public:
		ExecuteStage(
			const IdExLatch& inp,
			ExMemLatch& out,
			HazardSignals& haz,
			ForwardingPaths& fwd
		) : input(inp), output(out), hazard(haz), forward(fwd) {} 

		void tick(void);
		uint32_t alu(ALUOp op, uint32_t a, uint32_t b) const;
		bool eval_branch(BranchCond cond, uint32_t a, uint32_t b) const;
};


// ==== MemoryStage
class MemoryStage {
	private:
		Memory&           d_mem;
		const ExMemLatch& input;
		MemWbLatch&       output;
		HazardSignals&    hazard;
		ForwardingPaths&  forward;

	public:
		MemoryStage(
			Memory& dm,
			const ExMemLatch& inp,
			MemWbLatch& out,
			HazardSignals& haz,
			ForwardingPaths& fwd
		) : d_mem(dm), input(inp), output(out), hazard(haz), forward(fwd) {}

		void tick(void);
};



// ==== WritebackStage
class WritebackStage {
	private:
		RegisterFile& reg_file;     // Mutable as we write here in this stage
		const MemWbLatch& input;
		ForwardingPaths& forward;

	public:
		WritebackStage(
			RegisterFile& rf,
			const MemWbLatch& inp,
			ForwardingPaths& fwd
		) : reg_file(rf), input(inp), forward(fwd) {} 

		void tick(void);
};


// TODO: this is the CPU - do I want this in a new module?
// ==== Pipeline 
class Pipeline {
	private:
		Memory d_mem;   // data memory 
		Memory i_mem;   // instruction memory 
		RegisterFile reg_file;

		// Latches (pipeline registers)
		IfIdLatch  if_id;
		IdExLatch  id_ex;
		ExMemLatch ex_mem;
		MemWbLatch mem_wb;
		// TODO: do I need to hold the registers for the next pipeline stage as well?

		// Control signals 
		HazardSignals hazard;
		ForwardingPaths forward;

		// Stages 
		FetchStage stage_if;
		DecodeStage stage_id;
		ExecuteStage stage_ex;
		MemoryStage stage_mem;
		WritebackStage stage_wb;

		// Statistics
		uint32_t pc;
		uint64_t cycle_count;
		uint64_t instr_count;
		uint64_t stall_count;

	public:
		Pipeline() : 
			stage_if(i_mem, if_id, hazard),
			stage_id(reg_file, if_id, id_ex, hazard),
			stage_ex(id_ex, ex_mem, hazard, forward),
			stage_mem(d_mem, ex_mem, mem_wb, hazard, forward),
			stage_wb(reg_file, mem_wb, forward),
			pc(0),
			cycle_count(0),
			instr_count(0),
			stall_count(0) {}

		void cycle(void);
		bool is_halted(void) const;
		void run(uint64_t max_cycles = UINT64_MAX);
		void run_cycles(uint64_t n);
		void load_program(const std::vector<uint32_t>& program, uint32_t start_addr = 0);
		void reset(void);

	private:
		void detect_hazards(void);
};
