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
    // register types
    SYM_REG_TEMP,
    SYM_REG_SAVED,
    SYM_REG_ARG,
    SYM_REG_RET
} TokenType;


//std::string TokenTypeString(const TokenType& t)
//{
//    switch(t)
//    {
//        case SYM_NONE:
//            return "NONE";
//        case SYM_EOF:
//            return "EOF";
//        case SYM_INSTR:
//            return "INSTR";
//        case SYM_LITERAL:
//            return "LITERAL";
//        case SYM_REG_TEMP:
//            return "R_TEMP";
//            // and so on...
//        default:
//            return "NULL";
//    }
//}



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
        std::string toString(void) const;
};



/*
 * LineInfo
 * Information about a single line of assembly source
 */
struct LineInfo
{
    std::string label;
    std::string symbol;
    unsigned int line_num;
    unsigned int addr;
    bool         error;
    Opcode       opcode;

    public:
        LineInfo(); 
        void init(void);

        std::string toString(void);
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

        void add(const Symbol& s);
        void update(const unsigned int idx, const Symbol& s);
        Symbol& get(const unsigned int idx);
        uint32_t getAddr(const std::string& label) const;
        unsigned int size(void) const;

        void init(void);
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

        void add(const LineInfo& l);
        void update(const unsigned int idx, const LineInfo& l);
        LineInfo& get(const unsigned int idx);

        unsigned int getLineNum(const unsigned int idx) const;
        unsigned int getNumErr(void) const;
        unsigned int getNumLines(void) const;


};

#endif /*__SOURCE_HPP*/
