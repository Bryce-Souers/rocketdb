

#include <stdio.h>
#include "driver.h"


int main(int argc, char* argv[]) {

    ROCKET con;
    if(rocket_connect(&con, "127.0.0.1", "username", "password", "database1", 3306))
        rocket_die(&con, stderr);

    //printf("client connected\n");

    return 0;
}