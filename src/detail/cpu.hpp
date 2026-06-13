#include "pipeline_stages.hpp"


struct PipelineState {
	// Hold the register states 
	IfIdLatch  if_id;
	IdExLatch  id_ex;
	ExMemLatch ex_mem;
	MemWbLatch mem_wb;
	// Also hold pc and cycle count 
	uint32_t pc;
	uint64_t cycle;

	public:
		PipelineState() = default;
		PipelineState(
			IfIdLatch if_latch,
			IdExLatch id_latch,
			ExMemLatch ex_latch,
			MemWbLatch mem_latch,
			uint32_t cur_pc,
			uint64_t cur_cycle
		) : if_id(if_latch), id_ex(id_latch), ex_mem(ex_latch), mem_wb(mem_latch), pc(cur_pc), cycle(cur_cycle) {} 
};



class CPU {
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
		CPU() : 
			stage_if(i_mem, if_id, hazard),
			stage_id(reg_file, if_id, id_ex, hazard),
			stage_ex(id_ex, ex_mem, hazard, forward),
			stage_mem(d_mem, ex_mem, mem_wb, hazard, forward),
			stage_wb(reg_file, mem_wb, forward),
			pc(0),
			cycle_count(0),
			instr_count(0),
			stall_count(0) {}

		void     cycle(void);
		bool     is_halted(void) const;
		uint32_t reg(uint8_t r) const;
		void     run(uint64_t max_cycles = UINT64_MAX);
		void     run_cycles(uint64_t n);
		void     load_program(const std::vector<uint32_t>& program, uint32_t start_addr = 0);
		void     reset(void);

		std::string dump_reg(void) const;
		PipelineState dump_state(void) const;

	private:
		void detect_hazards(void);
};
