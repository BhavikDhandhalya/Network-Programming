/*
Bhavik Dhandhalya
2018H1030118P
Network Programming 4th Excersice
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

void doWrite();
void doMMAP();

char *input_file;
char *output1;
char *output2;

int
main(int argN, char *argV[]) {
    if (argN != 4) {
        printf("Command line argument format\n");
        printf("<./mmap_write> <reading file> <output file 1> <output file 2>\n");
        printf("output file 1 - MMAP will write in this output file\n");
        printf("output file 2 - write will write in this output file\n");
        printf("Example: ./mmap 1GB.txt out1.txt out2.txt\n\n");
        exit(EXIT_FAILURE);
    }

    input_file = argV[1];
    output1 = argV[2];
    output2 = argV[3];

    /* Time using Mmap*/
    doMMAP();
    /* Time using write() */
    doWrite();
    return 0;
}

void
doWrite() {
    /* Calculate begin time */
    clock_t begin = clock();

    int fd = open(output1, O_WRONLY | O_CREAT | O_TRUNC, 0777);

    if (fd < 0) {
        perror("[-] doWrite fd");
        exit(EXIT_FAILURE);
    }

    FILE *fp;
    fp = fopen(input_file, "r");

    char *line = NULL;
    size_t read, len;

    /* Reading line from source file &
    writing to destination file line by line */
    while ((read = getline(&line, &len, fp)) != -1) {
        write(fd, line, strlen(line));
        line = NULL;
    }

    if (line)
        free(line);

    close(fd);
    fclose(fp);

    /* Calculate Finish time */
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("[TIME - write] %lf\n", time_spent);
}

void
doMMAP() {
    /* Calculate begin time */
    clock_t begin = clock();

    int sfd; /* Source file descriptor */
    int dfd; /* Desti file descriptor */

    int fileSize;
    struct stat SB; /* Details about file that we read */

    /* source file */
    if ((sfd = open(input_file, O_RDONLY)) == -1) {
        printf("[-] Error opening source file");
        exit(EXIT_FAILURE);
    }

    /* destination file */
    if ((dfd = open(output2, O_RDWR | O_CREAT | O_TRUNC, 0777)) == -1) {
        printf("[-] Error while opening destination file");
        exit(EXIT_FAILURE);
    }

    /* Obtain the size of the file and use it to specify the size of
    the mapping and the size of the buffer to be written  */
    fstat(sfd, &SB);
    fileSize = SB.st_size;

    /* write at the end of destination file */
    lseek(dfd, fileSize-1, SEEK_SET);
    write(dfd, "", 1);

    /* src contains content of source file / segment by segment */
    char *src;
    /* we will copy "src" segment in this segment*/
    char *dst;

    if ((
        src = mmap(
            (caddr_t) 0, /* address - we don't care */
            fileSize,   /* length */
            PROT_READ,   /* The contents of the region can be read */
            MAP_SHARED,  /* It is shared part */
            sfd,         /* source file descriptor */
            0      /* offset - from where to start READING next segment */
        )) == MAP_FAILED
    ) {
        printf("[-] Error source mmap\n");
        exit(EXIT_FAILURE);
    }

    if ((
        dst = mmap(
            (caddr_t) 0, /* address - we don't care */
            fileSize,   /* length */
            PROT_READ | PROT_WRITE,   /* The contents of the region can be read or modified */
            MAP_SHARED,  /* It is shared part */
            dfd,         /* destination file descriptor */
            0      /* offset - from where to start WRITING next segment */
        )) == MAP_FAILED
    ) {
        printf("[-] Error desti mmap\n");
        exit(EXIT_FAILURE);
    }

    /* Copy source segment to Destination segment */
    memcpy(dst, src, fileSize);

    /* De-allocate segment */
    if ((munmap(src, fileSize)) == -1) {
        printf("[-] Error unmaping src\n");
    }

    if ((munmap(dst, fileSize)) == -1) {
        printf("[-] Error unmaping src\n");
    }

    close(sfd);
    close(dfd);

    /* Calculate Finish time */
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("[TIME - mmap] %lf\n", time_spent);
}