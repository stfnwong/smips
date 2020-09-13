/*
 * SOURCE
 * Objects for processing assembly source 
 *
 * Stefan Wong 2019
 */

#include <iomanip>
#include <sstream>
#include "Source.hpp"
#include "Register.hpp"

/*
 * TODO: 
 * To match both the ELF format, and the layout of many example programs it might be 
 * better to restructure this to work with 'segments' rather than just raw symbols.
 *
 * The idea I suppose would be to have the .data and .text directives insert a new data
 * or text segment, and that segment would contain the IR for the actual assembly 
 * inside of it.
 *
 * TODO  for TODO : go back to ELF file specification and think some more about
 * how that should be reperesented. Eg - should the parser hold different section 'types' 
 * for the sections in the ELF file?
 *
 */



/* 
 * TOKEN
 */
Token::Token() : type(SYM_NONE), val("\0"), reg_offset("\0") {} 

Token::Token(const TokenType& t, const std::string& v) : type(t), val(v), reg_offset("\0") {} 

/*
 * Token::init()
 */
void Token::init(void)
{
    this->type       = SYM_NONE;
    this->val        = "\0";
    this->reg_offset = "\0";
}

/*
 * Token::isReg()
 */
bool Token::isReg(void) const
{
    return (this->type == SYM_REGISTER) ? true : false;
}

/*
 * Token::isOffset()
 */
bool Token::isOffset(void) const
{
    return (this->reg_offset != "\0") ? true : false;
}

/*
 * Token::toString()
 */
std::string Token::toString(void) const
{
    switch(this->type)
    {
        case SYM_NONE:
            return "NONE <" + this->val + ">";
        case SYM_EOF:
            return "EOF <" + this->val + ">";
        case SYM_LABEL:
            return "LABEL <" + this->val + ">";
        case SYM_INSTR:
            return "INSTR <" + this->val + ">";
        case SYM_LITERAL:
            return "LITERAL <" + this->val + ">";
        case SYM_DIRECTIVE:
            return "DIRECTIVE <" + this->val + ">";
        case SYM_CHAR:
            return "CHAR <" + this->val + ">";
        case SYM_STRING:
            return "STRING <" + this->val + ">";
        case SYM_SYSCALL:
            return "SYSCALL";
        // Registers
        case SYM_REGISTER:
            {
                if(this->reg_offset != "\0")
                    return "REGISTER <" + this->val + ">[" + this->reg_offset + "]";
                return "REGISTER <" + this->val + ">";
            }
        default:
            return "NULL <" + this->val + ">";
    }
}


/*
 * Token (==)
 */
bool Token::operator==(const Token& that) const
{
    if(this->type != that.type)
        return false;
    if(this->val != that.val)
        return false;
    if(this->reg_offset != that.reg_offset)
        return false;

    return true;
}

/*
 * Token (!=)
 */
bool Token::operator!=(const Token& that) const
{
    if(this->type == that.type)
        return false;
    if(this->val == that.val)
        return false;
    if(this->reg_offset != that.reg_offset)
        return false;

    return true;
}


/*
 * TextInfo
 */
TextInfo::TextInfo()
{
    this->init();
}

/*
 * TextInfo::init()
 */
void TextInfo::init(void)
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
    this->upper        = false;
    this->lower        = false;
    this->psuedo       = false;
    this->opcode.init();

    for(int i = 0; i < 3; ++i)
    {
        this->val[i]    = 0;
        this->type[i]   = SYM_NONE;
    }
}

/*
 * TextInfo::hasOp()
 */
bool TextInfo::hasOp(void) const
{
    return (this->opcode.instr == 0) ? true : false;
}

/*
 * TextInfo::toString()
 */
