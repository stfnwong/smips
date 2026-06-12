#include <iostream>    // TODO: remove this, debug only
#include "decoder.hpp"
#include "pipeline_types.hpp"
#include "pipeline_stages.hpp"


// ==== FetchStage
void FetchStage::tick(void) {
	if (this->hazard.flush_if) { 
		output.invalidate();
		return;
	}

	if (this->hazard.stall_if)
		return;

	this->output.valid = true;
	this->output.pc    = this->pc;
	this->output.instr = this->i_mem.read_word(this->pc);
	this->pc += 4;
}



// ==== DecodeStage
void DecodeStage::tick(void) {
	if (!this->input.valid || this->hazard.stall_id || this->hazard.flush_id) {
		this->output.invalidate();
		return;
	}

	// Decode the instruction 
	DecodedInstr decoded = Decoder::decode(this->input.instr, this->input.pc);

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
	this->output.immediate   = decoded.immediate;   // TODO: do I need a cast here?
	this->output.alu_op      = decoded.alu_op;
	this->output.mem_op      = decoded.mem_op;
	this->output.wb_dest     = decoded.wb_dest;
	this->output.mem_to_reg  = decoded.mem_to_reg;
	this->output.branch_info = decoded.branch_info;
}




// ==== ExecuteStage
void ExecuteStage::tick(void) { 
	if( !this->input.valid ) { 
		this->output.invalidate();
		return;
	}

	// Get operands with forwarding applied 
	uint32_t op_a = this->forward.get_forward(this->input.rs_val, this->input.rs_addr);
	uint32_t op_b = this->forward.get_forward(this->input.rt_val, this->input.rt_addr);

	// For immediate instructions the second operand is the immediate 
	if( this->input.immediate ) {
		op_b = static_cast<uint32_t>(this->input.immediate);
	}

	// Execute ALU operation
	uint32_t alu_result = this->alu(this->input.alu_op, op_a, op_b);

	// Resolve branches
	bool branch_taken = false;
	if( this->input.branch_info.has_value() ) {
		const auto& br = this->input.branch_info.value();
		branch_taken = this->eval_branch(br.cond, op_a, op_b);

		if( branch_taken != br.predicted_taken ) {
			// Mispredict - flush the two instructions behind us 
			this->hazard.flush_if = true;
			this->hazard.flush_id = true;
			this->hazard.branch_target = branch_taken ? br.target : this->input.pc + 8;
		}
	}

	// Write EX/MEM latch
	this->output.valid = true;
	this->output.pc = this->input.pc;
	this->output.alu_result = alu_result;
	this->output.mem_op = this->input.mem_op;
	this->output.store_data = op_b;     // rt value, needed for stores 
	this->output.wb_dest = this->input.wb_dest;
	this->output.mem_to_reg = this->input.mem_to_reg;

	// Setup forwarding for instructions behind us in the pipeline 
	if( this->input.wb_dest.has_value() ) {
		this->forward.set_ex(this->input.wb_dest.value(), alu_result);
	}
}


uint32_t ExecuteStage::alu(ALUOp op, uint32_t a, uint32_t b) const {
	switch( op ) {
		case ALUOp::Add:
			return a + b;
		case ALUOp::Sub:
			return a - b;
		case ALUOp::And:
			return a & b;
		case ALUOp::Or:
			return a | b;
		case ALUOp::Xor:
			return a ^ b;
		case ALUOp::Nor:
			return ~(a | b);
		case ALUOp::Slt:
			return static_cast<int32_t>(a) < static_cast<int32_t>(b) ? 1 : 0;
		case ALUOp::Sltu:
			return a < b ? 1 : 0;
		case ALUOp::Sll:
			return b << (a & 0x1F);
		case ALUOp::Srl:
			return b >> (a & 0x1F);
		case ALUOp::Sra:
			return static_cast<uint32_t>(static_cast<int32_t>(b) >> (a & 0x1F));
		default:
			throw std::runtime_error("Unknown ALU operation");
	}
}


bool ExecuteStage::eval_branch(BranchCond cond, uint32_t a, uint32_t b) const {
	switch( cond ) {
		case BranchCond::Equal:
			return a == b;
		case BranchCond::NotEqual:
			return a != b;
		case BranchCond::LessThan:
			return static_cast<int32_t>(a) < 0;
		case BranchCond::GreaterEqual:
			return static_cast<int32_t>(a) >= 0;
		default:
			throw std::runtime_error("Unknown branch condition");
	}
}


