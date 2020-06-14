/*
 * ADDRESS
 * Address ranges for MIPS assembler and emulator 
 *
 * Stefan Wong 2020
 */

#ifndef __SMIPS_ADDRESSS_HPP
#define __SMIPS_ADDRESSS_HPP

#include <cstdint>

constexpr int TEXT_START_ADDR  = 0x00400000;
constexpr int TEXT_END_ADDR    = 0x0FFFFFFF;
constexpr int DATA_START_ADDR  = 0x10000000;
constexpr int DATA_END_ADDR    = 0x7FFFFFFF;     

constexpr int STACK_START_ADDR = 0x7FFFFFFF;

constexpr int KTEXT_START_ADDR = 0x80000000;
constexpr int KTEXT_END_ADDR   = 0x8FFFFFFF;
constexpr int KDATA_START_ADDR = 0x90000000;
constexpr int KDATA_END_ADDR   = 0xFFFEFFFF;
// MIMO area in 0xFFFF0000 -> 0xFFFFFFFF


//static bool isDataAddr(uint32_t addr)
//{
//    return (addr >= DATA_START_ADDR && addr <= DATA_END_ADDR) ? true  : false;
//}
//
//static bool isTextAddr(uint32_t addr)
//{
//    return (addr >= TEXT_START_ADDR && addr <= TEXT_END_ADDR) ? true  : false;
//}



#endif /*__SMIPS_ADDRESSS_HPP*/
