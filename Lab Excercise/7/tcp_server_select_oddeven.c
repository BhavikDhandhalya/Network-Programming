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
#include <sys/select.h>

#define BUF_SIZE 256
#define SA struct sockaddr

int i, j;
int sfd;
int cfd, lfd, maxfd;
int status;
int opt;
int addrlen;
int _FD_SETSIZE;
int nready;
int client_fds[BUF_SIZE];
int connected_clients;

int N;
fd_set all_set, rset;
struct sockaddr_in srv_addr, clt_addr;

void EXIT_ERROR(char *msg);
void print(char *msg);

int
main (int argN, char *argV[]) {
    if (argN != 3)
        EXIT_ERROR("[-] Expecting <./filename> <no. of clients> <port> as command line arguments");

    N = atoi(argV[1]);
    _FD_SETSIZE = N;
    printf("Expecting maximum %d clients\n", N);

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
    srv_addr.sin_port = htons(atoi(argV[2]));

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

    maxfd = sfd;
    for (i = 0; i < _FD_SETSIZE; i++)
        client_fds[i] = -1;

    /* initializes the set pointed to by fd_set to be empty */
    FD_ZERO(&all_set);

    /* adding sfd to set */
    FD_SET(sfd, &all_set);

    for ( ; ; ) {
        rset = all_set;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        /* if sfd is part of our socket or fd set */
        if (FD_ISSET(sfd, &rset)) {
            addrlen = sizeof(clt_addr);

            cfd = accept(sfd, (SA *) &clt_addr, &addrlen);
            printf("[+] got connection from client with %d cfd\n", cfd);

            for (i = 0; i < _FD_SETSIZE; i++)
                if (client_fds[i] == -1) {
                    client_fds[i] = cfd;
                    /* add new client fd to our fd_set */
                    FD_SET(cfd, &all_set);
                    connected_clients = (i > connected_clients) ? i : connected_clients;
                    break;
                }

            maxfd = (cfd > maxfd) ? cfd : maxfd;
        }

        /* if all clients are connected */
        if (client_fds[_FD_SETSIZE - 1] != -1)
            break;
    }

#ifdef FOREVER
    for (;;) {
#endif
    print("Waiting for clients data");
    char *data[_FD_SETSIZE];
    for (i = 0; i < _FD_SETSIZE; i++) {
        char received_data[BUF_SIZE];
        bzero(received_data, sizeof(received_data));

        read(client_fds[i], received_data, BUF_SIZE);
        printf("[=] received %s from client %d\n", received_data, client_fds[i]);

        data[i] = (char *) malloc (sizeof(char) * strlen(received_data));
        strcpy(data[i], received_data);
    }

    print("received data for all clients are as follow");
    for (i = 0; i < _FD_SETSIZE; i++)
        printf("[+] from client %d data = %s\n", client_fds[i], data[i]);

    print("Send Odd fd client data to All Even fd clients");
    for (i = 0; i < _FD_SETSIZE; i++)
        if (client_fds[i] & 1)
            for (j = 0; j < _FD_SETSIZE; j++)
                if (i != j && client_fds[j] % 2 == 0)
                    write(client_fds[j], data[i], strlen(data[i]));

    print("Send Even fd client data to All Odd fd clients");
    for (i = 0; i < _FD_SETSIZE; i++)
        if (client_fds[i] % 2 == 0)
            for (j = 0; j < _FD_SETSIZE; j++)
                if (i != j && client_fds[j] & 1) 
                    write(client_fds[j], data[i], strlen(data[i]));

#ifdef FOREVER
    }
#endif
    return 0;
}

void
EXIT_ERROR (char *msg) {
    printf("%s\n", msg);
    printf("[-] Exiting\n");
    exit(EXIT_FAILURE);
}

void
print (char *msg) {
    printf("[+] %s\n", msg);
}