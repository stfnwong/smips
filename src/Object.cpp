/*
 * OBJECT
 * Represents an object file for the SMIPS assembler
 *
 * Stefan Wong 2019
 */


#include "Object.hpp"


// Segment 

Segment::Segment()
{
    this->offset = 0;
    this->type = SEG_NONE;
}

Segment::Segment(const uint32_t offset, const segment_t t )
{
    this->offset = offset;
    this->type = t;
}



// Object
Object::Object() {} 


unsigned int Object::getNumSectionHeaders(void) const
{
    return this->file_header.get_e_shnum();
}

unsigned int Object::getNumProgramHeaders(void) const
{
    return this->file_header.get_e_phnum();
}
