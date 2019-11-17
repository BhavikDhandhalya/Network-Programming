/*
This file contains all functions and global variables used
by BROKER
*/
#include "libraries.h"
#include "myHash.h"

int i, j;
int sfd;
int cfd;
int leftFD;
int opt;
int addrlen;
int counter;
int maxfd;
int client_fds[TOTAL_FDS];
int N;
int nready;
int connected_clients;

char *ipAdd = NULL;
char *port = NULL;
char *MESSAGE_PATH = NULL;
int my_ID;
int next_BROKER;

struct Message m;
fd_set all_set, rset;
struct sockaddr_in srv_addr, clt_addr, servaddr;

void
EXIT_ERROR (char *msg) {
    printf("%s\n", msg);
    printf("[-] Exiting\n");
    exit(EXIT_FAILURE);
}

void
check_arg(int argN) {
    if (argN != 5) {
        printf("Expected Arguments\n");
        printf("<./filename> <broker address> <server port> <client port> <folder name/>\n");
        exit(EXIT_FAILURE);
    }

    init_hash();
}

void
save_content(char *topic_name, char *content, int X) {
    char file_name[20];
    sprintf(file_name, "%d", X);
    printf("Creating file %s\n", file_name);

    FILE *fp;
    fp = fopen(file_name, "w");
    fprintf(fp, "%s", content);
    fclose(fp);
}

void
save_topic(char *topic_name, char *content) {
    print_hash();
    /* check if topic is already exist */
    counter = get_counter_hash(topic_name) + 1;
    printf("counter %d\n", counter);

    if (counter == 0) {
        /* If topic doesn't exist, create one */
        printf("Topic doesn't exist\n");

        chdir(MESSAGE_PATH);
        mkdir(topic_name, 0777);
        chdir(topic_name);

        insert_hash(topic_name, counter);
        save_content(topic_name, content, counter);
        
        chdir("..");
        chdir("..");
    } else {
        /* If topic exists */
        printf("Topic exist\n");
        printf("Adding Content\n");

        chdir(MESSAGE_PATH);
        chdir(topic_name);

        update_insert_hash(topic_name, counter);
        save_content(topic_name, content, counter);

        chdir("..");
        chdir("..");
    }
}

void
temp_accept() {
    cfd = accept(sfd, (__TYPE_CAST *) &clt_addr, &addrlen);

    printf("publisher connected\n");

    printf("waiting for publisher message\n");

    struct Message m;
    read(cfd, &m, MESSAGE_SIZE);
    printf("TOPIC IS = %s\n", m.topic);
    printf("WHO IS = %d\n", m.whois);
    printf("SEQ NO = %d\n", m.seqNo);
    printf("CONTENT = %s\n", m.content);

    printf("Saving topic\n");
    save_topic(m.topic, m.content);
    printf("\n\n");
}

void
connect_to_broker() {
    // socket create
	leftFD = socket(AF_INET, SOCK_STREAM, 0);
	if (leftFD == -1) { 
		printf("socket creation failed...\n"); 
		close(leftFD);
		return;
	}
	
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ipAdd); 
	servaddr.sin_port = htons(atoi(port));
    
    // connect the client socket to server socket 
	if (connect(leftFD, (__TYPE_CAST *) &servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n");
		close(leftFD);
		return;
	}
}

void
handle_PUBLISHER(int confd) {
    printf("[ handling PUBLISHER ]\n\n");
    /* get publisher data, and store it */
    if (m.isFile == 0) 
        save_topic(m.topic, m.content);
    /* HANDLE FILE HERE */
}

void
handle_SUBSCRIBER(int confd) {
    printf("[ handling SUBSCRIBER ]\n\n");
    printf("Broker ID %d\n\n", my_ID);
    
    m.BROKER_ID = my_ID;
    /* search topic in your directory first */
    if (get_counter_hash(m.topic) == -1) {
        /* topic doesn't exist */
        printf("Topic doesn't EXIST\n");
        printf("Connecting to Broker\n");

        connect_to_broker();
        
        struct Message Q;
        bzero(&Q, sizeof(Q));

        Q.whois = IS_BROKER;
        Q.seqNo = -1;
        Q.isQuery = 1;
        Q.gotIt = 0;
        Q.BROKER_ID = m.BROKER_ID;
        strcpy(Q.topic, m.topic);

        printf("Sending Query Message to Broker\n");
        void *ptr = &Q;
        write(leftFD, (char *) ptr, sizeof(Q));

        printf("Waiting for Replay from Broker\n");

        struct Message response;
        bzero(&response, sizeof(response));

        printf("received response from Broker\n");
        read(leftFD, &response, MESSAGE_SIZE);
        perror("Error leftFD");
        printf("response = %s\n", response.content);

        printf("Sending Received response to subscriber\n");
        void *ptr2 = &response;
        write(confd, (char *) ptr2, sizeof(response));
        perror("Error confd");

        close(leftFD);
    } else {
        /* topic exists */
        printf("Topic EXISTS\n");

        struct Message response;

        chdir(MESSAGE_PATH);
        chdir(m.topic);

        char file_name[20];
        bzero(file_name, sizeof(file_name));

        sprintf(file_name, "%d", get_start_counter_hash(m.topic));
        printf("reading from file %s\n", file_name);

        FILE *fp;
        fp = fopen(file_name, "r");

        char c;
        int ind = 0;

        while ((c = fgetc(fp)) != EOF)
            response.content[ind++] = c;

        response.content[ind] = '\0';

        printf("got data = %s\n", response.content);

        response.gotIt = 1;
        response.whois = IS_SUBSCRIBER;

        chdir("..");
        chdir("..");

        printf("Sending data to subscriber\n");
        void *ptr = &response;
        write(confd, (char *) ptr, sizeof(response));
    }
}

