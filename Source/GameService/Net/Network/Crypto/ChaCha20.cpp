
#include "ChaCha20.h"

#if OPENSSL_VERSION_NUMBER >= NEED_OPENSSL_VERSION

ChaCha20Encrypter::ChaCha20Encrypter()
	: ctx_(nullptr)
{
}

ChaCha20Encrypter::~ChaCha20Encrypter()
{
	if (ctx_ != nullptr)
	{
		EVP_CIPHER_CTX_free(ctx_);
		ctx_ = nullptr;
	}
}

bool	ChaCha20Encrypter::setKey(const CHACAH20_KEY& key)
{
	if (ctx_ != nullptr)
	{
		EVP_CIPHER_CTX_free(ctx_);
		ctx_ = nullptr;
	}

	ctx_ = EVP_CIPHER_CTX_new();
	if (ctx_ == nullptr)
	{
		return false;
	}

	if (EVP_EncryptInit_ex(ctx_, EVP_chacha20(), nullptr, key.key, key.counter) != 1)
	{
		return false;
	}

	return true;
}

size_t	ChaCha20Encrypter::encrypt(void* data, size_t size)
{
	return encrypt(data, size, data, size);
}

size_t	ChaCha20Encrypter::encrypt(const void* src, size_t srcSize, void* dest, size_t destSize)
{
	if (src == nullptr || srcSize <= 0 ||
		dest == nullptr || destSize <= 0)
	{
		return 0;
	}

	size_t needSize = srcSize;
	if (needSize > destSize)
	{
		return 0;
	}

	const unsigned char* in = static_cast<const unsigned char*>(src);
	unsigned char* out = static_cast<unsigned char*>(dest);

	int outSize = 0;
	if (EVP_EncryptUpdate(ctx_, out, &outSize, in, static_cast<int>(srcSize)) != 1)
	{
		return 0;
	}

	if (static_cast<int>(srcSize) != outSize)
	{
		return 0;
	}

	return outSize;
}


ChaCha20Decrypter::ChaCha20Decrypter()
	: ctx_(nullptr)
{
}

ChaCha20Decrypter::~ChaCha20Decrypter()
{
	if (ctx_ != nullptr)
	{
		EVP_CIPHER_CTX_free(ctx_);
		ctx_ = nullptr;
	}
}

bool	ChaCha20Decrypter::setKey(const CHACAH20_KEY& key)
{
	if (ctx_ != nullptr)
	{
		EVP_CIPHER_CTX_free(ctx_);
		ctx_ = nullptr;
	}

	ctx_ = EVP_CIPHER_CTX_new();
	if (ctx_ == nullptr)
	{
		return false;
	}

	if (EVP_DecryptInit_ex(ctx_, EVP_chacha20(), nullptr, key.key, key.counter) != 1)
	{
		return false;
	}

	return true;
}

size_t	ChaCha20Decrypter::decrypt(void* data, size_t size)
{
	return decrypt(data, size, data, size);
}

size_t	ChaCha20Decrypter::decrypt(const void* src, size_t srcSize, void* dest, size_t destSize)
{
	if (src == nullptr || srcSize <= 0 ||
		dest == nullptr || destSize < srcSize)
	{
		return 0;
	}

	const unsigned char* in = static_cast<const unsigned char*>(src);
	unsigned char* out = static_cast<unsigned char*>(dest);

	int outSize = 0;
	if (EVP_DecryptUpdate(ctx_, out, &outSize, in, static_cast<int>(srcSize)) != 1)
	{
		return 0;
	}

	if (static_cast<int>(srcSize) != outSize)
	{
		return 0;
	}

	return outSize;
}

#else


#include <cstring> 


namespace
{
	inline uint32_t U8TO32_LITTLE(const uint8_t* a)
	{
		return (
			(uint32_t(a[0]) << 0) |
			(uint32_t(a[1]) << 8) |
			(uint32_t(a[2]) << 16) |
			(uint32_t(a[3]) << 24)
			);
	}

