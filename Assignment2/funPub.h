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
    if (argN != 4) {
        printf("Expected Arguments\n");
        printf("<./filename> <1 for publisher/2 for subscriber> <broker address> <port>\n");
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
show_pub_options() {
    printf("select your option\n");
    printf("1. create topic\n");
    printf("2. send Message\n");
    printf("3. send file\n");
    printf("4. Exit\n");
    printf(":> ");
}

void
show_sub_options() {
    printf("select your option\n");
    printf("1. subscribe to topic\n");
    printf("2. Get Message\n");
    printf("3. Get Next Message\n");
    printf("4. Get All Messages\n");
    printf("5. Exit\n");
    printf(":> ");
}

void
create_topic() {
    struct Message m;

    printf("Enter topic name: ");
    scanf("%s", m.topic);
    printf("Your topic is %s\n", m.topic);

    printf("Enter content of Message: ");
    scanf("%s", m.content);

    printf("Creating Message\n");
    m.whois = IS_PUBLISHER;
    m.seqNo = -1;
    m.isFile = 0;
    m.isQuery = 0;

    printf("Sending Message\n");

    void *ptr = &m;
    write(sfd, (char *) ptr, sizeof(m));
}

void
send_file() {
    struct Message m;
    char file_name[20];

    printf("Enter file name : ");
    scanf("%s", file_name);

    printf("Sending File to broker\n");

    FILE *fp;

    char c;
    int ind = 0;

    bzero(m.content, sizeof(m.content));

    do {
        c = fgetc(fp);
        m.content[ind++] = c;
        m.isFile = 1;
        m.whois = IS_PUBLISHER;
        m.seqNo = -1;
        m.isQuery = 0;

        if (ind == CONTENT_SIZE) {
            void *ptr = &m;
            write(sfd, (char *) ptr, sizeof(m));
            
            bzero(m.content, sizeof(m.content));
            ind = 0;
        }
    } while (c != EOF);

    strcpy(m.content, "EOF");
    void *ptr = &m;
    write(sfd, (char *) ptr, sizeof(m));
}

void
subscribe_topic() {

}

void
get_Message() {
    struct Message m;
    printf("Enter topic name\n");
    scanf("%s", m.topic);

    m.seqNo = -1;
    m.isFile = -1;
    m.whois = IS_SUBSCRIBER;

    void *ptr = &m;
    write(sfd, (char *) ptr, sizeof(m));

    /* wait for broker's reply */
    printf("wait for Broker's reply\n");
    read(sfd, &m, MESSAGE_SIZE);

    printf("Message %s\n", m.content);
}

void
get_Next_Message() {

}

void
get_All_Messages() {

}