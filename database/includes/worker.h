#ifndef DATABASE_WORKER_H
#define DATABASE_WORKER_H

#include "main.h"

typedef struct worker_args {
    int client_socket;
    size_t worker_id;
    int authenticated;
    char *request_buffer;
} worker_args;

void *worker_start(void *arg);
void worker_listen(worker_args *w_args);
void worker_initialize(worker_args *w_args);
void worker_authenticate(worker_args *w_args);
void worker_abort(worker_args *w_args, char *error);
void worker_print(worker_args *w_args, char *msg);

char *sha_256(char *input);

#endif