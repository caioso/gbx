CC := clang++
OBJ_DIR := $(CURDIR)/../obj
TARGET_DIR := $(CURDIR)/..
MODULE_FILES := $(wildcard ./*.ccm)
MODULE_SOURCE_FILES := $(patsubst ./%.ccm,%.cc,$(MODULE_FILES))
MODULE_PCM_FILES := $(patsubst ./%.ccm,$(OBJ_DIR)/%.pcm,$(MODULE_FILES))
SRC_FILES := $(wildcard ./*.cc)
OBJ_FILES := $(patsubst ./%.cc,$(OBJ_DIR)/%.o,$(SRC_FILES))
MODULE_OBJ_FILES := $(patsubst ./%.ccm,$(OBJ_DIR)/%.o,$(MODULE_FILES))
MODULE_INCL_FILES := $(patsubst ./%.ccm,-fmodule-file=$(OBJ_DIR)/%.pcm ,$(MODULE_FILES))
INCLUDE := 
LDFLAGS := 
CPPFLAGS := -Wall -std=c++2a -O0 -DDEBUG
TARGET_NAME := gbx
TARGET := $(TARGET_DIR)/$(TARGET_NAME)

$(TARGET): gbx.cc $(MODULE_OBJ_FILES)
	$(CC) $(CPPFLAGS) -fmodules-ts -c $(TARGET_NAME).cc $(MODULE_INCL_FILES) -o $(OBJ_DIR)/$(TARGET_NAME).o
	$(CC) $(MODULE_PCM_FILES) $(OBJ_DIR)/$(TARGET_NAME).o $(MODULE_OBJ_FILES) -o $(TARGET)

$(OBJ_DIR)/%.o: %.cc %.ccm
	$(CC) $(CPPFLAGS) -fmodules-ts --precompile $(basename $<).ccm -o $(OBJ_DIR)/$(basename $<).pcm          
	$(CC) $(CPPFLAGS) -fmodules-ts -c $< -fmodule-file=$(OBJ_DIR)/$(basename $<).pcm -o $@

clean:
	rm -rf *.o  *.so  *.stackdump  *.a  *.exe

.PHONY: all clean run
