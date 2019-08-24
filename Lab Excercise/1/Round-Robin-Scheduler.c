/*
Bhavik Dhandhalya
2018H1030118P
Network Programming
*/

#define _POSIX_SOURCE
#define _GNU_SOURCE
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>

int N;
int status;
int *process_id;
int *busy_id;
int done_reading;
int myID = -1;
int prev;
int isRunning;

FILE *fp;
char *line;
size_t len;
int X;
int line_count;
int file_p[1000][2];
int *got_ack;
int number_of_lines;
int global_counter;

int p[100][2];

void parent_signal_handler(int sigNum,siginfo_t *info,void *abc){
	//printf("Signal Sending Process Is %d\n",info->si_pid);

	int current_child_pid = info->si_pid;

	int ind = -1;
	for (int i = 0; i < N; i++) {
		if (current_child_pid == process_id[i])
			ind = i;
	}

	busy_id[ind] = 0;
	line_count--;
	//got_ack[global_counter++] = 1;
}

void child_signal_handler(int sigNum) {
	//kill(getppid(),SIGUSR2);
}

void error(char *msg) {
	printf("%s\n", msg);
	exit(0);
}

void pre_compute() {
	if (fp == NULL)
		error("Error while opning file !!!");

	while ((X = getline(&line, &len, fp)) != -1) {
		//printf("Retrieved line of length %zu:\n", X);
		//printf("%s\n", line);
		file_p[line_count++][0] = X + prev;
		X = X + prev;
		prev = X;
		//printf("line_count %d, line %s\n", line_count, line);
	}
	number_of_lines = line_count;
	fclose(fp);
}

void read_file() {
	//printf("My ID is %d %d\n",getpid(),myID);
	//close(p[myID][1]);
	while (1) {
		pause();

		int line_no;
		read(p[myID][0], &line_no, sizeof(int));

		//printf("%d child -- %d\n", getpid(), line_no);

		FILE *fp1; 
    	fp1 = fopen("text", "r"); 
    	if (fp1 == NULL)
    		error("Error while opening file");
    	//Moving pointer to end 
    	fseek(fp1, file_p[line_no-1][0], 0); 
    	//Printing position of pointer 
    	getline(&line, &len, fp1);

		printf("%d %s\n", getpid(), line);
		sleep(1);
		kill(getppid(), SIGUSR2);
	}
}

int isReceived() {
	int ok = 1;
	for (int i = 0; i < number_of_lines; i++)
		if (got_ack[i] == 0)
			ok = 0;

	return ok;
}

int main(int argN, char *argv[]) {

	fp = fopen(argv[1], "r");
	pre_compute();

	got_ack = (int *) malloc (sizeof(int) * number_of_lines);
	for (int i = 0; i < number_of_lines; i++)
		got_ack[i] = 0;

	N = atoi(argv[2]);
	for (int i = 0; i < N; i++) {
		pipe(p[i]);
	}

	process_id = (int *)malloc(sizeof(int) * N);
	busy_id = (int *)malloc(sizeof(int) * N);

    // initially all child are free
	for (int i = 0; i < N; i++)
		busy_id[i] = 0;

	struct sigaction parent_action;
	sigemptyset(&parent_action.sa_mask);
	parent_action.sa_flags = 0;
	parent_action.sa_handler = parent_signal_handler;
	sigaction(SIGUSR2,&parent_action,NULL);

    // signal registration
	signal(SIGUSR1, child_signal_handler);

	for (int i = 0; i < N; i++) {
		int child_id = fork();
		++myID;
		if (child_id == 0) {
			read_file();
		}

		process_id[i] = child_id;
	}

	// for (int i = 0; i < N; i++)
	// 	printf("process_id %d\n", process_id[i]);

    // ROUND-ROBIN
	done_reading = 0;
	//printf("line count %d\n", line_count);
	while (line_count > 0) {
		for (int i = 0; i < N; i++) {
			if (busy_id[i] == 0) {
				//printf("%d %d\n", i, done_reading);
				if (done_reading <= number_of_lines) {
					write(p[i][1], &done_reading, sizeof(done_reading));
				
					done_reading++;
				}
				kill(process_id[i],SIGUSR1);
	//			printf("parent wrote line %d\n", done_reading - 1);
			}
		}
	}

	for (int i = 0; i < N; i++)
		kill(process_id[i], SIGKILL);
	
	wait(&status);

	return 0;
}
