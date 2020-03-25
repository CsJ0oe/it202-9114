CFLAGS= -Llib/bin -lthread -Ilib/include
all:libr build/main

build:
	mkdir build

build/main:libr build tst/main.c
	gcc tst/main.c $(CFLAGS) -o build/main

check:build
	./build/main

libr:
	$(MAKE) -C ./lib/

clean:
	rm -rf ./build ./lib/build ./lib/bin

#valgrind:
#pthreads:
#graphs:
