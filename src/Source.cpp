/*
 * SOURCE
 * Objects for processing assembly source 
 *
 * Stefan Wong 2019
 */

#include "Source.hpp"


Token::Token()
{
    this->type = SYM_NONE;
    this->val = "\0";
}
