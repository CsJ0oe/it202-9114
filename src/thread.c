#include "thread.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <valgrind/valgrind.h>
#include <assert.h>
#include <stdint.h>

#ifndef USE_PTHREAD
// TODO : more input checks
// TODO : verify system function returns

////// variables
STAILQ_HEAD(, THREAD) thread_queue = STAILQ_HEAD_INITIALIZER(thread_queue);
STAILQ_HEAD(, THREAD) thread_finished_queue = STAILQ_HEAD_INITIALIZER(thread_finished_queue);
THREAD* thread_current = NULL;
THREAD* main_thread = NULL;
int thread_count = 0;
ucontext_t schedule_fifo;
int schedule_fifo_valgrind_stackid;


// functions
void schedule_fifo_goto() {
    // On enfile
    THREAD* thread_old = thread_current;
    THREAD* thread_next = main_thread;
    switch (thread_current->state) {
        case FINISHED: {
            STAILQ_INSERT_TAIL(&thread_finished_queue, thread_current, next);
            if (thread_current->waitingForMe != NULL)
                STAILQ_INSERT_TAIL(&thread_queue, thread_current->waitingForMe, next);
        } break;
        case JOINING: {
        } break;
        case MUTEX: {
        } break;
        default: { // ACTIVE
            STAILQ_INSERT_TAIL(&thread_queue, thread_current, next);
        } break;
    }
    if (!STAILQ_EMPTY(&thread_queue)) {
        thread_next = STAILQ_FIRST(&thread_queue);
        STAILQ_REMOVE_HEAD(&thread_queue, next);
    }
    thread_current = thread_next;
    swapcontext(&(thread_old->context), &(thread_next->context));
}

__attribute__((constructor)) void constr() {
    // init queue
    STAILQ_INIT(&thread_queue);
    STAILQ_INIT(&thread_finished_queue);
    // init main thread
    main_thread = malloc(sizeof(THREAD));
    main_thread->thread_num = thread_count++;
    main_thread->isMain = 1;
    main_thread->state = ACTIVE;
    main_thread->waitingForMe = NULL;
    getcontext(&(main_thread->context));
    //STAILQ_INSERT_TAIL(&thread_queue, main_thread, next);
    thread_current = main_thread;
    // go to schedule
    //schedule_fifo_goto();
}


__attribute__((destructor)) static void destr() {
    // free other threads
    while (!STAILQ_EMPTY(&thread_queue)) {
        printf("*************** IS THIS STILL NEEDED ? ******************\n");
        //THREAD* next_thread = STAILQ_FIRST(&thread_queue);
        //STAILQ_REMOVE_HEAD(&thread_queue, next);
        //VALGRIND_STACK_DEREGISTER(next_thread->valgrind_stackid);
        //if (!(next_thread->isMain)) free(next_thread->context.uc_stack.ss_sp);
        //if (!(next_thread->isMain)) free(next_thread);
    }
    while (!STAILQ_EMPTY(&thread_finished_queue)) {
        THREAD* next_thread = STAILQ_FIRST(&thread_finished_queue);
        STAILQ_REMOVE_HEAD(&thread_finished_queue, next);
        VALGRIND_STACK_DEREGISTER(next_thread->valgrind_stackid);
        if (!(next_thread->isMain)) free(next_thread->context.uc_stack.ss_sp);
        if (!(next_thread->isMain)) free(next_thread);
    }
    VALGRIND_STACK_DEREGISTER(schedule_fifo_valgrind_stackid);
    free(schedule_fifo.uc_stack.ss_sp);
    //free current thread (normalment le main ???)
    free(main_thread); // not needed any more ?
}


/********************************* THREAD **************************************/

void* thread_return_wrapper(void *(*func)(void*), void* arg) {
    thread_exit(func(arg));
    return arg; // to suppress warnings
}

extern thread_t thread_self(void){
    return thread_current;
}

extern int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg){  
    if (newthread == NULL) return -1;
    // n crée d'abord le contexte et enfile ensuite l'élément contenant ce contexte
    THREAD* new_thread = (THREAD*)malloc(sizeof(THREAD));
    new_thread->thread_num = thread_count++;
    new_thread->isMain = 0;
    new_thread->state = ACTIVE;
    new_thread->waitingForMe = NULL;
    // NEW CONTEXT
    getcontext(&(new_thread->context));
    // TODO: newuc->uc_stack.ss_size = 64*1024;
    new_thread->context.uc_stack.ss_size = 64*1024;
    new_thread->context.uc_stack.ss_sp = malloc(new_thread->context.uc_stack.ss_size);
    new_thread->valgrind_stackid = VALGRIND_STACK_REGISTER(new_thread->context.uc_stack.ss_sp,
                        new_thread->context.uc_stack.ss_sp + new_thread->context.uc_stack.ss_size);
    new_thread->context.uc_link = &schedule_fifo;
    makecontext(&(new_thread->context), (void(*)(void))thread_return_wrapper, 2, (void(*)(void))func, funcarg);
    // 
    STAILQ_INSERT_TAIL(&thread_queue, new_thread, next);
    *newthread = new_thread;
    // swap to the new thread
    schedule_fifo_goto();
    // RETURN OK
    return 0;

}

extern int thread_yield(void){
    schedule_fifo_goto();
    return 0;
}

extern int thread_join(thread_t thread, void **retval){
    if (thread == NULL) return -1;
    while ((thread->state != FINISHED) && (thread->waitingForMe != thread_current)) {
        if (thread->waitingForMe == NULL) {
            thread->waitingForMe = thread_current;
            thread_current->state = JOINING;
            schedule_fifo_goto();
        } else thread_yield();
    }
    if (retval != NULL) *retval = thread->retval;
    return 0;
}

 // TODO : Retirer les marques de commentaires une fois la fonction implémentée.
extern void thread_exit(void *retval) {
    thread_current->state = FINISHED;
    thread_current->retval = retval;
    //VALGRIND_STACK_DEREGISTER(thread_current->valgrind_stackid);
    //free(thread_current->context.uc_stack.ss_sp);
    schedule_fifo_goto();
    exit(0);
}

/********************************* MUTEX **************************************/

extern int thread_mutex_init(thread_mutex_t *mutex) {
    mutex->has_lock = NULL;
    STAILQ_INIT(&(mutex->waiting_queue));
    return 0;
}

extern int thread_mutex_destroy(thread_mutex_t *mutex) {
    if (!STAILQ_EMPTY(&(mutex->waiting_queue))) printf("mutex destroyed while being used\n");
    return 0;
}

extern int thread_mutex_lock(thread_mutex_t *mutex) {
    do {
        if (mutex->has_lock == NULL) {
            mutex->has_lock = thread_current;
        } else {
            STAILQ_INSERT_TAIL(&(mutex->waiting_queue), thread_current, next);
            thread_current->state = MUTEX;
            thread_yield();
        }
    } while (mutex->has_lock != thread_current);
    return 0;
}

extern int thread_mutex_unlock(thread_mutex_t *mutex) {
    mutex->has_lock = NULL;
    if (!STAILQ_EMPTY(&(mutex->waiting_queue))) {
        THREAD* next_thread = STAILQ_FIRST(&(mutex->waiting_queue));
        STAILQ_REMOVE_HEAD(&(mutex->waiting_queue), next);
        next_thread->state = ACTIVE;
        STAILQ_INSERT_TAIL(&thread_queue, next_thread, next);
    }
    return 0;
}


#endif
