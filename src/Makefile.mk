CC := clang++
OBJ_DIR := ${CURDIR}/../obj
TARGET_DIR := ${CURDIR}/..
SRC_FILES := $(wildcard ./*.cc)
OBJ_FILES := $(patsubst ./%.cc,$(OBJ_DIR)/%.o,$(SRC_FILES))
INCLUDE := 
LDFLAGS := 
CPPFLAGS := -Wall -std=c++2a -O0 -DDEBUG
TARGET := ${TARGET_DIR}/gbx

$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -rf *.o  *.so  *.stackdump  *.a  *.exe

.PHONY: all clean run

