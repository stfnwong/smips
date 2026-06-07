#include <stdexcept>

#include "mips/instruction.hpp"
#include "mips/instr_factory.hpp"
#include "mips/assembler.hpp"
#include "mips/register.hpp"

#include "instruction_db.hpp"
#include "instruction_builder.hpp"



AssemblyResult Assembler::assemble(
	const std::vector<std::string>& lines,
	uint32_t start_addr 
	) {
	AssemblyResult result; 
	result.success = true;			// TODO: set at end?
	this->cur_addr = start_addr;
	this->labels.clear();

	// First pass - collect labels
	for (const auto& line: lines) {
		std::string trimmed = this->trim(line);
		if( trimmed.empty() || trimmed[0] == '#' )
			continue;

		// Check for label
		size_t colon_pos = trimmed.find(':');
		if( colon_pos != std::string::npos ) {
			std::string label = this->trim(trimmed.substr(0, colon_pos));
			this->labels[label] = this->cur_addr;
			trimmed = this->trim(trimmed.substr(colon_pos + 1));
		}

		if( !trimmed.empty() && trimmed[0] != '#' ) {
			this->cur_addr += 4;
		}
	}

	// Second pass - assemble instructions 
	this->cur_addr = start_addr;
	for ( size_t line_num = 0; line_num < lines.size(); ++line_num ) {
		std::string trimmed = this->trim(lines[line_num]);
		if( trimmed.empty() || trimmed[0] == '#')
			continue;

		// Skip labels 
		size_t colon_pos = trimmed.find(':');
		if( colon_pos != std::string::npos ) {
			trimmed = this->trim(trimmed.substr(colon_pos + 1));
		}

		if( trimmed.empty() || trimmed[0] == '#' ) 
			continue;

		try {
			Instruction instr = this->assemble_line(trimmed);
			instr.set_pc(this->cur_addr);
			result.instrs.push_back(instr);
			this->cur_addr += 4;
		} catch( const std::exception& e ) {
			result.success = false;
			result.err_msg = "Line " + std::to_string(line_num + 1)
				+ ": " + e.what();

			return result;
		}
	}

	result.labels = this->labels;

	return result;
}


std::string Assembler::trim(const std::string& s) {
	size_t start = s.find_first_not_of(" \t\r\n");
	if( start == std::string::npos)
		return "";

	size_t end = s.find_last_not_of(" \t\r\n");
	return s.substr(start, end - start + 1);
}


std::vector<std::string> Assembler::tokenize(const std::string& s) {
	std::vector<std::string> tokens;
	std::string cur;

	for( char c : s ) {
		if ( c == ',' || c == '(' || c == ')' || c == ' ' || c == '\t') { 
			if( !cur.empty() ) {
				tokens.push_back(cur);
				cur.clear();
			} else {
				// consume one character
				cur += c;  
			}
		}

		if( !cur.empty() ) { 
			tokens.push_back(cur);
		}

		return tokens;
	}
}


int32_t Assembler::parse_immediate(const std::string& s) {
	// Handle hex (0x...), decimal, or label reference 
	if( s.size() > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X') ) {
		return std::stoi(s.substr(2), nullptr, 16);
	}

	// Check if this is a label 
	auto it = this->labels.find(s);
	if( it != labels.end() ) {
		// Compute offset for branches 
		int32_t offset = (it->second - (this->cur_addr + 4)) >> 2;
		return offset;
	}

	return std::stoi(s);
}


Instruction Assembler::assemble_r_type(
	const std::string& mnemonic,
	const std::vector<std::string>& tokens
) {
	if( mnemonic == "jr" ) {
		// jr rs 
		if( tokens.size() != 2) 
			throw std::runtime_error("jr expects 1 register");
		uint8_t rs = RegisterNames::parse(tokens[1]);
		return InstructionBuilder::r_type(mnemonic, 0, rs, 0).build();
	}

	// Standard: op, rd, rs, rt
	if( tokens.size() != 4) {
		throw std::runtime_error(mnemonic + " expects 3 registers, got " + std::to_string(tokens.size()));
	}

	uint8_t rd = RegisterNames::parse(tokens[1]);
	uint8_t rs = RegisterNames::parse(tokens[2]);
	uint8_t rt = RegisterNames::parse(tokens[3]);

	return InstructionBuilder::r_type(mnemonic, rd, rs, rt).build();
}


Instruction Assembler::assemble_i_type(
	const std::string& mnemonic,
	const std::vector<std::string>& tokens
) {
	const auto* meta = instruction_db().lookup(mnemonic);

	if( meta->is_load || meta->is_store ) {
		// lw/sw rt, offset(rs)
		if( tokens.size() != 4 ) {
			throw std::runtime_error(mnemonic + " expects rt, offset(rs)");
		}
	}

	uint8_t rt = RegisterNames::parse(tokens[1]);
	int16_t offset = static_cast<int16_t>(this->parse_immediate(tokens[2]));
	uint8_t rs = RegisterNames::parse(tokens[3]);  

	return InstructionBuilder::i_type(mnemonic, rt, rs, offset).build();
}


Instruction Assembler::assemble_j_type(
	const std::string& mnemonic,
	const std::vector<std::string>& tokens
) {
	if( tokens.size() != 2 ) {
		throw std::runtime_error(mnemonic + " expects target");
	}

	uint32_t target;
	if( this->labels.count(tokens[1])) { 
		target = this->labels[tokens[1]];
	}
	else {
		target = static_cast<uint32_t>(this->parse_immediate(tokens[1]));
	}

	return InstructionBuilder::j_type(mnemonic, target).build();
}


Instruction Assembler::assemble_line(const std::string& line) {
	auto tokens = this->tokenize(line);
	if( tokens.empty() ) {
		throw std::runtime_error("Empty instruction");
	}

	std::string mnemonic = tokens[0];

	// NOP case 
	if( mnemonic == "nop") {
		return Instr::instr_nop(0, 0, 0);
	}

	// Lookup instruction metadata 
	const auto* meta = instruction_db().lookup(mnemonic);
	if( !meta ) {
		throw std::runtime_error("Unknown instruction: " + mnemonic);
	}

	// Parse based on format 
	switch( meta->format ) {
		case InstrFormat::RType:
			return this->assemble_r_type(mnemonic, tokens);
		case InstrFormat::IType:
			return this->assemble_i_type(mnemonic, tokens);
		case InstrFormat::JType:
			return this->assemble_j_type(mnemonic, tokens);
		// TODO: do I actually need a handler for this case? 
		// Read up some more on exceptions and exception handling for this
	}

	throw std::runtime_error("Unknown InstrFormat [" + instr_format_str(meta->format) + "]");
}
