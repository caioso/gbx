$(info -------------------------------)
$(info [BUILD::GBX] Entering directory '$(CURDIR)')
$(info -------------------------------)
ARGS = $(filter-out $(KNOWN_TARGETS),$(MAKECMDGOALS))
ASM_TESTS_FLAG = asm
CORE_TESTS_FLAG = core
COMMONS_TESTS_FLAG = commons

# Prepare build variables
BUILD_TOP = $(CURDIR)/build
BUILD_LIB = $(BUILD_TOP)/lib
BUILD_APPS = $(BUILD_TOP)/apps
BUILD_TEMP = $(CURDIR)/temp
BUILD_TEST = $(BUILD_TOP)/test

TEST_DATA = ./build/test/test_data/

SOURCE_TOP = $(CURDIR)/source
SOURCE_CORE_TOP = $(SOURCE_TOP)/core

APPLICATIONS = $(SOURCE_TOP)/applications
TEST_TOP = $(CURDIR)/test

INCLUDE_TOP = $(CURDIR)/include
INCLUDE_ASSEMBLER_TOP = $(INCLUDE_TOP)/assembler
INCLUDE_ASSEMBLER_FRONTEND_TOP = $(INCLUDE_ASSEMBLER_TOP)/frontend
INCLUDE_ASSEMBLER_FRONTEND_PARSERS = $(INCLUDE_ASSEMBLER_FRONTEND_TOP)/parsers
INCLUDE_ASSEMBLER_FRONTEND_PASSES = $(INCLUDE_ASSEMBLER_FRONTEND_TOP)/passes
INCLUDE_ASSEMBLER_INTERFACES = $(INCLUDE_ASSEMBLER_TOP)/interfaces
INCLUDE_ASSEMBLER_INTERMEDIATE_REPRESENTATION = $(INCLUDE_ASSEMBLER_TOP)/intermediate_representation
INCLUDE_ASSEMBLER_STREAMS = $(INCLUDE_ASSEMBLER_TOP)/streams
INCLUDE_ASSEMBLER_UTILITIES = $(INCLUDE_ASSEMBLER_TOP)/utilities
INCLUDE_COMMONS_TOP = $(INCLUDE_TOP)/commons
INCLUDE_CORE_TOP = $(INCLUDE_TOP)/core
INCLUDE_CORE_CONSTANTS = $(INCLUDE_CORE_TOP)/constants
INCLUDE_CORE_INSTRUCTIONS = $(INCLUDE_CORE_TOP)/instructions
INCLUDE_CORE_MEMORY = $(INCLUDE_CORE_TOP)/memory
INCLUDE_CORE_INTERFACES = $(INCLUDE_CORE_TOP)/interfaces
INCLUDE_DEBUGGER_TOP = $(INCLUDE_TOP)/debugger
INCLUDE_DEBUGGER_INPUT = $(INCLUDE_DEBUGGER_TOP)/input
INCLUDE_DEBUGGER_INTERFACES = $(INCLUDE_DEBUGGER_TOP)/interfaces
INCLUDE_DEBUGGER_PROTOCOL = $(INCLUDE_DEBUGGER_TOP)/protocol
INCLUDE_DEBUGGER_OUTPUT = $(INCLUDE_DEBUGGER_TOP)/output
INCLUDE_DEBUGGER_TRANSPORT = $(INCLUDE_DEBUGGER_TOP)/transport
INCLUDE_RUNTIME_TOP = $(INCLUDE_TOP)/runtime
INCLUDE_RUNTIME_RUNNER = $(INCLUDE_RUNTIME_TOP)/runner

# Compiler flags
GLOBAL_CPP_FLAGS = -Wall -Wextra -std=c++2a -O3 -g -DDEBUG

# Build artifacts
ASM_LIB = $(BUILD_LIB)/libgbxasm.a
CLI_DEBUGGER = $(BUILD_APPS)/gbxdb_cli
CORE_LIB = $(BUILD_LIB)/libgbxcore.a
COMMONS_LIB = $(BUILD_LIB)/libgbxcommons.a
DEBUG_LIB = $(BUILD_LIB)/libgbxdebug.a
RUNTIME_LIB = $(BUILD_LIB)/libgbxruntime.a
GBX_TEST = $(BUILD_TEST)/gbxtest
RUNTIME = $(BUILD_APPS)/gbx

