#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <syslog.h>
#include <netdb.h>

#define PORT 9997
#define BUF_SIZE 4096
#define LIMIT 100
#define _TYPE_CAST struct sockaddr

int __stop = 20;

int i;
int j;
int totalCommands;
int sfd;

char buf[BUF_SIZE];
char hostIP[20];
char gInput[BUF_SIZE];
struct sockaddr_in servaddr, cli;

int
main(int argN, char *argV[]) {
    if (argN != 3) {
        printf("Expecting <./filename> <port> <message> as argument\n");
        exit(EXIT_FAILURE);
    }

	// socket create and varification 
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) { 
		printf("socket creation failed...\n"); 
		close(sfd);
		return 0; 
	}
	else
		printf("Socket successfully created..\n");
	
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(atoi(argV[1]));

	// connect the client socket to server socket 
	if (connect(sfd, (_TYPE_CAST *) &servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n");
		close(sfd);
		return 0;
	}
	else
		printf("connected to the server..\n");

    printf("[+] sleeping for 5 seconds\n");
    sleep(5);
	
    printf("[+] sending %s to server\n\n", argV[2]);
	write(sfd, argV[2], strlen(argV[2]));


	while (--__stop) {
        char buf[BUF_SIZE];
        bzero(buf, sizeof(buf));

        read(sfd, buf, BUF_SIZE);
        if (strlen(buf) != 0)
            printf("[+] received message %s\n", buf);

        sleep(1);
    }

	close(sfd);

	return 0;
}