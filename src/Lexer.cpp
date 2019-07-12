/*
 * LEXER
 * MIPS Assembly language Lexer 
 *
 * Stefan Wong 2019
 */

#include <cctype>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "Lexer.hpp"
#include "Codes.hpp"
#include "Data.hpp"     // MemLine is here at the moment...


Lexer::Lexer()
{
    this->token_buf_size = 512;
    this->verbose        = false;
    this->cur_char       = '\0';
    this->cur_line       = 0;
    this->cur_pos        = 0;
    this->cur_addr       = 0;
    // create token buffer
    this->alloc_mem();
    this->init_instr_table();
    this->init_directive_code_table();
}

Lexer::~Lexer()
{
    delete[] this->token_buf;
}

/*
 * init_instr_table()
 */
void Lexer::init_instr_table(void)
{
    for(const Opcode& code : lex_instr_codes)
        this->instr_code_table.add(code);
}

/*
 * init_directive_code_table()
 */
void Lexer::init_directive_code_table(void)
{
    for(const Opcode& code : lex_directive_codes)
        this->directive_code_table.add(code);
}

/*
 * alloc_mem()
 * Allocate token buffer memory
 */
void Lexer::alloc_mem(void)
{
    this->token_buf = new char[this->token_buf_size];
}


// ======== MOTION THROUGH SOURCE ======== //
/*
 * advance()
 * Advance the current position pointer, checking for end of sequence
 * and updating the current line pointer.
 */
void Lexer::advance(void)
{
    this->cur_pos++;
    // dont let the cur_pos go over the end of the source text array
    if(this->cur_pos >= this->source_text.size())
        this->cur_char = '\0';
    else
        this->cur_char = this->source_text[this->cur_pos];

    if(this->cur_char == '\n')
        this->cur_line = this->cur_line + 1;
}

/*
 * exhausted()
 * Returns true if there are no more characters in the source file
 */
bool Lexer::exhausted(void) const
{
    return (this->cur_char == '\0' ||
            this->cur_pos >= this->source_text.size()) ? true : false;
}

/*
 * skipWhitespace()
 */
void Lexer::skipWhitespace(void) 
{
    while(!this->exhausted())
    {
        if(this->cur_char == ' '  || 
           this->cur_char == '\n' || 
           this->cur_char == '\t')
            this->advance();
        else
            break;
    }
}

/*
 * skipComment()
 */
void Lexer::skipComment(void)
{
    while(this->cur_char != '\n')
        this->advance();
}

/*
 * skipSeperators()
 */
void Lexer::skipSeperators(void)
{
    while(!this->exhausted())
    {
        if(this->cur_char == ',' ||
           this->cur_char == ':' ||
           this->cur_char == ';')
            this->advance();
        else
            break;
    }
}

// ======== CHARACTER TYPE ======== //
/*
 * isSpace()
 */
bool Lexer::isSpace(void) const
{
    return (this->cur_char == ' '  || 
            this->cur_char == '\t' ||
            this->cur_char == '\n') ? true : false;
}
bool Lexer::isComment(void) const
{
    return (this->cur_char == ';') || (this->cur_char == '#');
}


/*
 * getRegType()
 */
// TODO : what to do about $ra, $sp, etc?
TokenType Lexer::getRegType(const char& reg_char) const
{
    switch(reg_char)
    {
        case 'A':
            return SYM_REG_ARG;
        case 'F':
            return SYM_REG_FRAME;
        case 'R':
            return SYM_REG_RET;
        case 'S':
            return SYM_REG_SAVED;
        case 'T':
            return SYM_REG_TEMP;
        case 'Z':
            return SYM_REG_ZERO;
        case 'G':
            return SYM_REG_GLOBAL;
        case 'K':
            return SYM_REG_KERN;
        default:
            return SYM_NONE;
    }

    // in case we somehow fall through
    return SYM_NONE;
}


