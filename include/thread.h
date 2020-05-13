#ifndef __THREAD_H__
#define __THREAD_H__

#ifndef USE_PTHREAD
#include <sys/queue.h>
#include <ucontext.h>
#include <stdint.h>

/*La structure proprement dite (à compléter au fur et à mesure des besoins)
 */
typedef struct THREAD{
    int thread_num;
    // TODO : parent id
    void* retval;
    int isMain;
    uint32_t signals;
    void* signal_handlers[32];
    struct THREAD* waitingForMe;
    enum { ACTIVE, JOINING, MUTEX, FINISHED, DEAD } state;
    ucontext_t context;
    int valgrind_stackid;
    STAILQ_ENTRY(THREAD) next;
} THREAD;

/* identifiant de thread
 * NB: pourra être un entier au lieu d'un pointeur si ca vous arrange,
 *     mais attention aux inconvénient des tableaux de threads
 *     (consommation mémoire, cout d'allocation, ...).
 */
typedef THREAD* thread_t;

/* recuperer l'identifiant du thread courant.
 */
extern thread_t thread_self(void);

/* creer un nouveau thread qui va exécuter la fonction func avec l'argument funcarg.
 * renvoie 0 en cas de succès, -1 en cas d'erreur.
 */
extern int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg);

/* passer la main à un autre thread.
 */
extern int thread_yield(void);

/* attendre la fin d'exécution d'un thread.
 * la valeur renvoyée par le thread est placée dans *retval.
 * si retval est NULL, la valeur de retour est ignorée.
 */
extern int thread_join(thread_t thread, void **retval);

/* terminer le thread courant en renvoyant la valeur de retour retval.
 * cette fonction ne retourne jamais.
 *
 * L'attribut noreturn aide le compilateur à optimiser le code de
 * l'application (élimination de code mort). Attention à ne pas mettre
 * cet attribut dans votre interface tant que votre thread_exit()
 * n'est pas correctement implémenté (il ne doit jamais retourner).
 */
 // TODO : Retirer les marques de commentaires une fois la fonction implémentée.
extern void thread_exit(void *retval) __attribute__ ((__noreturn__));

/* Interface possible pour les mutex */
typedef struct thread_mutex {
	thread_t has_lock;
	STAILQ_HEAD(, THREAD) waiting_queue;
} thread_mutex_t;

int thread_mutex_init(thread_mutex_t *mutex);

int thread_mutex_destroy(thread_mutex_t *mutex);

int thread_mutex_lock(thread_mutex_t *mutex);

int thread_mutex_unlock(thread_mutex_t *mutex);


/* signals */
// signal : integer [0-31]

// send signal to thread
int thread_kill(thread_t thread, int signal);

// set signal handler
int thread_signal(int signal, void (*handler)(int));


#else /* USE_PTHREAD */

/* Si on compile avec -DUSE_PTHREAD, ce sont les pthreads qui sont utilisés */
#include <sched.h>
#include <pthread.h>
#define thread_t pthread_t
#define thread_self pthread_self
#define thread_create(th, func, arg) pthread_create(th, NULL, func, arg)
#define thread_yield sched_yield
#define thread_join pthread_join
#define thread_exit pthread_exit

/* Interface possible pour les mutex */
#define thread_mutex_t            pthread_mutex_t
#define thread_mutex_init(_mutex) pthread_mutex_init(_mutex, NULL)
#define thread_mutex_destroy      pthread_mutex_destroy
#define thread_mutex_lock         pthread_mutex_lock
#define thread_mutex_unlock       pthread_mutex_unlock

#endif /* USE_PTHREAD */

#endif /* __THREAD_H__ */
