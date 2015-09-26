
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include "rand.c"

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

HyperSpace hyperSpaceSimulate(HyperSpace *h) {
	const long double dt = 0.01, o = 10, r = 28, b = 8.0/3.0;
	HyperSpace n = *h;
	for (long double t = 0; t < h->T; t += dt) {
		HyperSpace m = n;
		// Forward Euler
		n.x += dt * (o * (m.y-m.x));
		n.y += dt * (r*m.x - m.y - m.x*m.z);
		n.z += dt * (m.x*m.y - b*m.z);
	}
	return n;
}

long double hyperSpaceCost(HyperSpace *h) {
	const long double tx = 18, ty = 20, tz = 45;
	HyperSpace n = hyperSpaceSimulate(h);
	return sqrtl(powl(h->x, 2) + powl(h->y, 2) + powl(h->z, 2)) + sqrtl(powl(n.x-tx, 2) + powl(n.y-ty, 2) + powl(n.z-tz, 2));
}

typedef struct {
	HyperSpace h;
	long double cost;
	uint64_t evals;
} Result;

void resultPprint(Result *r) {
	printf("%llu, %Lf, %Lf, %Lf, %Lf, %Lf\n", r->evals, r->cost, r->h.x, r->h.y, r->h.z, r->h.T);
}

// Generates all single-hamming-distance solutions
// and selects the best one.
// Slight optimization combines neighborhood generation
// and greedy descent selection for O(1) vs O(n) space.
Result hyperSpaceNext(HyperSpace *h) {
	const int blen = 56;
	Result t = {
		.h = *h,
		.cost = hyperSpaceCost(&t.h),
	};
	for (int i = 0; i < blen; i++) {
		Result lt = {
			.h = hyperSpaceFromBits(h->bits ^ (1 << i)),
			.cost = hyperSpaceCost(&lt.h)
		};
		if (lt.cost < t.cost) {
			t = lt;
		}
	}
	t.evals = blen;
	return t;
}

// Returns an array of S result tuples
Result *search(size_t S) {
	Result *r = calloc(sizeof(Result), S);
	for (int s = 0; s < S; s++) {
		r[s] = (Result){
			.h = hyperSpaceFromBits(randDouble() * 0xffffffffffffff),
			.cost = hyperSpaceCost(&r[s].h),
			.evals = 1
		};
		for (;;) {
			// Keep running total of evals
			Result n = hyperSpaceNext(&r[s].h);
			if (r[s].h.bits == n.h.bits) {
				break;
			}
			uint64_t e = r[s].evals;
			r[s] = n;
			r[s].evals += e;
		}
	}
	return r;
}

int main() {
	const int max = 1000;
	Result *t = search(max);
	for (int i = 0; i < max; i++) {
		resultPprint(&t[i]);
	}
}
