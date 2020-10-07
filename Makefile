# SMIPS MIPS emulator Makefile
# Stefan Wong 2018
#

# OUTPUT DIRS
BIN_DIR=bin
#OBJ_DIR=obj
#SRC_DIR=src
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

MODULES  := cpu tools
SRC_DIR  := $(addprefix src/,$(MODULES))
OBJ_DIR  := $(addprefix obj/,$(MODULES))

SOURCES  := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJECTS  := $(patsubst src/%.cpp,obj/%.o,$(SOURCES))
INCLUDES := $(addprefix -I,$(SRC_DIR))

vpath %.cpp $(SRC_DIR)

.PHONY: clean cpu

# Implicit rule for making objects
define make-obj
$1/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $$< -o $$@
endef

# Unit tests 
TEST_SOURCES  = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS := $(patsubst $(TEST_DIR)/%.cpp,obj/%.o,$(TEST_SOURCES))

$(TEST_OBJECTS): obj/%.o: test/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ 

#TEST_OBJECTS  := $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(OBJ_DIR)/%.o)
#$(TEST_OBJECTS): %.o : $(TEST_DIR)/%.cpp
#	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $@ 

##
### ==== TEST TARGETS ==== #
TESTS=test_elf test_assembler test_opcode test_lexer test_register \
	  test_source test_disassembler test_program 
$(TESTS): $(OBJECTS) $(TEST_OBJECTS) 
	$(CXX) $(LDFLAGS) $(INCS) $(OBJECTS)  obj/$@.o\
		-o $(TEST_BIN_DIR)/$@ $(LIBS) $(TEST_LIBS)
#
### ===== TOOL TARGETS ===== # 
## Tools (program entry points)
#TOOLS=assem disassem
#TOOL_SOURCES = $(wildcard $(TOOL_DIR)/*.cpp)
#TOOL_OBJECTS  := $(TOOL_SOURCES:$(TOOL_DIR)/%.cpp=$(OBJ_DIR)/%.o)
#
#$(TOOL_OBJECTS) : $(OBJ_DIR)/%.o : $(TOOL_DIR)/%.cpp
#	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $@
#
#$(TOOLS): $(OBJECTS) $(TOOL_OBJECTS) 
#	$(CXX) $(LDFLAGS) $(OBJECTS) $(OBJ_DIR)/$@.o\
#		-o $(BIN_DIR)/$@ $(LIBS) $(TEST_LIBS)

# Main targets 
all : test tools

test : $(OBJECTS) $(TESTS)

tools : $(TOOLS)

assem : $(ASSEM_OBJECTS)

obj: $(OBJECTS)

clean:
	rm -fv obj/cpu/*.o
	rm -fv obj/tools/*.o
	rm -fv obj/*.o
	rm -rfv $(TEST_BIN_DIR)/test_*

print-%:
	@echo $* = $($*)


$(foreach dir,$(OBJ_DIR),$(eval $(call make-obj,$(dir))))
