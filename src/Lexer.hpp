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

    private:
        OpcodeTable directive_code_table;
        void init_directive_code_table(void);

    // Token memory
    private:
        int   token_buf_size;
        char* token_buf;
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
        TextInfo     text_info;
        DataInfo     data_info;
        SymbolTable  sym_table;
        unsigned int cur_addr;

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
        void scanString(void);
        void nextToken(void);

    // Assembler directives
    private:
        // TODO : no support for floats in the first version
        void parseASCIIZ(void);
        void parseAlign(void);
        void parseByte(void);
        void parseData(void);
        void parseDouble(void);
        void parseHalf(void);
        void parseMacro(void);
        void parseEndMacro(void);
        void parseWord(void);
        void parseSpace(void);
        void parseText(void);

    private:
        void resolveLabels(void);
        void parseBranch(void);
        void parseBranchZero(void);
        void parseMemArgs(void);
        void parseRegArgs(const int num);
        void parseJump(void);
        void parseLine(void);
        TokenType getRegType(const char& reg_char) const;

    public:
        Lexer();
        ~Lexer();

        void  lex(void);
        int   loadFile(const std::string& filename);
        // getters 
        bool  getVerbose(void) const;
        const SourceInfo& getSourceInfo(void) const;
        const SymbolTable& getSymTable(void) const;
        // setters 
        void  setVerbose(const bool v);

};

#endif /*__LEXER_HPP*/
