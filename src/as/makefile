
BINDIR	= ../../bin/
BIN		= $(BINDIR)as.exe

SRC		= lex.yy.c y.tab.c as.cpp
$(BIN): $(SRC) as.h y.tab.h ../op.h ../mod.h
	g++ -Os -Wno-write-strings $(SRC) -o $@

y.tab.c y.tab.h: yacc.as.y as.h ../op.h
	bison -d -y $<

lex.yy.c: lex.as.1 y.tab.h as.h
	flex -i $<

.PHONY: clean
clean:
	rm -f $(BIN) y.tab.c y.tab.h lex.yy.c
