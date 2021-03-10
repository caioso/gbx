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
INCLUDE := -I../../gbxcore/src/ -I../../gbxcommons/src/
DEPENDECIES := $(OBJ_DIR)/Runner.o $(OBJ_DIR)/CancellationToken.o $(OBJ_DIR)/MessageHandler.o $(OBJ_DIR)/DebugMessage.o \
			   $(OBJ_DIR)/DebugMessageNotificationArguments.o $(OBJ_DIR)/GBXExceptions.o $(OBJ_DIR)/DebugCommand.o \
			   $(OBJ_DIR)/ReadRegisterCommand.o $(OBJ_DIR)/ClientJoinedCommand.o $(OBJ_DIR)/RegisterBankSummaryCommand.o \
			   $(OBJ_DIR)/BoostAsioServerTransport.o

$(TARGET_DIR)/$(TARGET): $(OBJ_FILES)
	$(CC) $(INCLUDE) -o $@ $^ $(DEPENDECIES) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cc
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	rm -rf *.o  *.so  *.stackdump  *.a  *.exe

.PHONY: all clean run


