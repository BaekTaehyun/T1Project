
#pragma once

#include <cstddef>
#include <cstdint>

#include "OpenSSLHeader.h"


struct CHACAH20_KEY
{
	uint8_t	key[32];
	uint8_t	counter[16];
};


#if OPENSSL_VERSION_NUMBER >= NEED_OPENSSL_VERSION

class ChaCha20Encrypter
{
public:

	ChaCha20Encrypter();
	~ChaCha20Encrypter();

	bool	setKey(const CHACAH20_KEY& key);

	size_t	encrypt(void* data, size_t size);
	size_t	encrypt(const void* src, size_t srcSize, void* dest, size_t destSize);

private:

	EVP_CIPHER_CTX*	ctx_ = nullptr;

};


class ChaCha20Decrypter
{
public:

	ChaCha20Decrypter();
	~ChaCha20Decrypter();

	bool	setKey(const CHACAH20_KEY& key);

	size_t	decrypt(void* data, size_t size);
	size_t	decrypt(const void* src, size_t srcSize, void* dest, size_t destSize);

private:

	EVP_CIPHER_CTX*	ctx_ = nullptr;

};

#else


class ChaCha20
{
public:

	ChaCha20();

	void	setKey(const CHACAH20_KEY& key);

	bool	run(void* data, size_t size);
	bool	run(const void* src, size_t srcSize, void* dest, size_t destSize);

private:

	void	next(int rounds);	// rounds = 8, 12, 20

private:

	uint32_t	state_[16];
	uint8_t		buff_[64];
	size_t		pos_;

};



class ChaCha20Encrypter
{
public:

	bool	setKey(const CHACAH20_KEY& key)
	{
		chacha20_.setKey(key);
		return true;
	}

	size_t	encrypt(void* data, size_t size)
	{
		if (chacha20_.run(data, size) == false)
		{
			return 0;
		}

		return size;
	}
	size_t	encrypt(const void* src, size_t srcSize, void* dest, size_t destSize)
	{
		if (chacha20_.run(src, srcSize, dest, destSize) == false)
		{
			return 0;
		}

		return srcSize;
	}

private:

	ChaCha20 chacha20_;

};


class ChaCha20Decrypter
{
public:

	bool	setKey(const CHACAH20_KEY& key)
	{
		chacha20_.setKey(key);
		return true;
	}

	size_t	decrypt(void* data, size_t size)
	{
		return chacha20_.run(data, size);
	}
	size_t	decrypt(const void* src, size_t srcSize, void* dest, size_t destSize)
	{
		return chacha20_.run(src, srcSize, dest, destSize);
	}

private:

	ChaCha20 chacha20_;

};


#endif