std::string TextInfo::toString(void) const
{
    std::ostringstream oss;

    oss << "---------------------------------------------------------------------" << std::endl;
    oss << "Line  Type   Addr       Mnemonic   Opcode   Arguments   literal   error" << std::endl;

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
    if(this->psuedo)
        oss << "p";
    else
        oss << ".";
    oss << "] ";
    oss << std::right << "0x" << std::hex << std::setw(8) << std::setfill('0') << this->addr << " ";
    oss << std::left << std::setw(11) << std::setfill(' ') << this->opcode.mnemonic;
    oss << "0x" << std::right << std::hex << std::setw(4) << std::setfill('0') << this->opcode.instr << "  ";

    // Insert arg/register chars
    for(auto i = 0; i < 3; ++i)
    {
        // TODO : come back to this... temp registers are in two places in register map
        //if(this->type[i] == SYM_REG_TEMP)
        //    oss << "t" << this->val[i] << " ";
        if(this->type[i] == SYM_REGISTER)
        {
            if(this->val[i] == REG_AT)
                oss << "at" << " ";     
            else if(this->val[i] >= REG_ARG_0 && this->val[i] <= REG_ARG_3)
                oss << "a" << std::dec << this->val[i] << " ";
            else if(this->val[i] == REG_RETURN)
                oss << "r" << std::dec << this->val[i] << " ";
            else if(this->val[i] == REG_RETURN)
                oss << "RA ";
            else if(this->val[i] == REG_ZERO)
                oss << "Z ";
            else if(this->val[i] == REG_GLOBAL)
                oss << "G  ";
            else if(this->val[i] == REG_FRAME)
                oss << "F+" << std::dec << this->val[i];
            else if(this->val[i] == REG_STACK)
                oss << "S+" << std::dec << this->val[i];
            else if(this->val[i] >= REG_SAVED_0 && this->val[i] <= REG_SAVED_7)
                oss << "s" << std::dec << this->val[i] << " ";
        }
        else if(this->type[i] == SYM_LITERAL)       // TODO: SYM_OFFSET? Would work the same as literal except for string formatting
            oss << "L  ";
        else
            oss << "   ";
    }
    oss << "  "; 
    // TODO : maybe add the 'symbol' first, then print the correct value
    // literal (if applicable)
    if(this->is_symbol)
        oss << "0x" << std::hex << std::setw(8) << this->val[2];
    //else if(this->is_imm)
    //    oss << "0x" << std::hex << std::setw(8) << this->val[2];
    else if(!this->is_imm && (this->type[1] == SYM_LITERAL))
        oss << " +" << std::left << std::hex << std::setw(8) << std::setfill(' ') << this->val[1] << "  ";
    else if(!this->is_imm && (this->type[2] == SYM_LITERAL))
        oss << " +" << std::left << std::dec << std::setw(8) << std::setfill(' ') << this->val[2] << "  ";
    else if(this->is_imm && this->upper)
        oss << "U 0x" << std::left << std::hex << std::setw(8) << std::setfill(' ') << this->val[1] << "  ";
    else if(this->is_imm && this->lower)
        oss << "L 0x" << std::left << std::hex << std::setw(8) << std::setfill(' ') << this->val[1] << "  ";
    else if(this->is_imm && (this->type[1] == SYM_LITERAL))
        oss << "0x" << std::hex << std::setw(8) << this->val[1];
    else if(this->is_imm && (this->type[2] == SYM_LITERAL))
        oss << "0x" << std::hex << std::setw(8) << this->val[2];
    else
        oss << "          ";
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

/*
 * TextInfo::==()
 */
bool TextInfo::operator==(const TextInfo& that) const
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
    if(this->upper != that.upper)
        return false;
    if(this->lower != that.lower)
        return false;
    if(this->psuedo != that.psuedo)
        return false;
    if(this->opcode != that.opcode)
        return false;

    for(int i = 0; i < 3; ++i)
    {
        if(this->val[i] != that.val[i])
            return false;
    }

    return true;
}

/*
 * TextInfo::!=()
 */
bool TextInfo::operator!=(const TextInfo& that) const
{
    return !(*this == that);
}

/*
 * TextInfo::diff()
 */
