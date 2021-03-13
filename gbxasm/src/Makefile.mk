CC := clang++
#CCCOVERAGE_FLAGS = -fprofile-instr-generate -fcoverage-mapping
#LDCOVERAGE_FLAGS = -fprofile-instr-generate
OBJ_DIR := $(CURDIR)/../obj
TARGET_DIR := $(CURDIR)/..
SRC_FILES := $(wildcard ./*.cc)
SRC_FILES := $(SRC_FILES) $(wildcard ./interfaces/*.cc)
SRC_FILES := $(SRC_FILES) $(wildcard ./language/*.cc)
SRC_FILES := $(SRC_FILES) $(wildcard ./frontend/parsers/*.cc)
SRC_FILES := $(SRC_FILES) $(wildcard ./frontend/passes/*.cc)
SRC_FILES := $(SRC_FILES) $(wildcard ./frontend/*.cc)
SRC_FILES := $(SRC_FILES) $(wildcard ./intermediate-representation/*.cc)
SRC_FILES := $(SRC_FILES) $(wildcard ./utilities/*.cc)
SRC_FILES := $(SRC_FILES) $(wildcard ./streams/*.cc)
OBJ_FILES := $(patsubst ./%.cc,$(OBJ_DIR)/%.o,$(SRC_FILES))
OBJ_FILES := $(subst interfaces/,,$(OBJ_FILES))
OBJ_FILES := $(subst language/,,$(OBJ_FILES))
OBJ_FILES := $(subst frontend/parsers/,,$(OBJ_FILES))
OBJ_FILES := $(subst frontend/passes/,,$(OBJ_FILES))
OBJ_FILES := $(subst frontend/,,$(OBJ_FILES))
OBJ_FILES := $(subst intermediate-representation/,,$(OBJ_FILES))
OBJ_FILES := $(subst utilities/,,$(OBJ_FILES))
OBJ_FILES := $(subst streams/,,$(OBJ_FILES))
LDFLAGS := $(LDCOVERAGE_FLAGS)
CPPFLAGS := $(CCCOVERAGE_FLAGS) -Wall -Wextra -std=c++2a -O3 -g -DDEBUG
TARGET := gbxasm

$(TARGET_DIR)/$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cc %.h
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./interfaces/%.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./interfaces/%.cc ./interfaces/%.h
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./language/%.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./language/%.cc ./language/%.h
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./frontend/%.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./frontend/%.cc ./frontend/%.h
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./frontend/parsers/%.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./utilities/%.cc ./utilities/%.h
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./utilities/%.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./intermediate-representation/%.cc ./intermediate-representation/%.h
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./intermediate-representation/%.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./frontend/parsers/%.cc ./frontend/parsers/%.h
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./frontend/passes/%.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./frontend/passes/%.cc ./frontend/passes/%.h
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./streams/%.cc
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: ./streams/%.h
	$(CC) $(CPPFLAGS) -c -o $@ $<


clean:
	rm -rf *.o  *.so  *.stackdump  *.a  *.exe

.PHONY: all clean run


