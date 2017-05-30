
BINDIR	= ../../bin/
BIN		= $(BINDIR)bin2c.exe

SRC		= bin2c.cpp

$(BIN): $(SRC)
	g++ -Os $(SRC) -o $@

.PHONY: clean
clean:
	rm -f $(BIN)
