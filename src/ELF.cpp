/*
 * ELF
 * Stuff for dealing with ELF Files
 * Stefan Wong 2019
 */

#include "ELF.hpp"


// ELF file header stuff
ELFFileHeader::ELFFileHeader()
{
    // zero out all bytes
    for(int i = 0; i < 0x34; ++i)
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

// getters

uint8_t ELFFileHeader::get_e_type(void) const
{
    return this->header[0x10];
}

uint32_t ELFFileHeader::get_e_version(void) const
{
    uint32_t version = this->header[0x14] + 
        (this->header[0x15] << 8) +
        (this->header[0x16] << 16) + 
        (this->header[0x17] << 24);
    return version;
}

uint32_t ELFFileHeader::get_e_entry(void) const
{
    uint32_t offset = this->header[0x18] + 
        (this->header[0x19] << 8) + 
        (this->header[0x1A] << 16) + 
        (this->header[0x1B] << 24);

    return offset;
}

uint32_t ELFFileHeader::get_e_phoff(void) const
{
    uint32_t entry = this->header[0x1C] + 
        (this->header[0x1D] << 8) +
        (this->header[0x1E] << 16) + 
        (this->header[0x1F] << 24);

    return entry;
}

uint32_t ELFFileHeader::get_e_shoff(void) const
{
    uint32_t offset = this->header[0x20] + 
        (this->header[0x21] << 8) + 
        (this->header[0x22] << 16) + 
        (this->header[0x23] << 24);

    return offset;
}

uint32_t ELFFileHeader::get_e_flags(void) const
{
    uint32_t flags = this->header[0x24] + 
        (this->header[0x25] << 8) + 
        (this->header[0x26] << 16) + 
        (this->header[0x27] << 24);
    return flags;
}

uint16_t ELFFileHeader::get_e_ehsize(void) const
{
    uint16_t size = this->header[0x28] +
        (this->header[0x29] << 8);
    return size;
}

uint16_t ELFFileHeader::get_e_phentsize(void) const
{
    uint16_t size = this->header[0x2A] + 
        (this->header[0x2B] << 8);
    return size;
}

uint16_t ELFFileHeader::get_e_phnum(void) const
{
    uint16_t num = this->header[0x2C] + 
        (this->header[0x2D] << 8);
    return num;
}
  
uint16_t ELFFileHeader::get_e_shentsize(void) const
{
    uint16_t size = this->header[0x2E] + 
        (this->header[0x2F] << 8);
    return size;
}

uint16_t ELFFileHeader::get_e_shnum(void) const
{
    uint16_t num = this->header[0x30] + 
        (this->header[0x31] << 8);
    return num;
}

uint16_t ELFFileHeader::get_e_shstrndx(void) const
{
    uint16_t num = this->header[0x32] + 
        (this->header[0x33] << 8);
    return num;
}

// setters 
void ELFFileHeader::set_e_entry(const uint32_t val)
{
    this->header[0x18] =  val & 0x000000FF;
    this->header[0x19] = (val & 0x0000FF00) >> 8;
    this->header[0x1A] = (val & 0x00FF0000) >> 16;
    this->header[0x1B] = (val & 0xFF000000) >> 24;
}

void ELFFileHeader::set_e_phoff(const uint32_t val)
{
    this->header[0x1C] =  val & 0x000000FF;
    this->header[0x1D] = (val & 0x0000FF00) >> 8;
    this->header[0x1E] = (val & 0x00FF0000) >> 16;
    this->header[0x1F] = (val & 0xFF000000) >> 24;
}

void ELFFileHeader::set_e_shoff(const uint32_t val)
{
    this->header[0x20] =  val & 0x000000FF;
    this->header[0x21] = (val & 0x0000FF00) >> 8;
    this->header[0x22] = (val & 0x00FF0000) >> 16;
    this->header[0x23] = (val & 0xFF000000) >> 24;
}

void ELFFileHeader::set_e_flags(const uint32_t val)
{
    this->header[0x24] =  val & 0x000000FF;
    this->header[0x25] = (val & 0x0000FF00) >> 8;
    this->header[0x26] = (val & 0x00FF0000) >> 16;
    this->header[0x27] = (val & 0xFF000000) >> 24;
}

void ELFFileHeader::set_e_ehsize(const uint16_t val)
{
    this->header[0x28] = val & 0x00FF;
    this->header[0x29] = (val & 0xFF00) >> 8;
}

void ELFFileHeader::set_e_phentsize(const uint16_t val)
{
    this->header[0x2A] = val & 0x00FF;
    this->header[0x2B] = (val & 0xFF00) >> 8;
}

void ELFFileHeader::set_e_phnum(const uint16_t val)
{
    this->header[0x2C] = val & 0x00FF;
    this->header[0x2D] = (val & 0xFF00) >> 8;
}

void ELFFileHeader::set_e_shentsize(const uint16_t val)
{
    this->header[0x2E] = val & 0x00FF;
    this->header[0x2F] = (val & 0xFF00) >> 8;
}

void ELFFileHeader::set_e_shnum(const uint16_t val)
{
    this->header[0x30] = val & 0x00FF;
    this->header[0x31] = (val & 0xFF00) >> 8;
}

void ELFFileHeader::set_e_shstrndx(const uint16_t val)
{
    this->header[0x32] = val & 0x00FF;
    this->header[0x33] = (val & 0xFF00) >> 8;
}

bool ELFFileHeader::operator==(const ELFFileHeader& that) const
{
    return this->header == that.header;
}

bool ELFFileHeader::operator!=(const ELFFileHeader& that) const
{
    return this->header != that.header;
}


// ELF Program Header
ELFProgramHeader::ELFProgramHeader()
{
    // init all header bytes
    for(int i = 0; i < 0x20; ++i)
        this->header[i] = 0;
}

// getters
uint32_t ELFProgramHeader::get_p_type(void) const
{
    uint32_t type = this->header[0x00] + 
        (this->header[0x1] << 8) + 
        (this->header[0x2] << 16) + 
        (this->header[0x3] << 24);
    return type;
}

uint32_t ELFProgramHeader::get_p_offset(void) const
{
    uint32_t offset = this->header[0x4] +
        (this->header[0x5] << 8) +
        (this->header[0x6] << 16) + 
        (this->header[0x7] << 24);
    return offset;
}

uint32_t ELFProgramHeader::get_p_vaddr(void) const
{
    uint32_t addr = this->header[0x08] +
        (this->header[0x9] << 8) + 
        (this->header[0xA] << 16) + 
        (this->header[0xB] << 24);
    return addr;
}

uint32_t ELFProgramHeader::get_p_paddr(void) const
{
    uint32_t addr = this->header[0x0C] +
        (this->header[0xD] << 8) + 
        (this->header[0xE] << 16) + 
        (this->header[0xF] << 24);
    return addr;
}

uint32_t ELFProgramHeader::get_p_filesz(void) const
{
    uint32_t addr = this->header[0x10] +
        (this->header[0x11] << 8) + 
        (this->header[0x12] << 16) + 
        (this->header[0x13] << 24);
    return addr;
}

uint32_t ELFProgramHeader::get_p_memsz(void) const
{
    uint32_t addr = this->header[0x14] +
        (this->header[0x15] << 8) + 
        (this->header[0x16] << 16) + 
        (this->header[0x17] << 24);
    return addr;
}

uint32_t ELFProgramHeader::get_p_flags(void) const
{
    uint32_t addr = this->header[0x18] +
        (this->header[0x19] << 8) + 
        (this->header[0x1A] << 16) + 
        (this->header[0x1B] << 24);
    return addr;
}

uint32_t ELFProgramHeader::get_p_align(void) const
{
    uint32_t addr = this->header[0x1C] +
        (this->header[0x1D] << 8) + 
        (this->header[0x1E] << 16) + 
        (this->header[0x1F] << 24);
    return addr;
}

// setters 
void ELFProgramHeader::set_p_type(const uint32_t val)
{
    this->header[0x00] =  val & 0x000000FF;
    this->header[0x01] = (val & 0x0000FF00) >> 8;
    this->header[0x02] = (val & 0x00FF0000) >> 16;
    this->header[0x03] = (val & 0xFF000000) >> 24;
}

void ELFProgramHeader::set_p_offset(const uint32_t val)
{
    this->header[0x04] =  val & 0x000000FF;
    this->header[0x05] = (val & 0x0000FF00) >> 8;
    this->header[0x06] = (val & 0x00FF0000) >> 16;
    this->header[0x07] = (val & 0xFF000000) >> 24;
}

void ELFProgramHeader::set_p_vaddr(const uint32_t val)
{
    this->header[0x08] =  val & 0x000000FF;
    this->header[0x09] = (val & 0x0000FF00) >> 8;
    this->header[0x0A] = (val & 0x00FF0000) >> 16;
    this->header[0x0B] = (val & 0xFF000000) >> 24;
}

void ELFProgramHeader::set_p_paddr(const uint32_t val)
{
    this->header[0x0C] =  val & 0x000000FF;
    this->header[0x0D] = (val & 0x0000FF00) >> 8;
    this->header[0x0E] = (val & 0x00FF0000) >> 16;
    this->header[0x0F] = (val & 0xFF000000) >> 24;
}

void ELFProgramHeader::set_p_filesz(const uint32_t val)
{
    this->header[0x10] =  val & 0x000000FF;
    this->header[0x11] = (val & 0x0000FF00) >> 8;
    this->header[0x12] = (val & 0x00FF0000) >> 16;
    this->header[0x13] = (val & 0xFF000000) >> 24;
}

void ELFProgramHeader::set_p_memsz(const uint32_t val)
{
    this->header[0x14] =  val & 0x000000FF;
    this->header[0x15] = (val & 0x0000FF00) >> 8;
    this->header[0x16] = (val & 0x00FF0000) >> 16;
    this->header[0x17] = (val & 0xFF000000) >> 24;
}

void ELFProgramHeader::set_p_flags(const uint32_t val)
{
    this->header[0x18] =  val & 0x000000FF;
    this->header[0x19] = (val & 0x0000FF00) >> 8;
    this->header[0x1A] = (val & 0x00FF0000) >> 16;
    this->header[0x1B] = (val & 0xFF000000) >> 24;
}

void ELFProgramHeader::set_p_align(const uint32_t val)
{
    this->header[0x1C] =  val & 0x000000FF;
    this->header[0x1D] = (val & 0x0000FF00) >> 8;
    this->header[0x1E] = (val & 0x00FF0000) >> 16;
    this->header[0x1F] = (val & 0xFF000000) >> 24;
}

bool ELFProgramHeader::operator==(const ELFProgramHeader& that) const
{
    return this->header == that.header;
}

bool ELFProgramHeader::operator!=(const ELFProgramHeader& that) const
{
    return this->header != that.header;
}


// ELF Section Header
ELFSectionHeader::ELFSectionHeader()
{
    for(int i = 0; i < 0x28; ++i)
        this->header[i] = 0;
}

// getters
uint32_t ELFSectionHeader::get_sh_name(void) const
{
    uint32_t name = this->header[0x00] + 
        (this->header[0x01] << 8) +
        (this->header[0x02] << 16) + 
        (this->header[0x03] << 24);
    return name;
}

uint32_t ELFSectionHeader::get_sh_type(void) const
{
    uint32_t type = this->header[0x04] + 
        (this->header[0x05] << 8) +
        (this->header[0x06] << 16) + 
        (this->header[0x07] << 24);
    return type;
}

uint32_t ELFSectionHeader::get_sh_flags(void) const
{
    uint32_t flags = this->header[0x08] + 
        (this->header[0x09] << 8) +
        (this->header[0x0A] << 16) + 
        (this->header[0x0B] << 24);
    return flags;
}

uint32_t ELFSectionHeader::get_sh_addr(void) const
{
    uint32_t addr = this->header[0x0C] + 
        (this->header[0x0D] << 8) +
        (this->header[0x0E] << 16) + 
        (this->header[0x0F] << 24);
    return addr;
}

uint32_t ELFSectionHeader::get_sh_offset(void) const
{
    uint32_t offset = this->header[0x10] + 
        (this->header[0x11] << 8) +
        (this->header[0x12] << 16) + 
            (this->header[0x13] << 24);
        return offset;
}

uint32_t ELFSectionHeader::get_sh_size(void) const
{
    uint32_t offset = this->header[0x14] + 
        (this->header[0x15] << 8) +
        (this->header[0x16] << 16) + 
        (this->header[0x17] << 24);
    return offset;
}

uint32_t ELFSectionHeader::get_sh_link(void) const
{
    uint32_t offset = this->header[0x18] + 
        (this->header[0x19] << 8) +
        (this->header[0x1A] << 16) + 
        (this->header[0x1B] << 24);
    return offset;
}

uint32_t ELFSectionHeader::get_sh_info(void) const
{
    uint32_t offset = this->header[0x1C] + 
        (this->header[0x1D] << 8) +
        (this->header[0x1E] << 16) + 
        (this->header[0x1F] << 24);
    return offset;
}

uint32_t ELFSectionHeader::get_sh_addralign(void) const
{
    uint32_t offset = this->header[0x20] + 
        (this->header[0x21] << 8) +
        (this->header[0x22] << 16) + 
        (this->header[0x23] << 24);
    return offset;
}

uint32_t ELFSectionHeader::get_sh_entsize(void) const
{
    uint32_t offset = this->header[0x24] + 
        (this->header[0x25] << 8) +
        (this->header[0x26] << 16) + 
        (this->header[0x27] << 24);
    return offset;
}

// settters 
void ELFSectionHeader::set_sh_name(const uint32_t val)
{
    this->header[0x0] =  val & 0x000000FF;
    this->header[0x1] = (val & 0x0000FF00) >> 8;
    this->header[0x2] = (val & 0x00FF0000) >> 16;
    this->header[0x3] = (val & 0xFF000000) >> 24;
}

void ELFSectionHeader::set_sh_type(const uint32_t val)
{
    this->header[0x4] =  val & 0x000000FF;
    this->header[0x5] = (val & 0x0000FF00) >> 8;
    this->header[0x6] = (val & 0x00FF0000) >> 16;
    this->header[0x7] = (val & 0xFF000000) >> 24;
}

void ELFSectionHeader::set_sh_flags(const uint32_t val)
{
    this->header[0x08] =  val & 0x000000FF;
    this->header[0x09] = (val & 0x0000FF00) >> 8;
    this->header[0x0A] = (val & 0x00FF0000) >> 16;
    this->header[0x0B] = (val & 0xFF000000) >> 24;
}

void ELFSectionHeader::set_sh_addr(const uint32_t val)
{
    this->header[0xC] =  val & 0x000000FF;
    this->header[0xD] = (val & 0x0000FF00) >> 8;
    this->header[0xE] = (val & 0x00FF0000) >> 16;
    this->header[0xF] = (val & 0xFF000000) >> 24;
}

void ELFSectionHeader::set_sh_offset(const uint32_t val)
{
    this->header[0x10] =  val & 0x000000FF;
    this->header[0x11] = (val & 0x0000FF00) >> 8;
    this->header[0x12] = (val & 0x00FF0000) >> 16;
    this->header[0x13] = (val & 0xFF000000) >> 24;
}

void ELFSectionHeader::set_sh_size(const uint32_t val)
{
    this->header[0x14] =  val & 0x000000FF;
    this->header[0x15] = (val & 0x0000FF00) >> 8;
    this->header[0x16] = (val & 0x00FF0000) >> 16;
    this->header[0x17] = (val & 0xFF000000) >> 24;
}

void ELFSectionHeader::set_sh_link(const uint32_t val)
{
    this->header[0x18] =  val & 0x000000FF;
    this->header[0x19] = (val & 0x0000FF00) >> 8;
    this->header[0x1A] = (val & 0x00FF0000) >> 16;
    this->header[0x1B] = (val & 0xFF000000) >> 24;
}

void ELFSectionHeader::set_sh_info(const uint32_t val)
{
    this->header[0x1C] =  val & 0x000000FF;
    this->header[0x1D] = (val & 0x0000FF00) >> 8;
    this->header[0x1E] = (val & 0x00FF0000) >> 16;
    this->header[0x1F] = (val & 0xFF000000) >> 24;
}

void ELFSectionHeader::set_sh_addralign(const uint32_t val)
{
    this->header[0x20] =  val & 0x000000FF;
    this->header[0x21] = (val & 0x0000FF00) >> 8;
    this->header[0x22] = (val & 0x00FF0000) >> 16;
    this->header[0x23] = (val & 0xFF000000) >> 24;
}

void ELFSectionHeader::set_sh_entsize(const uint32_t val)
{
    this->header[0x24] =  val & 0x000000FF;
    this->header[0x25] = (val & 0x0000FF00) >> 8;
    this->header[0x26] = (val & 0x00FF0000) >> 16;
    this->header[0x27] = (val & 0xFF000000) >> 24;
}


bool ELFSectionHeader::operator==(const ELFSectionHeader& that) const
{
    return this->header == that.header;
}

bool ELFSectionHeader::operator!=(const ELFSectionHeader& that) const
{
    return this->header != that.header;
}
