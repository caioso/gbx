$(info -------------------------------)
$(info [BUILD::GBX] Entering directory '$(CURDIR)')
$(info -------------------------------)
SUBDIRS = registers

CC = clang++

LDFLAGS = $(LDCOVERAGE_FLAGS)
CPPFLAGS = $(CCCOVERAGE_FLAGS) $(GLOBAL_CPP_FLAGS)
INCLUDE =  -I$(INCLUDE_CORE_TOP) -I$(INCLUDE_CORE_INSTRUCTIONS) -I$(INCLUDE_CORE_INTERFACES) -I$(INCLUDE_CORE_MEMORY_REGISTERS) -I$(INCLUDE_CORE_MEMORY) \
		   -I$(INCLUDE_ASSEMBLER_STREAMS) -I$(INCLUDE_ASSEMBLER_UTILITIES)
		  
SRC_FILES = $(notdir $(wildcard ./*.cc)) $(notdir $(wildcard */*.cc))
OBJ_FILES = $(patsubst %.cc,$(BUILD_TEMP)/%.o,$(SRC_FILES))
DEP_FILES = $(patsubst %.o,%.d,$(OBJ_FILES))

.PHONY: $(SUBDIRS)

all: $(SUBDIRS) $(TARGET) $(OBJ_FILES)

$(SUBDIRS):
	$(call MakeTarget, $@)

-include $(DEP_FILES)
$(BUILD_TEMP)/%.o: $(CURDIR)/%.cc
	$(CC) $(INCLUDE) $(CPPFLAGS) -MMD -MT"$@" -c $< -o $@ 

define MakeTarget
	$(call EnteringMessage, ${1})
	@$(MAKE) -C ${1} -f Makefile.mk
	$(call ExitingMessage, ${1})
endef