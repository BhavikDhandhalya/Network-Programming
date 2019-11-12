#include "funPub.h"

int
main(int argN, char *argV[]) {
	check_arg(argN);

	connect_to_broker(argV[1], argV[2]);

	for (;;) {
		show_options();

		int user_input;
		scanf("%d", &user_input);

		switch (user_input) {
			case 1:
				create_topic();
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				exit(0);
				break;
			default:
				printf("Invalid input\n");
				break;
		}
	}

	close(sfd);
	return 0;
}