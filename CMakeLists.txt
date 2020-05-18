# CMAKE config for SMIPS
#
# Stefan Wong 2020


cmake_minimum_required(VERSION 2.8)
project(smips)

# Source directories 
include_directories(src)
file(GLOB SOURCES "src/*cpp")

# Unit test executables 
enable_testing()
find_package(GTest, REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})
add_executable(test_lexer, test_lexer.cpp)

add_test(
    NAME lexer
    COMMAND test_lexer
)
