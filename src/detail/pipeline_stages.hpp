#pragma once 

#include "mips/register_file.hpp"
#include "pipeline_types.hpp"
#include "memory.hpp"



// ==== FetchStage
class FetchStage {
    private:
        const Memory&  i_mem;
        HazardSignals& hazard;
        IfIdLatch&     output;
        uint32_t       pc;

    public:
        FetchStage(const Memory& mem, HazardSignals& hazard, IfIdLatch& output)
            : i_mem(mem), hazard(hazard), output(output), pc(0) {}

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




