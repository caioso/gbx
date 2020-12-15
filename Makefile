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
	rm -f ./obj/*.o ./$(TARGET)
	rm -f ./tst/*.o ./tst/$(TEST_TARGET)

test:
	./tst/$(TEST_TARGET)

run:
	./$(TARGET)
	
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
