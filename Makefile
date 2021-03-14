# Prepare build variables
BUILD_TOP = $(CURDIR)/build
BUILD_LIB = $(BUILD_TOP)/lib
BUILD_TEMP = $(CURDIR)/temp
BUILD_TEST = $(BUILD_TOP)/test

SOURCE_TOP = $(CURDIR)/source
SOURCE_CORE_TOP = $(SOURCE_TOP)/core

TEST_TOP = $(CURDIR)/test
TEST_CORE_TOP = $(TEST_TOP)/core

INCLUDE_TOP = $(CURDIR)/include
INCLUDE_CORE_TOP = $(INCLUDE_TOP)/core
INCLUDE_CORE_CONSTANTS = $(INCLUDE_TOP)/core/constants
INCLUDE_CORE_INSTRUCTIONS = $(INCLUDE_TOP)/core/instructions
INCLUDE_CORE_INTERFACES = $(INCLUDE_TOP)/core/interfaces

# Compiler flags
GLOBAL_CPP_FLAGS = -Wall -Wextra -std=c++2a -O3 -g -DDEBUG

# Build artifacts
CORE_LIB = $(BUILD_LIB)/libgbxcore.a
GBX_TEST = $(BUILD_TEST)/gbxtest

export BUILD_TOP
export BUILD_LIB
export BUILD_TEMP
export BUILD_TEST

export SOURCE_TOP
export SOURCE_CORE_TOP

export TEST_TOP
export TEST_CORE_TOP

export INCLUDE_TOP
export INCLUDE_CORE_TOP
export INCLUDE_CORE_CONSTANTS
export INCLUDE_CORE_INSTRUCTIONS
export INCLUDE_CORE_INTERFACES

export GLOBAL_CPP_FLAGS

export CORE_LIB
export GBX_TEST

all: $(CORE_LIB) $(GBX_TEST)

$(CORE_LIB):
	$(call MakeTarget, source/core)

$(GBX_TEST):
	$(call MakeTarget, test)

.PHONY: $(CORE_LIB) $(GBX_TEST)

clean:
	rm -f ./temp/*.o ./temp/*.d
	rm -f $(CORE_LIB) $(GBX_TEST)

define MakeTarget
	$(call EnteringMessage, ${1})
	@$(MAKE) -C ${1} -f Makefile.mk
	$(call ExitingMessage, ${1})
endef