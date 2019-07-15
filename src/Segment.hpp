/*
 * SEGMENT
 * Trying to create 'segments' to make the conversion from 'IR' to 
 * binary simpler.
 *
 * Stefan Wong 2019
 */

#ifndef __SEGMENT_HPP
#define __SEGMENT_HPP

#include <cstdint>

typedef enum segment_t
{
    SEG_NONE,
    SEG_TEXT,
    SEG_CODE,
    SEG_DATA,
    SEG_ZERO,
    SEG_BSS,
} segment_t;



/*
 * Segment
 * Represents a segment in the program binary
 */
struct Segment
{
    uint32_t base_addr;
    segment_t type;

    public:
        Segment();
        Segment(uint32_t base, segment_t type);
        // TODO : equality?
};


#endif /*__SEGMENT_HPP*/
