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
DEPENDECIES := $(OBJ_DIR)/GBXAsmExceptions.o $(OBJ_DIR)/LexicalAnalyzer.o $(OBJ_DIR)/SyntacticAnalyzer.o $(OBJ_DIR)/PreProcessor.o \
			   $(OBJ_DIR)/Construction.o $(OBJ_DIR)/LexemeToDeclaredMemberType.o $(OBJ_DIR)/ConstructionSyntacticAnalyzer.o \
			   $(OBJ_DIR)/PackSyntacticAnalyzer.o $(OBJ_DIR)/ConditionalAssemblyPass.o $(OBJ_DIR)/IdentifierValidator.o \
			   $(OBJ_DIR)/RemoveCommentsPass.o $(OBJ_DIR)/OutputStream.o  $(OBJ_DIR)/IntermediateRepresentation.o \
			   $(OBJ_DIR)/PackIntermediateRepresentation.o $(OBJ_DIR)/DeclaredMember.o
		
$(TARGET_DIR)/$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(DEPENDECIES) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -rf *.o  *.so  *.stackdump  *.a  *.exe

.PHONY: all clean run


