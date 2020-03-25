#include "../include/thread.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

static SLIST_HEAD(, THREAD) head;

static thread_t current = NULL;
static thread_t main_t = NULL;

static int count = 0;

thread_t thread_init(ucontext_t cont){
    thread_t newt = malloc(sizeof(THREAD));
    printf("wallah je crée \n");
    newt->no_thread = count ++;
    newt->context = cont;
    printf("crea finie \n"); 
    return newt;
}

void thread_pool_init(){
    SLIST_INIT(&head);
    ucontext_t random_context;
    getcontext(&random_context);
    thread_t newt = thread_init(random_context);
    SLIST_INSERT_HEAD(&head, newt, next);
    current = newt;
    main_t = newt;
}


/* recuperer l'identifiant du thread courant.
 */
extern thread_t thread_self(void){
    if (current == NULL){
	thread_pool_init();
    }
    return current;
}

/* creer un nouveau thread qui va exécuter la fonction func avec l'argument funcarg.
 * renvoie 0 en cas de succès, -1 en cas d'erreur.
 */
extern int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg){
    
    if (newthread == NULL){
	return -1;
    }
    
    // On crée d'abord le contexte
    ucontext_t newuc;
    getcontext(&newuc);
    
    if (main_t == NULL){
	thread_pool_init();
    }
    
    newuc.uc_link = &(main_t->context);
    makecontext(&newuc, (void (*)(void)) func, 1, funcarg);
    *newthread = thread_init(newuc);
    (*newthread)->context = newuc;
    // On enfile ensuite l'élément contenant ce contexte
    SLIST_INSERT_HEAD(&head, *newthread, next);
    return 0;

}

/* passer la main à un autre thread.
 */
extern int thread_yield(void){
    printf("1 \n");
    if (current == NULL){
	printf("dans le if \n");
	thread_pool_init();
    }
    printf("%p \n", current);
    thread_t next_thread = SLIST_NEXT(current, next);
    printf("2 \n");
    thread_t tempo;
    if (next_thread == NULL){ // If we are at the end of the list
	next_thread = SLIST_FIRST(&head);
	if (next_thread == NULL){ // If no thread are currently in the list
	    thread_pool_init();
	    next_thread = SLIST_FIRST(&head);
	}
    }
    printf("3 \n");
    tempo = current;
    printf("4 \n");
    current = next_thread;
    printf("5 \n");
    swapcontext(&(tempo->context), &(next_thread->context));
    printf("6 \n");
    current = tempo;
    return 0;
}

/* attendre la fin d'exécution d'un thread.
 * la valeur renvoyée par le thread est placée dans *retval.
 * si retval est NULL, la valeur de retour est ignorée.
 */
extern int thread_join(thread_t thread, void **retval){
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
extern void thread_exit(void *retval) {}

