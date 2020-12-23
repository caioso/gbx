CC := clang++
#CCCOVERAGE_FLAGS = -fprofile-instr-generate -fcoverage-mapping
#LDCOVERAGE_FLAGS = -fprofile-instr-generate
OBJ_DIR := $(CURDIR)/../obj
TARGET_DIR := $(CURDIR)/..
SRC_FILES := $(wildcard ./*.cc)
OBJ_FILES := $(patsubst ./%.cc,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := $(LDCOVERAGE_FLAGS)
CPPFLAGS := $(CCCOVERAGE_FLAGS) -Wall -Wextra -std=c++2a -O3 -g -DDEBUG
TARGET := gbx

$(TARGET_DIR)/$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cc %.h
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -rf *.o  *.so  *.stackdump  *.a  *.exe

.PHONY: all clean run


