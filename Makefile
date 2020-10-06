# SMIPS MIPS emulator Makefile
# Stefan Wong 2018
#

# OUTPUT DIRS
BIN_DIR=bin
OBJ_DIR=obj
SRC_DIR=src
TEST_DIR=test
TEST_BIN_DIR=$(BIN_DIR)/test
TOOL_DIR=tools

# Tool options
CXX=g++
OPT=-O0
CXXFLAGS=-Wall -g2 -std=c++17 -D_REENTRANT $(OPT)
TESTFLAGS=
LDFLAGS=-pthread
LIBS =
TEST_LIBS=

# style for assembly output
ASM_STYLE=intel

# Object targets
INCS=-I$(SRC_DIR) 
INCS+=-I$(SRC_DIR)/cpu
INCS+=-I$(TEST_DIR)/catch
INCLUDES = $(wildcard $(SRC_DIR)/*.hpp) $(wildcard $(SRC_DIR)*/*.hpp)
SOURCES = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)cpu/*.cpp)
# Unit tests 
TEST_SOURCES  = $(wildcard $(TEST_DIR)/*.cpp)
# Tools (program entry points)
TOOL_SOURCES = $(wildcard $(TOOL_DIR)/*.cpp)

.PHONY: clean

.PHONY: cpu

# Objects 
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Toolchain objects

# CPU objects 
#CPU_DIR=$(SRC_DIR)/cpu
#CPU_SOURCES=$(wildcard $(CPU_SRC_DIR)/*.cpp)
#CPU_INCLUDES=$(wildcard $(CPU_SRC_DIR)/*.hpp)
#CPU_OBJECTS := $(SOURCES:$(CPU_SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
#$(CPU_OBJECTS): $(OBJ_DIR)/%.o : $(CPU_SRC_DIR)/%.cpp
#	$(CXX) $(CXXFLAGS) -c $< -o $@

# Objects, but output as assembly
$(ASSEM_OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -S $< -o $(OBJ_DIR)/$@.asm -masm=$(ASM_STYLE)


# Unit tests 
TEST_OBJECTS  := $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(OBJ_DIR)/%.o)
$(TEST_OBJECTS): $(OBJ_DIR)/%.o : $(TEST_DIR)/%.cpp 
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $@ 

# ==== TEST TARGETS ==== #
TESTS=test_elf test_assembler test_opcode test_lexer test_register \
	  test_source test_disassembler test_program 
$(TESTS): $(TEST_OBJECTS) $(OBJECTS)
	$(CXX) $(LDFLAGS) $(INCS) $(OBJECTS) $(OBJ_DIR)/$@.o\
		-o $(TEST_BIN_DIR)/$@ $(LIBS) $(TEST_LIBS)

## ===== TOOL TARGETS ===== # 
TOOLS=assem disassem
TOOL_OBJECTS  := $(TOOL_SOURCES:$(TOOL_DIR)/%.cpp=$(OBJ_DIR)/%.o)

$(TOOL_OBJECTS) : $(OBJ_DIR)/%.o : $(TOOL_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $@

$(TOOLS): $(OBJECTS) $(TOOL_OBJECTS) 
	$(CXX) $(LDFLAGS) $(OBJECTS) $(OBJ_DIR)/$@.o\
		-o $(BIN_DIR)/$@ $(LIBS) $(TEST_LIBS)

# Main targets 
all : test tools

test : $(TESTS)

tools : $(TOOLS)

assem : $(ASSEM_OBJECTS)

clean:
	rm -rfv *.o $(OBJ_DIR)/*.o 
	rm -rfv $(TEST_BIN_DIR)/test_*

print-%:
	@echo $* = $($*)
