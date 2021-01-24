CC := clang++
#CCCOVERAGE_FLAGS = -fprofile-instr-generate -fcoverage-mapping
#LDCOVERAGE_FLAGS = -fprofile-instr-generate
OBJ_DIR := $(CURDIR)/../obj
SRC_FILES := $(wildcard ./*.cc)
OBJ_FILES := $(patsubst ./%.cc,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -lgtest -lgmock $(LDCOVERAGE_FLAGS)
CPPFLAGS := $(CCCOVERAGE_FLAGS) -Wall -Wextra -std=c++2a -O0 -g -DDEBUG
TARGET := gbxTest
TARGET_DIR := $(CURDIR)/..
DEPENDECIES := $(OBJ_DIR)/GBXExceptions.o $(OBJ_DIR)/RegisterBank.o $(OBJ_DIR)/ROM.o \
			   $(OBJ_DIR)/MemoryController.o $(OBJ_DIR)/AddressRange.o \
			   $(OBJ_DIR)/Clock.o $(OBJ_DIR)/ControlUnit.o $(OBJ_DIR)/ArithmeticLogicUnit.o  \
			   $(OBJ_DIR)/AddressingModeFormat.o $(OBJ_DIR)/OpcodePatternMatcher.o \
			   $(OBJ_DIR)/InstructionAddBase.o $(OBJ_DIR)/InstructionLd.o $(OBJ_DIR)/OpcodeDecoder.o \
			   $(OBJ_DIR)/InstructionAdc.o $(OBJ_DIR)/InstructionAdd.o $(OBJ_DIR)/InstructionSub.o \
			   $(OBJ_DIR)/InstructionSubBase.o $(OBJ_DIR)/InstructionSbc.o $(OBJ_DIR)/InstructionAnd.o \
			   $(OBJ_DIR)/InstructionOr.o $(OBJ_DIR)/InstructionXor.o $(OBJ_DIR)/InstructionCp.o \
			   $(OBJ_DIR)/InstructionInc.o $(OBJ_DIR)/InstructionDec.o $(OBJ_DIR)/InstructionPush.o \
			   $(OBJ_DIR)/InstructionPop.o $(OBJ_DIR)/InstructionLdhl.o  $(OBJ_DIR)/InstructionRlca.o \
			   $(OBJ_DIR)/InstructionRla.o $(OBJ_DIR)/InstructionRrca.o $(OBJ_DIR)/InstructionRra.o \
			   $(OBJ_DIR)/InstructionRlc.o $(OBJ_DIR)/InstructionRl.o $(OBJ_DIR)/InstructionRrc.o \
			   $(OBJ_DIR)/InstructionRr.o  $(OBJ_DIR)/InstructionSla.o $(OBJ_DIR)/InstructionSra.o \
			   $(OBJ_DIR)/InstructionSrl.o $(OBJ_DIR)/InstructionSwap.o

$(TARGET_DIR)/$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(DEPENDECIES) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -rf *.o  *.so  *.stackdump  *.a  *.exe

.PHONY: all clean run


