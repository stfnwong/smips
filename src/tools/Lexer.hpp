/*
 * LEXER
 * MIPS Assembly language Lexer 
 *
 * Stefan Wong 2019
 */

#ifndef __SMIPS_LEXER_HPP
#define __SMIPS_LEXER_HPP

#include <string>
#include "Opcode.hpp"
#include "Source.hpp"
#include "Register.hpp"

// Give some kind of named identifier to the Lexer mode
enum LexMode {LEX_DATA_SEG, LEX_TEXT_SEG};

/*
 * Lexer
 * Lexes a source file and produces a sequence of Tokens
 */
class Lexer
{
    // general
    private:
        void init_state(void);

    // Instruction table
    private:
        OpcodeTable instr_code_table;
        void init_instr_table(void);

    // Psuedo instruction table
    private:
        OpcodeTable psuedo_op_table;
        void init_psuedo_op_table(void);

    //  Register mapping
    private:
        RegisterMap reg_map;

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
        int   text_start_addr;
        int   data_start_addr;
        bool  verbose;

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
        void incrTextAddr(void);
        void incrDataAddr(void);

    // Segments 
    private:
        TextInfo     text_info;
        DataInfo     data_info;
        LexMode      cur_mode;          // whether to place the current instruction in text or data seg

    // Motion through source file
    private:
        void advance(void);
        bool exhausted(void) const;
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
        // parser helpers 
        void parse_i(void);
        void parse_r(void);
        void parse_rr(void);
        void parse_ri(void);
        void parse_rri(void);
        void parse_rro(void);
        void parse_rrr(void);
        void add_noop(void);

        Argument parseRegister(void);
        Argument parseImmediate(void);  // could be literal or symbol

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
        void parseInstr(int line_num);
        void parseDirective(int line_num);
        void resolveLabels(void);
        void parseLine(void);

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
        /*
         * lex()
         * Lex the source in this->source_text and produce a 
         * SourceInfo representation.
         */
        void  lex(void);
        /*
         * loadFile()
         * Load source from file 
         */
        int   loadFile(const std::string& filename);
        /*
         * loadSource()
         * Load source from a string
         */
        void  loadSource(const std::string& src);   
        // getters 
        bool  getVerbose(void) const;
        const SourceInfo& getSourceInfo(void) const;
        const SymbolTable& getSymTable(void) const;
        int   getTextStartAddr(void) const;
        int   getDataStartAddr(void) const;
        // setters 
        void  setVerbose(const bool v);
};

#endif /*__SMIPS_LEXER_HPP*/
