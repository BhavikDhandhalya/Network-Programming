/*
This program helps to find unique msgid for each process,
that will help to get message from Message Queue System V IPC.
*/

#include <sys/ipc.h>
#include <sys/msg.h>

#define DO_EXIT 1
#define DONT_EXIT -1

struct msg_buffer {
	long message_type;
	int vote;
	int _exit;
} message;

int 
get_msgid(int _child_id) {
	return msgget(_child_id, 0666 | IPC_CREAT);
}