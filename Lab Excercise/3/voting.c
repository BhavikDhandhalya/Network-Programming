#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>

#include "child.h"

int N;
int inp;
int status;
int p_msgid;
int child_id;
int *process_ids;
int *mq_ids;
int ones, zeros;
char __vote;

void 
check_arguments (int n) {
	if (n < 2) {
		printf("Less Arguments then Expected !\n");
		printf("Argument format : ./voting <int>\n");
		exit(0);
	}
}

int 
main (int argN, char *argv[]) {
	check_arguments(argN);

	N = atoi(argv[1]);
	process_ids = (int *) malloc (sizeof(int) * N);
	mq_ids = (int *) malloc (sizeof(int) * N);

	printf("parent creating message queue for itself\n");
	p_msgid = msgget(getpid(), 0666 | IPC_CREAT);

	printf("Creating %d Childs\n", N);

	for (int i = 0; i < N; i++) {
		child_id = fork();

		if (child_id == 0) {
			/*Inside child process*/
			_run_child();
			exit(0);
		} else process_ids[i] = child_id;

		/*identifying message queue for every child and storing its identifier - msgid*/
		mq_ids[i] = msgget(child_id, 0666 | IPC_CREAT);
	}

	printf("Parent will wait for 2 seconds\n");
	sleep(2);

	while (1) {
		printf("Press 1 for voting and -1 to exit\n");
		scanf("%d", &inp);

		if (inp == 1) {
			/*parent will request for votes*/
			message._exit = DONT_EXIT;
			message.message_type = 1;

			/*requesting vote from every child*/
			for (int i = 0; i < N; i++)
				msgsnd(mq_ids[i], &message, sizeof(message), 0);

		} else {
			/*parent will tell them to finish execution*/
			message._exit = DO_EXIT;
			message.message_type = 1;

			/*requesting vote from every child*/
			for (int i = 0; i < N; i++)
				msgsnd(mq_ids[i], &message, sizeof(message), 0);

			break;
		}

		if (inp == 1) {
			/*parent will fetch votes*/
			ones = zeros = 0;
			for (int i = 0; i < N; i++) {
				/*parent fetching individual child's vote from their message queue*/
				msgrcv(p_msgid, &message, sizeof(message), 1, 0);

				__vote = message.vote;

				if (__vote) ++ones;
				else ++zeros;
			}

			if (ones > zeros) printf("Ones are majority\n");
			else printf("Zeros are majority\n");
		}
	}

	/*parent will wait for all child to finish their execution*/
	for (int i = 0; i < N; i++)
		waitpid(process_ids[i], &status, 0);

	printf("[Execution Done] !!\n\n");
	return 0;
}