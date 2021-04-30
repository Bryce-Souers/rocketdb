#include "includes/rocketdb.h"


unsigned int rocket_connect(ROCKET* con, char* host, char* username, char* password, char* db, unsigned int port) {
    if((con->sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        con->last_error_code = ROCKET_FAILURE;
        strcpy(con->error, strerror(errno));
        return 1;
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(host);
    if((connect(con->sock_fd, (struct sockaddr*) &address, sizeof(address))) == -1) {
        con->last_error_code = ROCKET_FAILURE;
        strcpy(con->error, strerror(errno));
        return 1;
    }

    unsigned char hash[32];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA224_Update(&ctx, password, sizeof(password));
    SHA256_Final(hash, &ctx);

    if((send(con->sock_fd, hash, 32, 0)) == -1) {
        con->last_error_code = ROCKET_FAILURE;
        strcpy(con->error, strerror(errno));
        return 1;
    }
    close(con->sock_fd);
    con->last_error_code = ROCKET_SUCCESS;
    return 0;
}

const char* rocket_read_error(ROCKET* con) {
    switch(con->last_error_code) {
        case ROCKET_FAILURE:
            return con->error;
            break;
        default:
            return NULL;
            break;
    }
}

void rocket_print_error(ROCKET* con, FILE* fd) {
    fprintf(fd, "ROCKET > [ERROR] %s\n", rocket_read_error(con));
}

void rocket_die(ROCKET* con, FILE* fd) {
    rocket_print_error(con, fd);
    exit(EXIT_FAILURE);
}