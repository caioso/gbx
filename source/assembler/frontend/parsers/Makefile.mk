$(info -------------------------------)
$(info [BUILD::GBX] Entering directory '$(CURDIR)')
$(info -------------------------------)
CC = clang++

LDFLAGS = $(LDCOVERAGE_FLAGS)
CPPFLAGS = $(CCCOVERAGE_FLAGS) $(GLOBAL_CPP_FLAGS)
INCLUDE = -I$(INCLUDE_ASSEMBLER_TOP) -I$(INCLUDE_ASSEMBLER_FRONTEND_TOP) -I$(INCLUDE_ASSEMBLER_FRONTEND_PARSERS) \
		  -I$(INCLUDE_ASSEMBLER_FRONTEND_PASSES) -I$(INCLUDE_ASSEMBLER_INTERFACES) -I$(INCLUDE_ASSEMBLER_INTERMEDIATE_REPRESENTATION) \
		  -I$(INCLUDE_ASSEMBLER_STREAMS) -I$(INCLUDE_ASSEMBLER_UTILITIES) -I$(INCLUDE_CORE_INSTRUCTIONS)

SRC_FILES = $(notdir $(wildcard ./*.cc)) $(notdir $(wildcard */*.cc))
OBJ_FILES = $(patsubst %.cc,$(BUILD_TEMP)/%.o,$(SRC_FILES))
DEP_FILES = $(patsubst %.o,%.d,$(OBJ_FILES))

.PHONY: all

all: $(OBJ_FILES)

-include $(DEP_FILES)
$(BUILD_TEMP)/%.o: $(CURDIR)/%.cc
	$(CC) $(INCLUDE) $(CPPFLAGS) -MMD -MT"$@" -c $< -o $@ 