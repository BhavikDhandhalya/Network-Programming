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
#define __TYPE_CAST struct sockaddr

#define IS_PUBLISHER 1
#define IS_SUBSCRIBER 2
#define IS_BROKER 3


struct Message {
    int whois;
    int seqNo;
    char topic[20];
};