std::string TextInfo::diff(const TextInfo& that) const
{
    std::ostringstream oss;
    int num_err = 0;

    if(this->label != that.label)
    {
        oss << "label [" << this->label << "] does not match ["  <<
            that.label << "]" << std::endl;
        num_err += 1;
    }
    if(this->symbol != that.symbol)
    {
        oss << "symbol [" << this->symbol << "] does not match [" <<
            that.label << "]" << std::endl;
        num_err += 1;
    }
    if(this->errstr != that.errstr)
    {
        oss << "errstr does not match" << std::endl;
        num_err += 1;
    }
    if(this->line_num != that.line_num)
    {
        oss << "line num [" << this->line_num << "] does not match [" <<
            that.line_num << "]" << std::endl;
        num_err += 1;
    }
    if(this->addr != that.addr)
    {
        oss << "addr [" << this->addr << "] does not match [" <<
            that.addr << "]" << std::endl;
        num_err += 1;
    }
    if(this->error != that.error)
    {
        oss << "error does not match" << std::endl;
        num_err += 1;
    }
    if(this->is_label != that.is_label)
    {
        oss << "is_label does not match" << std::endl;
        num_err += 1;
    }
    if(this->is_symbol != that.is_symbol)
    {
        oss << "is_symbol does not match" << std::endl;
        num_err += 1;
    }
    if(this->is_imm != that.is_imm)
    {
        oss << "is_imm does not match" << std::endl;
        num_err += 1;
    }
    if(this->upper != that.upper)
    {
        oss << "upper does not match" << std::endl;
        num_err += 1;
    }
    if(this->lower != that.lower)
    {
        oss << "lower does not match" << std::endl;
        num_err += 1;
    }
    //if(this->psuedo != that.psuedo)
    //{
    //    oss << "psuedo does not match" << std::endl;
    //    num_err += 1;
    //}
    if(this->opcode != that.opcode)
    {
        oss << "opcode [" << this->opcode.toString() << 
            "] does not match [" << that.opcode.toString() << 
            "]" << std::endl;
        num_err += 1;
    }

    // create one error for each mismatched argument
    for(int i = 0; i < 3; ++i)
    {
        if(this->val[i] != that.val[i])
        {
            oss << "val " << i << " [" << this->val[i] << 
                "] does not match [" << that.val[i] << 
                "]" << std::endl;
            num_err += 1;
        }
    }
    oss << num_err << " differences total" << std::endl;

    return oss.str();
}

/*
 * DataInfo
 */
DataInfo::DataInfo()
{
    this->init();
}

void DataInfo::init(void)
{
    this->errstr       = "\0";
    this->directive    = SYM_DIR_NONE;
    this->label        = "\0";
    this->line_num     = 0;
    this->addr         = 0;
    this->space        = 0;
    this->error        = false;
    this->is_label     = false;
    this->data.clear();
}

/*
 * DataInfo::dirTypeString()
 */
std::string DataInfo::dirTypeString(void) const
{
    switch(this->directive)
    {
        case SYM_DIR_NONE:
            return "NONE";
        case SYM_DIR_ALIGN:
            return ".align";
        case SYM_DIR_ASCIIZ:
            return ".asciiz";
        case SYM_DIR_BYTE:
            return ".byte";
        case SYM_DIR_CHAR:
            return ".char";
        case SYM_DIR_GLOBL:
            return ".globl";
        case SYM_DIR_HALF:
            return ".half";
        case SYM_DIR_MACRO:
            return ".macro";
        case SYM_DIR_END_MACRO:
            return ".end_macro";
        case SYM_DIR_SPACE:
            return ".space";
        case SYM_DIR_WORD:
            return ".word";
        default:
            return "NONE";
    }
}

/*
 * DataInfo::toString()
 * Convert DataInfo to a std::string
 */
std::string DataInfo::toString(void) const
{
    std::ostringstream oss;

    // TODO : how to print data segment (in cases where its more than 5 bytes)?
    oss << "---------------------------------------------------------------------" << std::endl;
    // extend to the right as needed
    oss << "Line  Type   Addr      Directive  Label    Len  Error  Data                    " << std::endl;

    oss << std::left << std::setw(6) << std::setfill(' ') << this->line_num;
    oss << "[";
    if(this->is_label)
        oss << "l";
    else
        oss << ".";
    if(this->space > 0)
        oss << "s";
    else
        oss << ".";

    oss << "] ";
    oss << std::right << "0x" << std::hex << std::setw(8) << std::setfill('0') << this->addr << " ";
    oss << "   ";
    // directive
    if(this->directive != SYM_DIR_NONE)
        oss << std::left << std::setw(8) << std::setfill(' ') << this->dirTypeString();
    else
        oss << std::left << std::setw(7) << std::setfill(' ') << " ";
    oss << " ";
    // label
    if(this->label != "\0")
        oss << std::left << std::setw(10) << std::setfill(' ') << this->label;
    else
        oss << std::left << std::setw(9) << std::setfill(' ') << " ";
    // len
    oss << std::dec << std::setw(4) << this->data.size();

    // error 
    if(this->error)
        oss << "  YES  ";
    else
        oss << "  NO   ";
    if(this->space > 0)
    {
        oss << std::dec << std::setw(4) << this->space 
            << " bytes";
    }
    else
    {
        for(unsigned int i = 0; i < this->data.size(); ++i)
        {
            oss << "$" << std::right << std::hex  << std::setfill('0') 
                << unsigned(this->data[i]) << ",";
        }
    }
    oss << std::endl;
    
    return oss.str();
}

