CFLAGS?= -Wall -Wextra -I. -I./deps/
LDFLAGS= -lm

all: test

test: cytest.o test1.o test2.o rax.o
	$(CC) -o bin/$@ $^ $(LDFLAGS) -DUSE_RAX
	./bin/test

rax.o: deps/rax/rax.c deps/rax/rax.h
	@echo rax support
	$(CC) -c $(CFLAGS) $<

%.o: %.c %.h
	@echo Testing $@ from $*
	$(CC) -c $(CFLAGS) $<

%.o: tests/%.c
	@echo Testing $@ from $*
	$(CC) -c $(CFLAGS) $<

.PHONY: clean
clean:
	rm -f bin/* *.o deps/rax/*.o tests/*.o
