#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>

int i;
int totalCommands;
int sfd;
int cfd, lfd;
int status;
int opt;
int addrlen;
int child_id;
int errno;

int N;
struct sembuf sop;

#define BUF_SIZE 256
#define SA struct sockaddr
struct sockaddr_in srv_addr, clt_addr;

void
handleRequest() {
    int p[2];
    pipe(p);
    char buf[BUF_SIZE];

    printf("Executing blocking call\n");
    read(p[0], buf, BUF_SIZE);
}

void
sysERROR(char *msg) {
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}

int
main (int argN, char *argV[]) {
    N = atoi(argV[1]);

    // creating server socket file descriptor
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed !!\n");
        exit(EXIT_FAILURE);
    }

    // use port and address again even if they are already in use
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("error in reusage of address & port\n");
        exit(EXIT_FAILURE);
    }

    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    // Accept any connection
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    srv_addr.sin_port = htons(9999);

    int enable = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    // bind the socket
    if (bind(sfd, (struct sockaddr *) &srv_addr, sizeof(srv_addr)) < 0) {
        perror("bind failed\n");
        exit(EXIT_FAILURE);    
    }

    // listen
    if (listen(sfd, N) < 0) {
        perror("error while listening\n");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(clt_addr);

    /* Create Socket */
    int semID = semget(IPC_PRIVATE, 1 /*No of set*/, 0660 | IPC_CREAT);

    if (semID < 0)
        sysERROR("Error semget");

    if (semctl(semID, 0, SETVAL, N) == -1)
        sysERROR("Error semCTL");
    
    for(;;) {
        /* Wait for connection */
        
        /* Decrement */
        sop.sem_op = -1;
        semop(semID, &sop, 1);

        cfd = accept(sfd, (SA *) &clt_addr, &addrlen);
        printf("Client Accepted !!\n");
        if (cfd == -1) {
            perror("Error accept\n");
            exit(EXIT_FAILURE);
        }

        if (fork() == 0) {
            /* Each child process will handle
            linux command execution */
            sleep(10);
            /* Increment */
            sop.sem_op = 1;
            semop(semID, &sop, 1);
            _exit(1);
        }
    }

    return 0;
}