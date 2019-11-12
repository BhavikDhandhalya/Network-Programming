/*
This file contains all functions and global variables used
by PUBLISHER
*/
#include "libraries.h"

int i;
int j;
int sfd;

struct sockaddr_in servaddr;

void
check_arg(int argN) {
    if (argN != 3) {
        printf("Expected Arguments\n");
        printf("<./filename> <broker address> <port>\n");
        exit(EXIT_FAILURE);
    }
}

void
connect_to_broker(char *ipAdd, char *port) {
    // socket create
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) { 
		printf("socket creation failed...\n"); 
		close(sfd);
		return; 
	}
	
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ipAdd); 
	servaddr.sin_port = htons(atoi(port));

	// connect the client socket to server socket 
	if (connect(sfd, (__TYPE_CAST *) &servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n");
		close(sfd);
		return;
	}
}

void
show_options() {
    printf("select your option\n");
    printf("1. create topic\n");
    printf("2. send Message\n");
    printf("3. send file\n");
    printf("4. Exit\n");
    printf(":> ");
}

void
create_topic() {
    struct Message m;

    printf("Enter topic name: ");
    scanf("%s", m.topic);

    printf("Your topic is %s\n", m.topic);
    printf("Creating Message\n");

    m.whois = IS_PUBLISHER;
    m.seqNo = -1;

    printf("Sending Message\n");

    void *ptr = &m;
    write(sfd, (char *) ptr, sizeof(m));
}