/*
 * scanToken()
 * Scan a complete token into the token buffer
 */
void Lexer::scanToken(void)
{
    int idx = 0;

    this->skipWhitespace();     // eat any leading whitespace 
    this->skipSeperators();     // eat any seperators that might be left
    while(idx < (this->token_buf_size-1))
    {
        if(this->cur_char == ' ')       // end 
            break;
        if(this->cur_char == '\n')      // newline
            break;
        if(this->cur_char == ';')       // comment
            break;
        if(this->cur_char == '#')       // also comment
            break;
        if(this->cur_char == ',')       // seperator
            break;
        if(this->cur_char == ':')       // end of label
            break;
        this->token_buf[idx] = toupper(this->cur_char);
        this->advance();
        idx++;
    }
    this->token_buf[idx] = '\0';
    // If we are on a seperator now, advance the source pointer 
    if(this->cur_char == ',')
        this->advance();
}

/*
 * nextToken()
 * Get the next token in the stream
 */
void Lexer::nextToken(void)
{
    Opcode op;
    std::string token_str;

    this->scanToken();
    token_str = std::string(this->token_buf);
    op = this->instr_code_table.get(token_str);
    // set the offset string for the current token back to null
    this->cur_token.offset = "\0";

    // this is a directive token
    if(token_str[0] == '.')
    {
        this->cur_token.type = SYM_DIRECTIVE;
        this->cur_token.val  = token_str;

        goto TOKEN_END;
    }

    // This is a simple register token
    if(token_str[0] == '$')
    {
        this->cur_token.type = this->getRegType(token_str[1]);
        if(this->cur_token.type == SYM_NONE)
        {
            this->text_info.error = true;
            this->text_info.errstr = "Invalid register type " +
                token_str[1];
        }
        else
            this->cur_token.val  = token_str.substr(2, token_str.length()-1);

        goto TOKEN_END;
    }

    // This is either an immediate or a register with an offset
    if(std::isdigit(token_str[0]))
    {
        unsigned int tok_ptr = 0;
        while(std::isdigit(token_str[tok_ptr]))
            tok_ptr++;

        // if there are more characters, check whether or not this is a 
        // register with offsets 
        if(token_str.size() > tok_ptr)
        {
            // NOTE: this fall-through structure is a bit hard to read
            if((token_str[tok_ptr] == '(') && (token_str[tok_ptr+1] == '$'))
            {
                this->cur_token.type = this->getRegType(token_str[tok_ptr+2]);

                if(this->cur_token.type == SYM_NONE)
                {
                    this->text_info.error = true;
                    this->text_info.errstr = "Invalid offset syntax " + 
                        this->cur_token.toString();
                    goto TOKEN_END;
                }

                this->cur_token.val    = token_str.substr(tok_ptr+3, token_str.length()-2);
                this->cur_token.offset = token_str.substr(0, tok_ptr);
            }
            else
            {
                this->text_info.error = true;
                this->text_info.errstr = "Syntax error (" + token_str + ")";
            }
        }
        else
        {
            this->cur_token.val = token_str.substr(0, tok_ptr);
            this->cur_token.type = SYM_LITERAL;
        }

        goto TOKEN_END;
    }

    // Found an instruction
    if(op.mnemonic != "\0")
    {
        this->cur_token.type = SYM_INSTR;
        this->cur_token.val  = std::string(this->token_buf);
    }
    // Exhausted all options - assign as label 
    else
    {
        this->cur_token.type = SYM_LABEL;
        this->cur_token.val  = token_str;
    }

TOKEN_END:
    if(this->verbose)
    {
        std::cout << "[" << __func__ << "] (line " << std::dec << 
            this->cur_line << ") got " << this->cur_token.toString() << 
            " token <" << token_str << "> with value <" << 
            this->cur_token.val << ">" << std::endl;

        if(this->text_info.error)
            std::cout << "[" << __func__ << "] " << this->text_info.errstr << std::endl;
    }
}

