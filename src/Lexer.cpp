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
}

Lexer::~Lexer()
{
    delete[] this->token_buf;
}

void Lexer::init_instr_table(void)
{
    for(const Opcode& code : lex_instr_codes)
        this->instr_code_table.add(code);
}

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

void Lexer::skipComment(void)
{
    while(this->cur_char != '\n')
        this->advance();
}

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
            this->line_info.error = true;
            this->line_info.errstr = "Invalid register type " +
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
                    this->line_info.error = true;
                    this->line_info.errstr = "Invalid offset syntax " + 
                        this->cur_token.toString();
                    goto TOKEN_END;
                }

                this->cur_token.val    = token_str.substr(tok_ptr+3, token_str.length()-2);
                this->cur_token.offset = token_str.substr(0, tok_ptr);
            }
            else
            {
                this->line_info.error = true;
                this->line_info.errstr = "Syntax error (" + token_str + ")";
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
        std::cout << "[" << __func__ << "] (line " << this->cur_line << 
            ") got " << this->cur_token.toString() << " token <" << 
            token_str << "> with value <" << this->cur_token.val <<
            ">" << std::endl;

        if(this->line_info.error)
            std::cout << "[" << __func__ << "] " << this->line_info.errstr << std::endl;
    }
}

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
    this->line_info.type[0]   = this->cur_token.type;
    // if we have an offset, convert it here 
    if(this->cur_token.offset != "\0")
        this->line_info.offset[0] = std::stoi(this->cur_token.offset, nullptr, 10);

    switch(this->cur_token.type)
    {
        case SYM_REG_ZERO:
        case SYM_REG_GLOBAL:
        case SYM_REG_FRAME:
            this->line_info.val[0] = 0;
            break;

        default:
            this->line_info.val[0] = std::stoi(this->cur_token.val, nullptr, 10);
            break;
    }

    // Next token must be a symbol
    this->nextToken();
    if(this->cur_token.type != SYM_LABEL)
    {
        error = true;
        goto BRANCH_END;
    }
    this->line_info.is_symbol = true;
    this->line_info.symbol    = this->cur_token.val;

