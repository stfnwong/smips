/*
 * LEXER
 * MIPS Assembly language Lexer 
 *
 * Stefan Wong 2019
 */

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <stack>

#include "Address.hpp"
#include "Lexer.hpp"
#include "Codes.hpp"
#include "Data.hpp"     // MemLine is here at the moment...



Lexer::Lexer()
{
    this->token_buf_size  = 512;
    this->line_buf_size   = 512;
    void nextLine(void);
    this->verbose         = false;
    this->cur_char        = '\0';
    this->cur_line        = 0;
    this->cur_pos         = 0;
    this->text_addr       = 0;
    this->data_addr       = 0;
    this->text_start_addr = TEXT_START_ADDR;
    this->data_start_addr = DATA_START_ADDR;
    this->cur_mode        = LEX_TEXT_SEG;
    // create token buffer
    this->alloc_mem();
    this->init_instr_table();
    this->init_psuedo_op_table();
    this->init_directive_code_table();
}

Lexer::~Lexer()
{
    delete[] this->token_buf;
    delete[] this->line_buf;
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
 * init_psuedo_op_table()
 */
void Lexer::init_psuedo_op_table(void)
{
    for(const Opcode& code : lex_psuedo_ops)
        this->psuedo_op_table.add(code);
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
    this->line_buf = new char[this->line_buf_size];
}

// Memory position
void Lexer::incrTextAddr(void)
{
    this->text_addr += 4;
}

void Lexer::incrDataAddr(void)
{
    this->data_addr += 1;
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
        if(this->cur_char == ' ')       // space 
            break;
        if(this->cur_char == '\n')      // newline
            break;
        if(this->cur_char == ';')       // comment
            break;
        if(this->cur_char == '#')       // also comment
        {
            this->skipComment();
            break;
        }
        if(this->cur_char == ',')       // seperator
            break;
        if(this->cur_char == ':')       // end of label
            break;

        this->token_buf[idx] = this->cur_char;
        this->advance();
        idx++;
    }
    this->token_buf[idx] = '\0';
    // If we are on a seperator now, advance the source pointer 
    if(this->cur_char == ',')
        this->advance();
}

// TODO : re-write all the extraction methods, most of this shit is hot garbage

/*
 * extractLiteral()
 */
Token Lexer::extractLiteral(const std::string& token, unsigned int start_offset, unsigned int& end_offset)
{
    Token out_token;
    unsigned int tok_ptr;
    std::string mem_offset;

    // TODO : update to handle hex using c-style (0x) prefix
    tok_ptr = start_offset;
    while(std::isdigit(token[tok_ptr]))
        tok_ptr++;
    
    if(tok_ptr == start_offset)     // we didn't move
    {
        end_offset = start_offset;
        goto LITERAL_REG_END;
    }

    if(tok_ptr >= (token.size()))   // no more chars, this is just a literal
        out_token.val = token.substr(start_offset);     
    else
        out_token.val = token.substr(start_offset, tok_ptr - start_offset);

LITERAL_REG_END:
    end_offset = tok_ptr;
    out_token.type = SYM_LITERAL;
    return out_token;
}


/*
 * extractReg()
 * Extract a register
 */
Token Lexer::extractReg(const std::string& token, unsigned int start_offset, unsigned int& end_offset)
{
    Token out_token;
    unsigned int tok_ptr;
    unsigned int paren_ptr = 0;
    std::stack<int> paren_stack;
    std::string offset;
    int reg_idx;

    tok_ptr = start_offset;
    
    out_token.reg_offset = "-1";
    while(tok_ptr < token.length())
    {
        if(token[tok_ptr] == '(')
            paren_stack.push(tok_ptr);
        if(token[tok_ptr] == ')')
            paren_stack.pop();
        if(token[tok_ptr] == '$')
            break;

        // We handle offsets here so that any number of parens 
        // can appear before the register name. Offsets only occur
        // before a '$' character
        if(std::isdigit(token[tok_ptr]))
        {
            out_token = this->extractLiteral(token, tok_ptr, end_offset);
            out_token.reg_offset = out_token.val;        // we will overwrite the other fields
            tok_ptr = end_offset-1;      // we must have advaneced by this amount
        }
        tok_ptr++;
    }

    // we should now be right on the '$' character
    start_offset = tok_ptr;
    if(paren_stack.empty())
    {
        end_offset = token.length();
    }
    else
    {
        // there is at least one parenthesis
        paren_ptr = tok_ptr;
        while(paren_ptr < token.length())
        {
            if(token[paren_ptr] == ')')
            {
                paren_stack.pop();
                break;
            }
            paren_ptr++;
        }
        end_offset = paren_ptr;
    }

    //  TODO: also check the paren stack can be cleared?
    std::string token_substr = token.substr(start_offset, end_offset - start_offset);

    // match the register string with a register
    reg_idx = this->reg_map.getIdx(token_substr);
    if(reg_idx >= 0)
    {
        out_token.type = SYM_REGISTER;
        out_token.val = std::to_string(reg_idx);    // TODO : think about string conversions (ie: do we need to do it?)
    }
    else
    {
        out_token.type = SYM_NONE;
        out_token.val = "\0";
    }

    return out_token;
}