// DIRECTIVES 
void Lexer::parseASCIIZ(void)
{

}

// insert a data segment
void Lexer::parseData(void)
{
    this->data_info.init();
    this->data_info.line_num = this->cur_line;
    this->data_info.addr     = this->cur_addr;
}

void Lexer::parseByte(void)
{

}


void Lexer::parseWord(void)
{
    unsigned int word_line = this->cur_line;

    while(this->cur_line < word_line+1)
    {
        this->nextToken();
        if(this->cur_token.type == SYM_LITERAL)
            std::cout << "Need to add to array of literals.." << std::endl;

    }

}

void Lexer::parseSpace(void)
{

}




// INSTRUCTIONS 

/*
 * parseBranchZero()
 * Parse branch instructions where the comparison is done 
 * against zero.
 */
void Lexer::parseBranchZero(void)
{
    bool error = false;

    this->nextToken();
    if(!this->cur_token.isReg())
    {
        error = true;
        goto BRANCH_END;
    }

    this->text_info.type[0]   = this->cur_token.type;
    // if we have an offset, convert it here 
    if(this->cur_token.offset != "\0")
    {
        this->text_info.val[1] = std::stoi(this->cur_token.offset, nullptr, 10);
        this->text_info.type[1] = SYM_LITERAL;
    }

    switch(this->cur_token.type)
    {
        case SYM_REG_ZERO:
        case SYM_REG_GLOBAL:
        case SYM_REG_FRAME:
            this->text_info.val[0] = 0;
            break;

        default:
            this->text_info.val[0] = std::stoi(this->cur_token.val, nullptr, 10);
            break;
    }

    // Next token must be a symbol
    this->nextToken();
    if(this->cur_token.type != SYM_LABEL)
    {
        error = true;
        goto BRANCH_END;
    }
    this->text_info.is_symbol = true;
    this->text_info.symbol    = this->cur_token.val;

BRANCH_END:
    if(error)
    {
        this->text_info.error = true;
        this->text_info.errstr = "Invalid argument " + 
            this->cur_token.toString() + " for instruction " + 
            this->text_info.opcode.toString();

        if(this->verbose)
        {
            std::cout << "[" << __func__ << "] " << 
                this->text_info.errstr << std::endl;
        }
    }
}

/*
 * parseBranch()
 * Parse a branch instruction
 */
void Lexer::parseBranch(void)
{
    int argn;
    bool error = false;

    for(argn = 0; argn < 2; ++argn)
    {
        this->nextToken();
        if(!this->cur_token.isReg())
        {
            error = true;
            goto BRANCH_END;
        }
        this->text_info.type[argn] = this->cur_token.type;

        switch(this->cur_token.type)
        {
            case SYM_REG_ZERO:
            case SYM_REG_GLOBAL:
            case SYM_REG_FRAME:
                this->text_info.val[argn] = 0;
                break;

            default:
                this->text_info.val[argn] = std::stoi(this->cur_token.val, nullptr, 10);
                break;
        }
    }

    // Finally, there should be one label token at the end
    this->nextToken();
    if(this->cur_token.type != SYM_LABEL)
    {
        error = true;
        goto BRANCH_END;
    }
    this->text_info.is_symbol = true;
    this->text_info.symbol    = this->cur_token.val;

BRANCH_END:
    if(error)
    {
        this->text_info.error = true;
        this->text_info.errstr = "Invalid argument " + 
            this->cur_token.toString() + " for instruction " + 
            this->text_info.opcode.toString();

        if(this->verbose)
        {
            std::cout << "[" << __func__ << "] " << 
                this->text_info.errstr << std::endl;
        }
    }
}


/*
 * parseMemArgs()
 */
