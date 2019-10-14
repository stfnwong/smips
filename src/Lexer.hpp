/*
 * LEXER
 * MIPS Assembly language Lexer 
 *
 * Stefan Wong 2019
 */

#ifndef __LEXER_HPP
#define __LEXER_HPP

#include <string>
#include "Opcode.hpp"
#include "Source.hpp"

// Give some kind of named identifier to the Lexer mode
enum LexMode {LEX_DATA_SEG, LEX_TEXT_SEG};

/*
 * Lexer
 * Lexes a source file and produces a sequence of Tokens
 */
class Lexer
{
    // Instruction table
    private:
        OpcodeTable instr_code_table;
        void init_instr_table(void);

    // Psuedo instruction table
    private:
        OpcodeTable psuedo_op_table;
        void init_psuedo_op_table(void);

    // Assembler directives 
    private:
        OpcodeTable directive_code_table;
        void init_directive_code_table(void);

    // Token memory
    private:
        int   token_buf_size;
        int   line_buf_size;
        char* token_buf;
        char* line_buf;
        int   start_addr;
        bool  verbose;
        bool  expand_psuedo;

    private:
        void  alloc_mem(void);

    // Position in source text 
    private:
        char          cur_char;
        unsigned int  cur_line;
        unsigned int  cur_pos;

    // Source 
    private:
        std::string  source_text;
        std::string  filename;
        SourceInfo   source_info;
        SymbolTable  sym_table;
        unsigned int text_addr;
        unsigned int data_addr;

    // Segments 
    private:
        TextInfo     text_info;
        DataInfo     data_info;
        LexMode      cur_mode;          // whether to place the current instruction in text or data seg

    // Motion through source file
    private:
        bool exhausted(void) const;
        void advance(void);
        void skipWhitespace(void);
        void skipComment(void);
        void skipSeperators(void);
        // character type id 
        bool isSpace(void) const;
        bool isComment(void) const;

    // Token handling
    private:
        Token cur_token;
        void scanToken(void);
		// extra token handling functions
		Token extractLiteral(
				const std::string& token, 
				unsigned int start_offset, 
				unsigned int& end_offset);
        Token extractReg(
				const std::string& token, 
				unsigned int start_offset, 
				unsigned int& end_offset);

        void nextToken(void);

    // Assembler directives
    private:
        // TODO : no support for floats in the first version
        void parseASCIIZ(void);
        void parseAlign(void);
        void parseByte(void);
        void parseDouble(void);
        void parseHalf(void);
        void parseMacro(void);
        void parseEndMacro(void);
        void parseWord(void);
        void parseSpace(void);

        // segment mode directives
        void dataSeg(void);
        void textSeg(void);

    private:
        void resolveLabels(void);
        void parseBranch(void);
        void parseBranchZero(void);
        void parseMemArgs(void);
        void parseRegArgs(const int num);
        void parseJump(void);
		void parseLabel(void);
        void parseLine(void);
        TokenType getRegType(const char& reg_char) const;

    private:
        // update instruction addresses when expanding psuedo ops
        void  advanceAddrs(int start_idx, int offset);

    // disable copy construction, assignment
    private:
        Lexer(const Lexer& that) = delete;
        Lexer& operator=(const Lexer& that) = delete;

    public:
        Lexer();
        ~Lexer();

        void  expandPsuedo(void);
        void  lex(void);
        int   loadFile(const std::string& filename);
        // getters 
        bool  getVerbose(void) const;
        bool  getExpandPsuedo(void) const;
        const SourceInfo& getSourceInfo(void) const;
        const SymbolTable& getSymTable(void) const;
        int   getStartAddr(void) const;
        // setters 
        void  setVerbose(const bool v);
        void  setExpandPsuedo(const bool v);
        void  setStartAddr(int a);

};

#endif /*__LEXER_HPP*/
