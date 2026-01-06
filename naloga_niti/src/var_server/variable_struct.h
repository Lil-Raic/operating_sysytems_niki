#ifndef VARIABLE_STRUCT_H
#define VARIABLE_STRUCT_H

#include <pthread.h>

#define MAX_NAME_LEN 128
#define MAX_VALUE_LEN 256

struct variable_struct {
    char name[MAX_NAME_LEN];
    char value[MAX_VALUE_LEN];
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int changed;
};

void init_variable(struct variable_struct *var, const char *name);
void clear_variable(struct variable_struct *var);

void read_variable_lock(
    struct variable_struct *var,
    void (*read_fn)(struct variable_struct *, void *),
    void *args
);

void write_variable_lock(
    struct variable_struct *var,
    void (*write_fn)(struct variable_struct *, void *),
    void *args
);

int wait_variable_lock(struct variable_struct *var);

#endif