void Lexer::parseMemArgs(void)
{
    bool error = false;

    this->nextToken();
    if(!this->cur_token.isReg())
    {
        error = true;
        goto ARG_END;
    }
    this->text_info.type[0] = this->cur_token.type;
    this->text_info.val[0]  = std::stoi(this->cur_token.val, nullptr, 10);

    // this should be a register that may or may not
    // have an offset 
    this->nextToken();
    this->text_info.type[1] = this->cur_token.type;

    switch(this->cur_token.type)
    {
        case SYM_REG_ZERO:
        case SYM_REG_GLOBAL:
        case SYM_REG_FRAME:
            this->cur_token.val[1] = 0;
            break;

        default:
            this->cur_token.val[1] = std::stoi(this->cur_token.val, nullptr, 10);
            break;
    }

    if(this->cur_token.offset != "\0")
    {
        this->text_info.val[2]  = std::stoi(this->cur_token.offset, nullptr, 10);
        this->text_info.type[2] = SYM_LITERAL;
    }


ARG_END:
    if(error)
    {
        this->text_info.error = true;
        this->text_info.errstr = "Invalid argument " + 
            this->cur_token.toString();

        if(this->verbose)
        {
            std::cout << "[" << __func__ << "] " << 
                this->text_info.errstr << std::endl;
        }
    }
}


/*
 * parseRegArgs()
 * Parse some number of register arguments, eg for arithmetic
 * and logic instructions
 */
void Lexer::parseRegArgs(const int num_args)
{
    int argn = 0;
    bool error = false;

    for(argn = 0; argn < num_args; ++argn)
    {
        this->nextToken();
        // Offsets can only occur in certain instructions which have immediates.
        // The rule needs to be that 
        // 1) if the instruction is an immediate instruction
        // 2) AND there is an offset string in the current Token 
        // 3) THEN we convert that offset string to an int and store it in 
        // this->text_info.val[2] as a SYM_LITERAL
        if(this->text_info.is_imm && argn == num_args-1)
        {
            // Check if there is an offset 
            if(this->cur_token.offset != "\0")
                this->text_info.val[argn] = std::stoi(this->cur_token.offset, nullptr, 10);
            else
                this->text_info.val[argn] = std::stoi(this->cur_token.val, nullptr, 10);
            this->text_info.type[argn] = SYM_LITERAL;
        }
        else
        {
            if(!this->cur_token.isReg())
            {
                error = true;
                goto ARG_ERR;
            }
        }
        this->text_info.type[argn] = this->cur_token.type;

        switch(this->cur_token.type)
        {
            case SYM_REG_ZERO:
            case SYM_REG_GLOBAL:
            case SYM_REG_FRAME:
                if(this->cur_token.offset != "\0")
                    this->text_info.val[argn] = std::stoi(this->cur_token.offset, nullptr, 10);
                else
                    this->text_info.val[argn] = 0;
                break;

            default:
                this->text_info.val[argn] = std::stoi(this->cur_token.val, nullptr, 10);
                break;
        }
    }

ARG_ERR:
    if(error)
    {
        this->text_info.error = true;
        this->text_info.errstr = "Invalid argument " + std::to_string(argn+1) + 
            " to instruction " + this->text_info.opcode.toString();
        if(this->verbose)
        {
            std::cout << "[" << __func__ << "] (line " << 
                this->cur_line << ") " << this->text_info.errstr << std::endl;
        }
    }
}

/*
 * parseJump()
 * Jump instructions only have labels
 * TODO: update to allow for an immediate to be used 
 * as well as a symbol?  This is presumably legal, but 
 * probably mostly useless except in very small programs
 */
void Lexer::parseJump(void)
{
    bool error = false;

    this->nextToken();
    if(this->cur_token.type != SYM_LABEL)
    {
        error = true;
        goto JUMP_END;
    }

    this->text_info.is_symbol = true;
    this->text_info.symbol    = this->cur_token.val;

JUMP_END:
    if(error)
    {
        this->text_info.error = true;
        this->text_info.errstr = "Invalid argument " + 
            this->cur_token.toString();

        if(this->verbose)
        {
            std::cout << "[" << __func__ << "] " << 
                this->text_info.errstr << std::endl;
        }
    }
}


