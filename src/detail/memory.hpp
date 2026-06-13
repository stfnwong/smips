// Simple memory model 

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>


class Memory {
    private:
        static constexpr size_t MEM_SIZE = 1024 * 1024;   // 1MB
        uint8_t mem[MEM_SIZE];

    public:
        Memory() {
            this->reset();
        }

		void reset(void) { 
			memset(this->mem, 0, MEM_SIZE);
		}

        uint32_t read_word(uint32_t addr) const {
            if (addr + 3 >= MEM_SIZE) {
                throw std::runtime_error("Memory read out of bounds");
            }

            // Big endian 
            return (static_cast<uint32_t>(this->mem[addr]) << 24) | 
                   (static_cast<uint32_t>(this->mem[addr + 1]) << 16) |
                   (static_cast<uint32_t>(this->mem[addr + 2]) << 8) |
                   (static_cast<uint32_t>(this->mem[addr + 3]));
        }

        void write_word(uint32_t addr, uint32_t value) { 
            if (addr + 3 >= MEM_SIZE) {
                throw std::runtime_error("Memory write out of bounds");
            }

            this->mem[addr + 0] = (value >> 24) & 0xFF;
            this->mem[addr + 1] = (value >> 16) & 0xFF;
            this->mem[addr + 2] = (value >> 8) & 0xFF;
            this->mem[addr + 3] = value & 0xFF;
        }

        uint8_t read_byte(uint32_t addr) const {
            if (addr >= MEM_SIZE) {
                throw std::runtime_error("Memory read out of bounds");
            }

            return this->mem[addr];
        }

        void write_byte(uint32_t addr, uint8_t value) {
            if (addr >= MEM_SIZE) {
                throw std::runtime_error("Memory read out of bounds");
            }

            this->mem[addr] = value;
        }

        // Load a program into memory
        void load_program(const std::vector<uint32_t>& program, uint32_t start_addr = 0) {
            for (size_t i = 0; i < program.size(); ++i) {
                this->write_word(static_cast<uint32_t>(start_addr + i * 4), program[i]);
			}
		}
};
