#include "funPub.h"

void
runPublisher(char *ipAdd, char *port) {
	printf("PUBLISHER\n");
	for (;;) {
		show_pub_options();
		connect_to_broker(ipAdd, port);

		int user_input;
		scanf("%d", &user_input);

		switch (user_input) {
			case 1:
				create_topic();
				break;
			case 2:
				break;
			case 3:
				send_file();
				break;
			case 4:
				close(sfd);
				exit(0);
				break;
			default:
				printf("Invalid input\n");
				break;
		}

		close(sfd);
	}
}

void
runSubscriber(char *ipAdd, char *port) {
	printf("SUBSCRIBER\n");
	for (;;) {
		show_sub_options();

		connect_to_broker(ipAdd, port);

		int user_input;
		scanf("%d", &user_input);

		switch (user_input) {
			case 1:
				get_Message(port);
				break;
			case 2:
				get_Message(port);
				break;
			case 3:
				get_Message(port);
				break;
			case 4:
				get_All_Messages(port);
				break;
			case 5:
				close(sfd);
				exit(0);
				break;
			default:
				printf("Invalid input\n");
				break;
		}

		close(sfd);
	}
}

int
main(int argN, char *argV[]) {
	check_arg(argN);

	if (argV[1][0] == '1')
		runPublisher(argV[2], argV[3]);

	else if (argV[1][0] == '2')
		runSubscriber(argV[2], argV[3]);

	return 0;
}