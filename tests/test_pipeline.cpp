#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include "mips/instr_factory.hpp"

#include "detail/pipeline_stages.hpp"   // TODO: is this part of public interface?




TEST_CASE("Register zero is always zero", "[pipeline][basic]") {
	Pipeline cpu;
	cpu.load_program({
		Instr::instr_addi(0, 0, 99).bits(),    // try to write to $zero 
		Instr::instr_nop().bits(),
		Instr::instr_nop().bits(),
		Instr::instr_nop().bits(),
		Instr::instr_nop().bits()
	});

	cpu.run_cycles(10);
	REQUIRE(cpu.reg(0) == 0);
}

TEST_CASE("Single ADDI executes correctly", "[pipeline][basic]") {
	Pipeline cpu;
	cpu.load_program({
		// addi $t0, $zero, 42
		Instr::instr_addi(8, 0, 42).bits(),   
		Instr::instr_nop().bits(),
		Instr::instr_nop().bits(),
		Instr::instr_nop().bits(),
		Instr::instr_nop().bits()
	});

	cpu.run_cycles(10);
	std::cout << cpu.dump_reg() << std::endl;

	REQUIRE(cpu.reg(8) == 42);
}


TEST_CASE("ADD produces correct result", "[pipeline][basic]") {
	Pipeline cpu;
	cpu.load_program({
		Instr::instr_addi(8, 0, 10).bits(),   // $t0 = 10
		Instr::instr_addi(9, 0, 20).bits(),   // $t1 = 20
		Instr::instr_add(10, 8, 9).bits(),    // $t2 = $t0 + $t1
		Instr::instr_nop().bits(),
		Instr::instr_nop().bits(),
		Instr::instr_nop().bits(),
		Instr::instr_nop().bits()
	});

	cpu.run_cycles(10);
	std::cout << cpu.dump_reg() << std::endl;
	//std::println("{0}", cpu.dump_reg());

	REQUIRE(cpu.reg(10) == 30);
}


TEST_CASE("Empty pipeline drains cleanly", "[pipeline][edge]") {
	Pipeline cpu;
	cpu.load_program({ Instr::instr_nop().bits() });

	REQUIRE_NOTHROW(cpu.run_cycles(20));
}



