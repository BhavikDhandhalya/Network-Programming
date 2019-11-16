#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#define MESSAGE_SIZE 512
#define CONTENT_SIZE 333
#define TOPIC_SIZE 20
#define TOTAL_FDS 20
#define __TYPE_CAST struct sockaddr

#define IS_PUBLISHER 1
#define IS_SUBSCRIBER 2
#define IS_BROKER 3


struct Message {
    int whois;
    int seqNo;
    int isFile;
    int isQuery;
    char topic[TOPIC_SIZE];
    char content[CONTENT_SIZE];
};

char *
string_con(char *__a, char *__b) {
    char *__c = NULL;
    __c = malloc(strlen(__a) + strlen(__b) + 1);

    strcat(__c, __a);
    strcat(__c, __b);
}