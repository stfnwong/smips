/*
 * DATACACHE
 */

#include <cstdint>
#include <cstring>
#include "DataCache.hpp"


// ======== DATA CACHE ======== //

DataCache::DataCache()
{
    this->mem = new uint8_t[SMIPS_MEM_SIZE];
    this->mem_size = SMIPS_MEM_SIZE;
}

DataCache::DataCache(unsigned int size)
{
    this->mem = new uint8_t[size];
    this->mem_size = size;
}

DataCache::DataCache(const DataCache& that)
{
    this->mem_size = that.mem_size;
    this->mem = new uint8_t[this->mem_size];
    
    std::memcpy(this->mem, that.mem, sizeof(uint8_t) * this->mem_size);
}

DataCache::~DataCache()
{
    delete[] this->mem;
}

/*
 * size()
 */
unsigned int DataCache::size(void) const
{
    return this->mem_size;
}

/*
 * clear()
 */
void DataCache::clear(void)
{
    for(unsigned int i = 0; i < this->mem_size; ++i)
        this->mem[i] = 0;
}

/*
 * []
 */
uint8_t& DataCache::operator[](const int i)
{
    return this->mem[i];
}

/*
 * readHalf()
 */
int16_t DataCache::readHalf(const int i) const
{
    int16_t h = 0;

    h = h | (this->mem[i+0] << 8);
    h = h | (this->mem[i+1] << 0);

    return h;
}

/*
 * readWord()
 */
int32_t DataCache::readWord(const int i) const
{
    int32_t d = 0;

    d = d | (this->mem[i+0] << 24);
    d = d | (this->mem[i+1] << 16);
    d = d | (this->mem[i+2] << 8);
    d = d | (this->mem[i+3] << 0);

    return d;
}

