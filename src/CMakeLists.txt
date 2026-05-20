# ==== Core emulator library 
add_library(mips_emulator
    instruction.cpp
)


# Canonical target names: project::library
add_library(MipsEmulator::mips_emulator ALIAS smips)

target_include_directories(smips
    PUBLIC
        # When building: use include/ from source tree 
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        # When installed, use include/ from source tree
        $<INSTALL_INTERFACE:include>
    PRIVATE
        # Private headers for the library internally 
        ${CMAKE_CURRENT_SOURCE_DIR}/detail


target_link_libraries(smips
    PRIVATE
        mips_warnings
        mips_sanitizers
)

# Per-target compile definitions 
target_compile_definitions(smips
    PRIVATE
        SMIPS_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        SMIPS_VERSION_MINOR=${PROJECT_VERSION_MINOR}
)
