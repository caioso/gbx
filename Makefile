ARGS = $(filter-out $(KNOWN_TARGETS),$(MAKECMDGOALS))
ASM_TESTS_FLAG = asm
CORE_TESTS_FLAG = core

# Prepare build variables
BUILD_TOP = $(CURDIR)/build
BUILD_LIB = $(BUILD_TOP)/lib
BUILD_APPS = $(BUILD_TOP)/apps
BUILD_TEMP = $(CURDIR)/temp
BUILD_TEST = $(BUILD_TOP)/test

SOURCE_TOP = $(CURDIR)/source
SOURCE_CORE_TOP = $(SOURCE_TOP)/core

APPLICATIONS = $(SOURCE_TOP)/applications
TEST_TOP = $(CURDIR)/test

INCLUDE_TOP = $(CURDIR)/include
INCLUDE_CORE_TOP = $(INCLUDE_TOP)/core
INCLUDE_CORE_CONSTANTS = $(INCLUDE_TOP)/core/constants
INCLUDE_CORE_INSTRUCTIONS = $(INCLUDE_TOP)/core/instructions
INCLUDE_CORE_INTERFACES = $(INCLUDE_TOP)/core/interfaces
INCLUDE_ASSEMBLER_TOP = $(INCLUDE_TOP)/assembler
INCLUDE_ASSEMBLER_FRONTEND_TOP = $(INCLUDE_ASSEMBLER_TOP)/frontend
INCLUDE_ASSEMBLER_FRONTEND_PARSERS = $(INCLUDE_ASSEMBLER_FRONTEND_TOP)/parsers
INCLUDE_ASSEMBLER_FRONTEND_PASSES = $(INCLUDE_ASSEMBLER_FRONTEND_TOP)/passes
INCLUDE_ASSEMBLER_INTERFACES = $(INCLUDE_ASSEMBLER_TOP)/interfaces
INCLUDE_ASSEMBLER_INTERMEDIATE_REPRESENTATION = $(INCLUDE_ASSEMBLER_TOP)/intermediate_representation
INCLUDE_ASSEMBLER_STREAMS = $(INCLUDE_ASSEMBLER_TOP)/streams
INCLUDE_ASSEMBLER_UTILITIES = $(INCLUDE_ASSEMBLER_TOP)/utilities
INCLUDE_COMMONS_TOP = $(INCLUDE_TOP)/commons
INCLUDE_EMULATOR_TOP = $(INCLUDE_TOP)/emulator
INCLUDE_EMULATOR_INTERFACES = $(INCLUDE_EMULATOR_TOP)/interfaces
INCLUDE_EMULATOR_PROTOCOL = $(INCLUDE_EMULATOR_TOP)/protocol
INCLUDE_EMULATOR_RUNTIME = $(INCLUDE_EMULATOR_TOP)/runtime
INCLUDE_EMULATOR_TRANSPORT = $(INCLUDE_EMULATOR_TOP)/transport

# Compiler flags
GLOBAL_CPP_FLAGS = -Wall -Wextra -std=c++2a -O3 -g -DDEBUG

# Build artifacts
CORE_LIB = $(BUILD_LIB)/libgbxcore.a
ASM_LIB = $(BUILD_LIB)/libgbxasm.a
COMMONS_LIB = $(BUILD_LIB)/libgbxcommons.a
EMU_LIB = $(BUILD_LIB)/libgbxemu.a
RUNTIME = $(BUILD_APPS)/gbx
CLI_DEBUGGER = $(BUILD_APPS)/gbxdb_cli
GBX_TEST = $(BUILD_TEST)/gbxtest

export BUILD_TOP
export BUILD_LIB
export BUILD_TEMP
export BUILD_TEST

export SOURCE_TOP
export SOURCE_CORE_TOP

export APPLICATIONS
export TEST_TOP

export INCLUDE_TOP
export INCLUDE_CORE_TOP
export INCLUDE_CORE_CONSTANTS
export INCLUDE_CORE_INSTRUCTIONS
export INCLUDE_CORE_INTERFACES
export INCLUDE_ASSEMBLER_TOP
export INCLUDE_ASSEMBLER_FRONTEND_TOP
export INCLUDE_ASSEMBLER_FRONTEND_PARSERS
export INCLUDE_ASSEMBLER_FRONTEND_PASSES
export INCLUDE_ASSEMBLER_INTERFACES
export INCLUDE_ASSEMBLER_INTERMEDIATE_REPRESENTATION
export INCLUDE_ASSEMBLER_STREAMS
export INCLUDE_ASSEMBLER_UTILITIES
export INCLUDE_COMMONS_TOP
export INCLUDE_EMULATOR_TOP
export INCLUDE_EMULATOR_INTERFACES
export INCLUDE_EMULATOR_PROTOCOL
export INCLUDE_EMULATOR_RUNTIME
export INCLUDE_EMULATOR_TRANSPORT

export GLOBAL_CPP_FLAGS

export CORE_LIB
export ASM_LIB
export COMMONS_LIB
export EMU_LIB
export RUNTIME
export CLI_DEBUGGER
export GBX_TEST

all: $(CORE_LIB) $(ASM_LIB) $(COMMONS_LIB) $(EMU_LIB) applications $(GBX_TEST) 

$(CORE_LIB):
	$(call MakeTarget, source/core)

$(ASM_LIB):
	$(call MakeTarget, source/assembler)

$(EMU_LIB):
	$(call MakeTarget, source/emulator)

$(GBX_TEST):
	$(call MakeTarget, test)

$(COMMONS_LIB):
	$(call MakeTarget, source/commons)

applications:
	$(call MakeTarget, $(APPLICATIONS))

.PHONY: $(CORE_LIB) $(ASM_LIB) $(COMMONS_LIB) $(EMU_LIB) applications $(GBX_TEST) test

clean:
	rm -f ./temp/*.o ./temp/*.d
	rm -f $(CORE_LIB) $(ASM_LIB) $(COMMONS_LIB) $(EMU_LIB) $(GBX_TEST) $(RUNTIME) $(CLI_DEBUGGER)

test:
ifeq ($(word 2,$(ARGS)), $(ASM_TESTS_FLAG))
	./build/test/gbxtest --gtest_filter="AssemblerTests_*" 
else
ifeq ($(word 2,$(ARGS)), $(CORE_TESTS_FLAG))
	./build/test/gbxtest --gtest_filter="CoreTests_*" 
else
	./build/test/gbxtest 
endif
endif

asm:
	@true

core:
	@true

define MakeTarget
	$(call EnteringMessage, ${1})
	@$(MAKE) -C ${1} -f Makefile.mk
	$(call ExitingMessage, ${1})
endef