#include "includes/driver.h"

unsigned int rocket_connect(ROCKET *con, char *host, char *username, char *password, char *db, unsigned int port) {
    // Connect to database socket
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

    // Read challenge bytes
    char challenge_bytes[16];
    if(read(con->sock_fd, challenge_bytes, 16) != 16) {
        con->last_error_code = ROCKET_FAILURE;
        strcpy(con->error, "Failed to read challenge bytes.");
        return 1;
    }

    // Calculate challenge response
    char challenge_response[32];
    size_t challenge_nonce_size = 16 + strlen(password);
    char *challenge_nonce = (char *) malloc(256);
    if(challenge_nonce == NULL) {
        con->last_error_code = ROCKET_FAILURE;
        strcpy(con->error, "Failed to allocate challenge nonce.");
        return 1;
    }
    memcpy(challenge_nonce, challenge_bytes, 16);
    memcpy(challenge_nonce + 16, password, strlen(password));
    crypto_hash_sha256((unsigned char *) challenge_response, (const unsigned char *) challenge_nonce, challenge_nonce_size);

    // Send challenge response
    if(send(con->sock_fd, challenge_response, 32, 0) != 32) {
        con->last_error_code = ROCKET_FAILURE;
        strcpy(con->error, "Failed to send challenge response.");
        return 1;
    }

    // Read acknowledgement
    char authentication_acknowledgement[1];
    if(read(con->sock_fd, authentication_acknowledgement, 1) != 1) {
        con->last_error_code = ROCKET_FAILURE;
        strcpy(con->error, "Failed authentication.");
        return 1;
    }

    if(strncmp(authentication_acknowledgement, "1", 1) != 0) {
        con->last_error_code = ROCKET_FAILURE;
        strcpy(con->error, "Received incorrect authentication acknowledgement.");
        return 1;
    }

    con->last_error_code = ROCKET_SUCCESS;
    return 0;
}

void rocket_exit(ROCKET *con) {
    close(con->sock_fd);
    exit(EXIT_SUCCESS);
}

void rocket_print(char *msg) {
    printf("ROCKET > %s\n", msg);
}

const char *rocket_read_error(ROCKET *con) {
    switch(con->last_error_code) {
        case ROCKET_FAILURE:
            return con->error;
            break;
        default:
            return NULL;
            break;
    }
}

void rocket_print_error(ROCKET *con, FILE *fd) {
    fprintf(fd, "ROCKET > [ERROR] %s\n", rocket_read_error(con));
}

void rocket_die(ROCKET *con, FILE *fd) {
    rocket_print_error(con, fd);
    exit(EXIT_FAILURE);
}