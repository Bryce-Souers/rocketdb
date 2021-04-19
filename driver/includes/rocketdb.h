#ifndef ROCKET_DRIVER_H
#define ROCKET_DRIVER_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    ROCKET_SUCCESS,
    ROCKET_FAILURE
} rocket_result;

typedef struct ROCKET {
    char* host;
    char* username;
    char* password;
    char* db;
    unsigned int port;
    unsigned int last_error_code;
} ROCKET;

rocket_result rocket_connect(ROCKET* con, char* host, char* username, char* password, char* db, unsigned int port);

const char* rocket_read_error(ROCKET* con);
void rocket_print_error(ROCKET* con, FILE* fd);
void rocket_die(ROCKET* con, FILE* fd);

#endif