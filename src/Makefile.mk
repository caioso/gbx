CC := clang++
#CCCOVERAGE_FLAGS = -fprofile-instr-generate -fcoverage-mapping
#LDCOVERAGE_FLAGS = -fprofile-instr-generate
OBJ_DIR := $(CURDIR)/../obj
TARGET_DIR := $(CURDIR)/..
SRC_FILES := $(wildcard ./*.cc)
SRC_FILES := $(SRC_FILES) $(wildcard ./interfaces/*.cc)
SRC_FILES := $(SRC_FILES) $(wildcard ./requests/debug-server-status/*.cc)
SRC_FILES := $(SRC_FILES) $(wildcard ./requests/register-data/*.cc)
SRC_FILES := $(SRC_FILES) $(wildcard ./requests/*.cc)
OBJ_FILES := $(patsubst ./%.cc,$(OBJ_DIR)/%.o,$(SRC_FILES))
OBJ_FILES := $(subst interfaces/,,$(OBJ_FILES))
OBJ_FILES := $(subst requests/debug-server-status/,,$(OBJ_FILES))
OBJ_FILES := $(subst requests/register-data/,,$(OBJ_FILES))
OBJ_FILES := $(subst requests/,,$(OBJ_FILES))
LDFLAGS := $(LDCOVERAGE_FLAGS) -Wall -Wextra -std=c++2a -O0 -g -DDEBUG -lboost_system
CPPFLAGS := $(CCCOVERAGE_FLAGS) -Wall -Wextra -std=c++2a -O3 -g -DDEBUG 
INCLUDE := -I../../gbxcore/src/ -I../../gbxcommons/src/
INCLUDE_REQUESTS := -I../../../../gbxcore/src/ -I../../../../gbxcommons/src/
TARGET := gbx

$(TARGET_DIR)/$(TARGET): $(OBJ_FILES)
	$(CC) $(CPPFLAGS) $(INCLUDE) ../../gbxcore/libgbxcore.a ../../gbxcommons/libgbxcommons.a -o $@ $(OBJ_FILES)  $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cc %.h
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJ_DIR)/%.o: %.cc
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJ_DIR)/%.o: ./interfaces/%.cc
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJ_DIR)/%.o: ./interfaces/%.cc ./interfaces/%.h
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJ_DIR)/%.o: ./requests/%.cc
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJ_DIR)/%.o: ./requests/%.cc ./requests/%.h
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJ_DIR)/%.o: ./requests/debug-server-status/%.cc
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJ_DIR)/%.o: ./requests/debug-server-status/%.cc ./requests/debug-server-status/%.h
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJ_DIR)/%.o: ./requests/register-data/%.cc
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJ_DIR)/%.o: ./requests/register-data/%.cc ./requests/register-data/%.h
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	rm -rf *.o  *.so  *.stackdump  *.a  *.exe

run:
	./gbx

.PHONY: all clean run


