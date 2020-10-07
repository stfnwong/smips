# SMIPS MIPS emulator Makefile
# Stefan Wong 2018
#

# OUTPUT DIRS
BIN_DIR=bin
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

# Sources, etc
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
TEST_SOURCES := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS := $(patsubst $(TEST_DIR)/%.cpp,obj/%.o,$(TEST_SOURCES))

$(TEST_OBJECTS): obj/%.o: test/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ 

### ==== TEST TARGETS ==== #
TESTS=test_elf test_assembler test_opcode test_lexer test_register \
	  test_source test_disassembler test_program test_state test_datacache
$(TESTS): $(OBJECTS) $(TEST_OBJECTS) 
	$(CXX) $(LDFLAGS) $(INCS) $(OBJECTS)  obj/$@.o\
		-o $(TEST_BIN_DIR)/$@ $(LIBS) $(TEST_LIBS)


### ===== TOOL TARGETS ===== # 
## Tools (program entry points)
TOOL_SOURCES := $(wildcard $(TOOL_DIR)/*.cpp)
TOOL_OBJECTS := $(patsubst $(TOOL_DIR)/%.cpp,obj/%.o,$(TOOL_SOURCES))

$(TOOL_OBJECTS): obj/%.o : tools/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

TOOLS=assem disassem

$(TOOLS): $(OBJECTS) $(TOOL_OBJECTS)
	$(CXX) $(LDFLAGS) $(INCS) $(OBJECTS)  obj/$@.o\
		-o $(BIN_DIR)/$@ $(LIBS) 


# Main targets 
all : test tools

test : $(OBJECTS) $(TESTS)

tools : $(OBJECTS) $(TOOLS)

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
