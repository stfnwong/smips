/*
 * DATA
 * Represents the data region in memory 
 *
 * Stefan Wong 2019
 */

#include "Data.hpp"


MemData::MemData()
{
    this->init();
}

MemData::MemData(uint32_t a, uint32_t d)
{
    this->addr = a;
    this->data = d;
}


void MemData::init(void)
{
    this->addr = 0;
    this->data = 0;
}

bool MemData::operator==(const MemData& that) const
{
    if(this->addr != that.addr)
        return false;
    if(this->data != that.data)
        return false;
    return true;
}

bool MemData::operator!=(const MemData& that) const
{
    if(this->addr == that.addr)
        return false;
    if(this->data == that.data)
        return false;
    return true;
}

/*
 * MemLine()
 */
MemLine::MemLine()
{
    this->start_addr = 0;
}

MemLine::MemLine(const uint32_t start)
{
    this->start_addr = start;
}

void MemLine::init(void)
{
    this->start_addr = 0;
    this->data_words.clear();
}


// Data area
DataRegion::DataRegion() {} 



void DataRegion::init(void)
{
    this->data_elements.clear();
}
