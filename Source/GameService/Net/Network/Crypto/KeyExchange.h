
#pragma once

#include <vector>

#include "DiffieHellman.h"


class KeyExchange
{
	std::vector<DiffieHellman> dhs_;

public:

	std::vector<uint64_t> getPublicKey(uint64_t p, uint64_t g, size_t count);
	std::vector<uint8_t> getSecretKey(const std::vector<uint64_t>& Bs) const;

};