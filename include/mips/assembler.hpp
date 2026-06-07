#pragma once
#include "mips/instruction.hpp"
#include "mips/instr_factory.hpp"

#include <map>
#include <string>
#include <vector>

struct AssemblyResult {
	std::vector<Instruction> instrs;
	std::map<std::string, uint32_t> labels;
	bool success;
	std::string err_msg;
};


// Simple two-pass assembler
class Assembler {
	private:
		std::map<std::string, uint32_t> labels;
		uint32_t cur_addr;

	public:
	AssemblyResult assemble(
		const std::vector<std::string>& lines,
		uint32_t start_addr = 0
		);

	private:
		std::string trim(const std::string& s);
		std::vector<std::string> tokenize(const std::string& s);
		int32_t parse_immediate(const std::string& s);
		Instruction assemble_r_type(const std::string& mnemonic, const std::vector<std::string>& tokens);
		Instruction assemble_i_type(const std::string& mnemonic, const std::vector<std::string>& tokens);
		Instruction assemble_j_type(const std::string& mnemonic, const std::vector<std::string>& tokens);
		Instruction assemble_line(const std::string& line);
};
