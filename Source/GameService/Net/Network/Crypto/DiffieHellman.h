
#pragma once

#include <cstdint>
#include <vector>


class DiffieHellman
{
public:

	// The biggest n-bit prime
	static constexpr uint64_t P64 = 18446744073709551557;
	static constexpr uint32_t P32 = 4294967291;
	static constexpr uint16_t P16 = 65521;
	static constexpr uint8_t P8 = 251;

	static constexpr uint64_t G = 5;

public:

	uint64_t getPublicKey(uint64_t p, uint64_t g);
	std::vector<uint8_t> getSecretKey(uint64_t B) const;

private:

	uint64_t p_ = 0; // ¼Ò¼ö
	uint64_t g_ = 0; // 1 ~ (p - 1)

	uint64_t a_ = 0; // random

};
