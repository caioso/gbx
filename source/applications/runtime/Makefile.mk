CC = clang++

LDFLAGS = $(LDCOVERAGE_FLAGS) -lboost_system -pthread
CPPFLAGS = $(CCCOVERAGE_FLAGS) $(GLOBAL_CPP_FLAGS)

INCLUDE = -I$(INCLUDE_TOP) -I$(INCLUDE_CORE_TOP) -I$(INCLUDE_CORE_CONSTANTS) \
		  -I$(INCLUDE_CORE_INSTRUCTIONS) -I$(INCLUDE_CORE_INTERFACES) -I$(INCLUDE_COMMONS_TOP) -I$(INCLUDE_RUNTIME_TOP) \
		  -I$(INCLUDE_DEBUGGER_TOP) -I$(INCLUDE_DEBUGGER_INTERFACES) -I$(INCLUDE_DEBUGGER_PROTOCOL) -I$(INCLUDE_RUNTIME_RUNNER) \
		  -I$(INCLUDE_DEBUGGER_TRANSPORT)
DEP_FILES = $(BUILD_TEMP)/gbx.d

TARGET_SOURCE = gbx.cc
TARGET = $(RUNTIME)

all: $(TARGET)

-include $(DEP_FILES)
$(TARGET): $(TARGET_SOURCE) $(CORE_LIB) $(COMMONS_LIB) $(RUNTIME_LIB)
	$(CC) -MMD -MT"$@" -MF"$(DEP_FILES)" $(INCLUDE) $(TARGET_SOURCE) $(CPPFLAGS) -o $(TARGET) $(CORE_LIB) $(COMMONS_LIB) $(RUNTIME_LIB) $(LDFLAGS)