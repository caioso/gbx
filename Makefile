TARGET := gbx
TEST_TARGET := gbxTest

all:
	$(call MakeTarget, src)
	$(call MakeTarget, tst)

source:
	$(call MakeTarget, src)

unit:
	$(call MakeTarget, tst)

clean:
	$(call CleanTarget, src)
	$(call CleanTarget, tst)
	rm -f ./obj/*.o ./obj/*.gcno ./$(TARGET)
	rm -f ./tst/*.o ./$(TEST_TARGET) \
	rm default.profraw \
	rm gbxTestCoverage

tidy:
	cppcheck --std=c++20 --enable=all --inconclusive -v  ./src  --output-file=./check/result.txt

test:
	./$(TEST_TARGET)

debug-test:
	lldb $(TEST_TARGET)

debug:
	lldb $(TARGET)

run:
	./$(TARGET)

coverage:
	xcrun llvm-profdata merge -o gbxTestCoverage default.profraw
	xcrun llvm-cov report gbxTest -instr-profile=gbxTestCoverage -use-color --ignore-filename-regex="(gtest|gmock)" --ignore-filename-regex="(Test(.*)\.cc)"
	
.PHONY: all clean run

define CleanTarget
	$(call EnteringMessage, ${1})
	@$(MAKE) -C ${1} -f Makefile.mk clean
	$(call ExitingMessage, ${1})
endef

define MakeTarget
	$(call EnteringMessage, ${1})
	@$(MAKE) -C ${1} -f Makefile.mk
	$(call ExitingMessage, ${1})
endef

define EnteringMessage
@echo Entering ${1}
endef

define ExitingMessage
@echo Exiting ${1}
endef
