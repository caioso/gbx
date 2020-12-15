CC := clang++
OBJ_DIR := $(CURDIR)/../obj
SRC_FILES := $(wildcard ./*.cc)
OBJ_FILES := $(patsubst ./%.cc,%.o,$(SRC_FILES))
LDFLAGS := -lgtest -lgmock
CPPFLAGS := -Wall -Wextra -std=c++2a -O3 -g -DDEBUG
TARGET := gbxTest
DEPENDECIES := $(OBJ_DIR)/GBXExceptions.o $(OBJ_DIR)/RegisterBank.o $(OBJ_DIR)/ROM.o $(OBJ_DIR)/CPU.o

$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(DEPENDECIES) $(LDFLAGS)

%.o: %.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -rf *.o  *.so  *.stackdump  *.a  *.exe

.PHONY: all clean run


