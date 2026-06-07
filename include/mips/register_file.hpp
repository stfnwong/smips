// Register file

#pragma once

#include "pipeline_types.hpp"
#include <cstdio>


class RegisterFile { 
    private:
        uint32_t regs[NUM_REGS];

    public:
        RegisterFile() {
            for(size_t i = 0; i < NUM_REGS; ++i) {
                regs[i] = 0;
            }
        }

        uint32_t read(uint8_t addr) const { 
            if (addr >= NUM_REGS)
                return 0;

            return (addr == 0) ? 0 : regs[addr];
        }

        void write(uint8_t addr, uint8_t value) {
            if (addr > 0 && addr < NUM_REGS) { 
                regs[addr] = value;
            }
        }

        // Dump (for debugging) 
        void dump(void) const { 
            for (size_t i = 0; i < NUM_REGS; i += 4) {
                printf("r%-2zu: %08X  r%-2zu: %08X  r%-2zu: %08X  r%-2zu: %08X\n", i,
                regs[i], i + 1, regs[i + 1], i + 2, regs[i + 2], i + 3,
                regs[i + 3]);
            }
        }
};
