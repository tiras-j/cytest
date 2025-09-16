CFLAGS?= -Wall -Wextra -I. -I./deps/
LDFLAGS= -lm

tests/test1.o: tests/test1.c cytest.h 
deps/rax/rax.o: deps/rax/rax.c deps/rax/rax.h

test1.o: tests/test1.o
rax.o: deps/rax/rax.o
cytest.o: cytest.h deps/rax/rax.h

test: cytest.o test1.o rax.o
	$(CC) -o bin/$@ $^ $(LDFLAGS) -DUSE_RAX

.c.o:
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f bin/* *.o
