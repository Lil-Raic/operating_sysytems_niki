#ifndef VARIABLE_LIST_STRUCT_H
#define VARIABLE_LIST_STRUCT_H

#include "variable_struct.h"
#include <pthread.h>

#define MAX_VARIABLES 128

struct variables_list_struct {
    struct variable_struct vars[MAX_VARIABLES];
    int count;
    pthread_mutex_t mutex;
};

void init_variables_list(struct variables_list_struct *list);
void clear_variables_list(struct variables_list_struct *list);

struct variable_struct *
find_named_variable(struct variables_list_struct *list, const char *name);

struct variable_struct *
create_new_variable(struct variables_list_struct *list, const char *name);

int remove_named_variable(struct variables_list_struct *list, const char *name);

#endif