void
handle_BROKER(int confd) {
    printf("[ handling BROKER ]\n\n");
    printf("[next broker = %d] and Message Broker id %d\n", next_BROKER, m.BROKER_ID);

    /* In this function we will handle Query Messages */
    if (get_counter_hash(m.topic) == -1) {
        /* Topic doesn't EXIST */
        printf("Topic doesn't EXIST\n");

        /* If we are not at the end of our Topology */
        if (m.BROKER_ID != next_BROKER) {
            connect_to_broker();

            struct Message Q;
            bzero(&Q, sizeof(Q));

            Q.whois = IS_BROKER;
            Q.seqNo = -1;
            Q.isQuery = 1;
            Q.BROKER_ID = m.BROKER_ID;
            strcpy(Q.topic, m.topic);

            printf("Sending Query Message to Broker\n");
            void *ptr = &Q;
            write(leftFD, (char *) ptr, sizeof(Q));

            printf("Waiting for Replay from Broker\n");

            struct Message response;
            bzero(&response, sizeof(response));

            printf("received response from Broker\n");
            read(leftFD, &response, MESSAGE_SIZE);
            perror("Error leftFD");
            printf("response = %s\n", response.content);

            printf("Sending Received response to subscriber\n");
            void *ptr2 = &response;
            write(confd, (char *) ptr2, sizeof(response));
            perror("Error confd");

            close(leftFD);         
        } else {
            printf("No topic found in entire topology\n");

            struct Message emptyM;
            bzero(&emptyM, sizeof(emptyM));

            strcpy(emptyM.topic, m.topic);
            strcpy(emptyM.content, "EOF");

            void *ptr = &emptyM;
            write(confd, (char *) ptr, sizeof(emptyM));
        }
    } else {
        printf("Topic EXISTS\n");

        struct Message response;

        chdir(MESSAGE_PATH);
        chdir(m.topic);

        char file_name[20];
        bzero(file_name, sizeof(file_name));

        sprintf(file_name, "%d", get_start_counter_hash(m.topic));
        printf("reading from file %s\n", file_name);

        FILE *fp;
        fp = fopen(file_name, "r");

        char c;
        int ind = 0;

        while ((c = fgetc(fp)) != EOF)
            response.content[ind++] = c;

        response.content[ind] = '\0';

        printf("got data = %s\n", response.content);

        printf("deleting Message\n");
        remove(file_name);

        response.whois = IS_BROKER;
        response.isQuery = 1;
        response.gotIt = 1;

        chdir("..");
        chdir("..");

        printf("Sending data to broker\n");
        void *ptr = &response;
        write(confd, (char *) ptr, sizeof(response));
    }
}

void
processFD(int confd) {
    bzero(&m, sizeof(m));
    read(confd, &m, MESSAGE_SIZE);

    if (m.whois == IS_PUBLISHER) {
        printf("got Message from publisher\n");
        handle_PUBLISHER(confd);
    } else if (m.whois == IS_SUBSCRIBER) {
        printf("got Message from subscriber\n");
        handle_SUBSCRIBER(confd);
    } else if (m.whois == IS_BROKER) {
        printf("got Message from broker\n");
        handle_BROKER(confd);
    }
}

void
readFromFD() {
    for (int i = 0; i < TOTAL_FDS; i++)
        if (client_fds[i] != -1)
            if (FD_ISSET(client_fds[i], &rset)) {
                processFD(client_fds[i]);
                FD_CLR(client_fds[i], &all_set);
                close(client_fds[i]);
                client_fds[i] = -1;
            }
}

void
create_socket(char *serport) {
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
    srv_addr.sin_port = htons(atoi(serport));

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
    for (i = 0; i < TOTAL_FDS; i++)
        client_fds[i] = -1;

    /* initializes the set pointed to by fd_set to be empty */
    FD_ZERO(&all_set);

    /* adding sfd to set */
    FD_SET(sfd, &all_set);

    /* making ACCEPT call NON-Blocking */
    int flags = fcntl(sfd, F_GETFL, 0);
    fcntl(sfd, F_SETFL, flags | O_NONBLOCK);

    for ( ; ; ) {
        rset = all_set;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        /* if sfd is ready to read */
        if (FD_ISSET(sfd, &rset)) {
            addrlen = sizeof(clt_addr);

            cfd = accept(sfd, (__TYPE_CAST *) &clt_addr, &addrlen);
            printf("[+] got connection from client with %d cfd\n", cfd);

            for (i = 0; i < TOTAL_FDS; i++)
                if (client_fds[i] == -1) {
                    client_fds[i] = cfd;
                    /* add new client fd to our fd_set */
                    FD_SET(cfd, &all_set);
                    connected_clients = (i > connected_clients) ? i : connected_clients;
                    break;
                }

            maxfd = (cfd > maxfd) ? cfd : maxfd;
        }

        readFromFD();
    }
}