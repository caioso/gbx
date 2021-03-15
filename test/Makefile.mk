TEST_UTILS = $(TEST_TOP)/test_utils
ASM_TESTS = $(BUILD_TEMP)/asmtests.o
COMMONS_TESTS = $(BUILD_TEMP)/commonstests.o
CORE_TESTS = $(BUILD_TEMP)/coretests.o
DEBUG_TESTS = $(BUILD_TEMP)/debuggertests.o
EMU_TESTS = $(BUILD_TEMP)/emutests.o

export TEST_UTILS

export ASM_TESTS 
export COMMONS_TESTS 
export CORE_TESTS 
export DEBUG_TESTS 
export EMU_TESTS

# Build targets
TARGET = $(GBX_TEST)

.PHONY: $(CORE_TESTS) $(ASM_TESTS) $(COMMONS_TESTS) $(EMU_TESTS) $(DEBUG_TESTS) $(TARGET)

all: $(CORE_TESTS) $(ASM_TESTS) $(COMMONS_TESTS) $(EMU_TESTS) $(DEBUG_TESTS) $(TARGET)


$(ASM_TESTS):
	$(call MakeTarget, assembler)

$(COMMONS_TESTS):
	$(call MakeTarget, commons)

$(CORE_TESTS):
	$(call MakeTarget, core)

$(DEBUG_TESTS):
	$(call MakeTarget, debugger)

$(EMU_TESTS):
	$(call MakeTarget, emulator)

$(TARGET): $(CORE_TESTS) $(ASM_TESTS) $(COMMONS_TESTS) $(EMU_TESTS) $(CORE_LIB) $(ASM_LIB) $(DEBUG_TESTS) $(COMMONS_LIB) $(EMU_LIB)
	@$(MAKE) -f Makefile.sub.mk

define MakeTarget
	$(call EnteringMessage, ${1})
	@$(MAKE) -C ${1} -f Makefile.mk
	$(call ExitingMessage, ${1})
endef