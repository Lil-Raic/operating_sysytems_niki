#include "variable_struct.h"
#include <string.h>

void init_variable(struct variable_struct *var, const char *name) {
    strncpy(var->name, name, MAX_NAME_LEN);
    var->value[0] = '\0';
    pthread_mutex_init(&var->mutex, NULL);
    pthread_cond_init(&var->cond, NULL);
    var->changed = 0;
}

void clear_variable(struct variable_struct *var) {
    pthread_mutex_destroy(&var->mutex);
    pthread_cond_destroy(&var->cond);
}

void read_variable_lock(
    struct variable_struct *var,
    void (*read_fn)(struct variable_struct *, void *),
    void *args
) {
    pthread_mutex_lock(&var->mutex);
    read_fn(var, args);
    pthread_mutex_unlock(&var->mutex);
}

void write_variable_lock(
    struct variable_struct *var,
    void (*write_fn)(struct variable_struct *, void *),
    void *args
) {
    pthread_mutex_lock(&var->mutex);
    write_fn(var, args);
    var->changed = 1;
    pthread_cond_broadcast(&var->cond);
    pthread_mutex_unlock(&var->mutex);
}

int wait_variable_lock(struct variable_struct *var) {
    pthread_mutex_lock(&var->mutex);
    while (!var->changed) {
        pthread_cond_wait(&var->cond, &var->mutex);
    }
    var->changed = 0;
    pthread_mutex_unlock(&var->mutex);
    return 0;
}
