
AS		= ../bin/as

SRC		= $(wildcard *.s)
MOD		= $(SRC:.s=.m)

all: $(MOD)

%.m: %.s
	$(AS) $<

.PHONY: clean
clean:
	rm -f $(MOD)
