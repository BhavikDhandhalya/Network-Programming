#include "libraries.h"

int
main(int argN, char *argV[]) {

    for (;;) {
        /* sleep for 1 Minute */
        sleep(60);

        system("find 1/ -type f -cmin -1 -delete");
    }

    return 0;
}