/*
 * DataInfo::==
 */
bool DataInfo::operator==(const DataInfo& that) const
{
    if(this->line_num != that.line_num)
        return false;
    if(this->addr != that.addr)
        return false;
    if(this->space != that.space)
        return false;
    if(this->error != that.error)
        return false;
    if(this->label != that.label)
        return false;
    if(this->is_label != that.is_label)
        return false;

    if(this->data.size() != that.data.size())
        return false;
    
    if(this->data.size() > 0)
    {
        for(unsigned int i = 0; i < this->data.size(); ++i)
        {
            if(this->data[i] != that.data[i])
                return false;
        }
    }

    return true;
}

/*
 * DataInfo::!=
 */
bool DataInfo::operator!=(const DataInfo& that) const
{
    return !(*this == that);
}

// TODO: can probably default this..
/*
 * DataInfo::=
 */
DataInfo& DataInfo::operator=(const DataInfo& that)
{
    if(this != &that)
    {
        this->errstr = that.errstr;
        this->directive = that.directive;
        this->label = that.label;
        this->data = that.data;
        this->line_num = that.line_num;
        this->addr = that.addr;
        this->space = that.space;
        this->is_label = that.is_label;
        this->error = that.error;
    }

    return *this;
}


/*
 * DataInfo::addBytes()
 */
void DataInfo::addByte(const uint8_t byte)
{
    this->data.push_back(byte);
}

/*
 * DataInfo::addHalf()
 */
void DataInfo::addHalf(const uint16_t half)
{
    uint8_t byte;

    byte = half & 0x00FF;
    this->data.push_back(byte);
    byte = (half & 0xFF00) >> 8;
    this->data.push_back(byte);
}

/*
 * DataInfo::addWord()
 */
void DataInfo::addWord(const uint32_t word)
{
    uint8_t byte;

    byte = word & (0x000000FF);
    this->data.push_back(byte);
    byte = (word & 0x0000FF00) >> 8;
    this->data.push_back(byte);
    byte = (word & 0x00FF0000) >> 16;
    this->data.push_back(byte);
    byte = (word & 0xFF000000) >> 24;
    this->data.push_back(byte);
}

/*
 * DataInfo::size()
 */
unsigned int DataInfo::size(void) const
{
    return this->data.size();
}

/*
 * Symbol::Symbol
 */
Symbol::Symbol() : addr(0), label("\0"), section(0) {} 

/*
 * Symbol::==
 */
bool Symbol::operator==(const Symbol& that) const
{
    if(this->addr != that.addr)
        return false;
    if(this->label != that.label)
        return false;
    return true;
}

/*
 * Symbol::!=
 */
bool Symbol::operator!=(const Symbol& that) const
{
    return !(*this == that);
}

/*
 * Symbol::init()
 */
void Symbol::init(void)
{
    this->addr = 0;
    this->label = "\0";
    this->section = 0;
}

/*
 * Symbol::toString()
 */
std::string Symbol::toString(void) const
{
    std::ostringstream oss;
    oss << "[0x" << std::setw(8) << std::setfill('0') << 
        std::hex << this->addr << "] ";
    oss << this->label;

    return oss.str();
}

/*
 * SymbolTable
 */
SymbolTable::SymbolTable() {} 

/*
 * SymbolTable::add()
 */
void SymbolTable::add(const Symbol& s)
{
    this->syms.push_back(s);
}

/*
 * SymbolTable::update()
 */
void SymbolTable::update(const unsigned int idx, const Symbol& s)
{
    if(idx < this->syms.size())
        this->syms[idx] = s;
}

/*
 * SymbolTable::get()
 */
Symbol& SymbolTable::get(const unsigned int idx) 
{
    if(idx < this->syms.size())
        return this->syms[idx];

    return this->null_sym;
}

/*
 * SymbolTable::getAddr()
 */
uint32_t SymbolTable::getAddr(const std::string& label) const
{
    for(unsigned int idx = 0; idx < this->syms.size(); ++idx)
    {
        if(label == this->syms[idx].label)
            return this->syms[idx].addr;
    }

    return (uint32_t) 0;
}

/*
 * SymbolTable::size()
 */
