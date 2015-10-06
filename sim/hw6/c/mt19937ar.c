/*
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#include <stdio.h>

/* Period parameters */
enum {
    MT19937_N = 624,
    MT19937_M = 397,
    MT19937_MATRIX_A = 0x9908b0dfUL, /* constant vector a */
    MT19937_UPPER_MASK = 0x80000000UL, /* most significant w-r bits */
    MT19937_LOWER_MASK = 0x7fffffffUL /* least significant r bits */
};

typedef struct {
    unsigned long a[MT19937_N]; /* the array for the state vector  */
    int i; /* mti==N+1 means mt[N] is not initialized */
} MT19937State;

/* initializes mt[MT19937_N] with a seed */
void MT19937InitGenRand(MT19937State *mts, unsigned long s) {
    mts->a[0]= s & 0xffffffffUL;
    for (mts->i=1; mts->i<MT19937_N; mts->i++) {
        mts->a[mts->i] =
	    (1812433253UL * (mts->a[mts->i-1] ^ (mts->a[mts->i-1] >> 30)) + mts->i);
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mts->a[mts->i] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void MT19937InitByArray(MT19937State *mts, unsigned long initKey[], int keyLen) {
    int i, j, k;
    MT19937InitGenRand(mts, 19650218UL);
    i=1; j=0;
    k = (MT19937_N>keyLen ? MT19937_N : keyLen);
    for (; k; k--) {
        mts->a[i] = (mts->a[i] ^ ((mts->a[i-1] ^ (mts->a[i-1] >> 30)) * 1664525UL))
          + initKey[j] + j; /* non linear */
        mts->a[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=MT19937_N) { mts->a[0] = mts->a[MT19937_N-1]; i=1; }
        if (j>=keyLen) j=0;
    }
    for (k=MT19937_N-1; k; k--) {
        mts->a[i] = (mts->a[i] ^ ((mts->a[i-1] ^ (mts->a[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mts->a[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=MT19937_N) { mts->a[0] = mts->a[MT19937_N-1]; i=1; }
    }

    mts->a[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long MT19937GenRandInt32(MT19937State *mts) {
    unsigned long y;
    static const unsigned long mag01[2]={0x0UL, MT19937_MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mts->i >= MT19937_N) { /* generate MT19937_N words at one time */
        int kk;

        if (mts->i == MT19937_N+1)   /* if init_genrand() has not been called, */
            MT19937InitGenRand(mts, 5489UL); /* a default initial seed is used */

        for (kk=0;kk<MT19937_N-MT19937_M;kk++) {
            y = (mts->a[kk]&MT19937_UPPER_MASK)|(mts->a[kk+1]&MT19937_LOWER_MASK);
            mts->a[kk] = mts->a[kk+MT19937_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<MT19937_N-1;kk++) {
            y = (mts->a[kk]&MT19937_UPPER_MASK)|(mts->a[kk+1]&MT19937_LOWER_MASK);
            mts->a[kk] = mts->a[kk+(MT19937_M-MT19937_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mts->a[MT19937_N-1]&MT19937_UPPER_MASK)|(mts->a[0]&MT19937_LOWER_MASK);
        mts->a[MT19937_N-1] = mts->a[MT19937_M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mts->i = 0;
    }

    y = mts->a[mts->i++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
long MT19937GenRandInt31(MT19937State *mts) {
    return (long)(MT19937GenRandInt32(mts)>>1);
}

/* generates a random number on [0,1]-real-interval */
double MT19937GenRandReal1(MT19937State *mts) {
    return MT19937GenRandInt32(mts)*(1.0/4294967295.0);
    /* divided by 2^32-1 */
}

/* generates a random number on [0,1)-real-interval */
double MT19937GenRandReal2(MT19937State *mts) {
    return MT19937GenRandInt32(mts)*(1.0/4294967296.0);
    /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double MT19937GenRandReal3(MT19937State *mts) {
    return (((double)MT19937GenRandInt32(mts))+0.5)*(1.0/4294967296.0);
    /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double MT19937GenRandRes53(MT19937State *mts) {
    unsigned long a = MT19937GenRandInt32(mts)>>5,
        b = MT19937GenRandInt32(mts)>>6;
    return(a*67108864.0+b)*(1.0/9007199254740992.0);
}
/* These real versions are due to Isaku Wada, 2002/01/09 added */
