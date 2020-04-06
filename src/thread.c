#include "thread.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <valgrind/valgrind.h>
#include <assert.h>

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
////// functions
void newcontext(ucontext_t* newuc, void *(*func)(void *), void *funcarg, ucontext_t* link, int* valgrind_stackid);


void* schedule_fifo_func(void* arg) {
    while (!STAILQ_EMPTY(&thread_queue)) {
        THREAD* next_thread = STAILQ_FIRST(&thread_queue);
        STAILQ_REMOVE_HEAD(&thread_queue, next);
        thread_current = next_thread;
        // TODO: CREATE PROPER FREE
        swapcontext(&schedule_fifo, &(thread_current->context));
     }
    return arg; // to suppress warnings
}

void* thread_return_wrapper(void *(*func)(void*), void* arg) {
    thread_exit(func(arg));
    return arg; // to suppress warnings
}

__attribute__((constructor)) void constr() {
    // init queue
    STAILQ_INIT(&thread_queue);
    STAILQ_INIT(&thread_finished_queue);
    // init schedule
    getcontext(&schedule_fifo);
    // TODO: newuc->uc_stack.ss_size = 64*1024;
    schedule_fifo.uc_stack.ss_size = 64*1024;
    schedule_fifo.uc_stack.ss_sp = malloc(schedule_fifo.uc_stack.ss_size);
    schedule_fifo_valgrind_stackid = VALGRIND_STACK_REGISTER(schedule_fifo.uc_stack.ss_sp,
                        schedule_fifo.uc_stack.ss_sp + schedule_fifo.uc_stack.ss_size);
    schedule_fifo.uc_link = NULL;
    makecontext(&schedule_fifo, (void(*)(void))schedule_fifo_func, 0);
    // init main thread
    main_thread = malloc(sizeof(THREAD));
    main_thread->thread_num = thread_count++;
    main_thread->isMain = 1;
    getcontext(&(main_thread->context));
    STAILQ_INSERT_TAIL(&thread_queue, main_thread, next);
    thread_current = main_thread;
    // go to schedule
    swapcontext(&(main_thread->context), &schedule_fifo);

}


__attribute__((destructor)) static void destr() {
    // free other threads
    while (!STAILQ_EMPTY(&thread_queue)) {
        printf("*************** IS THIS STILL NEEDED ? ******************");
        THREAD* next_thread = STAILQ_FIRST(&thread_queue);
        STAILQ_REMOVE_HEAD(&thread_queue, next);
        VALGRIND_STACK_DEREGISTER(next_thread->valgrind_stackid);
        if (!(next_thread->isMain)) free(next_thread->context.uc_stack.ss_sp);
        if (!(next_thread->isMain)) free(next_thread);
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
    // free current thread (normalment le main ???)
    free(main_thread); // not needed any more ?
}


/********************************* THREAD **************************************/


/* recuperer l'identifiant du thread courant.
 */
extern thread_t thread_self(void){
    return thread_current;
}

/* creer un nouveau thread qui va exécuter la fonction func avec l'argument funcarg.
 * renvoie 0 en cas de succès, -1 en cas d'erreur.
 */
extern int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg){  
    if (newthread == NULL) return -1;
    // n crée d'abord le contexte et enfile ensuite l'élément contenant ce contexte
    THREAD* new_thread = (THREAD*)malloc(sizeof(THREAD));
    new_thread->thread_num = thread_count++;
    new_thread->isMain = 0;
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
    STAILQ_INSERT_TAIL(&thread_queue, thread_current, next);
    swapcontext(&(thread_current->context), &schedule_fifo);
    // RETURN OK
    return 0;

}

/* passer la main à un autre thread.
 */
extern int thread_yield(void){
    // On enfile
    STAILQ_INSERT_TAIL(&thread_queue, thread_current, next);
    swapcontext(&(thread_current->context), &schedule_fifo);
    // RETURN OK
    return 0;
}

/* attendre la fin d'exécution d'un thread.
 * la valeur renvoyée par le thread est placée dans *retval.
 * si retval est NULL, la valeur de retour est ignorée.
 */
extern int thread_join(thread_t thread, void **retval){
    // TODO : optimz for passif wait
    if (thread == NULL) return -1;
    while(thread->state != FINISHED) thread_yield();
    if (retval != NULL) *retval = thread->retval;
    return 0;
}

/* terminer le thread courant en renvoyant la valeur de retour retval.
 * cette fonction ne retourne jamais.
 *
 * L'attribut noreturn aide le compilateur à optimiser le code de
 * l'application (élimination de code mort). Attention à ne pas mettre
 * cet attribut dans votre interface tant que votre thread_exit()
 * n'est pas correctement implémenté (il ne doit jamais retourner).
 */
 // TODO : Retirer les marques de commentaires une fois la fonction implémentée.
extern void thread_exit(void *retval) {
    thread_current->state = FINISHED;
    thread_current->retval = retval;
    //VALGRIND_STACK_DEREGISTER(thread_current->valgrind_stackid);
    //free(thread_current->context.uc_stack.ss_sp);
    STAILQ_INSERT_TAIL(&thread_finished_queue, thread_current, next);
    setcontext(&schedule_fifo);
}

/********************************* UTILS **************************************/
 

void newcontext(ucontext_t* newuc, void *(*func)(void*), void *funcarg, ucontext_t* link, int* valgrind_stackid) {
    getcontext(newuc);
    // TODO: newuc->uc_stack.ss_size = 64*1024;
    newuc->uc_stack.ss_size = 64*1024;
    newuc->uc_stack.ss_sp = malloc(newuc->uc_stack.ss_size);
    *valgrind_stackid = VALGRIND_STACK_REGISTER(newuc->uc_stack.ss_sp, newuc->uc_stack.ss_sp + newuc->uc_stack.ss_size);
    newuc->uc_link = link;
    makecontext(newuc, (void(*)(void))func, 1, funcarg);
}
