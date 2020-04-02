CFLAGS= -Lobj -lthread -Iinclude -Wall -Werror -Wextra

all: build install

install: build obj/libthread.a obj
	mkdir -p install
	mkdir -p install/lib
	mkdir -p install/bin
	cp obj/libthread.a install/lib/libthread.a
	cp obj/01-main	   install/bin/01-main

build: obj obj/libthread.a obj/main
	gcc test/01-main.c 						$(CFLAGS) -o obj/01-main
	#gcc test/02-switch.c 					$(CFLAGS) -o obj/02-switch
	#gcc test/11-join.c 					$(CFLAGS) -o obj/11-join
	#gcc test/12-join-main.c 				$(CFLAGS) -o obj/12-join-main
	#gcc test/21-create-many.c 				$(CFLAGS) -o obj/21-create-many
	#gcc test/22-create-many-recursive.c 	$(CFLAGS) -o obj/22-create-many-recursive
	#gcc test/23-create-many-once.c 		$(CFLAGS) -o obj/23-create-many-once
	#gcc test/31-switch-many.c 				$(CFLAGS) -o obj/31-switch-many
	#gcc test/32-switch-many-join.c 		$(CFLAGS) -o obj/32-switch-many-join
	#gcc test/51-fibonacci.c 				$(CFLAGS) -o obj/51-fibonacci
	#gcc test/61-mutex.c 					$(CFLAGS) -o obj/61-mutex

obj/thread.o: src/thread.c
	gcc -c src/thread.c $(CFLAGS) -o obj/thread.o

obj/libthread.a: obj/thread.o
	ar rcs obj/libthread.a obj/thread.o

obj/main: include/thread.h obj/libthread.a test/main.c
	gcc test/main.c $(CFLAGS) -o obj/main

obj:
	mkdir obj

check: obj/main
	./obj/main

valgrind: obj/main
	valgrind --leak-check=full --show-leak-kinds=all --max-stackframe=137344398664 ./obj/main

clean:
	rm -rf obj/ install/

#pthreads:
#graphs:



