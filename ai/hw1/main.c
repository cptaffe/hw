
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

// PCG random numbers
#include "pcg_variants.h"

typedef struct {
	long double x, y, z, T;
	uint64_t bits;
} HyperSpace;

HyperSpace hyperSpaceFromBits(uint64_t b) {
	return (HyperSpace){
		.x = (long double) ((b & 0xffff0000000000) >> 40) / 0xffff * 40 - 20,
		.y = (long double) ((b & 0xffff000000) >> 24) / 0xffff * 60 - 30,
		.z = (long double) ((b & 0xffff00) >> 8) / 0xffff * 50,
		.T = (long double) (b & 0xff) / 0xff * 5,
		.bits = b
	};
}

inline long double simX(long double x, long double y, long double z);
inline long double simY(long double x, long double y, long double z);
inline long double simZ(long double x, long double y, long double z);

long double simX(long double x, long double y, long double z) {
	const long double o = 10;
	return o * (y-x);
}

long double simY(long double x, long double y, long double z) {
	const long double r = 28;
	return r*x - y - x*z;
}

long double simZ(long double x, long double y, long double z) {
	const long double b = 8l/3l;
	return x*y - b*z;
}

// Forward Euler
HyperSpace hyperSpaceSimulateForwardEuler(HyperSpace *h) {
	const long double dt = 0.01;
	HyperSpace n = *h;
	for (long double t = 0; t < h->T; t += dt) {
		HyperSpace m = n;
		// Forward Euler
		n.x += dt * simX(m.x, m.y, m.z);
		n.y += dt * simY(m.x, m.y, m.z);
		n.z += dt * simZ(m.x, m.y, m.z);
	}
	return n;
}

// Classical 4th order Runge-Kutta method
HyperSpace hyperSpaceSimulateRK4(HyperSpace *h) {
	const long double dt = 0.01;
	HyperSpace n = *h;
	for (long double t = 0; t < h->T; t += dt) {
		HyperSpace m = n;
		long double
			k1x = simX(m.x, m.y, m.z),
			k1y = simY(m.x, m.y, m.z),
			k1z = simZ(m.x, m.y, m.z),

			k2x = simX(m.x + dt/2*k1x, m.y + dt/2*k1y, m.z + dt/2*k1z),
			k2y = simY(m.x + dt/2*k1x, m.y + dt/2*k1y, m.z + dt/2*k1z),
			k2z = simZ(m.x + dt/2*k1x, m.y + dt/2*k1y, m.z + dt/2*k1z),

			k3x = simX(m.x + dt/2*k2x, m.y + dt/2*k2y, m.z + dt/2*k2z),
			k3y = simY(m.x + dt/2*k2x, m.y + dt/2*k2y, m.z + dt/2*k2z),
			k3z = simZ(m.x + dt/2*k2x, m.y + dt/2*k2y, m.z + dt/2*k2z),

			k4x = simX(m.x + dt*k3x, m.y + dt*k3y, m.z + dt*k3z),
			k4y = simY(m.x + dt*k3x, m.y + dt*k3y, m.z + dt*k3z),
			k4z = simZ(m.x + dt*k3x, m.y + dt*k3y, m.z + dt*k3z);

		n.x += dt/6*(k1x + 2*k2x + 2*k3x + k4x);
		n.y += dt/6*(k1y + 2*k2y + 2*k3y + k4y);
		n.z += dt/6*(k1z + 2*k2z + 2*k3z + k4z);
	}
	return n;
}

