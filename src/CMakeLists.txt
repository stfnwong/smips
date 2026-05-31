# ==== Core emulator library 
add_library(mips_emulator
    instruction.cpp
    instruction_db.cpp
    # Old stuff - to be removed 
    Program.cpp
    Object.cpp
    Data.cpp
    Lexer.cpp
    Segment.cpp
    Source.cpp
    Opcode.cpp
    Register.cpp
    State.cpp
    Assembler.cpp
    Disassembler.cpp
    ELF.cpp
)


# Canonical target names: project::library
add_library(MipsEmulator::mips_emulator ALIAS mips_emulator)

target_include_directories(mips_emulator
    PUBLIC
        # When building: use include/ from source tree 
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        # When installed, use include/ from source tree
        $<INSTALL_INTERFACE:include>
    PRIVATE
        # Private headers for the library internally 
        ${CMAKE_CURRENT_SOURCE_DIR}/detail
)

target_link_libraries(mips_emulator
    PRIVATE
        mips_warnings
        mips_sanitizers
)

# Per-target compile definitions 
target_compile_definitions(mips_emulator
    PRIVATE
        mips_emulator_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        mips_emulator_VERSION_MINOR=${PROJECT_VERSION_MINOR}
)
