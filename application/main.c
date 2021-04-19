#include <stdio.h>
#include "rocketdb.h"

int main(int argc, char* argv[]) {
    ROCKET con;
    rocket_connect(&con, "192.168.1.53", "username", "password", "database1", 0);
    return 0;
}