// ==== MemoryStage
void MemoryStage::tick(void) {
	if( !this->input.valid ) {
		this->output.invalidate();
		return;
	}

	if( this->input.mem_op == MemOp::Load ) {
		uint32_t data = this->d_mem.read_word(this->input.alu_result);   // const op
		this->output.data = data;
		this->output.wb_dest = this->input.wb_dest;
		this->output.valid = true;
	}
	else if( this->input.mem_op == MemOp::Store ) { 
		this->d_mem.write_word(this->input.alu_result, this->input.store_data);  // mutating op 
		this->output.valid = true;
	}
	else {
		// ALU result pass-through
		this->output.data = this->input.alu_result;
		this->output.wb_dest = this->input.wb_dest;
		this->output.valid = true;
	}

	// Setup forwarding from MEM stage
	if( this->output.wb_dest.has_value() ) { 
		this->forward.ex_value[this->output.wb_dest.value()] = this->output.data;
	}
}



// ==== WritebackStage
void WritebackStage::tick(void) { 
	if( !this->input.valid ) 
		return;

	if( !this->input.wb_dest.has_value() )
		return;

	this->reg_file.write(this->input.wb_dest.value(), this->input.data);
	// Setup forwarding from WB stage 
	this->forward.set_mem(this->input.wb_dest.value(), this->input.data);
}



// ==== Pipeline
void Pipeline::cycle(void) {
	// Clear control signals
	this->hazard.clear();
	this->forward.clear();

	//  Reverse order 
	this->stage_wb.tick();
	this->stage_mem.tick();
	this->stage_ex.tick();
	this->stage_id.tick();
	this->stage_if.tick();

	// TODO: check hazards here
	this->pc = this->hazard.branch_target.value_or(this->pc + 4);

	this->cycle_count++;

	// Since this is for debugging I don't need a very big cycle count
	std::string debug = std::format("(cycle {:04}) pc = {:04X}", this->cycle_count, this->pc);
	std::cout << "[" << __func__ << "] " << debug << std::endl;
}


bool Pipeline::is_halted(void) const {
	return !this->if_id.valid && !this->id_ex.valid && !this->ex_mem.valid && !this->mem_wb.valid;
}

uint32_t Pipeline::reg(uint8_t r) const { 
	return this->reg_file.read(r);
}


void Pipeline::run(uint64_t max_cycles) {
	while( this->cycle_count < max_cycles && !this->is_halted() ) {
		this->cycle();
	}
}


void Pipeline::run_cycles(uint64_t n) {
	for( uint64_t i = 0; i < n; ++i) { 
		this->cycle();
		if( this->is_halted() ) {
			// TODO: debug, remove 
			std::cout << "[" << __func__ << "] halting" << std::endl;
			break;
		}
	}

	//for( uint64_t i = 0; i < n && !this->is_halted(); ++i) {
	//	this->cycle();
	//}
}


void Pipeline::load_program(const std::vector<uint32_t>& program, uint32_t start_addr) {
	this->i_mem.load_program(program, start_addr);
}


void Pipeline::reset(void) {
	this->d_mem.reset();
	this->i_mem.reset();

	this->hazard.clear();
	this->forward.clear();

	this->pc = 0;
	this->cycle_count = 0;
	this->instr_count = 0;
	this->stall_count = 0;
}


std::string Pipeline::dump_reg(void) const {
	return this->reg_file.dump();
}


void Pipeline::detect_hazards(void) {
	// Load-use hazard 
	if( this->ex_mem.valid && 
		this->ex_mem.mem_op.has_value() && 
		this->ex_mem.mem_op.value() == MemOp::Load && 
		this->ex_mem.wb_dest.has_value()
	  ) {
		uint8_t load_dest = this->ex_mem.wb_dest.value();

		// Check if ID/EX stage needs this register 
		if( this->id_ex.valid && 
		    (this->id_ex.rs_addr == load_dest || this->id_ex.rt_addr == load_dest)
			) {
			// Stall the pipline 
			this->hazard.stall_if = true;
			this->hazard.stall_id = true;
			this->hazard.flush_ex = true;   // put a bubble in EX
			return;
		}
	}

	// TODO: control hazards 
	// TODO: structural hazards
}
