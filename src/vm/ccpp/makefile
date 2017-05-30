
BINDIR	= ../../../bin/
BIN		= $(BINDIR)jj.exe

SRC		= main.cpp native.cpp ld.cpp vm.cpp

$(BIN): $(SRC) ../../mod.h ../../op.h ld.h vm.h vm.hxx system.c
	g++ -Os $(SRC) -o $@

.PHONY: clean
clean:
	rm -f $(BIN)