// Dormand-Prince method
HyperSpace hyperSpaceSimulateDP(HyperSpace *h) {
	HyperSpace n = *h;
	long double dt = 0.01;
	for (long double t = 0; t < h->T; t += dt) {
		long double v[] = { n.x, n.y, n.z };
		long double (*f[])(long double, long double, long double) = { simX, simY, simZ };
		long double k[7][3]; // k1-7 for x,y,z
		const long double c[7][7] = {
			{1l/5l},
			{3l/4l, 9l/4l},
			{44l/45l, -56l/15l, 32l/9l},
			{19372l/6561l, -25360l/2187l, 64448l/6561l, -212l/729l},
			{9017l/3168l, -355l/33l, -46732l/5247l, 49l/176l, -5103l/18656l},
			{35l/384l, 0, 500l/1113l, 125l/192l, -2187l/6784l, 11l/84l}
		};
		for (int i = 0; i < 7; i++) {
			for (int j = 0; j < 3; j++) {
				long double o = 0;
				for (int a = 0; a < i; a++) {
					o += c[i][a]*k[a][j];
				}
				k[i][j] = f[j](v[0]+o, v[1]+o, v[2]+o);
			}
		}
		// Order 4 & 5 step calculation constants
		const long double sc[2][7] = {
			{ 35l/384l, 0, 500l/1113l, 125l/192l, -2187l/6784l, 11l/84l },
			{ 5179l/57600l, 0, 7571l/16695l, 393l/640l, -92097l/339200l, 187l/2100l, 1l/40l }
		};
		long double sv[2][3]; // step values (this, and next for x,y,z)
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 3; j++) {
				long double o = 0;
				for (int a = 0; a < 7; a++) {
					o += sc[i][a]*k[a][j];
				}
				sv[i][j] = v[j] + o;
			}
		}
		// Calculate difference and error
		long double d[3];
		for (int i = 0; i < 3; i++) {
			d[i] = fabsl(sv[0][i] - sv[0][i]);
		}
		// Calculate optimum time difference
		long double s[3];
		for (int i = 0; i < 3; i++) {
			s[i] = powl(0.000001l*0.1l/(2*d[i]), 1l/5l);
		}
		dt = (s[0]+s[1]+s[2])/3*dt;
		n.x = sv[0][0];
		n.y = sv[0][1];
		n.z = sv[0][2];
	}
	return n;
}

uint64_t randomPCG() {
	return pcg64_boundedrand(0x100000000000000);
}

uint64_t randomC() {
	return ((double) rand())/RAND_MAX * 0x100000000000000;
}

// Global program configuration
struct {
	HyperSpace (*simulate)(HyperSpace *h);
	uint64_t (*random)();
	int threads, restarts;
} Config = {
	.simulate = hyperSpaceSimulateForwardEuler,
	.random = randomPCG,
	.threads = 1,
	.restarts = 1000 // S
};

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

// Generates all single-hamming-distance solutions
// and selects the best one.
// Slight optimization combines neighborhood generation
// and greedy descent selection for O(1) vs O(n) space.
bool hyperSpaceNext(Result *r) {
	Result t = *r;
	const int blen = 56;
	uint64_t bits = t.h.bits; // Original bits
	for (int i = 0; i < blen; i++) {
		Result lt = {
			.h = hyperSpaceFromBits(bits ^ (1 << i)),
			.cost = hyperSpaceCost(&lt.h)
		};
		if (lt.cost < t.cost) {
			t = lt;
		}
	}
	bool haveAdvanced = r->h.bits != t.h.bits;
	*r = (Result){
		.h = t.h,
		.cost = t.cost,
		// Accumulate evals
		.evals = r->evals + blen
	};
	return haveAdvanced;
}

// Search thread parameters
struct SearchThreadParams {
	int S;
	Result *r;
};

// search thread function
// for use with pthread_create
// void* cast to a SearchThreadParams*
void *searchThread(void *stp) {
	struct SearchThreadParams *p = (struct SearchThreadParams *) stp;
	for (int s = 0; s < p->S; s++) {
		Result l = {
			// PCG bounded rand is on the range [0, bound).
			.h = hyperSpaceFromBits(Config.random()),
			.cost = hyperSpaceCost(&l.h),
			.evals = 1
		};
		// Keeps a running total of evals
		while (hyperSpaceNext(&l)) {}
		p->r[s] = l;
	}
	return NULL;
}