unsigned int SymbolTable::size(void) const
{
    return this->syms.size();
}

/*
 * SymbolTable::init()
 */
void SymbolTable::init(void)
{
    this->syms.clear();
}


/*
 * SourceInfo
 */
SourceInfo::SourceInfo() {}

/*
 * SourceInfo::addText()
 */
void SourceInfo::addText(const TextInfo& l)
{
    this->text_info.push_back(l);
}

/*
 * SourceInfo::addData()
 */
void SourceInfo::addData(const DataInfo& d)
{
    this->data_info.push_back(d);
}

/*
 * SourceInfo::update()
 */
void SourceInfo::update(const unsigned int idx, const TextInfo& l)
{
    if(idx < this->text_info.size())
        this->text_info[idx] = l;
}

/*
 * SourceInfo::insert()
 */
void SourceInfo::insert(const unsigned int idx, const TextInfo& l)
{
    this->text_info.insert(this->text_info.begin() + idx, l);
}

/*
 * SourceInfo::getdata()
 */
DataInfo& SourceInfo::getData(const unsigned int idx)
{
    if(idx < this->data_info.size())
        return this->data_info[idx];

    return this->null_data;
}

/*
 * SourceInfo::getText()
 */
TextInfo& SourceInfo::getText(const unsigned int idx)
{
    if(idx < this->text_info.size())
        return this->text_info[idx];
    
    return this->null_line;
}

/*
 * SourceInfo::getLineNum()
 */
unsigned int SourceInfo::getLineNum(const unsigned int idx) const
{
    if(idx < this->text_info.size())
        return this->text_info[idx].line_num;

    return 0;
}

/*
 * SourceInfo::getNumErr()
 */
unsigned int SourceInfo::getNumErr(void) const
{
    unsigned int num_err = 0;
    for(unsigned int idx = 0; idx < this->text_info.size(); ++idx)
        num_err += (this->text_info[idx].error) ? 1 : 0;

    return num_err;
}

/*
 * SourceInfo::getNumLines()
 */
unsigned int SourceInfo::getNumLines(void) const
{
    return this->text_info.size();
}

/*
 * SourceInfo::hasError()
 */
bool SourceInfo::hasError(void) const
{
    for(unsigned int idx = 0; idx < this->text_info.size(); ++idx)
    {
        if(this->text_info[idx].error)
            return true;
    }

    return false;
}

/*
 * SourceInfo::getTextInfoSize()
 */
unsigned int SourceInfo::getTextInfoSize(void) const
{
    return this->text_info.size();
}

/*
 * SourceInfo::getDataInfoSize()
 */
unsigned int SourceInfo::getDataInfoSize(void) const
{
    return this->data_info.size();
}

/*
 * SourceInfo::getDataByAddr()
 */
uint8_t SourceInfo::getDataByAddr(const uint32_t addr)
{
    // TODO : for now we just search linearly through all addresses. 
    // If this turns out to be a bottleneck then we can make a dictionary
    // of pointers or something like that.
    for(unsigned int a = 0; a < this->data_info.size(); ++a)
    {
        if(this->data_info[a].addr == addr)
        {
            if(this->data_info[a].size() > 0)
                return this->data_info[a].data[0];
            return 0;
        }
    }

    return 0;
}


/*
 * SourceInfo::toString()
 */
std::string SourceInfo::toString(void) const
{
    std::ostringstream oss;
    for(unsigned int l = 0; l < this->data_info.size(); ++l)
        oss << this->data_info[l].toString();

    // Text infos
    for(unsigned int l = 0; l < this->text_info.size(); ++l)
        oss << this->text_info[l].toString();

    return oss.str();
}

/*
 * SourceInfo::errString()
 */
std::string SourceInfo::errString(void) const
{
    std::ostringstream oss;
    for(unsigned int l = 0; l < this->data_info.size(); ++l)
    {
        if(this->data_info[l].error)
        {
            oss << "[line " << this->data_info[l].line_num << "] ";
            oss << this->data_info[l].errstr;
            oss << std::endl;
        }
    }

    oss << std::endl;

    // Text infos
    for(unsigned int l = 0; l < this->text_info.size(); ++l)
    {
        if(this->text_info[l].error)
        {
            oss << "[line " << this->text_info[l].line_num << "] ";
            oss << this->text_info[l].errstr;
            oss << std::endl;
        }
    }

    return oss.str();
}
