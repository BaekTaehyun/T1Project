
#pragma once

#include <cstddef>
#include <cstdint>

#include "OpenSSLHeader.h"


struct CHACAH20_KEY
{
	uint8_t	key[32];
	uint8_t	counter[16];
};

CHACAH20_KEY	GenerateChaCha20Key();


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
