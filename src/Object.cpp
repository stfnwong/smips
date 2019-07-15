/*
 * OBJECT
 * Represents an object file for the SMIPS assembler
 *
 * Stefan Wong 2019
 */


#include "Object.hpp"


// Object
//Object::Object() : code(nullptr), data(nullptr), zero(nullptr), bss(nullptr) {} 
Object::Object() {} 


Object::~Object() 
{
    //delete this->code;
    //delete this->data;
    //delete this->zero;
    //delete this->bss;
}


unsigned int Object::getNumSectionHeaders(void) const
{
    return this->file_header.get_e_shnum();
}

unsigned int Object::getNumProgramHeaders(void) const
{
    return this->file_header.get_e_phnum();
}
