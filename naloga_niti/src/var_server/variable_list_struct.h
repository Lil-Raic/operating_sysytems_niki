#ifndef VARIABLE_LIST_STRUCT_H_
#define VARIABLE_LIST_STRUCT_H_

#include <pthread.h>
#include "definitions.h"

/* ===============================
   Variable structure
   =============================== */
struct variable_struct {
    char name[MAX_NAME_LEN];
    char data[4096];

    /* RW lock (reader priority) */
    pthread_mutex_t m1;
    pthread_mutex_t m2;
    int readers;

    /* WAIT support */
    pthread_cond_t changed;
    int deleted;
    int waiting;
};

/* ===============================
   Variables list structure
   =============================== */
struct variables_list_struct {
    struct variable_struct *vars;
    int size;
    int capacity;

    pthread_mutex_t list_mutex;
};

/* ===============================
   List management (professor)
   =============================== */
void init_variables_list(struct variables_list_struct *var_list_obj);
void clear_variables_list(struct variables_list_struct *var_list_obj);

struct variable_struct*
find_named_variable(struct variables_list_struct *var_array_obj,
                    const char var_name[]);

struct variable_struct*
create_new_variable(struct variables_list_struct *var_array_obj,
                    const char var_name[]);

int remove_named_variable(struct variables_list_struct *var_array_obj,
                          const char var_name[]);

/* ===============================
   Variable sync (YOU implement)
   =============================== */
void init_variable(struct variable_struct *var, const char name[]);
void clear_variable(struct variable_struct *var);

void read_variable_lock(struct variable_struct *var,
                        void (*read_function)(struct variable_struct *, void *),
                        void *args);

void write_variable_lock(struct variable_struct *var,
                         void (*write_function)(struct variable_struct *, void *),
                         void *args);

int wait_variable_lock(struct variable_struct *var);

#endif
