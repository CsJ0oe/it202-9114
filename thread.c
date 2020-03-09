#include "thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg){
    // On crée d'abord le contexte
    ucontext_t newuc;
    makecontext(&newuc, (void (*)(void)) func, 1, funcarg);

    // On enfile ensuite l'élément contenant ce contexte
    return 1;
}
