# ==== Test executables 
function(add_mips_test name source)
    add_executable(${name} ${source})

    target_link_libraries(${name}
        PRIVATE
            MipsEmulator::mips_emulator
            mips_warnings
            mips_sanitizers
            Catch2::Catch2WithMain
    )

    include(Catch)
    catch_discover_tests(${name})
endfunction()

# TODO: add the tests
