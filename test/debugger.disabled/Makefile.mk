TEST_DIRS = $(wildcard */)

CC = clang++
LD = ld

LDFLAGS = $(LDCOVERAGE_FLAGS)
CPPFLAGS = $(CCCOVERAGE_FLAGS) $(GLOBAL_CPP_FLAGS)
INCLUDE = -I$(INCLUDE_CORE_TOP) -I$(INCLUDE_CORE_INSTRUCTIONS)

SRC_FILES = $(notdir $(wildcard */*.cc))
OBJ_FILES = $(patsubst %.cc,$(BUILD_TEMP)/%.o,$(SRC_FILES))
TARGET = $(DEBUG_TESTS)

.PHONY: $(TARGET) $(TEST_DIRS)

all: $(TEST_DIRS) $(TARGET)

$(TEST_DIRS):
	$(MAKE) -C $@ -f Makefile.mk
	
$(TARGET): $(OBJ_FILES)
	$(LD) -r -o $(TARGET) $(OBJ_FILES)