/*
 * nextToken()
 * Get the next token in the stream, and set the parameters of 
 * this->cur_token to be that token.
 *
 */
void Lexer::nextToken(void)
{
    Opcode op;
    Token out_token;
    std::string token_str;
    unsigned int start_offset; 
    unsigned int end_offset;

    this->scanToken();
    token_str = std::string(this->token_buf);
    // set the offset string for the current token back to null
    this->cur_token.init();

    // This is a character 
    if(token_str[0] == '\'')
    {
        this->cur_token.type = SYM_CHAR;
        this->cur_token.val = token_str[1];
        // TODO : remove this check - move it to parseChar() or some similarly named function
        // check that there is a closing '''
        if(token_str[2] != '\'')
        {
            this->text_info.error = true;
            this->text_info.errstr = "expected closing ' after ["
                + this->cur_token.val + "] token\n";
            goto TOKEN_END;
        }
    }

    // This is a string
    if(token_str[0] == '"')
    {
        this->cur_token.type = SYM_STRING; 
        this->cur_token.val = token_str;
        if(this->text_info.error)
        {
            if(this->verbose)
            {
                std::cout << "[" << __func__ << "] (line " << this->cur_line << ") " << 
                    this->text_info.errstr << std::endl;
            }
        }
        goto TOKEN_END;
    }

    // From here on we can force the case to match the case we are using 
    // in the source (in this instance, uppercase)
    std::transform(token_str.begin(), token_str.end(), token_str.begin(),
            [](unsigned char c){ return std::tolower(c); });

    // this is a directive token
    if(token_str[0] == '.')
    {
        this->cur_token.type = SYM_DIRECTIVE;
        this->cur_token.val  = token_str;

        goto TOKEN_END;
    }
    
    start_offset = 0;
    // Check digits, which may be either literals or register offsets 
    if(std::isdigit(token_str[0]) && token_str[1] != '(')
    {
        out_token = this->extractLiteral(token_str, start_offset, end_offset);
        if(out_token.type == SYM_NONE)
        {
            this->text_info.error  = true;
            this->text_info.errstr = "Got blank symbol " + out_token.toString();
        }
        else
            this->cur_token = out_token;
        goto TOKEN_END;
    }

    // Check registers or expand parenthesis
    if((token_str[0] == '$') || 
       (token_str[0] == '(') || 
       (std::isdigit(token_str[0]) && token_str[1] == '('))
    {
        out_token = this->extractReg(token_str, start_offset, end_offset);
        if(out_token.type == SYM_NONE)
        {
            this->text_info.error  = true;
            this->text_info.errstr = "Got blank symbol " + out_token.toString();
        }
        else
            this->cur_token = out_token;

        goto TOKEN_END;
    }

    // TODO : we actually do this twice - why not set the instruction type here as well?
    // Check if this matches any instructions 
    op = this->instr_code_table.get(token_str);
          
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
            this->cur_token.val << ">";
        if(cur_token.reg_offset != "\0")
            std::cout << " and offset " << cur_token.reg_offset;
       
        std::cout << std::endl;

        if(this->text_info.error)
            std::cout << "[" << __func__ << "] " << this->text_info.errstr << std::endl;
    }
}


// ======== DIRECTIVES ======== //

void Lexer::parseAlign(void)
{
    std::cout << "[" << __func__ << "] not yet implemented" << std::endl;
}


/*
 * parseASCIIZ()
 */
void Lexer::parseASCIIZ(void)
{
    this->data_info.directive = SYM_DIR_ASCIIZ;
    this->data_info.line_num = this->cur_line;
    this->data_info.addr = this->data_addr;

    std::cout << "[" << __func__ << "] start address = " << std::hex << this->data_info.addr << std::endl;
    this->nextToken();
    if(this->cur_token.type == SYM_STRING)
    {
        for(unsigned int c = 1; c < this->cur_token.val.length()-1; ++c)
        {
            this->data_info.addByte(uint8_t(this->cur_token.val[c]));
            this->incrDataAddr();
        }
    }
    else
    {
        this->data_info.errstr = "Invalid symbol type " + this->cur_token.toString() 
            + " for directive .asciiz";
        this->data_info.error = true;
        if(this->verbose)
            std::cout << "[" << __func__ << "] " << this->data_info.errstr << std::endl;
    }

    std::cout << "[" << __func__ << "] this->data_info : " << std::endl;
    std::cout << this->data_info.toString() << std::endl;
}

