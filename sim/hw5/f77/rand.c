#include <stdlib.h>
#include <stdio.h>
#include <f2c.h>

doublereal rand_() {
	return ((doublereal)rand()) / RAND_MAX;
}