// Returns an array of S result tuples
Result *search() {
	int S = Config.restarts;
	Result *r = calloc(sizeof(Result), S);
	pthread_t ts[Config.threads];
	struct SearchThreadParams params[Config.threads];
	bool spawnThreads = S/Config.threads > 0;
	if (spawnThreads) {
		for (int i = 0; i < Config.threads; i++) {
			params[i] = (struct SearchThreadParams){
				.S = S/Config.threads,
				.r = &r[i*(S/Config.threads)]
			};
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

	// Possible that S did not divide evenly
	// Run the remaining ones in this thread.
	if (S%Config.threads > 0) {
		searchThread(&(struct SearchThreadParams){
			.S = S%Config.threads,
			.r = &r[S-S%Config.threads]
		});
	}

	if (spawnThreads) {
		for (int i = 0; i < Config.threads; i++) {
			pthread_join(ts[i], NULL);
		}
	}
	return r;
}

void usage() {
	printf("main [-s|--simulate algorithm(dp|rk4|euler)] [-r|--random function(pcg)] [-n|--restarts nrestarts] [-t|--threads [nthreads]] [-h|--help]\n");
	exit(1);
}

int comp(const void *a0, const void *a1) {
	Result *a = (Result*) a0, *b = (Result*) a1;
	if (a->cost < b->cost) return 1;
	if (a->cost > b->cost) return -1;
	return 0;
}

int main(int argc, char *argv[]) {
	// Parse command line options
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--simulate") == 0) {
			if ((i + 1) < argc) {
				i++;
				if (strcmp(argv[i], "dp") == 0) {
					Config.simulate = hyperSpaceSimulateDP;
				} else if (strcmp(argv[i], "rk4") == 0) {
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
				if (strcmp(argv[i], "pcg") == 0) {
					Config.random = randomPCG;
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
				i++;
				int t = strtol(argv[i], NULL, 10);
				if (t > 0) {
					Config.threads = t;
				} else {
					printf("Threads must be on the range [1, %d)\n", INT_MAX);
					usage();
				}
			} else {
				// Default number of threads
				Config.threads = sysconf(_SC_NPROCESSORS_ONLN);
			}
		} else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--restarts") == 0) {
			if ((i + 1) < argc) {
				i++;
				int t = strtol(argv[i], NULL, 10);
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
			printf(
				"[-s|--simulate algorithm(dp|rk4|euler)]\n"
				"    Simulate system using forward euler, the more accurate RK4 algorithm,\n"
				"        or the even more accurate Dormand-Prince.\n"
				"[-r|--random function(pcg|crand)]\n"
				"    Select a source of randomness.\n"
				"    PCG random (default) is the best avaliable pseudo-random number generator,\n"
				"        while the stdlib C rand is very poor.\n"
				"[-n|--restarts nrestarts]\n"
				"    Number of random restarts, defaults to 1000 as per assignment.\n"
				"[-t|--threads [nthreads]]\n"
				"    Turn on threading, specifying no argument defaults to the machine's core number.\n"
				"[-h|--help]\n"
				"    Prints this help page.\n"
			);
			usage();
		} else {
			usage();
		}
	}

	printf("Running with %d %s.\n", Config.threads, (Config.threads > 1) ? "threads" : "thread");
	Result *t = search();
	qsort(t, Config.restarts, sizeof(Result), comp);
	for (int i = 0; i < Config.restarts; i++) {
		resultPprint(&t[i]);
	}
	free(t);
	// HyperSpace h = hyperSpaceFromBits(0x8FED66B63E53DE);
	// Config.simulate = hyperSpaceSimulateForwardEuler;
	// printf("cost: %Lf\n", hyperSpaceCost(&h));
	// Config.simulate = hyperSpaceSimulateRK4;
	// printf("cost: %Lf\n", hyperSpaceCost(&h));
	// Config.simulate = hyperSpaceSimulateDP;
	// printf("cost: %Lf\n", hyperSpaceCost(&h));
}
