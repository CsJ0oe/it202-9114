CFLAGS= -Llib/bin -lthread -Ilib/include
all:build/main
build:
	mkdir build
build/main: build tst/main.c
	gcc tst/main.c $(CFLAGS) -o build/main 
check:build
	./build/main

#valgrind:
#pthreads:
#graphs:
