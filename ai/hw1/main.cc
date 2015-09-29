
#include <random>
#include <functional>
#include <iostream>

namespace {
long double simX(long double x, long double y, long double z) {
	const long double o = 10;
	return o * (y-x);
}

long double simY(long double x, long double y, long double z) {
	const long double r = 28;
	return r*x - y - x*z;
}

long double simZ(long double x, long double y, long double z) {
	const long double b = 8.0/3.0;
	return x*y - b*z;
}
} // namespace

class Result;

class HyperSpace {
public:
	HyperSpace() {}
	HyperSpace(uint64_t bits);
	long double Cost();
	bool Next(Result *);
	static void Search(int S);
	HyperSpace Simulate();
private:
	long double x, y, z, T;
	uint64_t bits;
};

class Result {
public:
	Result() {}
	HyperSpace h;
	long double cost;
	uint64_t evals;
};

HyperSpace::HyperSpace(uint64_t b)
	: x((long double) ((b & 0xffff0000000000) >> 40) / 0xffff * 40 - 20),
	y((long double) ((b & 0xffff000000) >> 24) / 0xffff * 60 - 30),
	z((long double) ((b & 0xffff00) >> 8) / 0xffff * 50),
	T((long double) (b & 0xff) / 0xff * 5),
	bits(b) {}

long double HyperSpace::Cost() {
	const long double tx = 18, ty = 20, tz = 45;
	HyperSpace n = Simulate();
	return sqrtl(powl(x, 2) + powl(y, 2) + powl(z, 2)) + sqrtl(powl(n.x-tx, 2) + powl(n.y-ty, 2) + powl(n.z-tz, 2));
}

HyperSpace HyperSpace::Simulate() {
	const long double dt = 0.01;
	HyperSpace m = *this;
	for (long double t = 0; t < T; t += dt) {
		const HyperSpace n = m;
		m.x += dt * simX(n.x, n.y, n.z);
		m.y += dt * simY(n.x, n.y, n.z);
		m.z += dt * simZ(n.x, n.y, n.z);
	}
	return m;
}

bool HyperSpace::Next(Result *r) {
	Result t = *r;
	const int blen = 56;
	for (int i = 0; i < blen; i++) {
		HyperSpace h(bits ^ (1 << i));
		Result lt;
		lt.h = h;
		lt.cost = h.Cost();
		if (lt.cost < t.cost) {
			t = lt;
		}
	}
	bool haveAdvanced = r->h.bits != t.h.bits;
	t.evals = r->evals + blen;
	*r = t;
	return haveAdvanced;
}

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<uint64_t> dis(0, 0x100000000000000);

void HyperSpace::Search(int S) {
	for (int s = 0; s < S; s++) {
		HyperSpace h(dis(gen));
		// HyperSpace h(0x8FED66B63E53DE);
		Result l;
		l.h = h;
		l.cost = h.Cost();
		l.evals = 1;
		while (l.h.Next(&l)) {}
		std::cout << l.evals << ", " << l.cost << ", "
			<< l.h.x <<  ", " << l.h.y <<  ", " << l.h.z  << ", " << l.h.T << std::endl;
	}
}

int main() {
	HyperSpace::Search(1000);
}
