CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -g
LDFLAGS = -lfl

OBJS = lex.o grammar.y.o common.o

all: compiler


# executable building
lex_test: $(OBJS) main.lex_test.o
	$(CC) -o $@ $^ $(LDFLAGS)

compiler: $(OBJS) main.o
	$(CC) -o $@ $^ $(LDFLAGS)


# compiler rules
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<


# specific
lex.c: lex.l grammar.y.h common.h
	flex --yylineno -o lex.c lex.l

grammar.y.c grammar.y.h : grammar.y
	bison -d -o grammar.y.c grammar.y


# cleaning rules
.PHONY: clean cclean

clean:
	rm -f *.o grammar.y.* lex.c

cclean: clean
	rm -f lex_main compiler
