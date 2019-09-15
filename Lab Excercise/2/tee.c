/*
Bhavik Dhandhalya
Network Programming
Lab Exe.2
*/

#include <stdio.h>
#include <stdlib.h>

void check(int N) {
	if (N < 2) {
		printf("Less arguments then expected !\n");
		exit(0);
	}

	printf("\nArguments format: <command> | ./tee <output file name> <output file name> ...\n\n");
}

int main(int argN, char *argv[]) {
	check(argN);

	FILE *fpr[argN];

	for (int i = 1; i < argN; i++)
		fpr[i] = fopen(argv[i], "w");

	char c;
	for (;;) {
		c = fgetc(stdin);
		if (c == EOF) break;
		printf("%c", c);

		for (int i = 1; i < argN; i++) {
			fpr[i] = fopen(argv[i], "a");
			fputc(c, fpr[i]);
			fclose(fpr[i]);
		}
	}

	return 0;
}