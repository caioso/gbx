CC := clang++
#CCOVERAGE_FLAGS = -fprofile-instr-generate -fcoverage-mapping
#LDCOVERAGE_FLAGS = -fprofile-instr-generate
OBJ_DIR := $(CURDIR)/../obj
SRC_FILES := $(wildcard ./*.cc)
OBJ_FILES := $(patsubst ./%.cc,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -lgtest -lgmock $(LDCOVERAGE_FLAGS) -pthread
CPPFLAGS := $(CCCOVERAGE_FLAGS) -Wall -Wextra -std=c++2a -O0 -g -DDEBUG
TARGET := gbxTest
TARGET_DIR := $(CURDIR)/..
INCLUDE := -I../../gbxcore/src/
DEPENDECIES := $(OBJ_DIR)/Runner.o $(OBJ_DIR)/CancellationToken.o $(OBJ_DIR)/DebugRequest.o \
			   $(OBJ_DIR)/DebugRequestConsumer.o $(OBJ_DIR)/DebugResponse.o 
		
$(TARGET_DIR)/$(TARGET): $(OBJ_FILES)
	$(CC) $(INCLUDE) -o $@ $^ $(DEPENDECIES) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cc
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	rm -rf *.o  *.so  *.stackdump  *.a  *.exe

.PHONY: all clean run


