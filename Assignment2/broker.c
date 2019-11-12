#include "funBro.h"

int
main (int argN, char *argV[]) {
    check_arg(argN);

    create_socket(argV[1]);

    temp_accept();

    close(sfd);
    return 0;
}