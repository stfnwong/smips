/*
 * SEGMENT
 * Trying to create 'segments' to make the conversion from 'IR' to 
 * binary simpler.
 *
 * Stefan Wong 2019
 */


#include "Segment.hpp"


Segment::Segment() : base_addr(0), type(SEG_NONE) {} 

Segment::Segment(uint32_t base, segment_t type)
{
    this->base_addr = base;
    this->type = type;
}

