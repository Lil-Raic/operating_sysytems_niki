#include "variable_struct.h"
#include <string.h>
#include <stdio.h>

void init_variable(struct variable_struct *var, const char *name) {
    strncpy(var->name, name, MAX_NAME_LEN);
    memset(var->value, 0, MAX_VALUE_LEN);

    // Initialize locks
    pthread_mutex_init(&var->m1, NULL);
    pthread_mutex_init(&var->m2, NULL);
    var->readers_count = 0;

    // Initialize wait/notify primitives
    pthread_mutex_init(&var->change_mutex, NULL);
    pthread_cond_init(&var->change_cond, NULL);
    pthread_cond_init(&var->destroy_cond, NULL);
    var->changed = 0;
    var->is_deleted = 0;
    var->waiters_count = 0;
}

void clear_variable(struct variable_struct *var) {
    // 1. Lock Writer Mutex (m2) - block new readers/writers
    pthread_mutex_lock(&var->m2);

    // 2. Notify all waiters that variable is dying
    pthread_mutex_lock(&var->change_mutex);
    var->is_deleted = 1;
    pthread_cond_broadcast(&var->change_cond);
    
    // 3. Wait for waiters to leave
    while (var->waiters_count > 0) {
        pthread_cond_wait(&var->destroy_cond, &var->change_mutex);
    }
    pthread_mutex_unlock(&var->change_mutex);

    // 4. Clear memory
    memset(var->name, 0, MAX_NAME_LEN);
    memset(var->value, 0, MAX_VALUE_LEN);

    // 5. Unlock and destroy
    pthread_mutex_unlock(&var->m2);

    pthread_mutex_destroy(&var->m1);
    pthread_mutex_destroy(&var->m2);
    pthread_mutex_destroy(&var->change_mutex);
    pthread_cond_destroy(&var->change_cond);
    pthread_cond_destroy(&var->destroy_cond);
}

void read_variable_lock(
    struct variable_struct *var,
    void (*read_fn)(struct variable_struct *, void *),
    void *args
) {
    // READER ENTRY
    pthread_mutex_lock(&var->m1);
    var->readers_count++;
    if (var->readers_count == 1) {
        pthread_mutex_lock(&var->m2); // First reader locks out writers
    }
    pthread_mutex_unlock(&var->m1);

    // READ
    read_fn(var, args);

    // READER EXIT
    pthread_mutex_lock(&var->m1);
    var->readers_count--;
    if (var->readers_count == 0) {
        pthread_mutex_unlock(&var->m2); // Last reader releases lock
    }
    pthread_mutex_unlock(&var->m1);
}

void write_variable_lock(
    struct variable_struct *var,
    void (*write_fn)(struct variable_struct *, void *),
    void *args
) {
    // WRITER ENTRY
    pthread_mutex_lock(&var->m2);

    // WRITE
    write_fn(var, args);

    // NOTIFY WAITERS
    pthread_mutex_lock(&var->change_mutex);
    var->changed = 1;
    pthread_cond_broadcast(&var->change_cond);
    pthread_mutex_unlock(&var->change_mutex);

    // WRITER EXIT
    pthread_mutex_unlock(&var->m2);
}

int wait_variable_lock(struct variable_struct *var) {
    pthread_mutex_lock(&var->change_mutex);
    var->waiters_count++;

    // Wait until changed or deleted
    while (!var->changed && !var->is_deleted) {
        pthread_cond_wait(&var->change_cond, &var->change_mutex);
    }

    int ret_val = 0;
    if (var->is_deleted) {
        ret_val = 1; // UNSET
    } else {
        ret_val = 0; // SET
        var->changed = 0; // Reset flag (simple implementation)
    }

    var->waiters_count--;
    // If we are the last waiter and it's being deleted, tell clear_variable to proceed
    if (var->waiters_count == 0 && var->is_deleted) {
        pthread_cond_signal(&var->destroy_cond);
    }

    pthread_mutex_unlock(&var->change_mutex);
    return ret_val;
}