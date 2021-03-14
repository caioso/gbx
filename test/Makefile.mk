TEST_UTILS = $(TEST_TOP)/test_utils
CORE_TESTS = $(BUILD_TEMP)/coretests.o
ASM_TESTS = $(BUILD_TEMP)/asmtests.o

export CORE_TESTS 
export ASM_TESTS 
export TEST_UTILS

# Build targets
TARGET = $(GBX_TEST)

.PHONY: $(CORE_TESTS) $(ASM_TESTS) $(TARGET)

all: $(CORE_TESTS) $(ASM_TESTS) $(TARGET)

$(CORE_TESTS):
	$(call MakeTarget, core)

$(ASM_TESTS):
	$(call MakeTarget, assembler)

$(TARGET): $(CORE_TESTS) $(ASM_TESTS) $(CORE_LIB) $(ASM_LIB)
	@$(MAKE) -f Makefile.sub.mk

define MakeTarget
	$(call EnteringMessage, ${1})
	@$(MAKE) -C ${1} -f Makefile.mk
	$(call ExitingMessage, ${1})
endef