BRANCH_END:
    if(error)
    {
        this->line_info.error = true;
        this->line_info.errstr = "Invalid argument " + 
            this->cur_token.toString() + " for instruction " + 
            this->line_info.opcode.toString();

        if(this->verbose)
        {
            std::cout << "[" << __func__ << "] " << 
                this->line_info.errstr << std::endl;
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
        // if we have an offset, convert it here 
        if(this->cur_token.offset != "\0")
            this->line_info.offset[argn] = std::stoi(this->cur_token.offset, nullptr, 10);
        this->line_info.type[argn] = this->cur_token.type;

        switch(this->cur_token.type)
        {
            case SYM_REG_ZERO:
            case SYM_REG_GLOBAL:
            case SYM_REG_FRAME:
                this->line_info.val[argn] = 0;
                break;

            default:
                this->line_info.val[argn] = std::stoi(this->cur_token.val, nullptr, 10);
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
    this->line_info.is_symbol = true;
    this->line_info.symbol    = this->cur_token.val;

BRANCH_END:
    if(error)
    {
        this->line_info.error = true;
        this->line_info.errstr = "Invalid argument " + 
            this->cur_token.toString() + " for instruction " + 
            this->line_info.opcode.toString();

        if(this->verbose)
        {
            std::cout << "[" << __func__ << "] " << 
                this->line_info.errstr << std::endl;
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

        if(this->line_info.is_imm && argn == num_args-1)
        {
            this->line_info.val[argn] = std::stoi(this->cur_token.val, nullptr, 10);
            this->line_info.type[argn] = SYM_LITERAL;
        }
        else
        {
            if(!this->cur_token.isReg())
            {
                error = true;
                goto ARG_ERR;
            }
            this->line_info.type[argn]   = this->cur_token.type;

            // if we have an offset, convert it here 
            if(this->cur_token.offset != "\0")
                this->line_info.offset[argn] = std::stoi(this->cur_token.offset, nullptr, 10);

            switch(this->cur_token.type)
            {
                case SYM_REG_ZERO:
                case SYM_REG_GLOBAL:
                case SYM_REG_FRAME:
                    this->line_info.val[argn] = 0;
                    break;

                default:
                    this->line_info.val[argn] = std::stoi(this->cur_token.val, nullptr, 10);
                    break;
            }
        }
    }

ARG_ERR:
    if(error)
    {
        this->line_info.error = true;
        this->line_info.errstr = "Invalid argument " + std::to_string(argn+1) + 
            " to instruction " + this->line_info.opcode.toString();
        if(this->verbose)
        {
            std::cout << "[" << __func__ << "] (line " << 
                this->cur_line << ") " << this->line_info.errstr << std::endl;
        }
    }
}

/*
 * parseJump()
 * Jump instructions only have labels
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

    this->line_info.is_symbol = true;
    this->line_info.symbol    = this->cur_token.val;

JUMP_END:
    if(error)
    {
        this->line_info.error = true;
        this->line_info.errstr = "Invalid argument " + 
            this->cur_token.toString();

        if(this->verbose)
        {
            std::cout << "[" << __func__ << "] " << 
                this->line_info.errstr << std::endl;
        }
    }
}



/*
 * parseLine()
 */
void Lexer::parseLine(void)
{
    Opcode op;
    Symbol sym;
    unsigned int line_num = 0;

    this->line_info.init();
    line_num = this->cur_line;
    this->nextToken();          

    // if there is a label on this line, add it to the symbol table
    if(this->cur_token.type == SYM_LABEL)
    {
        this->line_info.is_label = true;
        // add symbol to table, remove any trailing characters
        char last_char = this->cur_token.val[this->cur_token.val.length() - 1];
        if((last_char == ';') || (last_char == '#'))
            sym.label = this->cur_token.val.substr(0, this->cur_token.val.length() - 1);
        else
            sym.label = this->cur_token.val;
        sym.addr = this->cur_addr;
        // add to symbol table 
        this->sym_table.add(sym);
        this->line_info.label = sym.label;
        this->skipComment();
        // scan in the next token
        this->nextToken(); 
        line_num = this->cur_line;
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

        this->line_info.opcode = op;

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
                this->line_info.is_imm = true;
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
            case LEX_JR:
                this->parseJump();
                break;

            case LEX_LW:
                this->parseRegArgs(2);
                break;

            case LEX_MULT:
                this->parseRegArgs(3);
                break;

            case LEX_OR:
                this->parseRegArgs(2);
                break;

            case LEX_ORI:
                this->line_info.is_imm = true;
                this->parseRegArgs(3);
                break;

            case LEX_SLL:
            case LEX_SRL:
                this->parseRegArgs(3);
                break;

            case LEX_SLTU:
                this->parseRegArgs(3);
                break;

            case LEX_SW:
                this->parseRegArgs(2);
                break;

            default:
                this->line_info.error = true;
                this->line_info.errstr = "Invalid instruction " + this->cur_token.val;
                if(this->verbose)
                {
                    std::cout << "[" << __func__ << "] (line " << 
                        std::dec << this->cur_line << ") " << 
                        this->line_info.errstr << std::endl;
                }
                goto LINE_END;
        }
    }

LINE_END:
    this->line_info.line_num = line_num;
    this->line_info.addr     = this->cur_addr;
    this->cur_addr++;
}

/*
 * resolveLabels()
 */
void Lexer::resolveLabels(void)
{
    unsigned int idx;
    uint32_t label_addr;
    LineInfo line;

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

    for(idx = 0; idx < this->sym_table.size(); ++idx)
    {
        line = this->source_info.get(idx);
        if(line.is_symbol)
        {
            label_addr = this->sym_table.getAddr(line.symbol);
            if(label_addr > 0)
            {
                switch(line.opcode.instr)
                {
                    case LEX_BEQ:
                    case LEX_BNE:
                        line.type[2]   = SYM_LITERAL;
                        line.offset[0] = label_addr;
                        break;

                    case LEX_BGTZ:
                    case LEX_BLEZ:
                        line.type[1]   = SYM_LITERAL;
                        line.offset[1] = label_addr;
                        break;

                    case LEX_J:
                    case LEX_JAL:
                    case LEX_JALR:
                    case LEX_JR:
                        line.type[0]   = SYM_LITERAL;
                        line.offset[0] = label_addr;
                        break;

                    default:
                        break;
                }
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
        this->source_info.add(this->line_info);
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
