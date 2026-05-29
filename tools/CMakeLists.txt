# Disassembler 
add_executable(disassem disassem.cpp)

target_link_libraries(disassem
    PRIVATE
        MipsEmulator::mips_emulator
        mips_warnings
)

install(TARGETS disassem RUNTIME DESTINATION bin)

# Assembler
add_executable(assem assem.cpp)

target_link_libraries(assem
    PRIVATE
        MipsEmulator::mips_emulator
        mips_warnings
)

install(TARGETS assem RUNTIME DESTINATION bin)

# Dependencies for CLIs
include(FetchContent)

FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG        v2.4.1
)
FetchContent_MakeAvailable(CLI11)

target_link_libraries(assem PRIVATE CLI11::CLI11)
target_link_libraries(disassem PRIVATE CLI11::CLI11)
