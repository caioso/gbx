$(info ------> Here)

.PHONY: all

all:
	$(call MakeTarget, runtime)
	$(call MakeTarget, debugger_cli)

define MakeTarget
	$(call EnteringMessage, ${1})
	@$(MAKE) -C ${1} -f Makefile.mk
	$(call ExitingMessage, ${1})
endef