/*
 * parseByte()
 */
void Lexer::parseByte(void)
{
    this->data_info.directive = SYM_DIR_BYTE;

    // there should only be one token after the .byte directive which 
    // must be a character (for numeric input use .word or .half directive)
    this->nextToken();
    if(this->cur_token.type != SYM_CHAR)
    {
        this->data_info.error = true;
        this->data_info.errstr = "Got invalid token " + this->cur_token.toString() 
            + " for .byte directive";
        if(this->verbose)
            std::cout << "[" << __func__ << "] " << this->data_info.errstr << std::endl;
    }
    else
    {
        std::cout << "[" << __func__ << "] about to stoi from token " << 
            this->cur_token.toString() << std::endl;
        int byte = std::stoi(this->cur_token.val);
        if(byte > 255)
            byte = 255;
        this->data_info.addByte(uint8_t(byte));
        this->incrDataAddr();
    }
}

/*
 * parseHalf()
 */
void Lexer::parseHalf(void)
{
    int word_idx = 0;
    uint32_t word;
    this->data_info.directive = SYM_DIR_HALF;
    this->data_info.line_num = this->cur_line;

    while(this->cur_line <= this->data_info.line_num)        // put upper bound on number of loops
    {
        this->nextToken();
        // awkward, but works.
        if(this->cur_line > this->data_info.line_num)
            break;

        if(this->cur_token.type != SYM_LITERAL)
        {
            this->data_info.error = true;
            this->data_info.errstr = ".half directive token " + std::to_string(word_idx) + 
                " (" + std::string(this->token_buf) + ") not a valid literal";
            if(this->verbose)
                std::cout << "[" << __func__ << "] " << this->data_info.errstr << std::endl;
            break;

        }

        word = std::stoi(this->cur_token.val);
        if(word > ((1 << 16) - 1))
        {
            if(this->verbose)
            {
                std::cout << "[" << __func__ << "] word " << word_idx 
                    << " (" << word << ") will be truncated to ("
                    << unsigned((1 << 16) - 1) << ")" << std::endl;
            }
            word = (1 << 16) - 1;
        }

        this->data_info.addByte(word);
        word_idx++;         // NOTE: only using this value for debugging... remove in final version
        this->incrDataAddr();
        //this->data_addr++;
    }
}


/*
 * parseWord()
 */
void Lexer::parseWord(void)
{
    uint32_t word;
    int word_idx = 0;
    this->data_info.directive = SYM_DIR_WORD;
    this->data_info.line_num = this->cur_line;
    this->data_info.addr     = this->data_addr;     // the address should be the start address at this time

    std::cout << "[" << __func__ << "] start address = " << std::hex << this->data_info.addr << std::endl;
    while(this->cur_line <= this->data_info.line_num)        // put upper bound on number of loops
    {
        this->nextToken();
        if(this->cur_token.type != SYM_LITERAL)
        {
            this->data_info.error = true;
            this->data_info.errstr = ".word directive token " + std::to_string(word_idx) + 
                " (" + std::string(this->token_buf) + ") not a valid literal";
            if(this->verbose)
                std::cout << "[" << __func__ << "] " << this->data_info.errstr << std::endl;
            break;
        }

        word = std::stoi(this->cur_token.val);
        this->data_info.addByte(word);
        word_idx++;
        std::cout << "[" << __func__ << "] wrote word " << std::hex << word <<
            " which should be at oddress 0x" << std::hex << this->data_addr << std::endl;
        this->incrDataAddr();
    }

    if(this->verbose)
    {
        std::cout << "[" << __func__ << "] this->data_info : " << std::endl;
        std::cout << this->data_info.toString() << std::endl;
    }
}


/*
 * parseSpace()
 * Reserve a block of memory
 */
void Lexer::parseSpace(void)
{
    this->data_info.directive = SYM_DIR_SPACE;
    this->data_info.line_num = this->cur_line;
    this->data_info.addr     = this->data_addr;

    // the next token should be a literal indicating how many bytes to reserve
    this->nextToken();
    if(this->cur_token.type != SYM_LITERAL)
    {
        this->data_info.error = true;
        this->data_info.errstr = "Expected literal after directive .space, got " + this->cur_token.toString();
        return;
    }

    this->data_info.space = std::stoi(this->cur_token.val);
    
    if(this->verbose)
    {
        std::cout << "[" << __func__ << "] adding " << std::dec << 
            this->data_info.space << " to this->data_addr (" << std::hex 
            << this->data_addr << ")" << std::endl;
    }
    this->data_addr += this->data_info.space;
}


