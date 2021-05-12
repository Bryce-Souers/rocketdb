

#ifndef DATABASE_MAIN_H
#define DATABASE_MAIN_H

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sodium.h>

#define APP_NAME "rocket"


char *host_ip;
int host_port;

void init_socket(int *sock_fd, char* host_ip, int host_port);

#endif