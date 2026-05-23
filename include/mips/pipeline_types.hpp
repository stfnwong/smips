// Pipeline types
#pragma once 

#include <cstdint>
#include <optional>
#include <variant>


// Architectural constants 
constexpr size_t NUM_REGS = 32;
constexpr size_t CACHE_LINE_SIZE = 64;

// ALU operations
enum class ALUOp {
    Add, Sub, And, Or, Xor, Slt, Sll, Srl, Sra, Nor
};


// Memory operations 
enum class MemOp { 
    Load, Store
};

// Decoded instruction types 
enum class Opcode { 
    RType, Addi, Lw, Sw, Beq, Bne, J, Jal,
};   // TODO: more instructions - look the rest up


// Branch condition 
enum class BranchCond { 
    Equal, NotEqual, LessThan, GreaterEqual
};


// TODO: branch prediction info 
struct BranchInfo {
    uint32_t pc;
    uint32_t target;
    BranchCond cond;
    bool predicted_token;
};


// Pipeline register IF/ID
struct IfIdLatch {
    bool valid = false;
    uint32_t pc = 0;
    uint32_t instr = 0;

    void invalidate(void) { valid = false; }

    void clear(void) {
        valid = false;
        pc = 0;
        instr = 0;
    }
};

// Pipeline register ID/EX 
struct IdExLatch {
    bool valid = false;
    uint32_t pc = 0;
    Opcode opcode;

    // Register operands (values and addresses)
    uint32_t rs_val = 0;
    uint32_t rt_val = 0;
    uint32_t rs_addr = 0;
    uint32_t rt_addr = 0;
    uint32_t rd_addr = 0;

    // Immediate / offset
    uint32_t immediate = 0;

    // Control signals
    ALUOp alu_op;
    std::optional<MemOp> mem_op;
    std::optional<uint8_t> wb_dest;  // which register to write back to?
    bool mem_to_reg = false;         // writeback from memory or ALU?

    // Branch info
    std::optional<BranchInfo> branch_info;

    void invalidate(void) { valid = false; }

    void clear(void) {
        valid = false;
        pc = 0;
        rs_val = 0;
        rt_val = 0;
        rs_addr = 0;
        rt_addr = 0;
        rd_addr = 0;
        mem_op.reset();
        wb_dest.reset();
        branch_info.reset();
    }
};


// Pipeline register EX/MEM
struct ExMemLatch {
    bool valid = false;
    uint32_t pc = 0;

    // ALU result 
    uint32_t alu_result = 0;

    // Memory operation 
    std::optional<MemOp> mem_op;
    uint32_t store_data = 0;     // for stores: data to write 

    // Writeback control
    std::optional<uint8_t> wb_dest;
    bool mem_to_reg = false;

    // Branch resolution
    bool branch_taken = false;
    uint32_t branch_target = 0;

    void invalidate(void) { valid = false; }

    void clear(void) {
        valid = false;
        pc = 0;
        alu_result = 0;
        mem_op.reset();
        store_data = 0;
        wb_dest.reset();
        branch_taken = false;
    }
};


// Pipeline register MEM/WB
struct MemWbLatch {
    bool valid = false;

    // Data to write back 
    uint32_t data = 0;
    std::optional<uint8_t> wb_dest;

    void invalidate(void) { valid = false; }

    void clear(void) {
        valid = false;
        data = 0;
        wb_dest.reset();
    }
};


// Hazard control signals 
struct HazardSignals {
    bool stall_if = false;
    bool stall_id = false;
    bool stall_ex = false;
    bool flush_if = false;
    bool flush_id = false;
    bool flush_ex = false;
    std::optional<uint32_t> branch_target;

    void clear(void) {
        stall_if = false;
        stall_id = false;
        stall_ex = false;
        flush_if = false;
        flush_id = false;
        flush_ex = false;
        branch_target.reset();
    }
};
