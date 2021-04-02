$(info -------------------------------)
$(info [BUILD::GBX] Entering directory '$(CURDIR)')
$(info -------------------------------) 
CC = clang++
LD = ld

LDFLAGS = $(LDCOVERAGE_FLAGS)
CPPFLAGS = $(CCCOVERAGE_FLAGS) $(GLOBAL_CPP_FLAGS)
INCLUDE = -I$(INCLUDE_CORE_TOP) -I$(INCLUDE_CORE_INSTRUCTIONS) -I$(INCLUDE_CORE_INTERFACES) -I$(TEST_UTILS) -I$(INCLUDE_CORE_MEMORY) 

SRC_FILES = $(notdir $(wildcard ./*.cc)) $(notdir $(wildcard */*.cc))
OBJ_FILES = $(patsubst %.cc,$(BUILD_TEMP)/%.o,$(SRC_FILES))
DEP_FILES = $(patsubst %.o,%.d,$(OBJ_FILES))

# Add test dependencies here (.cc files only)
MODULES = MemoryController RegisterBank ArithmeticLogicUnit InstructionHalt
MODULES_DEPS = $(addsuffix .o, $(addprefix $(BUILD_TEMP)/,$(MODULES)))

.PHONY: all

all: $(OBJ_FILES) $(MODULES_DEPS)

-include $(DEP_FILES)
$(BUILD_TEMP)/%.o: $(CURDIR)/%.cc $(MODULES_DEPS)
	$(CC) $(INCLUDE) $(CPPFLAGS) -MMD -MT"$@" -c $< -o $@