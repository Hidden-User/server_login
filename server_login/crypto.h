#pragma once
#include <Windows.h>
#include <wincrypt.h>

#define MD5LEN 16
#define BUFSIZE 128

const CHAR rgbDigits[] = "0123456789abcdef";

struct CryptoContainer
{
	HCRYPTPROV hProv;
	HCRYPTHASH hHash;
};

// init crypto 
int cryptoInit(CryptoContainer *cc);

// encript string to MD5
int encryptPasswordMD5(char *rs, char *ins, CryptoContainer *cc);

// release crypto
void cryptoRelease(CryptoContainer *cc);
