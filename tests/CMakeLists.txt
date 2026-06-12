FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.3
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Catch2)

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

	target_include_directories(${name}
		PRIVATE
			${PROJECT_SOURCE_DIR}/src    # should see src/detail/*.hpp
	)

    include(Catch)
    catch_discover_tests(${name})
endfunction()

# TODO: add the tests
add_mips_test(test_instruction  test_instruction.cpp)
add_mips_test(test_pipeline  test_pipeline.cpp)
