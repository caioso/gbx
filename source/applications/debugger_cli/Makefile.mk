CC = clang++
$(info ------> Here)
LDFLAGS = $(LDCOVERAGE_FLAGS) -lboost_system -pthread
CPPFLAGS = $(CCCOVERAGE_FLAGS) $(GLOBAL_CPP_FLAGS)

DEP_FILES = $(BUILD_TEMP)/gbxdb_cli.d

TARGET_SOURCE = gbxdb_cli.cc
TARGET =  $(CLI_DEBUGGER)

.PHONY: all

all: $(TARGET)

-include $(DEP_FILES)
$(TARGET): $(TARGET_SOURCE)
	$(CC) -MMD -MT"$@" -MF"$(DEP_FILES)" $(TARGET_SOURCE) $(CPPFLAGS) -o $(TARGET) $(LDFLAGS)