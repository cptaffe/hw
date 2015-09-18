#include <stdlib.h>

int hascalledrand = 0;

double rand_() {
	if (!hascalledrand) {
		srand(300);
		hascalledrand=1;
	}
	return ((double)rand())/RAND_MAX;
}
