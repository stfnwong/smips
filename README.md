# SMIPS
MIPS emulator and binary tools 

[![Build Status](https://travis-ci.org/stfnwong/smips.svg?branch=master)](https://travis-ci.org/stfnwong/smips)

## Features
- Assembler
- Disassembler
- CPU Emulation (C++)
- No RTL yet

## Assembler 
Basic usage is something like 

```bash
assem -i inp_file.asm -o out_file.bin
```

If `-o` is not present the output program is pretty-printed to the console. The assembler also accepts a `-v` argument (verbose) which sets verbose mode for both. Its possible to assemble a single string literal using `-l`

```bash
assem -l "beq $a0, $s2, 1024"
```
In bash this requires escaping each `$`. 
```bash
assem -l "beq \$a0, \$s2, 1024"
```


## Disassembler
Has basically the same interface. To disassemble a binary 
```bash
diassem -i input.bin 
```
The output is printed to the console. Adding the `-a` option will also print addresses. The disassembler also accepts hexadecimal literals with the `-l` option, for instance 

```bash
disassem -l 1d4001b
```
prints 
```bash
divu $t6, $s4,  
```


## Planned features 
- The actual emulation (not yet cycle accurate).
- An RTL verison with simulations, etc.


## Instructions 
The assembler and disassembler support the following instructions 

### I-instructions
- addi
- addiu
- andi
- ori
- xori
- lui
- lw
- sw
- bgt (psuedo-op)
- blt (psuedo-op)
- bge (psuedo-op)
- ble (psuedo-op)
- bgtu (psuedo-op)
- beq 
- beqz (psuedo-op)
- bne
- bgt (psuedo-op)
- bgtz (psuedo-op)
- la (psuedo-op)
- li (psuedo-op)

### R-instructions
- add
- and
- or
- div
- divu
- mul (psuedo-op)
- mult
- multu
- sub
- subu
- slt
- sltu
- sltiu
- sll
- xor

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

Although its a bit hit and miss with the disassembler as of this writing.

This assembler implements psuedo-instructions as per [this](https://github.com/MIPT-ILab/mipt-mips/wiki/MIPS-pseudo-instructions) page.

## TODOS
- Finish binary tools
- Write MIPS emulation core
- Do an RTL version, maybe in VHDL.
