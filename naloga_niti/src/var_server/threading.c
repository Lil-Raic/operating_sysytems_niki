#include "threading.h"
#include <pthread.h>
#include <stdlib.h>

struct thread_args {
    void (*handler)(void *);
    void *args;
};

static void *thread_start(void *arg)
{
    struct thread_args *t = arg;
    t->handler(t->args);
    free(t);
    return NULL;
}

void threading_setup(struct threading_struct *threading_obj)
{
    (void)threading_obj;
}

void threading_cleanup(struct threading_struct *threading_obj)
{
    (void)threading_obj;
}

void handle_in_thread(struct threading_struct *threading_obj,
                      void (*handler_function)(void *args),
                      void *args)
{
    (void)threading_obj;

    pthread_t tid;
    struct thread_args *t = malloc(sizeof(*t));
    t->handler = handler_function;
    t->args = args;

    pthread_create(&tid, NULL, thread_start, t);
    pthread_detach(tid);
}
