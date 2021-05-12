#ifndef DATABASE_WORKER_H
#define DATABASE_WORKER_H


#include "main.h"

#define CHALLENGE_SIZE 32

typedef struct worker_args {
    int client_socket;
    size_t worker_id;
} worker_args;

void* worker_start(void* arg);

void worker_initialize(worker_args* w_args);
void worker_authenticate(worker_args* w_args);
void worker_error(worker_args* w_args, char* topic, char* error);


#endif