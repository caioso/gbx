TEST_UTILS = $(TEST_TOP)/test_utils
ASM_TESTS = $(BUILD_TEMP)/asmtests.o
COMMONS_TESTS = $(BUILD_TEMP)/commonstests.o
CORE_TESTS = $(BUILD_TEMP)/coretests.o
RUNTIME_TESTS = $(BUILD_TEMP)/runtimetests.o

export TEST_UTILS

export ASM_TESTS 
export COMMONS_TESTS 
export CORE_TESTS 
export DEBUG_TESTS 
export RUNTIME_TESTS

# Build targets
TARGET = $(GBX_TEST)

.PHONY: $(CORE_TESTS) $(ASM_TESTS) $(COMMONS_TESTS) $(RUNTIME_TESTS) $(TARGET)

all: $(CORE_TESTS) $(ASM_TESTS) $(COMMONS_TESTS) $(RUNTIME_TESTS) $(TARGET)


$(ASM_TESTS):
	$(call MakeTarget, assembler)

$(COMMONS_TESTS):
	$(call MakeTarget, commons)

$(CORE_TESTS):
	$(call MakeTarget, core)

$(RUNTIME_TESTS):
	$(call MakeTarget, runtime)

$(TARGET): $(CORE_TESTS) $(ASM_TESTS) $(COMMONS_TESTS) $(RUNTIME_TESTS) $(CORE_LIB) $(ASM_LIB) $(COMMONS_LIB) $(RUNTIME_LIB)
	@$(MAKE) -f Makefile.sub.mk

define MakeTarget
	$(call EnteringMessage, ${1})
	@$(MAKE) -C ${1} -f Makefile.mk
	$(call ExitingMessage, ${1})
endef