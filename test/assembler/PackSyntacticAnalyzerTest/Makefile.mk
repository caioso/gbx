CC = clang++
LD = ld

LDFLAGS = $(LDCOVERAGE_FLAGS)
CPPFLAGS = $(CCCOVERAGE_FLAGS) $(GLOBAL_CPP_FLAGS)
INCLUDE = -I$(INCLUDE_ASSEMBLER_TOP) -I$(INCLUDE_ASSEMBLER_FRONTEND_TOP) -I$(INCLUDE_ASSEMBLER_FRONTEND_PARSERS) \
		  -I$(INCLUDE_ASSEMBLER_FRONTEND_PASSES) -I$(INCLUDE_ASSEMBLER_INTERFACES) -I$(INCLUDE_ASSEMBLER_INTERMEDIATE_REPRESENTATION) \
		  -I$(INCLUDE_ASSEMBLER_STREAMS) -I$(INCLUDE_ASSEMBLER_UTILITIES) -I$(TEST_UTILS)

SRC_FILES = $(notdir $(wildcard ./*.cc)) $(notdir $(wildcard */*.cc))
OBJ_FILES = $(patsubst %.cc,$(BUILD_TEMP)/%.o,$(SRC_FILES))
DEP_FILES = $(patsubst %.o,%.d,$(OBJ_FILES))

# Add test dependencies here (.cc files only)
MODULES = LexicalAnalyzer GBXAsmExceptions PACKSyntacticAnalyzer Construction PACKIntermediateRepresentation DeclaredMember
MODULES_DEPS = $(addsuffix .o, $(addprefix $(BUILD_TEMP)/,$(MODULES)))

.PHONY: all

all: $(OBJ_FILES) $(MODULES_DEPS)

-include $(DEP_FILES)
$(BUILD_TEMP)/%.o: $(CURDIR)/%.cc $(MODULES_DEPS)
	$(CC) $(INCLUDE) $(CPPFLAGS) -MMD -MT"$@" -c $< -o $@