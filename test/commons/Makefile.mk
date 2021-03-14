TEST_DIRS = $(wildcard */)

CC = clang++
LD = ld

LDFLAGS = $(LDCOVERAGE_FLAGS)
CPPFLAGS = $(CCCOVERAGE_FLAGS) $(GLOBAL_CPP_FLAGS)
INCLUDE = -I$(INCLUDE_COMMONS_TOP)

SRC_FILES = $(notdir $(wildcard */*.cc))
OBJ_FILES = $(patsubst %.cc,$(BUILD_TEMP)/%.o,$(SRC_FILES))
TARGET = $(COMMONS_TESTS)

.PHONY: tests $(TARGET)

all: tests $(TARGET)

tests:
	for TEST in $(TEST_DIRS); do $(MAKE) -C $$TEST -f Makefile.mk; done ||:

$(TARGET): $(OBJ_FILES)
	$(LD) -r -o $(TARGET) $(OBJ_FILES)