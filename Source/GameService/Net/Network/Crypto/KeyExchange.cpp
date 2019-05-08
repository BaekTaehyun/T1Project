
#include "KeyExchange.h"

#include <cassert>


std::vector<uint64_t> KeyExchange::getPublicKey(uint64_t p, uint64_t g, size_t count)
{
	dhs_.clear();

	std::vector<uint64_t> pk;

	for (size_t i = 0; i < count; ++i)
	{
		DiffieHellman dh;
		pk.push_back(dh.getPublicKey(p, g));
		dhs_.push_back(dh);
	}

	return pk;
}

std::vector<uint8_t> KeyExchange::getSecretKey(const std::vector<uint64_t>& Bs) const
{
	assert(dhs_.size() == Bs.size());

	std::vector<uint8_t> sk;
	for (size_t i = 0; i < dhs_.size(); ++i)
	{
		if (i >= Bs.size())
		{
			break;
		}

		auto s = dhs_[i].getSecretKey(Bs[i]);
		sk.insert(sk.end(), s.begin(), s.end());
	}

	return sk;
}