export BUILD_TOP
export BUILD_LIB
export BUILD_TEMP
export BUILD_TEST

export SOURCE_TOP
export SOURCE_CORE_TOP

export APPLICATIONS
export TEST_TOP

export INCLUDE_TOP
export INCLUDE_ASSEMBLER_TOP
export INCLUDE_ASSEMBLER_FRONTEND_TOP
export INCLUDE_ASSEMBLER_FRONTEND_PARSERS
export INCLUDE_ASSEMBLER_FRONTEND_PASSES
export INCLUDE_ASSEMBLER_INTERFACES
export INCLUDE_ASSEMBLER_INTERMEDIATE_REPRESENTATION
export INCLUDE_ASSEMBLER_STREAMS
export INCLUDE_ASSEMBLER_UTILITIES
export INCLUDE_COMMONS_TOP
export INCLUDE_CORE_TOP
export INCLUDE_CORE_CONSTANTS
export INCLUDE_CORE_INSTRUCTIONS
export INCLUDE_CORE_INTERFACES
export INCLUDE_CORE_MEMORY
export INCLUDE_DEBUGGER_TOP
export INCLUDE_DEBUGGER_INPUT
export INCLUDE_DEBUGGER_INTERFACES
export INCLUDE_DEBUGGER_PROTOCOL
export INCLUDE_DEBUGGER_OUTPUT
export INCLUDE_DEBUGGER_TRANSPORT
export INCLUDE_RUNTIME_TOP
export INCLUDE_RUNTIME_RUNNER

export GLOBAL_CPP_FLAGS

export ASM_LIB
export CLI_DEBUGGER
export CORE_LIB
export COMMONS_LIB
export DEBUG_LIB
export RUNTIME_LIB
export GBX_TEST
export RUNTIME

all: $(CORE_LIB) $(ASM_LIB) $(COMMONS_LIB) $(RUNTIME_LIB) applications $(GBX_TEST) 

$(CORE_LIB):
	$(call MakeTarget, source/core)

$(ASM_LIB):
	$(call MakeTarget, source/assembler)

$(RUNTIME_LIB):
	$(call MakeTarget, source/runtime)

$(GBX_TEST):
	$(call MakeTarget, test)

$(COMMONS_LIB):
	$(call MakeTarget, source/commons)

applications:
	$(call MakeTarget, $(APPLICATIONS))

.PHONY: $(CORE_LIB) $(ASM_LIB) $(COMMONS_LIB) $(RUNTIME_LIB) applications $(GBX_TEST) test

clean:
	rm -f ./temp/*.o ./temp/*.d
	rm -f $(CORE_LIB) $(ASM_LIB) $(COMMONS_LIB) $(RUNTIME_LIB) $(GBX_TEST) $(RUNTIME) $(CLI_DEBUGGER)

debug-test:
	lldb ./build/test/gbxtest

test:
ifeq ($(word 2,$(ARGS)), $(ASM_TESTS_FLAG))
	./build/test/gbxtest $(TEST_DATA) --gtest_filter="AssemblerTests_*" 
else
ifeq ($(word 2,$(ARGS)), $(CORE_TESTS_FLAG))
	./build/test/gbxtest $(TEST_DATA) --gtest_filter="CoreTests_*" 
else
ifeq ($(word 2,$(ARGS)), $(COMMONS_TESTS_FLAG))
	./build/test/gbxtest $(TEST_DATA) --gtest_filter="CommonsTests_*" 
else
	./build/test/gbxtest $(TEST_DATA)
endif
endif
endif

asm:
	@true

core:
	@true

commons:
	@true

define MakeTarget
	$(call EnteringMessage, ${1})
	@$(MAKE) -C ${1} -f Makefile.mk
	$(call ExitingMessage, ${1})
endef