/* 
 * Segment Mode directives
 */

/*
 * dataSeg()
 */
void Lexer::dataSeg(void)
{
    this->cur_mode = LEX_DATA_SEG;
    if(this->verbose)
    {
        std::cout << "[" << __func__ << "] (line " << this->cur_line
            << ") setting mode to LEX_DATA_SEG" << std::endl;
    }
}

/*
 * textSeg()
 */
void Lexer::textSeg(void)
{
    this->cur_mode = LEX_TEXT_SEG;
    if(this->verbose)
    {
        std::cout << "[" << __func__ << "] (line " << this->cur_line
            << ") setting mode to LEX_TEXT_SEG" << std::endl;
    }
}



// ======== PARSING ======== //

/*
 * parseDirective()
 * Parse an assembler directive.
 */
void Lexer::parseDirective(int line_num)
{
    Opcode directive;

    // Look up directive string in directive table
    directive = this->directive_code_table.get(this->cur_token.val);
    if(this->verbose)
    {
        std::cout << "[" << __func__ << "] (line " << 
            std::dec << this->cur_line << ") got directive " <<
            directive.toString() << std::endl;
    }

    // Check which directive this is
    switch(directive.instr)
    {
        case LEX_ALIGN:
            this->dataSeg();
            this->parseAlign();
            break;

        case LEX_ASCIIZ:
            this->dataSeg();
            this->parseASCIIZ();
            break;

        case LEX_BYTE:
            this->dataSeg();
            this->parseByte();
            break;

        // Add a number of half-words to the data segment
        case LEX_HALF:
            this->dataSeg();
            this->parseHalf();
            break;

        case LEX_SPACE:
            this->dataSeg();
            this->parseSpace();
            break;

        // data types 
        case LEX_WORD:
            this->dataSeg();
            this->parseWord();
            break;

        // Change segment type
        // Global variable segment 
        case LEX_DATA:
            this->dataSeg();
            return;

        // Text segment
        case LEX_TEXT:
            this->textSeg();
            return;

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

    this->data_info.line_num = line_num;
    if(this->data_info.addr == 0)
        this->data_info.addr     = this->data_addr;
    this->source_info.addData(this->data_info);
}

/*
 * parseInstruction()
 * Parse an instruction
 */
void Lexer::parseInstr(int line_num)
{
    Opcode op;

    this->textSeg();
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
        case LEX_JR:
            this->parse_r();
            break;

        case LEX_J:
        case LEX_JAL:
        case LEX_JALR:
            this->parse_i();
            break;
        
        // TODO : re-org by instruction format
        case LEX_ADDI:
        case LEX_ADDIU:
        case LEX_ANDI:
        case LEX_ORI:
        case LEX_SLTI:
        case LEX_SLTIU:
        case LEX_SLL:
            this->text_info.is_imm = true;
            this->parse_rri();
            break;

        // memory access with offsets
        case LEX_LW:
        case LEX_SW:
            this->parse_rro();
            break;
        
        case LEX_LUI:
            this->text_info.is_imm = true;
            this->text_info.upper = true;
            this->parse_ri();
            break;

        case LEX_DIV:
        case LEX_DIVU:
        case LEX_MULT:
        case LEX_MULTU:
            this->parse_rrr();
            break;

        case LEX_ADD:
        case LEX_AND:
        case LEX_NOR:
        case LEX_OR:
        case LEX_SLT:
        case LEX_SLTU:
        case LEX_SUB:
        case LEX_SUBU:
        //case LEX_XOR:
            this->parse_rrr();
            break;

        case LEX_BEQ:
        case LEX_BNE:
            this->text_info.is_imm = true;
            this->parse_rri();
            //this->branchInstructionArgSwap();
            break;

        // BGX instructions need to be able to handle symbols as immediate arg
        // These are all psuedo-ops that get expanded to slt + bne, slt + beq, o
        case LEX_BGT:
        case LEX_BLT:
        case LEX_BGE:
        case LEX_BLE:
        case LEX_BGTU:
            this->text_info.is_imm = true;
            this->text_info.psuedo = true;
            this->parse_rri();
            break;

        case LEX_BEQZ:
            this->text_info.psuedo = true;
            this->parse_ri();
            break;

        case LEX_BGTZ:
        case LEX_BLEZ:
            this->parse_ri();
            break;

        case LEX_SYSCALL:
            break;

        // psudo-ops 
        case LEX_LA:
        case LEX_LI:
            this->text_info.is_imm = true;
            this->parse_ri();
            this->text_info.psuedo = true;
            break;

            // no-op
        case LEX_NULL:
            this->add_noop();
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
    }

    this->text_info.line_num = line_num;
    this->text_info.addr     = this->text_addr;

    // NOTE: this is annoying and it makes reasoning about the operation a pain. The 
    // reason that this is here is because in expandPsuedo() when expanding an operation
    // we add the expanded instructions directly into this->text_info. The expansion looks
    // at the last instruction in this->text_info, and if its one of the psuedo ops then
    // it replaces it with the 'real' ops (ie: it mutates this->text_info behind the scenes).
    if(this->text_info.psuedo)
    {
        std::cout << "[" << __func__ << "] expanding psuedo opcode " << this->text_info.opcode.toString() << std::endl;
        this->expandPsuedo();
    }
    else
    {
        std::cout << "[" << __func__ << "] adding opcode " << this->text_info.opcode.toString() << std::endl;
        this->source_info.addText(this->text_info);
        this->incrTextAddr();
    }
}

