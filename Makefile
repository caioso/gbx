CC := clang++

OBJ_DIR := $(CURDIR)
TARGET_DIR := $(CURDIR)
SRC_FILES := $(wildcard ./*.cc)
OBJ_FILES := $(patsubst ./%.cc,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := $(LDCOVERAGE_FLAGS)
CPPFLAGS := $(CCCOVERAGE_FLAGS) -Wall -Wextra -std=c++2a -O3 -g -DDEBUG 
INCLUDE := -I../gbxcore/src/
TARGET := gbx

$(TARGET_DIR)/$(TARGET): $(OBJ_FILES)
	$(CC) $(CPPFLAGS) $(INCLUDE) ../gbxcore/libgbxcore.a gbx.cc -o $@

$(OBJ_DIR)/%.o: %.cc %.h
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJ_DIR)/%.o: %.cc
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	rm -rf *.o  *.so  *.stackdump  *.a  *.exe

run:
	./gbx

.PHONY: all clean run


