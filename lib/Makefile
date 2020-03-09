all:bin/libthread.a
build:
	mkdir build
bin:
	mkdir bin
build/thread.o:build src/thread.c
	gcc -Iinclude -c src/thread.c -o build/thread.o
bin/libthread.a: bin build/thread.o
	ar rcs bin/libthread.a build/thread.o