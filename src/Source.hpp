/*
 * SOURCE
 * Objects for processing assembly source 
 *
 * Stefan Wong 2019
 */

#ifndef __SOURCE_HPP
#define __SOURCE_HPP

#include <string>

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

struct Token
{
    TokenType type;
    std::string val;

    public:
        Token();
};

#endif /*__SOURCE_HPP*/
