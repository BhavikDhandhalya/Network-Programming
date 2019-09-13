#include <time.h>
#include <stdlib.h>

unsigned int 
toss_coin() {
	/* we will use current time to generate Random number */
	srand(time(0));

	return (rand() * rand()) % 2;
}