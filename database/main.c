#include "includes/main.h"
#include "includes/worker.h"

int main(int argc, char* argv[]) {
    handle_arguments(argc, argv);
    init();

    // Continually accept incoming connections
    for(;;) {

        // Set socket to listen
        if(listen(server_socket, 3) == -1) {
            fprintf(stderr, "%s > [ERROR] %s\n", APP_NAME, strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Block and accept connections
        int client_socket;
        struct sockaddr client_address;
        socklen_t client_address_length = sizeof((struct sockaddr*) &client_address);
        if((client_socket = accept(server_socket, &client_address, &client_address_length)) == -1) {
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

    clean_up();
    return 0;
}

void handle_arguments(int argc, char **argv) {
    if(argc < 3) {
        fprintf(stderr, "%s > [ERROR] Usage: %s <host_ip> <host_port>\n", APP_NAME, argv[0]);
        exit(EXIT_FAILURE);
    }
    host_ip = argv[1];
    host_port = atoi(argv[2]);
}

void init() {
    // Capture SIGINT signal
    signal(SIGINT, handle_interrupt);

    // Initialize server socket
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "%s > [ERROR] %s\n", APP_NAME, strerror(errno));
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if((setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) == -1) {
        fprintf(stderr, "%s > [ERROR] %s\n", APP_NAME, strerror(errno));
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host_ip);
    address.sin_port = htons(host_port);
    if((bind(server_socket, (struct sockaddr*) &address, sizeof(address))) == -1) {
        fprintf(stderr, "%s > [ERROR] %s\n", APP_NAME, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void clean_up() {
    close(server_socket);
    printf("%s > Cleaned up, goodbye!\n", APP_NAME);
    exit(EXIT_SUCCESS);
}

void handle_interrupt() {
    printf("\n%s > Received SIGINT signal...\n", APP_NAME);
    clean_up();
}