/*
 * parseLine()
 */
void Lexer::parseLine(void)
{
    Opcode op;
    Opcode directive;
    Symbol sym;
    unsigned int line_num = 0;

    this->text_info.init();
    line_num = this->cur_line;
    this->nextToken();          

    // if there is a label on this line, add it to the symbol table
    if(this->cur_token.type == SYM_LABEL)
    {
        this->text_info.is_label = true;
        // add symbol to table, remove any trailing characters
        char last_char = this->cur_token.val[this->cur_token.val.length() - 1];
        if((last_char == ';') || (last_char == '#'))
            sym.label = this->cur_token.val.substr(0, this->cur_token.val.length() - 1);
        else
            sym.label = this->cur_token.val;
        sym.addr = this->cur_addr;
        // add to symbol table 
        this->sym_table.add(sym);
        this->text_info.label = sym.label;
        this->skipComment();
        // scan in the next token
        this->nextToken(); 
        line_num = this->cur_line;
    }

    if(this->cur_token.type == SYM_DIRECTIVE)
    {
        // Look up directive string in directive table
        directive = this->directive_code_table.get(this->cur_token.val);
        if(this->verbose)
        {
            std::cout << "[" << __func__ << "] (line " << 
                std::dec << this->cur_line << "0 got directive " <<
                directive.toString() << std::endl;
        }

        // Most of the stuff that deals with the data section goes 
        // in here.
        switch(directive.instr)
        {
            case LEX_ALIGN:
                break;

            case LEX_ASCIIZ:
                break;

            // Global variable segment 
            case LEX_DATA:
                break;

            // Text segment
            case LEX_TEXT:
                break;

            // data types 
            case LEX_WORD:
                break;

            case LEX_HALF:
                break;

            case LEX_BYTE:
                break;

            default:
                this->text_info.error = true;
                this->text_info.errstr = "Unknown assembly directive " +
                    this->cur_token.val;

                if(this->verbose)
                {
                    std::cout << "[" << __func__ << "] (line " << 
                        this->cur_line << ") " << 
                        this->text_info.errstr << std::endl;
                }
                break;
        }
    }

    // parse instructions 
    if(this->cur_token.type == SYM_INSTR)
    {
        op = this->instr_code_table.get(this->cur_token.val);
        if(this->verbose)
        {
            std::cout << "[" << __func__ << "] (line " << std::dec << 
                this->cur_line << ") found instr token " << 
                this->cur_token.val << " (opcode x" << std::hex << 
                op.instr << ", mnemonic " << op.mnemonic << ")" << 
                std::endl;
        }

        this->text_info.opcode = op;

        switch(op.instr)
        {
            case LEX_ADD:
            case LEX_ADDU:
            case LEX_AND:
                this->parseRegArgs(3);
                break;

            case LEX_ADDI:
            case LEX_ADDIU:
            case LEX_ANDI:
                this->text_info.is_imm = true;
                this->parseRegArgs(3);
                break;

            case LEX_BEQ:
            case LEX_BNE:
                this->parseBranch();
                break;

            case LEX_BGTZ:
            case LEX_BLEZ:
                this->parseBranchZero();
                break;

            case LEX_J:
            case LEX_JAL:
            case LEX_JALR:
                this->parseJump();
                break;

            case LEX_JR:
                this->parseRegArgs(1);
                break;

            case LEX_LW:
                this->parseMemArgs();
                break;

            case LEX_MULT:
                this->parseRegArgs(3);
                break;

            case LEX_OR:
                this->parseRegArgs(2);
                break;

            case LEX_ORI:
                this->text_info.is_imm = true;
                this->parseRegArgs(3);
                break;

            case LEX_SLL:
            case LEX_SRL:
                this->text_info.is_imm = true;
                this->parseRegArgs(3);
                break;

            case LEX_SLTU:
                this->parseRegArgs(3);
                break;

            case LEX_SUB:
            case LEX_SUBU:
                this->parseRegArgs(3);
                break;

            case LEX_SW:
                this->parseMemArgs();
                break;

            default:
                this->text_info.error = true;
                this->text_info.errstr = "Invalid instruction " + this->cur_token.val;
                if(this->verbose)
                {
                    std::cout << "[" << __func__ << "] (line " << 
                        std::dec << this->cur_line << ") " << 
                        this->text_info.errstr << std::endl;
                }
                goto LINE_END;
        }
    }

LINE_END:
    this->text_info.line_num = line_num;
    this->text_info.addr     = this->cur_addr;
    this->cur_addr++;
}

