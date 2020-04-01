CFLAGS= -Lobj -lthread -Iinclude

all: install

install: obj/libthread.a obj/main
	gcc test/01-main.c 					$(CFLAGS) -o install/bin/01-main
	#gcc test/02-switch.c 				$(CFLAGS) -o install/bin/02-switch
	#gcc test/11-join.c 					$(CFLAGS) -o install/bin/11-join
	#gcc test/12-join-main.c 			$(CFLAGS) -o install/bin/12-join-main
	#gcc test/21-create-many.c 			$(CFLAGS) -o install/bin/21-create-many
	#gcc test/22-create-many-recursive.c $(CFLAGS) -o install/bin/22-create-many-recursive
	#gcc test/23-create-many-once.c 		$(CFLAGS) -o install/bin/23-create-many-once
	#gcc test/31-switch-many.c 			$(CFLAGS) -o install/bin/31-switch-many
	#gcc test/32-switch-many-join.c 		$(CFLAGS) -o install/bin/32-switch-many-join
	#gcc test/51-fibonacci.c 			$(CFLAGS) -o install/bin/51-fibonacci
	#gcc test/61-mutex.c 				$(CFLAGS) -o install/bin/61-mutex

buildpath: obj install install/lib install/bin

obj:
	mkdir obj
	mkdir install
	mkdir install/lib
	mkdir install/bin

obj/thread.o: buildpath src/thread.c
	gcc -c src/thread.c $(CFLAGS) -o obj/thread.o

obj/libthread.a: buildpath obj/thread.o
	ar rcs obj/libthread.a obj/thread.o

obj/main: include/thread.h obj/libthread.a test/main.c 
	gcc test/main.c $(CFLAGS) -o obj/main

check: obj/main
	./obj/main

valgrind: obj/main
	valgrind --leak-check=full --show-leak-kinds=all --max-stackframe=137344398664 ./obj/main

clean:
	rm -rf obj install

#pthreads:
#graphs:



