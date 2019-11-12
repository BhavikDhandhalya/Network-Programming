/*
This file contains all functions and global variables used
by BROKER
*/
#include "libraries.h"

int i, j;
int sfd;
int cfd;
int opt;
int addrlen;

struct sockaddr_in srv_addr, clt_addr;


void
EXIT_ERROR (char *msg) {
    printf("%s\n", msg);
    printf("[-] Exiting\n");
    exit(EXIT_FAILURE);
}

void
check_arg(int argN) {
    if (argN != 2) {
        printf("Expected Arguments\n");
        printf("<./filename> <broker address> <port>\n");
        exit(EXIT_FAILURE);
    }
}

void
create_socket(char *port) {
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
    srv_addr.sin_port = htons(atoi(port));

    int enable = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    // bind the socket
    if (bind(sfd, (struct sockaddr *) &srv_addr, sizeof(srv_addr)) < 0) {
        perror("bind failed\n");
        exit(EXIT_FAILURE);    
    }

    // listen
    if (listen(sfd, 10) < 0) {
        perror("error while listening\n");
        exit(EXIT_FAILURE);
    }
}

void
save_topic(char *topic_name) {
    /* add CLA here */
    char *file_name = NULL;
    char *folder_name = "1/";
    file_name = malloc (strlen(topic_name) + strlen(folder_name) + 1);

    strcat(file_name, folder_name);
    strcat(file_name, topic_name);
    printf("creating file to this path %s\n", file_name);

    FILE *fp;
    fp = fopen(file_name, "w");
    fclose(fp);

    printf("Topic added successfully\n");
}

void
temp_accept() {
    cfd = accept(sfd, (__TYPE_CAST *) &clt_addr, &addrlen);

    printf("publisher connected\n");

    struct Message m;

    printf("waiting for publisher message\n");

    read(cfd, &m, MESSAGE_SIZE);
    printf("TOPIC IS = %s\n", m.topic);
    printf("WHO IS = %d\n", m.whois);
    printf("SEQ NO = %d\n", m.seqNo);

    printf("Saving topic\n");
    save_topic(m.topic);
}