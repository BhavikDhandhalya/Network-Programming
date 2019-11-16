#include "funBro.h"

#define MESSAGE_PATH "Messages1/"

int
main (int argN, char *argV[]) {
    check_arg(argN);

    create_client_socket(argV[1], argV[3]);

    create_socket(argV[2]);

    temp_accept();

    close(sfd);
    return 0;
}