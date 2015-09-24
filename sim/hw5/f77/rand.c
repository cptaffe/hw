#include <stdlib.h>
#include <stdint.h>

typedef struct {
	uint64_t w, // word size in bits
		n,        // degree of recurrence
		m,        // middle word
		r,        // separation point of one word
		a,        // coefficients of the rational normal form twist matrix
		b, c,     // TGFSR(R) tempering bitmasks
		s, t,     // TGFSR(R) tempering bit shitfs
		u, d, l;  // Additional Mersenne Twister tempering bit shifts/masks
	uint64_t f;
} Random;

typedef struct {
	Random *r;
	uint64_t *mt;
	int i;
	uint64_t lmask, umask;
} RandomGeneratorState;

static uint64_t lowestWBits(Random *r) {
	uint64_t m = 0;
	for (int i = 0; i < r->w; i++) {
		m |= 1 << i;
	}
	return m;
}

RandomGeneratorState *makeRandomGeneratorState(Random *r) {
	RandomGeneratorState *s = calloc(sizeof(RandomGeneratorState), 1);
	*s = (RandomGeneratorState) {
		.r = r,
		.mt = calloc(sizeof(uint64_t), r->n),
		.i = r->n+1,
		.lmask = (1 << r->r) - 1,
		.umask = ~((1 << r->r) - 1) & lowestWBits(r),
	};
	return s;
}

void randomGeneratorStateSeed(RandomGeneratorState *s, uint64_t seed) {
	s->i = s->r->n;
	s->mt[0] = seed;
	for (int i = 1; i < s->i; i++) {
		s->mt[i] = (s->r->f * (s->mt[i-1] ^ (s->mt[i-1] >> (s->r->w-2))) + i) & lowestWBits(s->r);
	}
}

static void twist(RandomGeneratorState *s) {
	for (int i = 0; i < s->r->n; i++) {
		uint64_t x = (s->mt[i] & s->umask) + (s->mt[(i+1) % s->r->n] & s->lmask);
		uint64_t xA = x >> 1;
		if (x % 2 != 0) {
			xA = xA ^ s->r->a;
		}
		s->mt[i] = s->mt[(i + s->r->m) % s->r->n] ^ xA;
	}
	s->i = 0;
}

static uint64_t extract(RandomGeneratorState *s) {
	if (s->i >= s->r->n) {
		if (s->i > s->r->n) {
			// Has not been seeded
			randomGeneratorStateSeed(s, 300); // seed at 300, default
		}
		twist(s);
	}
	uint64_t y = s->mt[s->i];
	y ^= (y >> s->r->u) & s->r->d;
	y ^= (y << s->r->s) & s->r->b;
	y ^= (y << s->r->t) & s->r->c;
	y ^= y >> s->r->l;

	s->i++;
	return y & lowestWBits(s->r);
}

Random *makeMT199364Random() {
	Random *r = calloc(sizeof(Random), 1);
	*r = (Random){
		.w = 64, .n = 312, .m = 156, .r = 31,
		.a = 0xB5026F5AA96619E9,
		.u = 29, .d = 0x5555555555555555,
		.s = 17, .b = 0x71D67FFFEDA60000,
		.t = 37, .c = 0xFFF7EEE000000000,
		.l = 43,
		.f = 6364136223846793005
	};
	return r;
}

RandomGeneratorState *randomState;

double globalRandomGenerate() {
	if (randomState == NULL) {
		randomState = makeRandomGeneratorState(makeMT199364Random());
	}
	// lowest w bits will also be the largest generatable number.
	return ((double) extract(randomState))/lowestWBits(randomState->r);
}

double rand_() {
	return globalRandomGenerate();
}
