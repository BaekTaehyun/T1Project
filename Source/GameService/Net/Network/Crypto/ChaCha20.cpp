
#include "ChaCha20.h"

#include <random>


namespace
{
	void	GetRandomBytes(uint8_t* buff, size_t size)
	{
		std::random_device rd;
		std::uniform_int_distribution<long> dist;

		for (size_t i = 0; i < size; ++i)
		{
			buff[i] = static_cast<uint8_t>(dist(rd));
		}
	}
}


CHACAH20_KEY	GenerateChaCha20Key()
{
	CHACAH20_KEY key;
	GetRandomBytes(key.key, sizeof(key.key));
	GetRandomBytes(key.counter, sizeof(key.counter));
	return key;
}


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


