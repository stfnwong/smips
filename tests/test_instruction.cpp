#include <catch2/catch_test_macros.hpp>
#include "mips/instruction.hpp"
#include "mips/register.hpp"


///#include "mips/instr_factory.hpp"


TEST_CASE("Create Instruction from integer", "[instr][basic]") {
	Instruction i(0);

	REQUIRE(i.bits() == 0);
}

TEST_CASE("Create load instruction", "[instr][basic]") {
	Instruction i(0x0B);  // lw
	
	i.set_opcode(0x00);
	i.set_rs(RegisterNames::parse("$t0"));   
	i.set_rt(RegisterNames::parse("$t1"));  
	i.set_rd(RegisterNames::parse("$t2"));

	REQUIRE(i.bits() != 0);    // TODO: look up the expected bit pattern
}

//TEST_CASE("Create store instruction", "[instr][basic]") {
//}
