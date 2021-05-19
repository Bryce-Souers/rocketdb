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
#include <signal.h>

#define APP_NAME "rocket"
#define _DATABASE_PASSWORD "password"

char *host_ip;
int host_port;
int server_socket;

/* FUNCTION DECLARATIONS */
void handle_arguments(int argc, char **argv);
void init();
void clean_up();
void handle_interrupt();

#endif