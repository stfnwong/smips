/*
 * SOURCE
 * Objects for processing assembly source 
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
    // register types
    SYM_REG_FUNC,
    SYM_REG_TEMP,
    SYM_REG_SAVED,
    SYM_REG_ARG,
    SYM_REG_RET,
    SYM_REG_ZERO,
    SYM_REG_NUM,        // register that has just a number
    SYM_REG_GLOBAL,     // for the $GP register
} TokenType;


/*
 * Token
 */
struct Token
{
    TokenType type;
    std::string val;

    public:
        Token();
        Token(const TokenType& t, const std::string& v);
        bool isReg(void) const;
        bool isOffset(void) const;
        std::string toString(void) const;
};


/*
 * LineInfo
 * Information about a single line of assembly source
 */
struct LineInfo
{
    std::string  label;
    std::string  symbol;
    std::string  errstr;
    unsigned int line_num;
    unsigned int addr;
    bool         error;
    bool         is_label;
    bool         is_symbol;
    bool         is_directive;
    bool         is_imm;
    int          args[3];
    TokenType    types[3];      // record types for each register
    Opcode       opcode;

    public:
        LineInfo(); 
        void init(void);
        std::string toString(void) const;

        bool operator==(const LineInfo& that) const;
        bool operator!=(const LineInfo& that) const;

        std::string diff(const LineInfo& that) const;
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
        std::string toString(void) const;
        bool operator==(const Symbol& that) const;
        bool operator!=(const Symbol& that) const;
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
        std::vector<LineInfo> line_info;
        LineInfo null_line;

    public:
        SourceInfo();
        void         add(const LineInfo& l);
        void         update(const unsigned int idx, const LineInfo& l);
        LineInfo&    get(const unsigned int idx);
        unsigned int getLineNum(const unsigned int idx) const;
        unsigned int getNumErr(void) const;
        unsigned int getNumLines(void) const;

        std::string  toString(void) const;
};

#endif /*__SOURCE_HPP*/