// parse a single register
void Lexer::parse_r(void)
{
    this->text_info.args[0] = this->parseRegister();
}

// parse a single immediate
void Lexer::parse_i(void)
{
    this->text_info.args[0] = this->parseImmediate();
    if(this->text_info.args[0].type == SYM_LABEL)
    {
        this->text_info.is_symbol = true;
        this->text_info.symbol = this->cur_token.val;
    }
    else
        this->text_info.is_symbol = false;
}

// parse register, register
void Lexer::parse_rr(void)
{
    this->text_info.args[0] = this->parseRegister();
    this->text_info.args[1] = this->parseRegister();
    // if the second register has an offset then add 
    // an immediate in index 2 to reflect that
    if(this->text_info.args[1].offset > 0)
        this->text_info.args[2] = Argument(SYM_LITERAL, this->text_info.args[1].offset);
}

void Lexer::parse_ri(void)
{
    this->text_info.args[0] = this->parseRegister();
    this->text_info.args[1] = Argument(SYM_NONE, 0);
    this->text_info.args[2] = this->parseImmediate();

    // immediate must be either a symbol (label) or literal
    if(this->text_info.args[2].type == SYM_LABEL)
    {
        this->text_info.is_symbol = true;
        this->text_info.symbol = this->cur_token.val;
    }
    else if(this->text_info.args[2].type != SYM_LITERAL)
    {
        this->text_info.errstr = "Argument (1) expected literal or symbol, got [" + 
            TokenToString(this->text_info.args[2].type) + 
            "]";
        this->text_info.error = true;
    }
}

void Lexer::parse_rro(void)
{
    this->text_info.args[0] = this->parseRegister();
    this->text_info.args[1] = this->parseRegister();
    if(this->text_info.args[1].offset > -1)
    {
        std::cout << "[" << __func__ << "] got offset in args[1] " << std::dec 
            << this->text_info.args[1].offset << std::endl;
        this->text_info.args[2] = Argument(SYM_LITERAL, this->text_info.args[1].offset);
    }
    else
        this->text_info.args[2] = Argument(SYM_LITERAL, 0);

}

// parse register, register, immediate
void Lexer::parse_rri(void)
{
    this->text_info.args[0] = this->parseRegister();
    this->text_info.args[1] = this->parseRegister();
    if(this->text_info.args[1].offset > -1)
    {
        std::cout << "[" << __func__ << "] got offset in args[1] " << std::dec 
            << this->text_info.args[1].offset << std::endl;
        this->text_info.args[2] = Argument(SYM_LITERAL, this->text_info.args[1].offset);
    }
    else
        this->text_info.args[2] = this->parseImmediate();

    // immediate must be either a symbol (label) or literal
    if(this->text_info.args[2].type == SYM_LABEL)
    {
        this->text_info.is_symbol = true;
        this->text_info.symbol = this->cur_token.val;
    }
    else if(this->text_info.args[2].type != SYM_LITERAL)
    {
        this->text_info.errstr = "Argument (2) expected literal or symbol, got [" + 
            TokenToString(this->text_info.args[2].type) + 
            "]";
        this->text_info.error = true;
    }
}

// parse register, register, register
void Lexer::parse_rrr(void)
{
    this->text_info.args[0] = this->parseRegister();
    this->text_info.args[1] = this->parseRegister();
    this->text_info.args[2] = this->parseRegister();
}

