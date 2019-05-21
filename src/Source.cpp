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

bool Token::isReg(void) const
{
    switch(this->type)
    {
        case SYM_REG_TEMP:
        case SYM_REG_SAVED:
        case SYM_REG_ARG:
        case SYM_REG_RET:
        case SYM_ZERO_REG:
            return true;
        default:
            return false;
    }

    // If we somehow get here, then this is clearly not 
    // the value that we are looking for
    return false;
}

bool Token::isOffset(void) const
{
    if(this->type == SYM_REG_GLOBAL || this->type == SYM_REG_FUNC)
        return true;

    return false;
}

std::string Token::toString(void) const
{
    switch(this->type)
    {
        case SYM_NONE:
            return "NONE <" + this->val + ">";
        case SYM_EOF:
            return "EOF <" + this->val + ">";
        case SYM_INSTR:
            return "INSTR <" + this->val + ">";
        case SYM_LITERAL:
            return "LITERAL <" + this->val + ">";
        case SYM_REG_TEMP:
            return "R_TEMP <" + this->val + ">";
        case SYM_REG_SAVED:
            return "R_SAVED <" + this->val + ">";
        case SYM_REG_ARG:
            return "R_ARG <" + this->val + ">";
        case SYM_REG_RET:
            return "R_RET <" + this->val + ">";
        case SYM_ZERO_REG:
            return "R_ZERO <" + this->val + ">";
        case SYM_REG_NUM:
            return "R_NUM <" + this->val + ">";
        default:
            return "NULL <" + this->val + ">";
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
    this->label        = "\0";
    this->symbol       = "\0";
    this->errstr       = "\0";
    this->line_num     = 0;
    this->addr         = 0;
    this->error        = false;
    this->is_label     = false;
    this->is_symbol    = false;
    this->is_directive = false;
    this->is_imm       = false;
    this->opcode.init();

    for(int i = 0; i < 3; ++i)
    {
        this->args[i]  = 0;
        this->types[i] = SYM_NONE;
    }
}

std::string LineInfo::toString(void) const
{
    std::ostringstream oss;

    oss << "---------------------------------------------------------------------" << std::endl;
    oss << "Line  Type   Addr  Mnemonic   Opcode   Arguments  flags error" << std::endl;

    oss << std::left << std::setw(6) << std::setfill(' ') << this->line_num;
    oss << "[";
    if(this->is_label)
        oss << "l";
    else
        oss << ".";
    if(this->is_directive)
        oss << "d";
    else
        oss << ".";
    if(this->is_imm)
        oss << "i";
    else
        oss << ".";
    oss << "] ";
    oss << std::right << "0x" << std::hex << std::setw(4) << std::setfill('0') << this->addr << " ";
    oss << std::left << std::setw(12) << std::setfill(' ') << this->opcode.mnemonic;
    oss << "0x" << std::right << std::hex << std::setw(4) << std::setfill('0') << this->opcode.instr << "  ";

    // Insert arg/register chars
    for(auto i = 0; i < 3; ++i)
    {
        if(this->types[i] == SYM_REG_TEMP)
            oss << "t" << i << " ";
        else if(this->types[i] == SYM_REG_SAVED)
            oss << "s" << i << " ";
        else if(this->types[i] == SYM_REG_ARG)
            oss << "a" << i << " ";
        else if(this->types[i] == SYM_REG_RET)
            oss << "r" << i << " ";
        else if(this->types[i] == SYM_ZERO_REG)
            oss << "Z  ";
        else if(this->types[i] == SYM_REG_NUM)
            oss << "$" << i << " ";
        else if(this->types[i] == SYM_REG_GLOBAL)
            oss << "$gp";       // TODO: add offset
        else if(this->types[i] == SYM_REG_FUNC)
            oss << "$fp";       // TODO: add offset
        else if(this->types[i] == SYM_LITERAL)
            oss << std::left << std::setfill(' ') << std::setw(3) << this->args[i];
        else
            oss << "   ";
    }
    oss << "   "; 
    // Insert flag chars
    oss << "...";
    // spacing chars
    oss << " ";
    // insert error (T/F only, full string may not fit)
    if(this->error)
        oss << " YES ";
    else
        oss << " NO  ";
    // (Next line) Text 
    oss << std::endl;
    oss << "Label [" << std::left << std::setw(16) << std::setfill(' ') << this->label << "] ";
    oss << "Symbol[" << std::left << std::setw(16) << std::setfill(' ') << this->symbol << "] ";

    oss << std::endl;
    
    return oss.str();
}

bool LineInfo::operator==(const LineInfo& that) const
{
    if(this->label != that.label)
        return false;
    if(this->symbol != that.symbol)
        return false;
    if(this->errstr != that.errstr)
        return false;
    if(this->line_num != that.line_num)
        return false;
    if(this->addr != that.addr)
        return false;
    if(this->error != that.error)
        return false;
    if(this->is_label != that.is_label)
        return false;
    if(this->is_symbol != that.is_symbol)
        return false;
    if(this->is_imm != that.is_imm)
        return false;
    if(this->opcode != that.opcode)
        return false;

    for(int i = 0; i < 3; ++i)
    {
        if(this->args[i] != that.args[i])
            return false;
    }

    return true;
}

bool LineInfo::operator!=(const LineInfo& that) const
{
    return !(*this == that);
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

bool Symbol::operator==(const Symbol& that) const
{
    if(this->addr != that.addr)
        return false;
    if(this->label != that.label)
        return false;
    return true;
}

bool Symbol::operator!=(const Symbol& that) const
{
    if(this->addr == that.addr)
        return false;
    if(this->label == that.label)
        return false;
    return true;
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

std::string SourceInfo::toString(void) const
{
    std::ostringstream oss;

    for(unsigned int l = 0; l < this->line_info.size(); ++l)
        oss << this->line_info[l].toString();

    return oss.str();
}
