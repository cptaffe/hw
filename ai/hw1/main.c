
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

#include "mt19937ar.c"

typedef struct {
	long double x, y, z, T;
	uint64_t bits;
} HyperSpace;

HyperSpace hyperSpaceFromBits(uint64_t b) {
	HyperSpace h;
	memset(&h, 0, sizeof(HyperSpace));
	h.x = (long double) ((b & 0xffff0000000000) >> 40) / 0xffff * 40 - 20;
	h.y = (long double) ((b & 0xffff000000) >> 24) / 0xffff * 60 - 30;
	h.z = (long double) ((b & 0xffff00) >> 8) / 0xffff * 50;
	h.T = (long double) (b & 0xff) / 0xff * 5;
	h.bits = b;
	return h;
}

long double simX(long double v[]) {
	const long double o = 10;
	return o * (v[1]-v[0]);
}

long double simY(long double v[]) {
	const long double r = 28;
	return r*v[0] - v[1] - v[0]*v[2];
}

long double simZ(long double v[]) {
	const long double b = 8.0/3.0;
	return v[0]*v[1] - b*v[2];
}

/*
 * Forward Euler
 */
HyperSpace hyperSpaceSimulateForwardEuler(HyperSpace *h) {
	const long double dt = 0.01;
	HyperSpace n = *h;
	long double t;
	for (t = 0; t < h->T; t += dt) {
		long double p[3];
		p[0] = n.x;
		p[1] = n.y;
		p[2] = n.z;
		n.x += dt * simX(p);
		n.y += dt * simY(p);
		n.z += dt * simZ(p);
	}
	return n;
}

/*
 * Classical 4th order Runge-Kutta method
 */
HyperSpace hyperSpaceSimulateRK4(HyperSpace *h) {
	const long double dt = 0.01;
	HyperSpace n = *h;
	long double t;
	for (t = 0; t < h->T; t += dt) {
		long double p[3];
		long double k[4][3];

		p[0] = n.x;
		p[1] = n.y;
		p[2] = n.z;

		k[0][0] = simX(p);
		k[0][1] = simY(p);
		k[0][2] = simZ(p);

		p[0] = n.x + dt/2*k[0][0];
		p[1] = n.y + dt/2*k[0][1];
		p[2] = n.z + dt/2*k[0][2];

		k[1][0] = simX(p);
		k[1][1] = simY(p);
		k[1][2] = simZ(p);

		p[0] = n.x + dt/2*k[1][0];
		p[1] = n.y + dt/2*k[1][1];
		p[2] = n.z + dt/2*k[1][2];

		k[2][0] = simX(p);
		k[2][1] = simY(p);
		k[2][2] = simZ(p);

		p[0] = n.x + dt*k[2][0];
		p[1] = n.y + dt*k[2][1];
		p[2] = n.z + dt*k[2][2];

		k[3][0] = simX(p);
		k[3][1] = simY(p);
		k[3][2] = simZ(p);

		n.x += dt/6*(k[0][0] + 2*k[1][0] + 2*k[2][0] + k[3][0]);
		n.y += dt/6*(k[0][1] + 2*k[1][1] + 2*k[2][1] + k[3][1]);
		n.z += dt/6*(k[0][2] + 2*k[1][2] + 2*k[2][2] + k[3][2]);
	}
	return n;
}

uint64_t randomC() {
	return ((double) rand())/RAND_MAX * 0x100000000000000;
}

uint64_t randomMT() {
	return genrand_res53() * 0x100000000000000;
}

/*
 * Global program configuration
 */
struct {
	HyperSpace (*simulate)(HyperSpace *h);
	uint64_t (*random)();
	int threads, restarts;
} Config;

long double hyperSpaceCost(HyperSpace *h) {
	const long double tx = 18, ty = 20, tz = 45;
	HyperSpace n = Config.simulate(h);
	return sqrtl(powl(h->x, 2) + powl(h->y, 2) + powl(h->z, 2)) + sqrtl(powl(n.x-tx, 2) + powl(n.y-ty, 2) + powl(n.z-tz, 2));
}

typedef struct {
	HyperSpace h;
	long double cost;
	uint64_t evals;
} Result;

void resultPprint(Result *r) {
	printf("%"PRIu64", %Lf, %Lf, %Lf, %Lf, %Lf\n", r->evals, r->cost, r->h.x, r->h.y, r->h.z, r->h.T);
}

/*
 * Generates all single-hamming-distance solutions
 * and selects the best one.
 * Slight optimization combines neighborhood generation
 * and greedy descent selection for O(1) vs O(n) space.
 */
bool hyperSpaceNext(Result *r) {
	Result t = *r;
	const int blen = 56;
	uint64_t bits = t.h.bits; /* Original bits */
	bool haveAdvanced;
	int i;
	for (i = 0; i < blen; i++) {
		Result lt;
		lt.h = hyperSpaceFromBits(bits ^ (1 << i));
		lt.cost = hyperSpaceCost(&lt.h);
		if (lt.cost < t.cost) {
			t = lt;
		}
	}
	haveAdvanced = r->h.bits != t.h.bits;
	r->h = t.h;
	r->cost = t.cost;
	r->evals = r->evals + blen;
	return haveAdvanced;
}

/*
 * Search thread parameters
 */
struct SearchThreadParams {
	int S;
	Result *r;
};

/*
 * search thread function
 * for use with pthread_create
 * void* cast to a SearchThreadParams*
 */