void Lexer::add_noop(void)
{
    this->text_info.opcode = Opcode(LEX_NULL, 0x0);
}

/*
 * parseRegister()
 * Extract a single register argument from the input
 */
Argument Lexer::parseRegister(void)
{
    Argument arg;

    this->nextToken();
    if(!this->cur_token.isReg())
        return arg;     

    arg.type = this->cur_token.type;
    arg.val  = std::stoi(this->cur_token.val);
    if(this->cur_token.isOffset())
        arg.offset = std::stoi(this->cur_token.reg_offset, nullptr, 10);

    return arg; 
}

/*
 * parseImmediate()
 * Extract an immediate or label from the input
 */
Argument Lexer::parseImmediate(void)
{
    this->nextToken();
    if(this->cur_token.type == SYM_LITERAL)
    {
        std::cout << "[" << __func__ << "] val = " 
            << std::stoi(this->cur_token.val, nullptr, 10) 
            << std::endl;

        return Argument(SYM_LITERAL, std::stoi(this->cur_token.val, nullptr, 10));
    }
    if(this->cur_token.type == SYM_LABEL)
        return Argument(SYM_LABEL, 0);

    return Argument();
}


/*
 * parseLine()
 * TODO : consider a more functional design where we return a TextInfo .
 * The problem with that is that we would need to call it twice to expand psuedo-ops...
 */
void Lexer::parseLine(void)
{
    Symbol sym;
    unsigned int line_num = 0;

    line_num = this->cur_line;
    this->text_info.init();
    this->data_info.init();
    this->nextToken();          

    // if there is a label at the start of this line, add it to the symbol table
    if(this->cur_token.type == SYM_LABEL)
    {
        // add symbol to table, remove any trailing characters
        char last_char = this->cur_token.val[
            this->cur_token.val.length()
        ];

        if((last_char == ';') || 
           (last_char == '#') ||
           (last_char == ':'))
        {
            sym.label = this->cur_token.val.substr(
                    0, this->cur_token.val.length() - 1
            );
        }
        else
            sym.label = this->cur_token.val;

        sym.addr = (this->cur_mode == LEX_TEXT_SEG) ? this->text_addr : this->data_addr;
        sym.section = int(this->cur_mode);
        // add to symbol table 
        this->sym_table.add(sym);

        if(this->cur_mode == LEX_TEXT_SEG)
        {
            this->text_info.is_label = true;
            this->text_info.label = sym.label;
        }
        else
        {
            this->data_info.is_label = true;
            this->data_info.label = sym.label;
        }

        this->advance();        // skip ahead to align pointer for next token
        // scan in the next token
        this->nextToken(); 
        line_num = this->cur_line;
    }

    // check for directives
    if(this->cur_token.type == SYM_DIRECTIVE)
        this->parseDirective(line_num);
    // check for instructions instructions 
    else if(this->cur_token.type == SYM_INSTR)
        this->parseInstr(line_num);
    else
    {
        // add a standalone label (which is a valid construction)
        this->text_info.line_num = line_num;
        this->text_info.addr     = this->text_addr;
        this->source_info.addText(this->text_info);
    }

    std::cout << "[" << __func__ << "] there are " << std::dec << 
        this->source_info.getTextInfoSize() << " entries in text section" << std::endl;
}

/*
 * resolveLabels()
 */
void Lexer::resolveLabels(void)
{
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

    for(unsigned int idx = 0; idx < this->source_info.getTextInfoSize(); ++idx)
    {
        line = this->source_info.getText(idx);
        if(line.is_symbol)
        {
            label_addr = this->sym_table.getAddr(line.symbol);

            if(label_addr > 0)
            {
                uint32_t offset;
                switch(line.opcode.instr)
                {
                    case LEX_LUI:
                    {
                        line.args[1] = Argument();  
                        if(line.upper)
                            line.args[2] = Argument(SYM_LITERAL, (label_addr & 0xFFFF0000) >> 16);
                        else
                            line.args[2] = Argument(SYM_LITERAL, label_addr & 0x0000FFFF);
                    }
                    break;

                    case LEX_ORI:
                    {
                        line.args[2].type = SYM_LITERAL;
                        if(line.lower)
                            line.args[2].val = (label_addr & 0x0000FFFF);
                        else
                            line.args[2].val = (label_addr & 0xFFFF0000) >> 16;
                    }
                    break;

                    case LEX_BNE:
                    case LEX_BEQ:
                    {
                        // convert to offset
                        offset = label_addr - line.addr;
                        line.args[2].type = SYM_LITERAL;
                        line.args[2].val  = (offset & 0x0000FFFF);
                    }
                    break;

                    case LEX_J:
                    {
                        // same as default, except we also reset arg[0]
                        line.args[2].type = SYM_LITERAL;
                        line.args[2].val = label_addr;
                        line.args[0] = Argument();
                    }
                    break;

                    default:
                        line.args[2].type = SYM_LITERAL;
                        line.args[2].val = label_addr;
                        break;
                }

                if(this->verbose)
                {
                    std::cout << "[" << __func__ << "] updating line " << line.line_num 
                        << " to be " << std::endl << line.toString() << std::endl;
                }
                this->source_info.update(idx, line);

                if(this->verbose)
                {
                    std::cout << "[" << __func__ << "] updated symbol <" <<
                        line.symbol << "> on line " << std::dec << 
                        line.line_num << " [0x" << std::hex << 
                        std::setfill('0') << std::setw(8) << 
                        line.addr << "] to value [0x" << 
                        ")" << std::endl;
                }
            }
        }
    }
}


