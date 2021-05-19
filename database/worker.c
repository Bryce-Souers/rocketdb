#include "includes/worker.h"

void* worker_start(void* arg) {
    worker_args* w_args = (worker_args*) arg;

    worker_initialize(w_args);
    worker_authenticate(w_args);

    for(;;) worker_listen(w_args);

    close(w_args->client_socket);
    pthread_exit(NULL);
}

void worker_listen(worker_args *w_args) {
    printf("Listening for request...\n");
    int read_status = read(w_args->client_socket, w_args->request_buffer, 256);
    if(read_status == 0) // Client closed connection
        worker_abort(w_args, "Client closed connection.");
    else if(read_status == -1) // Read error
        worker_abort(w_args, "Failed to read from client socket.");

    worker_print(w_args, w_args->request_buffer);
}

void worker_authenticate(worker_args* w_args) {
    // Generate 16 challenge bytes
    char challenge_bytes[16];
    randombytes_buf(challenge_bytes, 16);

    // Send challenge challenge bytes
    if(send(w_args->client_socket, challenge_bytes, 16, 0) != 16)
        worker_abort(w_args, "Failed to send challenge bytes to client.");

    // Read challenge response
    char challenge_response[32];
    if(read(w_args->client_socket, challenge_response, 32) != 32)
        worker_abort(w_args, "Failed to read challenge response from client.");

    // Calculate expected challenge response
    char expected_response[32];
    size_t challenge_nonce_size = 16 + strlen(_DATABASE_PASSWORD);
    char *challenge_nonce = (char *) malloc(256);
    if(challenge_nonce == NULL) worker_abort(w_args, "Failed to allocate expected response");
    memcpy(challenge_nonce, challenge_bytes, 16);
    memcpy(challenge_nonce + 16, _DATABASE_PASSWORD, strlen(_DATABASE_PASSWORD));
    crypto_hash_sha256((unsigned char *) expected_response, (const unsigned char *) challenge_nonce, challenge_nonce_size);
    free(challenge_nonce);

    // Verify challenge and send acknowledgement
    if(strncmp(challenge_response, expected_response, 32) == 0) {
        if(send(w_args->client_socket, "1", 1, 0) != 1)
            worker_abort(w_args, "Failed to send authentication acknowledgement.");
        worker_print(w_args, "Client authenticated.");
        w_args->authenticated = 1;
    } else worker_abort(w_args, "Client failed authentication.");
}

void worker_initialize(worker_args* w_args) {
    w_args->worker_id = pthread_self();
    w_args->authenticated = 0;
    w_args->request_buffer = (char *) malloc(256);
    if(w_args->request_buffer == NULL) worker_abort(w_args, "Failed to allocate request buffer.");

    struct timeval tv;
    tv.tv_sec = 30;
    tv.tv_usec = 0;
    if(setsockopt(w_args->client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof(tv)) != 0)
        worker_abort(w_args, "Failed to set socket timeout.");
}

void worker_print(worker_args *w_args, char *msg) {
    printf("[W-%zu] > %s\n", w_args->worker_id, msg);
}

void worker_abort(worker_args* w_args, char* error) {
    fprintf(stderr, "[W-%zu] > [ERROR] %s\n", w_args->worker_id, error);
    close(w_args->client_socket);
    pthread_exit(NULL);
}

/*
    int i;
    for(i = 0; i < 32; i++) printf("%x ", expected_response[i] & 0xff);
    printf("\n");
 */