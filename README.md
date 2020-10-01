# SMIPS
MIPS emulator and binary tools 

[![Build Status](https://travis-ci.org/stfnwong/smips.svg?branch=master)](https://travis-ci.org/stfnwong/smips)

## Features
- Assembler
- Disassembler

## Planned features 
- The actual emulation.
- An RTL verison with simulations, etc.


## Instructions 
The assembler and disassembler support the following instructions 

### I-instructions
- addi
- andi
- ori
- lui
- lw
- sw
- bgt
- blt
- bge
- ble
- bgtu
- beq 
- beqz
- bne
- bgtz
- blex
- la
- li

### R-instructions
- add
- and
- or
- div
- divu
- mult
- multu
- sub
- subu
- slt
- sltu
- sltiu
- sll

### J-instructions
- j
- jal

## Assembler directives 
The following directives are supported 

- `.asciiz`
- `.byte`
- `.space`
- `.word` 
- `.text`
- `.data`

This assembler implements psuedo-instructions as per [this](https://github.com/MIPT-ILab/mipt-mips/wiki/MIPS-pseudo-instructions) page.

## TODOS
- Finish binary tools
- Write MIPS emulation core
- Do an RTL version, maybe in VHDL.
