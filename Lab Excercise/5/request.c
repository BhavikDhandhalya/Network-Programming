#define _XOPEN_SOURCE 700
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h> /* getprotobyname */
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

#define DEBUG

#define BUF_SIZE 20000

int
main(int argN, char *argV[]) {
    
    if (argN != 2) {
        printf("Command line arguments format\n");
        printf("Enter Command line arguments without http://");
        printf("<./request> <www.google.com/...>\n");
        return 0;
    }
#ifdef DEBUG
    printf("argN %d\n", argN);
    for (int i = 0;  i < argN; i++)
        printf("%d = %s\n", i+1, argV[i]);
#endif

    int sfd;
    struct hostent* clt;
    in_addr_t in_addr;
    int isSecondPart = 0;
    int original_len = strlen(argV[1]);

    char *p = strtok(argV[1], "/");
    char host[100];
    char secondpart[10000];
    strcpy(host, p);
    printf("host = %s\n", host);

    printf("original len %d, host %d\n", original_len, strlen(host));

    if (strlen(host) != original_len && original_len-1 != strlen(host))
        for (;;) {
            p = strtok(NULL, " ");
            if (strlen(p) == 0) break;
            isSecondPart = 1;
            printf("second part = %s\n", p);
            strcpy(secondpart, p);
            break;
        }

#ifdef DEBUG
    printf("[debug] secondpart = %s\n", secondpart);
#endif

    clt = gethostbyname(host);

    if (!clt) {
        printf("Error gethostbyname");
        exit(EXIT_FAILURE);
    }

    printf("ip = %s\n", inet_ntoa( (struct in_addr) *((struct in_addr *) clt->h_addr_list[0])));
    in_addr = inet_addr(inet_ntoa( (struct in_addr) *((struct in_addr *) clt->h_addr_list[0])));

    /* connect to the server */
    struct protoent *protoent;
    struct sockaddr_in sockaddr_in;
    unsigned short server_port = 80;

    /* bind */
    protoent = getprotobyname("tcp");
    if (protoent == NULL) {
        perror("getprotobyname");
        exit(EXIT_FAILURE);
    }

    sfd = socket(AF_INET, SOCK_STREAM, protoent->p_proto);
    if (sfd == -1) {
        printf("Error Socket\n");
        exit(EXIT_FAILURE);
    }

    sockaddr_in.sin_addr.s_addr = in_addr;
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(server_port);

    /* Actually connect. */
    if (connect(sfd, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG
    printf("[debug] Connected to host %s\n", host);
#endif

    /* create HTTP request */
    char a[] = "GET /";
    char *b = " HTTP/1.1\r\nHOST:";
    char *c = "\r\n\r\n";
    char *s = "/";

    char request[BUF_SIZE];
    bzero(request, sizeof(request));

    if (isSecondPart)
        snprintf(request, BUF_SIZE, "%s%s%s%s%s", a, secondpart, b, host, c);
    else
        snprintf(request, BUF_SIZE, "%s%s%s%s", a, b, host, c);

    printf("[request] %s\n\n", request);

    int len = strlen(request);
    int nbytes = 0;

    send(sfd, request, strlen(request), 0);
    clock_t begin = clock();

    char buf[BUF_SIZE];
    nbytes = 0;
    while ((nbytes = read(sfd, buf, BUF_SIZE)) > 0) {
        printf("%s\n", buf);

        if (strlen(buf) < BUF_SIZE) break;
    }

    close(sfd);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("[TIME] %lf\n", time_spent);
    return 0;
}