/*
 * SOURCE
 * Objects for processing assembly source. These are almost like a low-grade IR 
 * for the assembly source.
 *
 * Stefan Wong 2019
 */

#ifndef __SOURCE_HPP
#define __SOURCE_HPP

#include <string>
#include "Opcode.hpp"

typedef enum TokenType
{
    // null token
    SYM_NONE,
    // end token
    SYM_EOF,
    SYM_LABEL,
    SYM_INSTR,
    SYM_LITERAL,
    SYM_DIRECTIVE,
    SYM_CHAR,
    SYM_STRING,
    SYM_SYSCALL,
    // register types
    SYM_REG_AT,     // assembler temporary
    SYM_REG_STACK,
    SYM_REG_FRAME,
    SYM_REG_TEMP,
    SYM_REG_SAVED,
    SYM_REG_ARG,
    SYM_REG_RET,
    SYM_REG_RET_ADR,
    SYM_REG_KERN,
    SYM_REG_ZERO,
    SYM_REG_NUM,        // register that has just a number
    SYM_REG_GLOBAL,     // for the $GP register
} TokenType;


typedef enum DirectiveType
{
    SYM_DIR_NONE,
    SYM_DIR_ALIGN,
    SYM_DIR_ASCIIZ,
    SYM_DIR_BYTE,
    SYM_DIR_CHAR,
    SYM_DIR_GLOBL,
    SYM_DIR_HALF,
    SYM_DIR_MACRO,
    SYM_DIR_END_MACRO,
    SYM_DIR_SPACE,
    SYM_DIR_WORD,
} DirectiveType;


/*
 * Token
 * Represents a single token from the source stream.
 */
struct Token
{
    TokenType   type;
    std::string val;
    std::string offset;

    public:
        Token();
        Token(const TokenType& t, const std::string& v);
        void init(void);
        bool isReg(void) const;
        bool isOffset(void) const;
        std::string toString(void) const;

        // comparisons 
        bool operator==(const Token& that) const;
        bool operator!=(const Token& that) const;

        // assignment
        Token& operator=(const Token& that);
};


/*
 * TextInfo
 * Information about a single line of assembly source. This object is a kind of 
 * intermediate representation for text section data.
 */
struct TextInfo
{
    std::string  label;
    std::string  symbol;
    std::string  errstr;
    unsigned int line_num;
    unsigned int addr;
    bool         error;
    bool         is_label;
    bool         is_directive;
    bool         is_imm;
    bool         is_symbol;
    bool         upper;
    int          val[3];
    TokenType    type[3];      // record of types for each register
    Opcode       opcode;

    public:
        TextInfo(); 
        void init(void);
        bool hasOp(void) const;

        bool operator==(const TextInfo& that) const;
        bool operator!=(const TextInfo& that) const;

        // string formatting
        std::string toString(void) const;
        std::string diff(const TextInfo& that) const;
        // TODO: fomats the TextInfo like an instruction
        //std::string toInstrString(void) const;
};


/*
 * DataInfo
 * Information for a directive in the assembly source which contains memory 
 * information (eg: .text, .word, and so on). This object forms a kind of intermediate
 * representation for assembly lines that will ultimately be placed in the data section
 * of the output binary.
 */
struct DataInfo
{
    DirectiveType         directive;
    std::string           errstr;
    std::string           label;
    std::vector <uint8_t> data;
    unsigned int          line_num;
    unsigned int          addr;
    unsigned int          space;
    bool                  is_label;
    bool                  error;

    public:
        DataInfo();
        void init(void);
        std::string dirTypeString(void) const;
        std::string toString(void) const;

        DataInfo& operator=(const DataInfo& that);
        bool operator==(const DataInfo& that) const;
        bool operator!=(const DataInfo& that) const;

        // Insert a new byte into the data section
        void addByte(const uint8_t byte);
        void addHalf(const uint16_t half);
        void addWord(const uint32_t word);
};


/*
 * Symbol
 */
struct Symbol
{
    uint32_t    addr;
    std::string label;

    public:
        Symbol();
        bool operator==(const Symbol& that) const;
        bool operator!=(const Symbol& that) const;
        std::string toString(void) const;
};

/*
 * SymbolTable
 */
class SymbolTable
{
    private:
        std::vector<Symbol> syms;
        Symbol null_sym;

    public:
        SymbolTable();
        void         add(const Symbol& s);
        void         update(const unsigned int idx, const Symbol& s);
        Symbol&      get(const unsigned int idx);
        uint32_t     getAddr(const std::string& label) const;
        unsigned int size(void) const;
        void         init(void);
};


/*
 * SourceInfo
 */
class SourceInfo
{
    private:
        std::vector<TextInfo> text_info;        // text section(s)
        std::vector<DataInfo> data_info;        // data sections(s)
        TextInfo null_line;
        DataInfo null_data;

    public:
        SourceInfo();
        void         addText(const TextInfo& l);
        void         addData(const DataInfo& d);
        void         update(const unsigned int idx, const TextInfo& l);
        void         insert(const unsigned int idx, const TextInfo& l);
        DataInfo&    getData(const unsigned int idx);
        TextInfo&    getText(const unsigned int idx);
        unsigned int getLineNum(const unsigned int idx) const;
        unsigned int getNumErr(void) const;
        unsigned int getNumLines(void) const;
        bool         hasError(void) const;

        unsigned int getTextInfoSize(void) const;
        unsigned int getDataInfoSize(void) const;

        std::string  toString(void) const;
        std::string  errString(void) const;
};

#endif /*__SOURCE_HPP*/
