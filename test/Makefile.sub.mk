CC = clang++

LDFLAGS = $(LDCOVERAGE_FLAGS) -lgtest -lgmock -pthread
CPPFLAGS = $(CCCOVERAGE_FLAGS) $(GLOBAL_CPP_FLAGS)

TARGET = $(GBX_TEST)
TARGET_SOURCE = gbxTest.cc

$(TARGET): $(TARGET_SOURCE) $(CORE_TESTS) $(ASM_TESTS) $(COMMONS_TESTS) $(EMU_TESTS) $(CORE_LIB) $(ASM_LIB) $(COMMONS_LIB) $(EMU_LIB)
	$(CC) $(TARGET_SOURCE)$(CPPFLAGS) -o $(TARGET) $(CORE_TESTS) $(ASM_TESTS) $(COMMONS_TESTS) $(EMU_TESTS) $(CORE_LIB) $(ASM_LIB) $(COMMONS_LIB) $(EMU_LIB) $(LDFLAGS)