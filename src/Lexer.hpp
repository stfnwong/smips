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
        LineInfo     line_info;
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
        void nextToken(void);

    private:
        void parseRegArgs(const int num);
        void parseJump(void);
        void parseLine(void);
        void scanToken(void);

    public:
        Lexer();
        ~Lexer();

        void  lex(void);
        int   loadFile(const std::string& filename);
        // getters 
        bool  getVerbose(void) const;
        const SourceInfo& getSourceInfo(void) const;
        // setters 
        void  setVerbose(const bool v);

};

#endif /*__LEXER_HPP*/
