#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

#define PORT 3306

int main(int argc, char* argv[]) {
    int sock_fd;
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "ROCKET > [ERROR] %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if((setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) == -1) {
        fprintf(stderr, "ROCKET > [ERROR] %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if((bind(sock_fd, (struct sockaddr*) &address, sizeof(address))) == -1) {
        fprintf(stderr, "ROCKET > [ERROR] %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if(listen(sock_fd, 3) == -1) {
        fprintf(stderr, "ROCKET > [ERROR] %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    int client_socket;
    int address_length;
    if((client_socket = accept(sock_fd, (struct sockaddr*) &address, (socklen_t*) &address_length)) == -1) {
        fprintf(stderr, "ROCKET > [ERROR] %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    char buffer[1024] = {0};
    int num_bytes_read = read(client_socket, buffer, 1024);
    printf("recv (%d): %s\n", num_bytes_read, buffer);
    return 0;
}