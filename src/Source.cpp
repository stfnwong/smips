/*
 * SOURCE
 * Objects for processing assembly source 
 *
 * Stefan Wong 2019
 */

#include <iomanip>
#include <sstream>
#include "Source.hpp"


Token::Token()
{
    this->type = SYM_NONE;
    this->val = "\0";
}

Token::Token(const TokenType& t, const std::string& v)
{
    this->type = t;
    this->val = v;
}

std::string Token::toString(void) const
{
    switch(this->type)
    {
        case SYM_NONE:
            return "NONE";
        case SYM_EOF:
            return "EOF";
        case SYM_INSTR:
            return "INSTR";
        case SYM_LITERAL:
            return "LITERAL";
        case SYM_REG_TEMP:
            return "R_TEMP";
        case SYM_REG_SAVED:
            return "R_SAVED";
        case SYM_REG_ARG:
            return "R_ARG";
        case SYM_REG_RET:
            return "R_RET";
        default:
            return "NULL";
    }
}


/*
 * LineInfo
 */
LineInfo::LineInfo()
{
    this->init();
}

void LineInfo::init(void)
{
    this->label     = "\0";
    this->symbol    = "\0";
    this->errstr    = "\0";
    this->line_num  = 0;
    this->addr      = 0;
    this->error     = false;
    this->is_label  = false;
    this->is_symbol = false;
    this->opcode.init();
}

std::string LineInfo::toString(void)
{
    return "\0";
}




/*
 * Symbol
 */
Symbol::Symbol()
{
    this->addr = 0;
    this->label = "\0";
}

std::string Symbol::toString(void) const
{
    std::ostringstream oss;
    oss << "[0x" << std::setw(4) << std::hex << this->addr << "] ";
    oss << this->label;

    return oss.str();
}

/*
 * SymbolTable
 */
SymbolTable::SymbolTable() {} 


void SymbolTable::add(const Symbol& s)
{
    this->syms.push_back(s);
}


void SymbolTable::update(const unsigned int idx, const Symbol& s)
{
    if(idx < this->syms.size())
        this->syms[idx] = s;
}

Symbol& SymbolTable::get(const unsigned int idx) 
{
    if(idx < this->syms.size())
        return this->syms[idx];

    return this->null_sym;
}

uint32_t SymbolTable::getAddr(const std::string& label) const
{
    for(unsigned int idx = 0; idx < this->syms.size(); ++idx)
    {
        if(label == this->syms[idx].label)
            return this->syms[idx].addr;
    }

    return (uint32_t) 0;
}

unsigned int SymbolTable::size(void) const
{
    return this->syms.size();
}

void SymbolTable::init(void)
{
    this->syms.clear();
}



/*
 * SourceInfo
 */
SourceInfo::SourceInfo() {}

void SourceInfo::add(const LineInfo& l)
{
    this->line_info.push_back(l);
}

void SourceInfo::update(const unsigned int idx, const LineInfo& l)
{
    if(idx < this->line_info.size())
        this->line_info[idx] = l;
}

LineInfo& SourceInfo::get(const unsigned int idx)
{
    if(idx < this->line_info.size())
        return this->line_info[idx];
    
    return this->null_line;
}


unsigned int SourceInfo::getLineNum(const unsigned int idx) const
{
    if(idx < this->line_info.size())
        return this->line_info[idx].line_num;

    return 0;
}

unsigned int SourceInfo::getNumErr(void) const
{
    unsigned int num_err = 0;
    for(unsigned int idx = 0; idx < this->line_info.size(); ++idx)
        num_err += (this->line_info[idx].error) ? 1 : 0;

    return num_err;
}

unsigned int SourceInfo::getNumLines(void) const
{
    return this->line_info.size();
}
