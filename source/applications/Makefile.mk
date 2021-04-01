$(info -------------------------------)
$(info [BUILD::GBX] Entering directory '$(CURDIR)')
$(info -------------------------------)
.PHONY: all

all:
	$(call MakeTarget, runtime)

define MakeTarget
	$(call EnteringMessage, ${1})
	@$(MAKE) -C ${1} -f Makefile.mk
	$(call ExitingMessage, ${1})
endef