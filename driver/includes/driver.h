#ifndef ROCKET_DRIVER_H
#define ROCKET_DRIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <sodium.h>

#define PORT 3306

typedef enum {
    ROCKET_SUCCESS,
    ROCKET_FAILURE
} rocket_result;

typedef struct ROCKET {
    char *host;
    char *username;
    char *password;
    char *db;
    unsigned int port;
    int sock_fd;

    unsigned int last_error_code;
    char error[64];
} ROCKET;

rocket_result rocket_connect(ROCKET *con, char *host, char *username, char *password, char *db, unsigned int port);

const char *rocket_read_error(ROCKET *con);
void rocket_print(char *msg);
void rocket_print_error(ROCKET *con, FILE *fd);
void rocket_die(ROCKET *con, FILE *fd);
void rocket_exit(ROCKET *con);

#endif