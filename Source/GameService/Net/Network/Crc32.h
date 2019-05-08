#pragma once

#include <cstdint>


class Crc32
{
public:

	static uint32_t Compute(const void* data, size_t len);

	void update(const void* data, size_t len);

	uint32_t get() const;

private:

	uint32_t crc_ = 0;

};
