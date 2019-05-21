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
        switch(token_str[1])
        {
            case 'A':
                this->cur_token.type = SYM_REG_ARG;
                break;
            case 'F':
                this->cur_token.type = SYM_REG_FUNC;
                break;
            case 'R':
                this->cur_token.type = SYM_REG_RET;
                break;
            case 'S':
                this->cur_token.type = SYM_REG_SAVED;
                break;
            case 'T':
                this->cur_token.type = SYM_REG_TEMP;
                break;
            case 'Z':
                this->cur_token.type = SYM_REG_ZERO;
                break;
            default:
                // single numbers are also valid 
                if(std::isdigit(token_str[1]))
                    this->cur_token.type = SYM_REG_NUM;
                else
                {
                    this->line_info.error = true;
                    this->line_info.errstr = "Invalid register type " + token_str[1];
                    this->cur_token.type = SYM_NONE;
                    goto TOKEN_END;
                }
        }
        this->cur_token.val  = token_str.substr(2, token_str.length()-1);

        goto TOKEN_END;
    }

    // This is either an immediate or a special register with an offset
    if(std::isdigit(token_str[0]))
    {
        unsigned int tok_ptr = 0;
        while(std::isdigit(token_str[tok_ptr]))
            tok_ptr++;

        this->cur_token.val = token_str.substr(0, tok_ptr);
        // if there are more characters, check whether or not this is a 
        // register with offsets 
        if(token_str.size() > tok_ptr)
        {
            if((token_str[tok_ptr] == '(') && (token_str[tok_ptr+1] == '$'))
            {
                if(token_str[tok_ptr+2] == 'G')
                    this->cur_token.type = SYM_REG_GLOBAL;
                else if(token_str[tok_ptr+2] == 'F')
                    this->cur_token.type = SYM_REG_FUNC;
                else
                {
                    this->line_info.error = true;
                    this->line_info.errstr = "Invalid offset syntax " + 
                        this->cur_token.toString();
                }
            }
            else
            {
                this->line_info.error = true;
                this->line_info.errstr = "Syntax error (" + token_str + ")";
            }
        }
        else
            this->cur_token.type = SYM_LITERAL;

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
            this->line_info.args[argn] = std::stoi(this->cur_token.val, nullptr, 10);
            this->line_info.types[argn] = SYM_LITERAL;
        }
        else
        {
            if(!this->cur_token.isReg())
            {
                error = true;
                goto ARG_ERR;
            }
            if(this->cur_token.type == SYM_REG_ZERO)
                this->line_info.args[argn] = 0;
            else
                this->line_info.args[argn] = std::stoi(this->cur_token.val, nullptr, 10);
            this->line_info.types[argn] = this->cur_token.type;
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
 */
void Lexer::parseJump(void)
{
    std::cout << "[" << __func__ << "] TODO " << std::endl;
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
                this->parseRegArgs(3);
                break;
            case LEX_ADDI:
            case LEX_ADDIU:
                this->line_info.is_imm = true;
                this->parseRegArgs(3);
                break;

            case LEX_MULT:
                this->parseRegArgs(3);
                break;

            case LEX_LW:
                this->parseRegArgs(2);
                break;

            case LEX_OR:
                this->parseRegArgs(2);
                break;

            case LEX_ORI:
                this->line_info.is_imm = true;
                this->parseRegArgs(3);
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

            case LEX_SW:
                this->parseRegArgs(2);
                break;
        }
    }

LINE_END:
    this->line_info.line_num = line_num;
    this->line_info.addr     = this->cur_addr;
    this->cur_addr++;
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
    //this->resolveLabels();
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


// ==== SETTERS ===== //
void Lexer::setVerbose(const bool v)
{
    this->verbose = v;
}
