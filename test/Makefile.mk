# Prepare build variables
SUBDIRS = core

TEST_UTILS = $(TEST_TOP)/test_utils
CORE_TESTS = $(BUILD_TEMP)/coretests.o

export CORE_TESTS 
export TEST_UTILS

# Build targets
TARGET = $(GBX_TEST)

.PHONY: $(SUBDIRS) $(TARGET)

all: $(SUBDIRS) $(TARGET)

$(SUBDIRS):
	$(call MakeTarget, core)

$(TARGET): $(CORE_TESTS) $(CORE_LIB)
	@$(MAKE) -f Makefile.sub.mk

define MakeTarget
	$(call EnteringMessage, ${1})
	@$(MAKE) -C ${1} -f Makefile.mk
	$(call ExitingMessage, ${1})
endef