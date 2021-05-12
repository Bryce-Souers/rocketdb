#include "includes/driver.h"


/*
 * 1. Establish connection
 * 2. Receive challenge
 * 3. Send challenge response
 * 4. Send requests
 */

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

    char buffer[32] = {0};
    int num_bytes_read = read(con->sock_fd, buffer, 32);
    if(num_bytes_read < 32) {
        con->last_error_code = ROCKET_FAILURE;
        strcpy(con->error, "Invalid challenge string received.");
        return 1;
    }

    printf("Received: ");
    for(int i = 0; i < num_bytes_read; i++) {
        printf("%x ", buffer[i] & 0xff);
    }
    printf("\n");

    char* challenge_response = sha_256(password);
    if((send(con->sock_fd, challenge_response, 32, 0)) == -1) {
        con->last_error_code = ROCKET_FAILURE;
        strcpy(con->error, strerror(errno));
        return 1;
    }

    /*unsigned char* hash = sha256(password);

    if((send(con->sock_fd, hash, 32, 0)) == -1) {
        con->last_error_code = ROCKET_FAILURE;
        strcpy(con->error, strerror(errno));
        return 1;
    }*/

    // Receive challenge string
    /*char buffer[32] = {0};
    int num_bytes_read = read(con->sock_fd, buffer, 32);
    if(num_bytes_read < 32) {
        con->last_error_code = ROCKET_FAILURE;
        strcpy(con->error, "Invalid challenge string received.");
        return 1;
    }
    char hash_input[64] = {0};
    strcat(hash_input, password);
    strcat(hash_input, buffer);
    char* hash_output = sha_256(hash_input);*/

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

char* sha_256(char* input) {
    char* hash = malloc(32);
    crypto_hash_sha256((unsigned char*) hash, (const unsigned char*) input, sizeof(input));
    return hash;
}