/*
 * resolveLabels()
 */
void Lexer::resolveLabels(void)
{
    unsigned int idx;
    uint32_t label_addr;
    TextInfo line;

    // In verbose mode, dump entire symbol table
    if(this->verbose)
    {
        std::cout << "[" << __func__ << "] " << this->sym_table.size() 
            << " symbols in table" << std::endl;
        for(unsigned int s = 0; s < this->sym_table.size(); ++s)
        {
            Symbol sym = this->sym_table.get(s);
            std::cout << "    " << std::setw(4) << s+1 << 
                " :" << sym.toString() << std::endl;
        }
    }

    for(idx = 0; idx < this->source_info.getNumLines(); ++idx)
    {
        line = this->source_info.get(idx);
        if(line.is_symbol)
        {
            if(this->verbose)
            {
                std::cout << "[" << __func__ << "] checking address for symbol <" <<
                    line.symbol << ">" << std::endl;
            }
            label_addr = this->sym_table.getAddr(line.symbol);
            if(label_addr > 0)
            {
                line.type[2] = SYM_LITERAL;
                line.val[2] = label_addr;
                this->source_info.update(idx, line);

                if(this->verbose)
                {
                    std::cout << "[" << __func__ << "] updated symbol " <<
                        line.symbol << " on line " << line.line_num << 
                        " with address 0x" << std::hex << std::setw(8) << 
                        line.addr << std::endl;
                }
            }
        }
    }
}

/*
 * lex()
 * Lex the current source file
 */
void Lexer::lex(void)
{
    this->cur_line = 1;
    this->cur_pos = 0;
    this->cur_addr = 0x200;     // TODO : proper address init..

    while(!this->exhausted())
    {
        // eat spaces 
        if(this->isSpace())
        {
            this->advance();
            continue;
        }

        // eat comments 
        if(this->isComment())
        {
            this->skipComment();
            continue;
        }
        this->parseLine();
        this->source_info.addText(this->text_info);
    }
    // Resolve symbols
    this->resolveLabels();
}

/*
 * loadFile()
 * Load a text file from disk
 */
int Lexer::loadFile(const std::string& filename)
{
    std::ifstream infile(filename);
    std::string line;
    int status = 0;     // TODO: faliure checks, etc

    // save the filename
    this->filename = filename;
    while(std::getline(infile, line))
    {
        this->source_text += line;
        this->source_text.push_back('\n');
    }

    if(this->verbose)
    {
        std::cout << "[" << __func__ << "] read " << 
            std::dec << this->source_text.length() << " characters from file ["
            << filename << "]" << std::endl;
    }

    infile.close();
    this->source_text.push_back('\0');
    this->cur_char = this->source_text[0];

    return status;
}


// ==== GETTERS ===== //
bool Lexer::getVerbose(void) const
{
    return this->verbose;
}

const SourceInfo& Lexer::getSourceInfo(void) const
{
    return this->source_info;
}

const SymbolTable& Lexer::getSymTable(void) const
{
    return this->sym_table;
}

// ==== SETTERS ===== //
void Lexer::setVerbose(const bool v)
{
    this->verbose = v;
}
