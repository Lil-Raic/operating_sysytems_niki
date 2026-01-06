#ifndef VARIABLE_STRUCT_H
#define VARIABLE_STRUCT_H

#include <pthread.h>

#define MAX_NAME_LEN 128
#define MAX_VALUE_LEN 4096

struct variable_struct {
    char name[MAX_NAME_LEN];
    char value[MAX_VALUE_LEN]; // Ime mora biti 'value', da se ujema s .c datotekami

    // --- Reader-Writer Lock Fields ---
    pthread_mutex_t m1;
    pthread_mutex_t m2;
    int readers_count;

    // --- Wait/Notify Fields ---
    pthread_mutex_t change_mutex;
    pthread_cond_t change_cond;
    pthread_cond_t destroy_cond;
    int changed;
    int is_deleted;
    int waiters_count;
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