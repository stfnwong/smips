#include <catch2/catch_test_macros.hpp>
#include "pipeline_stages.hpp"   // TODO: is this part of public interface?
#include "mips/instr_factory.hpp"


//TEST_CASE("Single ADDI executes correctly", "[pipeline][basic]") {
//	Pipeline cpu;
//
//	cpu.load_program(


TEST_CASE("Empty pipeline drains cleanly", "[pipeline][edge]") {
	Pipeline cpu;
	cpu.load_program({ Instr::nop().bits() });

	REQUIRE_NOTHROW(cpu.run_cycles(20));
}
