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
        // New style parser functions 
        /*
         *
         * Re-write the instruction parsing to be more like 
        case LEX_LW:
            this->arg[0] = this->parseRegister();   // returns some Argument object
            this->arg[1] = this->parseRegister();



        case LEX_LUI:
            this->arg[0] = this->parseRegister();
            this->arg[2] = this->parseImmediate();

            One issue is how to handle symbols, but I think the best thing is to 
            just return a new TextInfo each time rather than re-use the same one 
            again and again

            It may also be that we just have a 'wrapper' method which 
            returns a fully formed TextInfo (with error strings, symbols, and 
            whatever else would be valid for that input) and inside that wrapper 
            function is where we deal with handling the correspondence between 
            each Argument structure and the TextInfo. For instance, if we try to
            parse and Immediate we should also set is_imm.

            How are offsets going to work? We could just put in the Argument. Since we
            know which instruction we are checking we can just pull it out only if its
            and instruction which we know accepts offsets.
        */
        // parser helpers 
        void parse_i(void);
        void parse_r(void);
        void parse_rr(void);
        void parse_ri(void);
        void parse_rri(void);
        void parse_rrr(void);

        Argument parseRegister(void);
        Argument parseImmediate(void);  // could be literal or symbol

        void branchInstructionArgSwap(void);
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
        void parseAddress(int num);
        void parseBranch(void);
        void parseBranchZero(void);
        void parseMemArgs(void); // TODO : drop - just use immediate
        void parseRegArgs(const int num);
        void parseJump(void);
		void parseLabel(void);
        void parseLine(void);


    private:
        // psuedo instruction expansion

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
        const SourceInfo& getSourceInfo(void) const;
        const SymbolTable& getSymTable(void) const;
        int   getTextStartAddr(void) const;
        int   getDataStartAddr(void) const;
        // setters 
        void  setVerbose(const bool v);
};

#endif /*__SMIPS_LEXER_HPP*/
