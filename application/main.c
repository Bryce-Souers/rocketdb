#include <stdio.h>
#include "driver.h"

int main(int argc, char* argv[]) {

    ROCKET con;
    if(rocket_connect(&con, "127.0.0.1", "username", "password", "database1", 3306))
        rocket_die(&con, stderr);

    printf("app > Connected to database!\n");

    char *command = (char *) malloc(256);
    for(;;) {
        printf("app > Enter a command: ");
        fgets(command, 256, stdin);
        command[strcspn(command, "\n")] = 0;
        printf("app > Command: %s\n", command);
        if(strcmp(command, "stop") == 0) {
            printf("stopping!\n");
            rocket_exit(&con);
        }
    }

    return 0;
}