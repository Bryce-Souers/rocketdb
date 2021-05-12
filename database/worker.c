#include "includes/worker.h"


/*
 * 1. Send challenge
 * 2. Authenticate challenge response
 * 3. Continually accept incoming requests
 */

void* worker_start(void* arg) {
    worker_args* w_args = (worker_args*) arg;

    worker_initialize(w_args);
    worker_authenticate(w_args);

    pthread_exit(NULL);
}

void worker_initialize(worker_args* w_args) {
    w_args->worker_id = pthread_self();

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if(setsockopt(w_args->client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof(tv)) != 0)
        worker_error(w_args, "Setting socket timeout", strerror(errno));
}

void worker_authenticate(worker_args* w_args) {
    char* challenge_bytes = malloc(CHALLENGE_SIZE);
    randombytes_buf(challenge_bytes, CHALLENGE_SIZE);
    if(challenge_bytes == NULL) worker_error(w_args, "Failed to allocate challenge bytes buffer", NULL);
    if(send(w_args->client_socket, challenge_bytes, CHALLENGE_SIZE, 0) == -1)
        worker_error(w_args, "Failed to send challenge bytes", strerror(errno));

    printf("Sent: ");
    for(int i = 0; i < CHALLENGE_SIZE; i++) {
        printf("%x ", challenge_bytes[i] & 0xff);
    }
    printf("\n");

    char challenge_response_buffer[CHALLENGE_SIZE];
    int num_challenge_br = read(w_args->client_socket, challenge_response_buffer, CHALLENGE_SIZE);
    if(num_challenge_br == -1) worker_error(w_args, "Failed to read challenge response", strerror(errno));
    if(num_challenge_br < CHALLENGE_SIZE) worker_error(w_args, "Failed to read challenge response", NULL);

    printf("Received: ");
    for(int i = 0; i < num_challenge_br; i++) {
        printf("%x ", challenge_response_buffer[i] & 0xff);
    }
    printf("\n");
}

void worker_error(worker_args* w_args, char* topic, char* error) {
    if(error == NULL) fprintf(stderr, "ERROR [W%zu] > %s\n", w_args->worker_id, topic);
    else fprintf(stderr, "ERROR [W%zu] > %s\n\t%s\n", w_args->worker_id, topic, error);
    pthread_exit(NULL);
}