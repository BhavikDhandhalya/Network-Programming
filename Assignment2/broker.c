#include "funBro.h"

int
main (int argN, char *argV[]) {
    check_arg(argN);
    MESSAGE_PATH = argV[4];
    /* we are identifying broker by port */
    ipAdd = argV[1];
    port = argV[3];
    next_BROKER = atoi(argV[3]);
    my_ID = atoi(argV[2]);

    create_socket(argV[2]);

    //temp_accept();

    close(sfd);
    return 0;
}