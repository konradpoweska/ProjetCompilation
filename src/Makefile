CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -g
LDFLAGS = -lfl

OBJS = lex.o grammar.y.o


lex.c: lex.l grammar.y.h common.h
	flex --yylineno -o lex.c lex.l

grammar.y.c grammar.y.h : grammar.y
	bison -d -o grammar.y.c grammar.y

lex_test: $(OBJS) main.lex_test.o
	$(CC) -o $@ $^ $(LDFLAGS)

compiler: $(OBJS) main.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean cclean

clean:
	rm -f *.o

cclean: clean
	rm -f lex_main compiler
