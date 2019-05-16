
#include "DiffieHellman.h"

#include <cassert>
#include <random>

namespace
{
	uint64_t gen_random()
	{
		std::mt19937_64 mt(std::random_device{}());
		std::uniform_int_distribution<uint64_t> dist;

		uint64_t a = dist(mt);
		uint64_t b = dist(mt);
		uint64_t c = dist(mt);
		uint64_t d = dist(mt);
		return a << 48 | b << 32 | c << 16 | d;
	}

	std::vector<uint8_t> to_bytes(uint64_t a)
	{
		std::vector<uint8_t> bytes;
		bytes.push_back(static_cast<std::uint8_t>(a >>  0));
		bytes.push_back(static_cast<std::uint8_t>(a >>  8));
		bytes.push_back(static_cast<std::uint8_t>(a >> 16));
		bytes.push_back(static_cast<std::uint8_t>(a >> 24));
		bytes.push_back(static_cast<std::uint8_t>(a >> 32));
		bytes.push_back(static_cast<std::uint8_t>(a >> 40));
		bytes.push_back(static_cast<std::uint8_t>(a >> 48));
		bytes.push_back(static_cast<std::uint8_t>(a >> 56));
		return bytes;
	}

	// https://gist.github.com/cloudwu/8838724
	// calc a * b % p, avoid 64bit overflow
	uint64_t multi_mod(uint64_t a, uint64_t b, uint64_t p)
	{
		uint64_t m = 0;
		while (b)
		{
			if (b & 1)
			{
				uint64_t t = p - a;
				if (m >= t)
					m -= t;
				else
					m += a;
			}

			if (a >= p - a)
				a = a * 2 - p;
			else
				a = a * 2;

			b >>= 1;
		}

		return m;
	}

	// calc a^b % p
	uint64_t pow_mod(uint64_t a, uint64_t b, uint64_t p)
	{
		if (b == 1)
		{
			return a;
		}

		uint64_t t = pow_mod(a, b >> 1, p);
		t = multi_mod(t, t, p);
		if (b % 2)
		{
			t = multi_mod(t, a, p);
		}

		return t;
	}

}

uint64_t DiffieHellman::getPublicKey(uint64_t p, uint64_t g)
{
	assert(0 < g && g < p);
	p_ = p;
	g_ = g;

	a_ = gen_random();
	return pow_mod(g_, a_, p_);
}

std::vector<uint8_t> DiffieHellman::getSecretKey(uint64_t B) const
{
	uint64_t s = pow_mod(B, a_, p_);
	return to_bytes(s);
}
