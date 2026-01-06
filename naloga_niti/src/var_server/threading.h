#ifndef THREADING_H
#define THREADING_H

struct threading_struct {
    /* no global state needed */
};

void threading_setup(struct threading_struct *threading_obj);
void threading_cleanup(struct threading_struct *threading_obj);

void handle_in_thread(struct threading_struct *threading_obj,
                      void (*handler_function)(void *args),
                      void *args);

#endif
