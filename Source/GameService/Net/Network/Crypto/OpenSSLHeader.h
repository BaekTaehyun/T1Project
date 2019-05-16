
#pragma once


#define USE_UNREAL4

#ifdef USE_UNREAL4
#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#endif


#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/pem.h>

// openssl v1.1.0 이상 (chacha20, x25519 사용)
#define NEED_OPENSSL_VERSION 0x10010000L
static_assert(OPENSSL_VERSION_NUMBER >= NEED_OPENSSL_VERSION, "OpenSSL version error");


#ifdef USE_UNREAL4
THIRD_PARTY_INCLUDES_END
#undef UI
#endif

