TFLAGS := -Lobj -Iinclude -Wall -Werror -Wextra -g 
CFLAGS := $(TFLAGS) -lthread 
suffix = 

.PHONY: all pthreads build install

all: build install


install: build obj/libthread$(suffix).a obj
	mkdir -p install
	mkdir -p install/lib
	mkdir -p install/bin
	cp obj/libthread${suffix}.a 				install/lib/libthread${suffix}.a
	cp obj/01-main${suffix}	   				install/bin/01-main${suffix}
	cp obj/02-switch${suffix}				install/bin/02-switch${suffix}
	cp obj/11-join${suffix}					install/bin/11-join${suffix}
	cp obj/12-join-main${suffix}				install/bin/12-join-main${suffix}
	cp obj/21-create-many${suffix}				install/bin/21-create-many${suffix}
	cp obj/22-create-many-recursive${suffix}		install/bin/22-create-many-recursive${suffix}
	cp obj/23-create-many-once${suffix}			install/bin/23-create-many-once${suffix}
	cp obj/31-switch-many${suffix}				install/bin/31-switch-many${suffix}
	cp obj/32-switch-many-join${suffix}			install/bin/32-switch-many-join${suffix}
	cp obj/33-switch-many-cascade${suffix}			install/bin/33-switch-many-cascade${suffix}
	cp obj/51-fibonacci${suffix}				install/bin/51-fibonacci${suffix}
	cp obj/61-mutex${suffix}				install/bin/61-mutex${suffix}
	cp obj/62-mutex${suffix}				install/bin/62-mutex${suffix}
	cp obj/71-preemption${suffix}                           install/bin/71-preemption${suffix}
	cp obj/72-signal${suffix}                               install/bin/72-signal${suffix}

build: obj obj/libthread$(suffix).a
	gcc test/01-main.c 					$(CFLAGS) -o obj/01-main${suffix}
	gcc test/02-switch.c 					$(CFLAGS) -o obj/02-switch${suffix}
	gcc test/11-join.c 					$(CFLAGS) -o obj/11-join${suffix}
	gcc test/12-join-main.c 				$(CFLAGS) -o obj/12-join-main${suffix}
	gcc test/21-create-many.c 				$(CFLAGS) -o obj/21-create-many${suffix}
	gcc test/22-create-many-recursive.c 			$(CFLAGS) -o obj/22-create-many-recursive${suffix}
	gcc test/23-create-many-once.c 				$(CFLAGS) -o obj/23-create-many-once${suffix}
	gcc test/31-switch-many.c 				$(CFLAGS) -o obj/31-switch-many${suffix}
	gcc test/32-switch-many-join.c 				$(CFLAGS) -o obj/32-switch-many-join${suffix}
	gcc test/33-switch-many-cascade.c 			$(CFLAGS) -o obj/33-switch-many-cascade${suffix}
	gcc test/51-fibonacci.c 				$(CFLAGS) -o obj/51-fibonacci${suffix}
	gcc test/61-mutex.c 					$(CFLAGS) -o obj/61-mutex${suffix}
	gcc test/62-mutex.c 					$(CFLAGS) -o obj/62-mutex${suffix}
	gcc test/71-preemption.c 				$(CFLAGS) -o obj/71-preemption${suffix}
	gcc test/72-signal.c 					$(CFLAGS) -o obj/72-signal${suffix}

obj/thread$(suffix).o: src/thread.c
	gcc -c src/thread.c $(CFLAGS) -o obj/thread${suffix}.o


obj/libthread$(suffix).a: obj/thread$(suffix).o
	ar rcs obj/libthread$(suffix).a obj/thread$(suffix).o

obj:
	mkdir obj


valgrind: all
	valgrind ./install/bin/01-main
	valgrind ./install/bin/02-switch
	valgrind ./install/bin/11-join
	valgrind ./install/bin/12-join-main
	valgrind ./install/bin/21-create-many 20
	valgrind ./install/bin/22-create-many-recursive 20
	valgrind ./install/bin/23-create-many-once 20
	valgrind ./install/bin/31-switch-many 10 20
	valgrind ./install/bin/32-switch-many-join 10 20
	valgrind ./install/bin/33-switch-many-cascade 20 5
	valgrind ./install/bin/51-fibonacci 8
	valgrind ./install/bin/61-mutex 20
	valgrind ./install/bin/62-mutex 20
	valgrind ./install/bin/71-preemption
	valgrind ./install/bin/72-signal

valgrind_pthread: pthreads
	valgrind ./install/bin/01-main_pthread
	valgrind ./install/bin/02-switch_pthread
	valgrind ./install/bin/11-join_pthread
	valgrind ./install/bin/12-join-main_pthread
	valgrind ./install/bin/21-create-many_pthread 20
	valgrind ./install/bin/22-create-many-recursive_pthread 20
	valgrind ./install/bin/23-create-many-once_pthread 20
	valgrind ./install/bin/31-switch-many_pthread 10 20
	valgrind ./install/bin/32-switch-many-join_pthread 10 20
	valgrind ./install/bin/33-switch-many-cascade_pthread 20 5
	valgrind ./install/bin/51-fibonacci_pthread 8
	valgrind ./install/bin/61-mutex_pthread 20
	valgrind ./install/bin/62-mutex_pthread 20
	valgrind ./install/bin/71-preemption_pthread
	valgrind ./install/bin/72-signal_pthread


clean:
	rm -rf obj/ install/


#-DUSE_PTHREAD -lpthread

pthreads:
	$(MAKE) CFLAGS="$(TFLAGS) -lthread_pthread -DUSE_PTHREAD -lpthread" suffix=_pthread all


graphs:
	make pthreads
	make
	python3 ./src/graphics_drawing.py 0 10
	python3 ./src/graphics_drawing.py 1 10
	python3 ./src/graphics_drawing.py 2 10
	python3 ./src/graphics_drawing.py 3 10
	python3 ./src/graphics_drawing.py 4 10
	python3 ./src/graphics_drawing.py 5 10
	python3 ./src/graphics_drawing.py 6 10
	python3 ./src/graphics_drawing.py 7 10 10
	python3 ./src/graphics_drawing.py 8 10 10
	python3 ./src/graphics_drawing.py 9 10



check: all
	./install/bin/01-main
	./install/bin/02-switch
	./install/bin/11-join
	./install/bin/12-join-main
	./install/bin/21-create-many 20
	./install/bin/22-create-many-recursive 20
	./install/bin/23-create-many-once 20
	./install/bin/31-switch-many 10 20
	./install/bin/32-switch-many-join 10 20
	./install/bin/33-switch-many-cascade 20 5
	./install/bin/51-fibonacci 8
	./install/bin/61-mutex 20
	./install/bin/62-mutex 20
	./install/bin/71-preemption
	./install/bin/72-signal



