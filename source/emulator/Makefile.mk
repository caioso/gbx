SUBDIRS = interfaces protocol runtime transport
CC = clang++
AR = ar

LDFLAGS = $(LDCOVERAGE_FLAGS)
CPPFLAGS = $(CCCOVERAGE_FLAGS) $(GLOBAL_CPP_FLAGS)
INCLUDE = -I$(INCLUDE_EMULATOR_TOP) -I$(INCLUDE_EMULATOR_INTERFACES) -I$(INCLUDE_EMULATOR_PROTOCOL) -I$(INCLUDE_EMULATOR_RUNTIME) -I$(INCLUDE_EMULATOR_TRANSPORT)

SRC_FILES = $(notdir $(wildcard ./*.cc)) $(notdir $(wildcard */*.cc))
OBJ_FILES = $(patsubst %.cc,$(BUILD_TEMP)/%.o,$(SRC_FILES))
DEP_FILES = $(patsubst %.o,%.d,$(OBJ_FILES))

TARGET = $(EMU_LIB)

.PHONY: $(SUBDIRS)

all: $(SUBDIRS) $(TARGET)

$(SUBDIRS):
	$(call MakeTarget, $@)
	
$(TARGET): $(OBJ_FILES) 
	$(AR) rcs $@ $(OBJ_FILES) 
	
-include $(DEP_FILES)
$(BUILD_TEMP)/%.o: $(CURDIR)/%.cc
	$(CC) $(INCLUDE) $(CPPFLAGS) -MMD -MT"$@" -c $< -o  $@ 

define MakeTarget
	$(call EnteringMessage, ${1})
	@$(MAKE) -C ${1} -f Makefile.mk
	$(call ExitingMessage, ${1})
endef