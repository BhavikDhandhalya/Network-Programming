#include "funBro.h"

int
main (int argN, char *argV[]) {
    check_arg(argN);
    MESSAGE_PATH = argV[4];

    create_client_socket(argV[1], argV[3]);

    create_socket(argV[2]);

    temp_accept();

    close(sfd);
    return 0;
}