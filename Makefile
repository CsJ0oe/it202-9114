CFLAGS= -Lobj -lthread${suffix} -Iinclude -Wall -Werror -Wextra


all: build install

install: build obj/libthread.a obj
	mkdir -p install
	mkdir -p install/lib
	mkdir -p install/bin
	cp obj/libthread${suffix}.a 					install/lib/libthread${suffix}.a
	cp obj/01-main${suffix}	   					install/bin/01-main${suffix}
	cp obj/02-switch${suffix}					install/bin/02-switch${suffix}
	cp obj/11-join${suffix}						install/bin/11-join${suffix}
	cp obj/12-join-main${suffix}					install/bin/12-join-main${suffix}
	cp obj/21-create-many${suffix}				install/bin/21-create-many${suffix}
	cp obj/22-create-many-recursive${suffix}		install/bin/22-create-many-recursive${suffix}
	cp obj/23-create-many-once${suffix}			install/bin/23-create-many-once${suffix}
	cp obj/31-switch-many${suffix}				install/bin/31-switch-many${suffix}
	cp obj/32-switch-many-join${suffix}			install/bin/32-switch-many-join${suffix}
	cp obj/51-fibonacci${suffix}					install/bin/51-fibonacci${suffix}

build: obj obj/libthread${suffix}.a obj/main
	gcc test/01-main.c 						$(CFLAGS) -o obj/01-main${suffix}
	gcc test/02-switch.c 					$(CFLAGS) -o obj/02-switch${suffix}
	gcc test/11-join.c 						$(CFLAGS) -o obj/11-join${suffix}
	gcc test/12-join-main.c 				$(CFLAGS) -o obj/12-join-main${suffix}
	gcc test/21-create-many.c 				$(CFLAGS) -o obj/21-create-many${suffix}
	gcc test/22-create-many-recursive.c 	$(CFLAGS) -o obj/22-create-many-recursive${suffix}
	gcc test/23-create-many-once.c 			$(CFLAGS) -o obj/23-create-many-once${suffix}
	gcc test/31-switch-many.c 				$(CFLAGS) -o obj/31-switch-many${suffix}
	gcc test/32-switch-many-join.c 			$(CFLAGS) -o obj/32-switch-many-join${suffix}
	gcc test/51-fibonacci.c 				$(CFLAGS) -o obj/51-fibonacci${suffix}
	#gcc test/61-mutex.c 					$(CFLAGS) -o obj/61-mutex${suffix}
	#gcc test/62-mutex.c 					$(CFLAGS) -o obj/62-mutex${suffix}

obj/thread${suffix}.o: src/thread.c
	gcc -c src/thread.c $(CFLAGS) -o obj/thread${suffix}.o

obj/libthread${suffix}.a: obj/thread${suffix}.o
	ar rcs obj/libthread${suffix}.a obj/thread${suffix}.o

obj/main: include/thread.h obj/libthread${suffix}.a test/main.c
	gcc test/main.c $(CFLAGS) -o obj/main${suffix}

obj:
	mkdir obj

check: build obj/main
	./obj/main

valgrind: build obj/main
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all --max-stackframe=137344398664 ./obj/main

clean:
	rm -rf obj/ install/

pthreads: CFLAGS += -DUSE_PTHREAD -lpthread
pthreads: suffix = _pthread
pthreads: all



#graphs:



