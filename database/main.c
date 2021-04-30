#include "includes/main.h"


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
    printf("%s > Database socket initialized.\n", APP_NAME);

    // Continually accept incoming connections
    for(;;) {
        if(listen(sock_fd, 3) == -1) {
            fprintf(stderr, "%s > [ERROR] %s\n", APP_NAME, strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("%s > Listening for connection...\n", APP_NAME);
        int client_socket;
        struct sockaddr client_address;
        socklen_t client_address_length;
        if((client_socket = accept(sock_fd, (struct sockaddr*) &client_address, (socklen_t*) &client_address_length)) == -1) {
            fprintf(stderr, "%s > [ERROR] %s\n", APP_NAME, strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("%s > Connection received, offloaded to thread.\n", APP_NAME);
        pthread_t worker_id;
        if(pthread_create(&worker_id, NULL, worker, (void*) NULL)) {
            fprintf(stderr, "%s > [ERROR] Failed to create worker thread for new connection.\n", APP_NAME);
            exit(EXIT_FAILURE);
        }
        if(pthread_detach(worker_id)) {
            fprintf(stderr, "%s > [ERROR] Failed to detach worker process.\n", APP_NAME);
            exit(EXIT_FAILURE);
        }
        /*char buffer[1024] = {0};
        int num_bytes_read = read(client_socket, buffer, 1024);
        printf("recv (%d): ", num_bytes_read);
        for(int i = 0; i < num_bytes_read; i++) {
            printf("0x %02x\n", buffer[i]);
        }
        printf("\n");*/
    }
    close(sock_fd);
    return 0;
}

void *worker(void *arg) {
    printf("%s > [W%d] Hello World!\n", APP_NAME, (int) pthread_self());
    pthread_exit(NULL);
}

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