void *searchThread(void *stp) {
	struct SearchThreadParams *p = (struct SearchThreadParams *) stp;
	int s;
	for (s = 0; s < p->S; s++) {
		Result l;
		memset(&l, 0, sizeof(l));
			/*
			 * PCG bounded rand is on the range [0, bound).
			 */
		l.h = hyperSpaceFromBits(Config.random());
		l.cost = hyperSpaceCost(&l.h);
		l.evals = 1;
		/* Keeps a running total of evals */
		while (hyperSpaceNext(&l)) {}
		p->r[s] = l;
	}
	return NULL;
}

/*
 * Returns an array of S result tuples
 */
Result *search() {
	int S = Config.restarts;
	Result *r = calloc(sizeof(Result), S);
	pthread_t *ts = calloc(Config.threads, sizeof(pthread_t));
	struct SearchThreadParams *params = calloc(Config.threads, sizeof(struct SearchThreadParams));
	bool spawnThreads = S/Config.threads > 0;
	if (spawnThreads) {
		int i;
		for (i = 0; i < Config.threads; i++) {
			params[i].S = S/Config.threads;
			params[i].r = &r[i*(S/Config.threads)];
			if (pthread_create(
				&ts[i],
				NULL,
				searchThread,
				(void *) &params[i]
			) != 0) {
				fprintf(stderr, "Failed to spawn thread %d!\n", i);
				exit(1);
			}
		}
	}

	/*
	 * Possible that S did not divide evenly
	 * Run the remaining ones in this thread.
	 */
	if (S%Config.threads > 0) {
		struct SearchThreadParams *p = calloc(1, sizeof(struct SearchThreadParams));
		p->S = S%Config.threads;
		p->r = &r[S-S%Config.threads];
		searchThread(p);
		free(p);
	}

	if (spawnThreads) {
		int i;
		for (i = 0; i < Config.threads; i++) {
			pthread_join(ts[i], NULL);
		}
	}
	free(params);
	free(ts);
	return r;
}

void usage() {
	printf("main [-s|--simulate algorithm(rk4|euler)] [-r|--random function(mt|crand)] [-n|--restarts nrestarts] [-t|--threads [nthreads]] [-h|--help]\n");
	exit(1);
}

int comp(const void *a0, const void *a1) {
	Result *a = (Result*) a0, *b = (Result*) a1;
	if (a->cost < b->cost) return 1;
	if (a->cost > b->cost) return -1;
	return 0;
}

int main(int argc, char *argv[]) {
	int i;
	Result *t;
	unsigned long int mt_init[4]={0x123, 0x234, 0x345, 0x456};

	/*
	 * Initialize mersenne twister
	 */
	init_by_array(mt_init, sizeof(mt_init)/sizeof(unsigned long int));

	/*
	 * Default configuration
	 */
	Config.simulate = hyperSpaceSimulateForwardEuler;
 	Config.random = randomMT;
 	Config.threads = 1;
 	Config.restarts = 1000; /* S */

	/*
	 * Parse command line options
	 */
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--simulate") == 0) {
			if ((i + 1) < argc) {
				i++;
				if (strcmp(argv[i], "rk4") == 0) {
					Config.simulate = hyperSpaceSimulateRK4;
				} else if (strcmp(argv[i], "euler") == 0) {
					Config.simulate = hyperSpaceSimulateForwardEuler;
				} else {
					usage();
				}
			} else {
				usage();
			}
		} else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--random") == 0) {
			if ((i + 1) < argc) {
				i++;
				if (strcmp(argv[i], "mt") == 0) {
					Config.random = randomMT;
				} else if (strcmp(argv[i], "crand") == 0) {
					Config.random = randomC;
				} else {
					usage();
				}
			} else {
				usage();
			}
		} else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--threads") == 0) {
			if ((i + 1) < argc && argv[i+1][0] > '0' && argv[i+1][0] < '9') {
				int t = strtol(argv[i], NULL, 10);
				i++;
				if (t > 0) {
					Config.threads = t;
				} else {
					printf("Threads must be on the range [1, %d)\n", INT_MAX);
					usage();
				}
			} else {
				/* Default number of threads */
				Config.threads = sysconf(_SC_NPROCESSORS_ONLN);
			}
		} else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--restarts") == 0) {
			if ((i + 1) < argc) {
				int t = strtol(argv[i], NULL, 10);
				i++;
				if (t > 0) {
					Config.restarts = t;
				} else {
					printf("Restarts must be on the range [1, %d)\n", INT_MAX);
					usage();
				}
			} else {
				usage();
			}
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			printf("%s\n%s\n%s\n%s\n%s\n",
				"[-s|--simulate algorithm(rk4|euler)]\n"
				"    Simulate system using forward euler, the more accurate (but slower) \n"
				"        RK4 algorithm.",
				"[-r|--random function(mt|crand)]\n"
				"    Mersenne Twister random (default) is one of the best avaliable\n"
				"        pseudo-random number generators, while the stdlib C rand is very poor.",
				"[-n|--restarts nrestarts]\n"
				"    Number of random restarts, defaults to 1000 as per assignment.",
				"[-t|--threads [nthreads]]\n"
				"    Turn on threading, specifying no argument defaults to the machine's core number.",
				"[-h|--help]\n"
				"    Prints this help page."
			);
			usage();
		} else {
			usage();
		}
	}

	fprintf(stderr, "Running with %d %s.\n", Config.threads, (Config.threads > 1) ? "threads" : "thread");
	t = search();
	qsort(t, Config.restarts, sizeof(Result), comp);
	for (i = 0; i < Config.restarts; i++) {
		resultPprint(&t[i]);
	}
	free(t);
}
