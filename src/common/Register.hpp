/*
 * REGISTER
 * Conversion between register strings and register numbers
 *
 * Stefan Wong 2020
 */

#ifndef __SMIPS_REGISTER_HPP
#define __SMIPS_REGISTER_HPP

#include <string>
#include <unordered_map>


typedef enum MIPS_REG
{
    REG_ZERO,     // $zero : 0
    REG_AT,       // $at   : 1
    REG_RETURN_0, // $v0   : 2
    REG_RETURN_1, // $v1   : 3
    REG_ARG_0,    // $a0   : 4
    REG_ARG_1,    // $a1   : 5
    REG_ARG_2,    // $a2   : 6
    REG_ARG_3,    // $a3   : 7
    REG_TEMP_0,   // $t0   : 8
    REG_TEMP_1,   // $t1   : 9
    REG_TEMP_2,   // $t2   : 10
    REG_TEMP_3,   // $t3   : 11
    REG_TEMP_4,   // $t4   : 12
    REG_TEMP_5,   // $t5   : 13
    REG_TEMP_6,   // $t6   : 14
    REG_TEMP_7,   // $t7   : 15
    REG_SAVED_0,  // $s0   : 16
    REG_SAVED_1,  // $s1   : 17
    REG_SAVED_2,  // $s2   : 18
    REG_SAVED_3,  // $s3   : 19
    REG_SAVED_4,  // $s4   : 20
    REG_SAVED_5,  // $s5   : 21
    REG_SAVED_6,  // $s6   : 22
    REG_SAVED_7,  // $s7   : 23
    REG_TEMP_8,   // $t8   : 24
    REG_TEMP_9,   // $t9   : 25
    REG_KERNEL_0, // $k0   : 26
    REG_KERNEL_1, // $k1   : 27
    REG_GLOBAL,   // $gp   : 28
    REG_STACK,    // $sp   : 29
    REG_FRAME,    // $fp   : 30
    REG_RETURN,   // $ra   : 31
} MIPS_REG;


/*
 * Register
 * Structure representation of a named register
 * in the MIPS processor, based on the layout in
 * (https://en.wikibooks.org/wiki/MIPS_Assembly/Register_File)
 */
struct Register
{
    int idx;
    std::string repr;

    public:
        Register();
        Register(int i, const std::string& n);
        Register(const Register& that) = default;

        bool operator==(const Register& that) const;
        bool operator!=(const Register& that) const;
        Register& operator=(const Register& that) ;
        void init(void);
        std::string toString(void) const;
};

/*
 * RegisterMap
 * Wraps two unordered maps that related register strings to register positions
 */
struct RegisterMap
{
    std::string invalid_name = "$?";
    int invalid_idx = -1;
    std::unordered_map<std::string, int> reg_name_to_reg_idx;
    std::unordered_map<int, std::string> reg_idx_to_reg_name;

    public:
        RegisterMap();
        std::string getName(const int idx) ;
        int getIdx(const std::string& name) ;
        unsigned int size(void) const;
};


const Register mips_reg_types[] = {
    Register(REG_ZERO, "$zero"),
    Register(REG_AT, "$at"),
    Register(REG_RETURN_0, "$v0"),
    Register(REG_RETURN_1, "$v1"),
    // function args
    Register(REG_ARG_0, "$a0"),
    Register(REG_ARG_1, "$a1"),
    Register(REG_ARG_2, "$a2"),
    Register(REG_ARG_3, "$a3"),
    // temp registers
    Register(REG_TEMP_0, "$t0"),
    Register(REG_TEMP_1, "$t1"),
    Register(REG_TEMP_2, "$t2"),
    Register(REG_TEMP_3, "$t3"),
    Register(REG_TEMP_4, "$t4"),
    Register(REG_TEMP_5, "$t5"),
    Register(REG_TEMP_6, "$t6"),
    Register(REG_TEMP_7, "$t7"),
    // saved registers
    Register(REG_SAVED_0, "$s0"),
    Register(REG_SAVED_1, "$s1"),
    Register(REG_SAVED_2, "$s2"),
    Register(REG_SAVED_3, "$s3"),
    Register(REG_SAVED_4, "$s4"),
    Register(REG_SAVED_5, "$s5"),
    Register(REG_SAVED_6, "$s6"),
    Register(REG_SAVED_7, "$s7"),
    // more temp
    Register(REG_TEMP_8, "$t8"),
    Register(REG_TEMP_9, "$t9"),
    // kernel registers
    Register(REG_KERNEL_0, "$k0"),
    Register(REG_KERNEL_1, "$k1"),
    // global
    Register(REG_GLOBAL, "$gp"),
    Register(REG_STACK,  "$sp"),
    Register(REG_FRAME,  "$fp"),
    Register(REG_RETURN, "$ra"),
};

#endif /*__SMIPS_REGISTER_HPP*/
