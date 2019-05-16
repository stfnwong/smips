/*
 * LEXER
 * MIPS Assembly language Lexer 
 *
 * Stefan Wong 2019
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include "Lexer.hpp"


Lexer::Lexer()
{
    this->token_buf_size = 512;
    this->verbose        = false;
    this->cur_char       = '\0';
    this->cur_line       = 0;
    this->cur_pos        = 0;
    // create token buffer
    this->alloc_mem();
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
    this->cur_char = this->source_text[this->cur_pos];
    if(this->cur_pos >= this->source_text.size())
        this->cur_char = '\0';
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
        this->token_buf[idx] = this->cur_char;
        this->advance();
        idx++;
    }
    this->token_buf[idx] = '\0';
    // If we are on a seperator now, advance the source pointer 
    if(this->cur_char == ',')
        this->advance();

    if(this->verbose)
    {
        std::cout << "[" << __func__ << "] (line " << std::dec << 
            this->cur_line << ") : token_buf contains <" << 
            std::string(this->token_buf) << "> " << std::endl;
    }
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

    // Check for special tokens 

}

void Lexer::parseLine(void)
{

}

/*
 * lex()
 * Lex the current source file
 */
void Lexer::lex(void)
{
    this->cur_line = 1;
    this->cur_pos = 0;
    //this->cur_addr = 0x200;     // TODO : proper address init..

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
        //this->source_info.add(this->line_info);
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





// ==== SETTERS ===== //
void Lexer::setVerbose(const bool v)
{
    this->verbose = v;
}
