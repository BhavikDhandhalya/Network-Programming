#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>

#include "vote.h"
#include "mq.h"

int __child_id;
int msgid;
int parent_mq_id;

void 
_run_child() {
	printf("Child %d created successfully !!\n", getpid());

	/*Lets create Message Queue per child*/
	__child_id = getpid();
	msgid = msgget(__child_id, 0666 | IPC_CREAT);

	while (1) {
		/*Waiting for parents message
		reading message from message queue*/
		msgrcv(msgid, &message, sizeof(message), 1, 0);

		/*if parent want to finish execution*/
		if (message._exit == DO_EXIT) break;

		/*tossing coin to decide vote 0 or 1*/
		message.vote = toss_coin();
		printf("Child %d voted %d\n", getpid(), message.vote);

		/*child sending its vote to parent*/
		parent_mq_id = msgget(getppid(), 0666 | IPC_CREAT);
		msgsnd(parent_mq_id, &message, sizeof(message), 0);
	}
}