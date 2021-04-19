#include "includes/rocketdb.h"

unsigned int rocket_connect(ROCKET* con, char* host, char* username, char* password, char* db, unsigned int port) {
    con->last_error_code = ROCKET_FAILURE;
    return 1;
}

const char* rocket_read_error(ROCKET* con) {
    switch(con->last_error_code) {
        case ROCKET_FAILURE:
            return "Something bad happened.";
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