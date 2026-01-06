#ifndef VARIABLE_STRUCT_H
#define VARIABLE_STRUCT_H

#include <pthread.h>
#include "definitions.h" // Include this to get MAX_NAME_LEN

#define MAX_VALUE_LEN 4096

struct variable_struct {
    char name[MAX_NAME_LEN];
    char value[MAX_VALUE_LEN]; // Variable content

    // --- Reader-Writer Lock Fields ---
    pthread_mutex_t m1;        // Protects readers_count
    pthread_mutex_t m2;        // Resource lock (Writers vs Readers)
    int readers_count;         // Number of active readers

    // --- Wait/Notify Fields ---
    pthread_mutex_t change_mutex; // Mutex for waiting condition
    pthread_cond_t change_cond;   // Condition var for value changes
    pthread_cond_t destroy_cond;  // Condition var for safe destruction
    int changed;                  // Flag: value changed?
    int is_deleted;               // Flag: variable is being deleted?
    int waiters_count;            // How many threads are waiting?
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