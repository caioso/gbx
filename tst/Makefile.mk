CC := clang++
#CCCOVERAGE_FLAGS = -fprofile-instr-generate -fcoverage-mapping
#LDCOVERAGE_FLAGS = -fprofile-instr-generate
OBJ_DIR := $(CURDIR)/../obj
SRC_FILES := $(wildcard ./*.cc)
OBJ_FILES := $(patsubst ./%.cc,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -lgtest -lgmock $(LDCOVERAGE_FLAGS)
CPPFLAGS := $(CCCOVERAGE_FLAGS) -Wall -Wextra -std=c++2a -O3 -g -DDEBUG
TARGET := gbxTest
TARGET_DIR := $(CURDIR)/..
DEPENDECIES := $(OBJ_DIR)/GBXExceptions.o $(OBJ_DIR)/RegisterBank.o $(OBJ_DIR)/ROM.o \
			   $(OBJ_DIR)/MemoryController.o $(OBJ_DIR)/AddressRange.o \
			   $(OBJ_DIR)/Clock.o $(OBJ_DIR)/ControlUnit.o $(OBJ_DIR)/ArithmeticLogicUnit.o  \
			   $(OBJ_DIR)/AddressingModeFormat.o $(OBJ_DIR)/OpcodePatternMatcher.o \
			   

$(TARGET_DIR)/$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(DEPENDECIES) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -rf *.o  *.so  *.stackdump  *.a  *.exe

.PHONY: all clean run


