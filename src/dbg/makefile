
BINDIR	= ../../bin/
BIN		= $(BINDIR)dbg.exe

SRC		= main.cpp unasm.cpp ../vm/ccpp/native.cpp ../vm/ccpp/ld.cpp ../vm/ccpp/vm.cpp

$(BIN): $(SRC) ../mod.h ../op.h ../vm/ccpp/ld.h ../vm/ccpp/vm.h ../vm/ccpp/vm.hxx ../vm/ccpp/system.c
	g++ -Os $(SRC) -o $@

.PHONY: clean
clean:
	rm -f $(BIN)