/*
 * advanceAddrs()
 */
void Lexer::advanceAddrs(int start_idx, int offset)
{
    TextInfo cur_ti;

    for(unsigned int idx = start_idx; idx < this->source_info.getTextInfoSize(); ++idx)
    {
        cur_ti = this->source_info.getText(idx);
        cur_ti.addr += offset;
        this->source_info.update(idx, cur_ti);
    }
}

/*
 * expandPsuedo()
 * Iterate over the SourceInfo and expand any psuedo instructions 
 * into 'real' instructions.
 */
void Lexer::expandPsuedo(void)
{
    Opcode cur_opcode;
    TextInfo ti;
    uint32_t instr = this->text_info.opcode.instr;

    // TODO : debug, show the psuedo op before expansion 
    std::cout << "[" << __func__ << "] expanding psuedo op : " << std::endl;
    std::cout << this->text_info.toString() << std::endl;
    switch(instr)
    {
        case LEX_BGT:
        case LEX_BLT:
        case LEX_BGE:
        case LEX_BLE:
        case LEX_BGTU:
            {
                std::cout << "[" << __func__ << "] expanding " << this->text_info.opcode.toString() << std::endl;
                // slt/sltu $at, $t, $s
                ti.init();
                ti.opcode    = (instr == LEX_BGTU) ? Opcode(LEX_SLTU, "sltu") : Opcode(LEX_SLT, "slt");
                ti.addr      = this->text_info.addr;
                ti.line_num  = this->text_info.line_num;

                ti.args[0] = Argument(SYM_REGISTER, REG_AT);
                if(instr == LEX_BLT || instr == LEX_BGE)
                {
                    ti.args[1] = this->text_info.args[0];
                    ti.args[2] = this->text_info.args[1];
                }
                else
                {
                    ti.args[1] = this->text_info.args[1];
                    ti.args[2] = this->text_info.args[0];
                }
                ti.label     = this->text_info.label;
                ti.is_label  = this->text_info.is_label;

                this->source_info.addText(ti);
                this->incrTextAddr();
                
                // bne/beq $at, $zero, C
                ti.init();
                if(instr == LEX_BGE || instr == LEX_BLE)
                    ti.opcode = Opcode(LEX_BEQ, "beq");
                else
                    ti.opcode = Opcode(LEX_BNE, "bne");
                ti.addr      = this->text_info.addr + 4;
                ti.line_num  = this->text_info.line_num;
                ti.args[0]   = Argument(SYM_REGISTER, REG_AT);
                ti.args[1]   = Argument(SYM_REGISTER, REG_ZERO);
                ti.args[2]   = this->text_info.args[2];
                ti.is_imm    = true;
                ti.symbol    = this->text_info.symbol;
                ti.is_symbol = this->text_info.is_symbol;
                
                this->source_info.addText(ti);
                this->incrTextAddr();
                break;
            }

        case LEX_BEQZ:
            {
                // Input is [beqz $s C]
                // beq $s $zero C
                ti.init();
                ti.opcode = Opcode(LEX_BEQ, "beq");
                ti.addr      = this->text_info.addr;
                ti.line_num  = this->text_info.line_num;
                ti.args[0]   = this->text_info.args[0];
                ti.args[1]   = Argument(SYM_REGISTER, REG_ZERO);
                ti.args[2]   = this->text_info.args[2];
                ti.label     = this->text_info.label;
                ti.is_label  = this->text_info.is_label;
                ti.symbol    = this->text_info.symbol;
                ti.is_symbol = this->text_info.is_symbol;
                ti.is_imm    = true;

                this->source_info.addText(ti);
                this->incrTextAddr();
                break;
            }

        case LEX_LA:
            {
                // la $t, A 
                //
                // lui $t, A_hi
                // ori $t, $t, A_lo

                // Note that literals get resolved from symbols later
                // This means that we actually need to set upper or lower to tell the resolve step 
                // how to shift the value correctly for the assembler stage. That means that here we
                // just place the entire value in both instructions and let resolveLabels() handle
                // the details.
                ti.init();
                ti.opcode = Opcode(LEX_LUI, "lui");
                ti.addr      = this->text_info.addr;
                ti.line_num  = this->text_info.line_num;
                ti.args[0]   = this->text_info.args[0];
                ti.args[1]   = Argument(SYM_NONE, 0);
                ti.args[2]   = Argument(
                        this->text_info.args[2].type,
                        (this->text_info.args[2].val & 0xFFFF0000) >> 16
                        );
                ti.is_imm    = true;
                ti.upper     = true;   
                ti.is_symbol = this->text_info.is_symbol;
                ti.symbol    = this->text_info.symbol;

                this->source_info.addText(ti);
                this->incrTextAddr();

                ti.init();
                ti.opcode = Opcode(LEX_ORI, "ori");
                ti.addr      = this->text_info.addr + 4;
                ti.line_num  = this->text_info.line_num;
                ti.args[0]   = this->text_info.args[0];
                ti.args[1]   = this->text_info.args[0];
                ti.args[2]   = Argument(
                        this->text_info.args[2].type,
                        this->text_info.args[2].val & 0x0000FFFF
                        );
                ti.is_imm    = true;
                ti.is_symbol = true;
                ti.lower     = true;
                ti.is_symbol = this->text_info.is_symbol;
                ti.symbol    = this->text_info.symbol;

                this->source_info.addText(ti);
                this->incrTextAddr();
            }
            break;

        case LEX_LI:
            ti.init();
            // 32-bit immediate (2 instrs)
            if(this->text_info.args[2].val > ((1 << 16)-1))
            {
                ti.opcode   = Opcode(LEX_LUI, "lui");
                ti.addr     = this->text_info.addr;
                ti.line_num = this->text_info.line_num;
                ti.args[0]  = this->text_info.args[0];
                ti.args[1]  = Argument(SYM_NONE, 0);
                ti.args[2]  = Argument(
                        this->text_info.args[2].type, 
                        (this->text_info.args[2].val & 0xFFFF0000) >> 16
                        );
                //ti.args[2]  = this->text_info.args[1];  // val & 0xFFFF0000  TODO: come back and implement bitwise ops?
                ti.is_imm   = true;
                ti.upper    = true;

                this->source_info.addText(ti);
                this->incrTextAddr();

                ti.init();
                ti.opcode   = Opcode(LEX_ORI, "ori");
                ti.addr     = this->text_info.addr + 4;
                ti.line_num = this->text_info.line_num;
                ti.args[0]  = this->text_info.args[0];
                ti.args[1]  = this->text_info.args[0];
                ti.args[2]  = Argument(
                        this->text_info.args[2].type,
                        this->text_info.args[2].val & 0x0000FFFF
                        );
                //ti.args[2]  = this->text_info.args[1];  // val & 0x0000FFFF
                ti.is_imm   = true;
                ti.lower    = true;

                this->source_info.addText(ti);
                this->incrTextAddr();
            }
            // 16-bit immediate (1 instr)
            else
            {
                ti.opcode   = Opcode(LEX_ORI, "ori");
                ti.addr     = this->text_info.addr;
                ti.line_num = this->text_info.line_num;
                ti.args[0]  = this->text_info.args[0];
                ti.args[1]  = Argument(SYM_REGISTER, REG_ZERO);
                ti.args[2]  = Argument(SYM_LITERAL, (this->text_info.args[2].val & 0x0000FFFF));
                ti.is_imm   = true;

                this->source_info.addText(ti);
                this->incrTextAddr();
            }
            break;

        default:
            if(this->verbose)
            {
                std::cout << "[" << __func__ << "] got something else..." << std::endl;
                std::cout << "[" << __func__ << "] this->text_info :..." 
                    << std::endl << this->text_info.toString() << std::endl;
            }
            this->text_info.error = true;
            this->text_info.errstr = "Unknown psuedo op " + this->text_info.opcode.toString();
            break;
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
    this->text_addr = this->text_start_addr;     
    this->data_addr = this->data_start_addr;

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

int Lexer::getTextStartAddr(void) const
{
    return this->text_start_addr;
}

int Lexer::getDataStartAddr(void) const
{
    return this->data_start_addr;
}

// ==== SETTERS ===== //
void Lexer::setVerbose(const bool v)
{
    this->verbose = v;
}
