#include "variable_list_struct.h"
#include <string.h>

void init_variable(struct variable_struct *var, const char name[])
{
    strncpy(var->name, name, MAX_NAME_LEN);
    memset(var->data, 0, sizeof(var->data));

    pthread_mutex_init(&var->m1, NULL);
    pthread_mutex_init(&var->m2, NULL);
    pthread_cond_init(&var->changed, NULL);

    var->readers = 0;
    var->deleted = 0;
    var->waiting = 0;
}

void clear_variable(struct variable_struct *var)
{
    pthread_mutex_lock(&var->m2);

    var->deleted = 1;
    pthread_cond_broadcast(&var->changed);

    while (var->waiting > 0)
        pthread_cond_wait(&var->changed, &var->m2);

    pthread_mutex_unlock(&var->m2);

    pthread_mutex_destroy(&var->m1);
    pthread_mutex_destroy(&var->m2);
    pthread_cond_destroy(&var->changed);

    memset(var->name, 0, MAX_NAME_LEN);
    memset(var->data, 0, sizeof(var->data));
}

void read_variable_lock(struct variable_struct *var,
                        void (*read_function)(struct variable_struct *, void *),
                        void *args)
{
    pthread_mutex_lock(&var->m1);
    var->readers++;
    if (var->readers == 1)
        pthread_mutex_lock(&var->m2);
    pthread_mutex_unlock(&var->m1);

    if (!var->deleted)
        read_function(var, args);

    pthread_mutex_lock(&var->m1);
    var->readers--;
    if (var->readers == 0)
        pthread_mutex_unlock(&var->m2);
    pthread_mutex_unlock(&var->m1);
}

void write_variable_lock(struct variable_struct *var,
                         void (*write_function)(struct variable_struct *, void *),
                         void *args)
{
    pthread_mutex_lock(&var->m2);

    if (!var->deleted) {
        write_function(var, args);
        pthread_cond_broadcast(&var->changed);
    }

    pthread_mutex_unlock(&var->m2);
}

int wait_variable_lock(struct variable_struct *var)
{
    pthread_mutex_lock(&var->m2);

    var->waiting++;

    if (!var->deleted)
        pthread_cond_wait(&var->changed, &var->m2);

    var->waiting--;

    if (var->deleted && var->waiting == 0)
        pthread_cond_signal(&var->changed);

    pthread_mutex_unlock(&var->m2);

    return var->deleted ? 1 : 0;
}
