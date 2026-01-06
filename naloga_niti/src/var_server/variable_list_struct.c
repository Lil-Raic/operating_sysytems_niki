#include "variable_list_struct.h"
#include <string.h>

void init_variables_list(struct variables_list_struct *list) {
    list->count = 0;
    pthread_mutex_init(&list->mutex, NULL);
}

void clear_variables_list(struct variables_list_struct *list) {
    for (int i = 0; i < list->count; i++) {
        clear_variable(&list->vars[i]);
    }
    pthread_mutex_destroy(&list->mutex);
}

struct variable_struct *
find_named_variable(struct variables_list_struct *list, const char *name) {
    pthread_mutex_lock(&list->mutex);

    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->vars[i].name, name) == 0) {
            pthread_mutex_unlock(&list->mutex);
            return &list->vars[i];
        }
    }

    pthread_mutex_unlock(&list->mutex);
    return NULL;
}

struct variable_struct *
create_new_variable(struct variables_list_struct *list, const char *name) {
    pthread_mutex_lock(&list->mutex);

    if (list->count >= MAX_VARIABLES) {
        pthread_mutex_unlock(&list->mutex);
        return NULL;
    }

    struct variable_struct *var = &list->vars[list->count++];
    init_variable(var, name);

    pthread_mutex_unlock(&list->mutex);
    return var;
}

int remove_named_variable(struct variables_list_struct *list, const char *name) {
    pthread_mutex_lock(&list->mutex);

    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->vars[i].name, name) == 0) {
            clear_variable(&list->vars[i]);
            list->vars[i] = list->vars[list->count - 1];
            list->count--;
            pthread_mutex_unlock(&list->mutex);
            return 0;
        }
    }

    pthread_mutex_unlock(&list->mutex);
    return -1;
}
