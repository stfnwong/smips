/*
 * SOURCE
 * Objects for processing assembly source. These are almost like a low-grade IR 
 * for the assembly source.
 *
 * Stefan Wong 2019
 */

#ifndef __SMIPS_SOURCE_HPP
#define __SMIPS_SOURCE_HPP

#include <string>
#include "Opcode.hpp"


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


// TODO : just have one type for registers, use map to get register number
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
    SYM_REGISTER,
} TokenType;


static std::string TokenToString(const TokenType& type)
{
    switch(type)
    {
        case SYM_NONE:
            return "NONE";
        case SYM_EOF:
            return "EOF";
        case SYM_LABEL:
            return "LABEL";
        case SYM_INSTR:
            return "INSTR";
        case SYM_LITERAL:
            return "LITERAL";
        case SYM_DIRECTIVE:
            return "DIRECTIVE";
        case SYM_CHAR:
            return "CHAR";
        case SYM_STRING:
            return "STRING";
        case SYM_SYSCALL:
            return "SYSCALL";
        case SYM_REGISTER:
            return "REGISTER";
        default:
            return "NULL";
    }

}



/*
 * Token
 * Represents a single token from the source stream.
 */
struct Token
{
    TokenType   type;
    std::string val;
    std::string reg_offset;   // eg: the 4 in 4($t1), etc

    public:
        Token();
        Token(const TokenType& t, const std::string& v);
        Token(const Token& that) = default;
        void init(void);
        bool isReg(void) const;
        bool isOffset(void) const;
        std::string toString(void) const;

        // comparisons 
        bool operator==(const Token& that) const;
        bool operator!=(const Token& that) const;

        // assignment
        Token& operator=(const Token& that) = default;
};


/*
 * Argument
 * Represents a single argument to an instruction 
 */
struct Argument
{
    TokenType type;
    int       val;
    int       offset;  

    public:
        Argument();
        Argument(const TokenType& t, int v);
        Argument(const Argument& that) = default;

        void init(void);

        bool operator==(const Argument& that) const;
        bool operator!=(const Argument& that) const;
        std::string toString(void) const;

        // assignment
        Argument& operator=(const Argument& that) = default;
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
    // NOTE : upper and lower are only required for symbol expansion 
    bool         upper;
    bool         lower;
    bool         psuedo;
    Argument     args[3];
    Opcode       opcode;

    public:
        TextInfo(); 
        void init(void);
        bool hasOp(void) const;

        bool operator==(const TextInfo& that) const;
        bool operator!=(const TextInfo& that) const;

        // string formatting
        std::string toString(void) const;
        std::string toInstrString(void) const;
        std::string diff(const TextInfo& that) const;
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

        unsigned int size(void) const;
};


/*
 * Symbol
 */
struct Symbol
{
    uint32_t    addr;
    std::string label;
    int         section;    // For now just 0 for data, 1 for text (as in lexer)

    public:
        Symbol();
        Symbol(const Symbol& that) = default;
        bool operator==(const Symbol& that) const;
        bool operator!=(const Symbol& that) const;
        Symbol& operator=(const Symbol& that) = default;
        void init(void);
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

        // TODO : search for data by address (dereference data section)
        uint8_t      getDataByAddr(const uint32_t addr);        // TODO : better to do by copy or by ref?

        std::string  toString(void) const;
        std::string  errString(void) const;
};

#endif /*__SMIPS_SOURCE_HPP*/
