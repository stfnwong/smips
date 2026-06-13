// TODO: remove these after debugging
#include <format>
#include <iostream>
#include <string>

#include "cpu.hpp"


// ==== CPU
void CPU::cycle(void) {
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


bool CPU::is_halted(void) const {
	return !this->if_id.valid && !this->id_ex.valid && !this->ex_mem.valid && !this->mem_wb.valid;
}

uint32_t CPU::reg(uint8_t r) const { 
	return this->reg_file.read(r);
}


void CPU::run(uint64_t max_cycles) {
	while( this->cycle_count < max_cycles && !this->is_halted() ) {
		this->cycle();
	}
}


void CPU::run_cycles(uint64_t n) {
	for( uint64_t i = 0; i < n; ++i) { 
		this->cycle();
		if( this->is_halted() ) {
			// TODO: debug, remove 
			std::cout << "[" << __func__ << "] halting" << std::endl;
			break;
		}
	}
}


void CPU::load_program(const std::vector<uint32_t>& program, uint32_t start_addr) {
	this->i_mem.load_program(program, start_addr);
}


void CPU::reset(void) {
	this->d_mem.reset();
	this->i_mem.reset();

	this->hazard.clear();
	this->forward.clear();

	this->pc = 0;
	this->cycle_count = 0;
	this->instr_count = 0;
	this->stall_count = 0;
}


std::string CPU::dump_reg(void) const {
	return this->reg_file.dump();
}


PipelineState CPU::dump_state(void) const {
	return PipelineState(
		this->if_id,
		this->id_ex,
		this->ex_mem,
		this->mem_wb,
		this->pc,
		this->cycle_count
	);
}


void CPU::detect_hazards(void) {
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
