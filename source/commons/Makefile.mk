CC := clang++
AR := ar
#CCCOVERAGE_FLAGS = -fprofile-instr-generate -fcoverage-mapping
#LDCOVERAGE_FLAGS = -fprofile-instr-generate
OBJ_DIR := $(CURDIR)/../obj
TARGET_DIR := $(CURDIR)/..
SRC_FILES := $(wildcard ./*.cc)
SRC_FILES := $(SRC_FILES) $(wildcard ./interfaces/*.cc)
SRC_FILES := $(SRC_FILES) $(wildcard ./requests/*.cc)
OBJ_FILES := $(patsubst ./%.cc,$(OBJ_DIR)/%.o,$(SRC_FILES))
OBJ_FILES := $(subst interfaces/,,$(OBJ_FILES))
OBJ_FILES := $(subst requests/,,$(OBJ_FILES))
LDFLAGS := $(LDCOVERAGE_FLAGS) -Wall -Wextra -std=c++2a -O0 -g -DDEBUG
CPPFLAGS := $(CCCOVERAGE_FLAGS) -Wall -Wextra -std=c++2a -O3 -g -DDEBUG 
TARGET := libgbxcommons.a

$(TARGET_DIR)/$(TARGET): $(OBJ_FILES)
	$(AR) rcs $@ $(OBJ_FILES)

$(OBJ_DIR)/%.o: %.cc %.h
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -rf *.o  *.so  *.stackdump  *.a  *.exe

run:
	./gbx

.PHONY: all clean run