	inline void U32TO8_LITTLE(uint8_t* a, uint32_t v)
	{
		a[0] = static_cast<uint8_t>(v >> 0);
		a[1] = static_cast<uint8_t>(v >> 8);
		a[2] = static_cast<uint8_t>(v >> 16);
		a[3] = static_cast<uint8_t>(v >> 24);
	}

	inline uint32_t ROTATE(uint32_t x, int n)
	{
		return (x << n) | (x >> (32 - n));
	}

	inline void QUARTERROUND(uint32_t* x, int a, int b, int c, int d)
	{
		x[a] += x[b]; x[d] = ROTATE(x[d] ^ x[a], 16);
		x[c] += x[d]; x[b] = ROTATE(x[b] ^ x[c], 12);
		x[a] += x[b]; x[d] = ROTATE(x[d] ^ x[a], 8);
		x[c] += x[d]; x[b] = ROTATE(x[b] ^ x[c], 7);
	}
}


ChaCha20::ChaCha20()
	: pos_(64)
{
}

void	ChaCha20::setKey(const CHACAH20_KEY& key)
{
	const uint8_t* k = key.key;
	const uint8_t* c = key.counter;

	// k
	static const uint8_t sigma[] = "expand 32-byte k";
	state_[0] = U8TO32_LITTLE(sigma + 0);
	state_[1] = U8TO32_LITTLE(sigma + 4);
	state_[2] = U8TO32_LITTLE(sigma + 8);
	state_[3] = U8TO32_LITTLE(sigma + 12);

	state_[4] = U8TO32_LITTLE(k + 0);
	state_[5] = U8TO32_LITTLE(k + 4);
	state_[6] = U8TO32_LITTLE(k + 8);
	state_[7] = U8TO32_LITTLE(k + 12);
	state_[8] = U8TO32_LITTLE(k + 16);
	state_[9] = U8TO32_LITTLE(k + 20);
	state_[10] = U8TO32_LITTLE(k + 24);
	state_[11] = U8TO32_LITTLE(k + 28);

	// counter
	state_[12] = U8TO32_LITTLE(c + 0);
	state_[13] = U8TO32_LITTLE(c + 4);
	state_[14] = U8TO32_LITTLE(c + 8);
	state_[15] = U8TO32_LITTLE(c + 12);

	pos_ = 64;
}

void	ChaCha20::next(int rounds)
{
	uint32_t x[16];
	std::memcpy(x, state_, sizeof(x));

	for (int i = rounds; i > 0; i -= 2)
	{
		QUARTERROUND(x, 0, 4, 8, 12);
		QUARTERROUND(x, 1, 5, 9, 13);
		QUARTERROUND(x, 2, 6, 10, 14);
		QUARTERROUND(x, 3, 7, 11, 15);
		QUARTERROUND(x, 0, 5, 10, 15);
		QUARTERROUND(x, 1, 6, 11, 12);
		QUARTERROUND(x, 2, 7, 8, 13);
		QUARTERROUND(x, 3, 4, 9, 14);
	}

	for (int i = 0; i < 16; ++i)
	{
		U32TO8_LITTLE(buff_ + (i * 4), (x[i] + state_[i]));
	}

	++state_[12];
	if (state_[12] == 0)
	{
		++state_[13];
	}

	pos_ = 64;
}

bool	ChaCha20::run(void* data, size_t size)
{
	return run(data, size, data, size);
}

bool	ChaCha20::run(const void* src, size_t srcSize, void* dest, size_t destSize)
{
	if (src == nullptr || srcSize <= 0 ||
		dest == nullptr || destSize < srcSize)
	{
		return false;
	}

	const uint8_t* src_bytes = static_cast<const uint8_t*>(src);
	uint8_t* dest_bytes = static_cast<uint8_t*>(dest);
	for (size_t i = 0; i < srcSize; ++i)
	{
		if (pos_ >= 64)
		{
			next(20);
			pos_ = 0;
		}

		dest_bytes[i] = src_bytes[i] ^ buff_[pos_];
		++pos_;
	}

	return true;
}

#endif

