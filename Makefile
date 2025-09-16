
.PHONY: test
test:
	gcc -o test cytest.c deps/rax/rax.c tests/test1.c -lm -I. -I./deps/ -DUSE_RAX
