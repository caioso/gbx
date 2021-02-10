CC := clang++
#CCOVERAGE_FLAGS = -fprofile-instr-generate -fcoverage-mapping
#LDCOVERAGE_FLAGS = -fprofile-instr-generate
OBJ_DIR := $(CURDIR)/../obj
SRC_FILES := $(wildcard ./*.cc)
OBJ_FILES := $(patsubst ./%.cc,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -lgtest -lgmock $(LDCOVERAGE_FLAGS)
CPPFLAGS := $(CCCOVERAGE_FLAGS) -Wall -Wextra -std=c++2a -O0 -g -DDEBUG
TARGET := gbxasmTest
TARGET_DIR := $(CURDIR)/..
DEPENDECIES := $(OBJ_DIR)/GBXAsmExceptions.o $(OBJ_DIR)/Lexer.o $(OBJ_DIR)/Parser.o \
			   $(OBJ_DIR)/Construction.o $(OBJ_DIR)/ConstructionPack.o $(OBJ_DIR)/ConstructionParser.o \
			   $(OBJ_DIR)/PackParser.o
		
$(TARGET_DIR)/$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(DEPENDECIES) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -rf *.o  *.so  *.stackdump  *.a  *.exe

.PHONY: all clean run


