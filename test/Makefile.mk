TEST_UTILS = $(TEST_TOP)/test_utils
CORE_TESTS = $(BUILD_TEMP)/coretests.o
ASM_TESTS = $(BUILD_TEMP)/asmtests.o
COMMONS_TESTS = $(BUILD_TEMP)/commonstests.o
EMU_TESTS = $(BUILD_TEMP)/emutests.o

export TEST_UTILS
export CORE_TESTS 
export ASM_TESTS 
export COMMONS_TESTS 
export EMU_TESTS

# Build targets
TARGET = $(GBX_TEST)

.PHONY: $(CORE_TESTS) $(ASM_TESTS) $(COMMONS_TESTS) $(EMU_TESTS) $(TARGET)

all: $(CORE_TESTS) $(ASM_TESTS) $(COMMONS_TESTS) $(EMU_TESTS) $(TARGET)

$(CORE_TESTS):
	$(call MakeTarget, core)

$(ASM_TESTS):
	$(call MakeTarget, assembler)

$(COMMONS_TESTS):
	$(call MakeTarget, commons)

$(EMU_TESTS):
	$(call MakeTarget, emulator)

$(TARGET): $(CORE_TESTS) $(ASM_TESTS) $(COMMONS_TESTS) $(EMU_TESTS) $(CORE_LIB) $(ASM_LIB) $(COMMONS_LIB) $(EMU_LIB)
	@$(MAKE) -f Makefile.sub.mk

define MakeTarget
	$(call EnteringMessage, ${1})
	@$(MAKE) -C ${1} -f Makefile.mk
	$(call ExitingMessage, ${1})
endef