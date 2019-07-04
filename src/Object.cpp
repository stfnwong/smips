/*
 * OBJECT
 * Represents an object file for the SMIPS assembler
 *
 * Stefan Wong 2019
 */


#include "Object.hpp"


// ELF file header stuff
ELFFileHeader::ELFFileHeader()
{
    // zero out all bytes
    for(int i = 0; i < 0x32; ++i)
        this->header[i] = 0;
    // add magic numbers 
    this->header[0] = 0x7F;
    this->header[1] = 0x45;
    this->header[2] = 0x4C;
    this->header[3] = 0x46;
    // we only support 32 bit a this time
    this->header[4] = 0x01;
    // I guess since this is MIPS this should be big-endian... but
    // it might work out more convenient to change this and make a 
    // non-compliant implementation
    this->header[5] = 0x2;      // big-endian for now
    this->header[6] = 0x1;
    // set to System V for now 
    this->header[7] = 0x0;
    // TODO: what is the type going to be?
    this->header[0x10] = 0;
    this->header[0x12] = 0x8;   // MIPS machine 
}

void ELFFileHeader::setProgStart(const uint32_t val)
{
    this->header[0x1C] =  val & 0x000000FF;
    this->header[0x1D] = (val & 0x0000FF00) >> 8;
    this->header[0x1E] = (val & 0x00FF0000) >> 16;
    this->header[0x1F] = (val & 0xFF000000) >> 24;
}

void ELFFileHeader::setProgTableStart(const uint32_t val)
{
    this->header[0x20] =  val & 0x000000FF;
    this->header[0x21] = (val & 0x0000FF00) >> 8;
    this->header[0x22] = (val & 0x00FF0000) >> 16;
    this->header[0x23] = (val & 0xFF000000) >> 24;
}

void ELFFileHeader::setSectionStart(const uint32_t val)
{
    this->header[0x24] =  val & 0x000000FF;
    this->header[0x25] = (val & 0x0000FF00) >> 8;
    this->header[0x26] = (val & 0x00FF0000) >> 16;
    this->header[0x27] = (val & 0xFF000000) >> 24;
}

void ELFFileHeader::setNumSectionHeaders(const uint16_t val)
{
    this->header[0x30] = val & 0x00FF;
    this->header[0x31] = (val & 0xFF00) >> 8;
}


// ELF Program Header
ELFProgramHeader::ELFProgramHeader()
{
    // init all header bytes
    for(int i = 0; i < 0x20; ++i)
        this->header[i] = 0;
}

void ELFProgramHeader::setType(const uint32_t val)
{
    this->header[0x00] =  val & 0x000000FF;
    this->header[0x01] = (val & 0x0000FF00) >> 8;
    this->header[0x02] = (val & 0x00FF0000) >> 16;
    this->header[0x03] = (val & 0xFF000000) >> 24;
}

void ELFProgramHeader::setOffset(const uint32_t val)
{
    this->header[0x04] =  val & 0x000000FF;
    this->header[0x05] = (val & 0x0000FF00) >> 8;
    this->header[0x06] = (val & 0x00FF0000) >> 16;
    this->header[0x07] = (val & 0xFF000000) >> 24;
}

void ELFProgramHeader::setVirtAddr(const uint32_t val)
{
    this->header[0x08] =  val & 0x000000FF;
    this->header[0x09] = (val & 0x0000FF00) >> 8;
    this->header[0x0A] = (val & 0x00FF0000) >> 16;
    this->header[0x0B] = (val & 0xFF000000) >> 24;
}

void ELFProgramHeader::setFileSize(const uint32_t val)
{
    this->header[0x10] =  val & 0x000000FF;
    this->header[0x11] = (val & 0x0000FF00) >> 8;
    this->header[0x12] = (val & 0x00FF0000) >> 16;
    this->header[0x13] = (val & 0xFF000000) >> 24;
}




// ELF Section Header
ELFSectionHeader::ELFSectionHeader()
{
    for(int i = 0; i < 0x28; ++i)
        this->header[i] = 0;
}

void ELFSectionHeader::setNameOffset(const uint32_t val)
{
    this->header[0x0] =  val & 0x000000FF;
    this->header[0x1] = (val & 0x0000FF00) >> 8;
    this->header[0x2] = (val & 0x00FF0000) >> 16;
    this->header[0x3] = (val & 0xFF000000) >> 24;
}

void ELFSectionHeader::setType(const uint32_t val)
{
    this->header[0x4] =  val & 0x000000FF;
    this->header[0x5] = (val & 0x0000FF00) >> 8;
    this->header[0x6] = (val & 0x00FF0000) >> 16;
    this->header[0x7] = (val & 0xFF000000) >> 24;
}

void ELFSectionHeader::setFlags(const uint32_t val)
{
    this->header[0x08] =  val & 0x000000FF;
    this->header[0x09] = (val & 0x0000FF00) >> 8;
    this->header[0x0A] = (val & 0x00FF0000) >> 16;
    this->header[0x0B] = (val & 0xFF000000) >> 24;
}

void ELFSectionHeader::setOffset(const uint32_t val)
{
    this->header[0x10] =  val & 0x000000FF;
    this->header[0x11] = (val & 0x0000FF00) >> 8;
    this->header[0x12] = (val & 0x00FF0000) >> 16;
    this->header[0x13] = (val & 0xFF000000) >> 24;
}

void ELFSectionHeader::setSize(const uint32_t val)
{
    this->header[0x14] =  val & 0x000000FF;
    this->header[0x15] = (val & 0x0000FF00) >> 8;
    this->header[0x16] = (val & 0x00FF0000) >> 16;
    this->header[0x17] = (val & 0xFF000000) >> 24;
}



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
    return this->section_headers.size();
}

unsigned int Object::getNumProgramHeaders(void) const
{
    return this->program_headers.size();
}
