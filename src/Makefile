CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -Wno-unused-parameter -g
LDFLAGS = -lfl


# A file containing a main() function must be prefixed by "main."
SRCS = $(wildcard *.c) grammar.y.c lex.c
HDRS = $(wildcard *.h) grammar.y.h

# List of modules (.o files), except those containing a main() function
OBJS = $(filter-out main.%,$(SRCS:.c=.o))

# An executables called xx will get its main() from main.xx.c
EXEC = $(patsubst main.%.c, %, $(filter main.%, $(SRCS)))


all: $(EXEC)


# specific
lex.c: lex.l
	flex --yylineno -o $@ $<

lex.o: lex.c $(HDRS)
	$(CC) $(CFLAGS) -Wno-unused-function -Wno-sign-compare -o $@ -c $<

grammar.y.c grammar.y.h : grammar.y structures.h
	bison -v -d -o grammar.y.c $<


# compiler rules
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -o $@ -c $<


# executable building
$(EXEC): %: main.%.o $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)


# cleaning rules
.PHONY: clean cclean

clean:
	rm -f *.o grammar.y.* lex.c

cclean: clean
	rm -f $(EXEC)
