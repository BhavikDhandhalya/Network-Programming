#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

const int sz = 5e8;

int main() {
    FILE *fp;

    fp = freopen("GB", "w", stdout);

    for (int i = 0; i < 2 * sz; i++)
        printf("b");
    
    fclose(fp);
    return 0;
}