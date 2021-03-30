SUBDIRS = interfaces protocol transport input output
CC = clang++
AR = ar

LDFLAGS = $(LDCOVERAGE_FLAGS)
CPPFLAGS = $(CCCOVERAGE_FLAGS) $(GLOBAL_CPP_FLAGS)
INCLUDE = -I$(INCLUDE_DEBUGGER_TOP) -I$(INCLUDE_DEBUGGER_INTERFACES) -I$(INCLUDE_DEBUGGER_PROTOCOL) -I$(INCLUDE_DEBUGGER_TRANSPORT) \
		  -I$(INCLUDE_COMMONS_TOP) -I$(INCLUDE_CORE_INTERFACES) -I$(INCLUDE_DEBUGGER_OUTPUT) -I$(INCLUDE_CORE_TOP)

SRC_FILES = $(notdir $(wildcard ./*.cc)) $(notdir $(wildcard */*.cc))
OBJ_FILES = $(patsubst %.cc,$(BUILD_TEMP)/%.o,$(SRC_FILES))
DEP_FILES = $(patsubst %.o,%.d,$(OBJ_FILES))

TARGET = $(DEBUG_LIB)

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