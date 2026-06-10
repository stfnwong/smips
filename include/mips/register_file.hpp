// Register file

#pragma once

#include "pipeline_types.hpp"
#include <cstdio>
#include <format>
#include <string>


class RegisterFile { 
    private:
        uint32_t regs[NUM_REGS];

    public:
        RegisterFile() {
			this->clear();
        }

        uint32_t read(uint8_t addr) const { 
            if (addr >= NUM_REGS)
                return 0;

            return (addr == 0) ? 0 : this->regs[addr];
        }

        void write(uint8_t addr, uint8_t value) {
            if (addr > 0 && addr < NUM_REGS) { 
                this->regs[addr] = value;
            }
        }

		void clear(void) {
			for( size_t i = 0; i < NUM_REGS; ++i) {
				this->regs[i] = 0;
			}
		}

		std::string format_reg_row(size_t i) const {
			return std::format("r{:<2}: {:08X}  r{:<2}: {:08X}  r{:<2}: {:08X}  r{:<2}: {:08X}\n",
				i,   this->regs[i],
				i+1, this->regs[i+1],
				i+2, this->regs[i+2],
				i+3, this->regs[i+3]
			);
		}

		std::string dump(void) const {
			std::string output;

			output.reserve(NUM_REGS * 20);
			for (size_t i = 0; i < NUM_REGS; i += 4)
				output += this->format_reg_row(i);

			return output;
		}
};
