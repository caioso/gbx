CC = clang++
LD = ld

LDFLAGS = $(LDCOVERAGE_FLAGS)
CPPFLAGS = $(CCCOVERAGE_FLAGS) $(GLOBAL_CPP_FLAGS)
INCLUDE = -I$(INCLUDE_RUNTIME_TOP) -I$(INCLUDE_DEBUGGER_TOP) -I$(INCLUDE_DEBUGGER_INTERFACES) -I$(INCLUDE_DEBUGGER_PROTOCOL) \
		  -I$(INCLUDE_RUNTIME_RUNNER) -I$(INCLUDE_DEBUGGER_TRANSPORT) -I$(INCLUDE_COMMONS_TOP) -I$(INCLUDE_CORE_INTERFACES) \
		  -I$(TEST_UTILS)

SRC_FILES = $(notdir $(wildcard ./*.cc)) $(notdir $(wildcard */*.cc))
OBJ_FILES = $(patsubst %.cc,$(BUILD_TEMP)/%.o,$(SRC_FILES))
DEP_FILES = $(patsubst %.o,%.d,$(OBJ_FILES))

# Add test dependencies here (.cc files only)
MODULES = CancellationToken
MODULES_DEPS = $(addsuffix .o, $(addprefix $(BUILD_TEMP)/,$(MODULES)))

.PHONY: all

all: $(OBJ_FILES) $(MODULES_DEPS)

-include $(DEP_FILES)
$(BUILD_TEMP)/%.o: $(CURDIR)/%.cc $(MODULES_DEPS)
	$(CC) $(INCLUDE) $(CPPFLAGS) -MMD -MT"$@" -c $< -o $@