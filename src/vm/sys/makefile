
BINDIR	= ../../../bin/

AS		= $(BINDIR)as
BIN2C	= $(BINDIR)bin2c

ASMS	= system.s
MODS	= $(ASMS:.s=.m)
SRCS	= ../ccpp/$(MODS:.m=.c)

$(SRCS):$(MODS)
	$(BIN2C) $? $@

$(MODS):$(ASMS)
	$(AS) $?

clean:
	rm $(SRCS) $(MODS)
