#include "includes/main.h"
#include "includes/worker.h"


int main(int argc, char* argv[]) {
    // Handle arguments
    if(argc < 3) {
        fprintf(stderr, "%s > [ERROR] Usage: %s <host_ip> <host_port>\n", APP_NAME, argv[0]);
        exit(EXIT_FAILURE);
    }
    host_ip = argv[1];
    host_port = atoi(argv[2]);

    // Initialize database socket
    int sock_fd;
    init_socket(&sock_fd, host_ip, host_port);

    // Continually accept incoming connections
    for(;;) {
        // Listen for connections
        if(listen(sock_fd, 3) == -1) {
            fprintf(stderr, "%s > [ERROR] %s\n", APP_NAME, strerror(errno));
            exit(EXIT_FAILURE);
        }
        int client_socket;
        struct sockaddr client_address;
        socklen_t client_address_length = sizeof((struct sockaddr*) &client_address);

        // Block and accept connections
        if((client_socket = accept(sock_fd, &client_address, &client_address_length)) == -1) {
            fprintf(stderr, "%s > [ERROR] %s\n", APP_NAME, strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Start a worker thread to handle connection and detach
        pthread_t worker_id;
        worker_args* w_args = malloc(sizeof(worker_args));
        if(w_args == NULL) {
            fprintf(stderr, "%s > [ERROR] Failed to allocate worker arguments.\n", APP_NAME);
            exit(EXIT_FAILURE);
        }
        w_args->client_socket = client_socket;
        if(pthread_create(&worker_id, NULL, worker_start, (void*) w_args)) {
            fprintf(stderr, "%s > [ERROR] Failed to create worker thread for new connection.\n", APP_NAME);
            exit(EXIT_FAILURE);
        }
        if(pthread_detach(worker_id)) {
            fprintf(stderr, "%s > [ERROR] Failed to detach worker process.\n", APP_NAME);
            exit(EXIT_FAILURE);
        }
    }
    close(sock_fd);
    return 0;
}

/*void *worker_init(void *arg) {
    pthread_t worker_id = pthread_self();
    worker_args* w_args = (worker_args*) arg;

    // Send challenge string
    void* challenge_string = malloc(32);
    randombytes_buf(challenge_string, 32);
    printf("challenge: %s\n", (char*) challenge_string);
    if(send(w_args->client_socket, challenge_string, 32, 0) == -1) {
        fprintf(stderr, "%s > [W%d] Failed to send challenge string to client.\n", APP_NAME, (int) worker_id);
        exit(EXIT_FAILURE);
    }

    char buffer[1024] = {0};
    int num_bytes_read = read(w_args->client_socket, buffer, 1024);
    printf("recv (%d): ", num_bytes_read);
    for(int i = 0; i < num_bytes_read; i++) {
        printf("%x ", buffer[i] & 0xff);
    }
    printf("\n");
    pthread_exit(NULL);
}*/

void init_socket(int *sock_fd, char* host_ip, int host_port) {
    if((*sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "%s > [ERROR] %s\n", APP_NAME, strerror(errno));
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if((setsockopt(*sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) == -1) {
        fprintf(stderr, "%s > [ERROR] %s\n", APP_NAME, strerror(errno));
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host_ip);
    address.sin_port = htons(host_port);
    if((bind(*sock_fd, (struct sockaddr*) &address, sizeof(address))) == -1) {
        fprintf(stderr, "%s > [ERROR] %s\n", APP_NAME, strerror(errno));
        exit(EXIT_FAILURE);
    }
}