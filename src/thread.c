#include "thread.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <valgrind/valgrind.h>
#include <assert.h>
#include <stdint.h>
#include <sys/time.h>
#include <string.h>

#ifndef USE_PTHREAD
// TODO : more input checks
// TODO : verify system function returns

////// variables
STAILQ_HEAD(, THREAD) thread_queue = STAILQ_HEAD_INITIALIZER(thread_queue);
STAILQ_HEAD(, THREAD) thread_finished_queue = STAILQ_HEAD_INITIALIZER(thread_finished_queue);
THREAD* thread_current = NULL;
THREAD* main_thread = NULL;
int thread_count = 0;
int schedule_fifo_valgrind_stackid;

unsigned int
preemption_timer(unsigned int seconds)
{
  //printf("change\n");
  struct itimerval new;
  new.it_interval.tv_usec = 0;
  new.it_interval.tv_sec = 0;
  new.it_value.tv_usec = seconds;
  new.it_value.tv_sec = 0;
  return setitimer (ITIMER_REAL, &new, NULL);

}

// functions
void schedule_fifo_goto() {
    // block interruptions
    sigset_t oldSet;
    sigset_t filledSet;
    sigfillset(&filledSet);
    sigprocmask(SIG_SETMASK, &filledSet, &oldSet);
    // On enfile
    THREAD* thread_old = thread_current;
    THREAD* thread_next = main_thread;
    switch (thread_current->state) {
        case FINISHED: {
            // add struct to be freed later
            STAILQ_INSERT_TAIL(&thread_finished_queue, thread_current, next);
            // check if there is a thread waiting for join
            if (thread_current->waitingForMe != NULL) {
                STAILQ_INSERT_TAIL(&thread_queue, thread_current->waitingForMe, next);
                thread_current->waitingForMe = NULL;
            }
            thread_current->state = DEAD;
        } break;
        case DEAD: {
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
    // handling signals
    int i;
    if (thread_current->signals){
	int current_shift = 1;
	for (i = 0; i < 32; ++i){
            if (((thread_current->signals & (current_shift))) && (thread_current->signal_handlers[i]!=NULL)) { 
                ((void (*)(int))(thread_current->signal_handlers[i]))(i);
                thread_current->signals &= -1 - current_shift;
	    }
	    current_shift = current_shift << 1;
	}
    }
    // set & swap
    preemption_timer(5000);
    swapcontext(&(thread_old->context), &(thread_next->context));
    // unblock interruptions
    sigprocmask(SIG_SETMASK, &oldSet, NULL);
}


void SIG_YIELD (int signo __attribute__((unused))) {
    thread_yield();
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
    main_thread->signals = 0;
    memset(main_thread->signal_handlers, 0, sizeof(main_thread->signal_handlers));
    getcontext(&(main_thread->context));
    //STAILQ_INSERT_TAIL(&thread_queue, main_thread, next);
    thread_current = main_thread;
    // go to schedule
    //schedule_fifo_goto();
    // for preemption
    signal(SIGALRM, SIG_YIELD);
    //alarm(1);
}


__attribute__((destructor)) static void destr() {
    // block interruptions
    sigset_t filledSet;
    sigfillset(&filledSet);
    sigprocmask(SIG_SETMASK, &filledSet, NULL);
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
    //free main thread
    free(main_thread);
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
    // block interruptions
    sigset_t oldSet;
    sigset_t filledSet;
    sigfillset(&filledSet);
    sigprocmask(SIG_SETMASK, &filledSet, &oldSet);
    // n crée d'abord le contexte et enfile ensuite l'élément contenant ce contexte
    THREAD* new_thread = (THREAD*)malloc(sizeof(THREAD));   
    new_thread->thread_num = thread_count++;
    new_thread->isMain = 0;
    new_thread->state = ACTIVE;
    new_thread->waitingForMe = NULL;
    new_thread->signals = 0;
    memset(new_thread->signal_handlers, 0, sizeof(new_thread->signal_handlers));
    // NEW CONTEXT
    getcontext(&(new_thread->context));
    // TODO: newuc->uc_stack.ss_size = 64*1024;
    new_thread->context.uc_stack.ss_size = 64*1024;
    new_thread->context.uc_stack.ss_sp = malloc(new_thread->context.uc_stack.ss_size);
    new_thread->valgrind_stackid = VALGRIND_STACK_REGISTER(new_thread->context.uc_stack.ss_sp,
                        new_thread->context.uc_stack.ss_sp + new_thread->context.uc_stack.ss_size);
    new_thread->context.uc_link = NULL;
    makecontext(&(new_thread->context), (void(*)(void))thread_return_wrapper, 2, (void(*)(void))func, funcarg);
    // 
    STAILQ_INSERT_TAIL(&thread_queue, new_thread, next);
    *newthread = new_thread;
    // unblock interruptions
    sigprocmask(SIG_SETMASK, &oldSet, NULL);
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
    // block interruptions
    sigset_t oldSet;
    sigset_t filledSet;
    sigfillset(&filledSet);
    sigprocmask(SIG_SETMASK, &filledSet, &oldSet);
    while ((thread->state != DEAD) && (thread->waitingForMe != thread_current)) {
        if (thread->waitingForMe == NULL) {
            thread->waitingForMe = thread_current;
            thread_current->state = JOINING;
            schedule_fifo_goto();
        } else thread_yield();
    }
    if (retval != NULL) *retval = thread->retval;
    // unblock interruptions
    sigprocmask(SIG_SETMASK, &oldSet, NULL);
    return 0;
}

 // TODO : Retirer les marques de commentaires une fois la fonction implémentée.
extern void thread_exit(void *retval) {
    thread_current->state = FINISHED;
    thread_current->retval = retval;
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
    // block interruptions
    sigset_t oldSet;
    sigset_t filledSet;
    sigfillset(&filledSet);
    sigprocmask(SIG_SETMASK, &filledSet, &oldSet);
    do {
        if (mutex->has_lock == NULL) {
            mutex->has_lock = thread_current;
        } else {
            STAILQ_INSERT_TAIL(&(mutex->waiting_queue), thread_current, next);
            thread_current->state = MUTEX;
            thread_yield();
        }
    } while (mutex->has_lock != thread_current);
    // unblock interruptions
    sigprocmask(SIG_SETMASK, &oldSet, NULL);
    return 0;
}

extern int thread_mutex_unlock(thread_mutex_t *mutex) {
    // block interruptions
    sigset_t oldSet;
    sigset_t filledSet;
    sigfillset(&filledSet);
    sigprocmask(SIG_SETMASK, &filledSet, &oldSet);
    mutex->has_lock = NULL;
    if (!STAILQ_EMPTY(&(mutex->waiting_queue))) {
        THREAD* next_thread = STAILQ_FIRST(&(mutex->waiting_queue));
        STAILQ_REMOVE_HEAD(&(mutex->waiting_queue), next);
        next_thread->state = ACTIVE;
        STAILQ_INSERT_TAIL(&thread_queue, next_thread, next);
    }
    // unblock interruptions
    sigprocmask(SIG_SETMASK, &oldSet, NULL);
    return 0;
}



// send signal to thread
extern int thread_kill(thread_t thread, int signal) {
    if (signal >= 32) return -1;
    thread->signals |= 1<<signal;
    return 0;
}

// set signal handler
extern int thread_signal(int signal, void (*handler)(int)) {
    if (signal >= 32) return -1;
    thread_current->signal_handlers[signal] = handler;
    return 0;
}

#endif
