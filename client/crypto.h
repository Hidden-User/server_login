#pragma once
#include <Windows.h>
#include <wincrypt.h>

#define MD5LEN 16
#define BUFSIZE 128

const CHAR rgbDigits[] = "0123456789abcdef";
const char openKey[] = "AAAABBBBCCCCDDDD";

struct CryptoContainer
{
	HCRYPTPROV hProv;
	HCRYPTHASH hHash;
};

// init crypto 
int cryptoInitToMD5(CryptoContainer *cc);

int cryptoInitToAES(CryptoContainer *cc);

// encript string to MD5
int encryptPasswordMD5(char *rs, char *ins, CryptoContainer *cc);

int encryptSession(BYTE * rb, char * ins, int ins_size, CryptoContainer * cc);

int decryptSession(BYTE * rb, BYTE* in, int in_size, CryptoContainer* cc);

// release crypto
void cryptoRelease(CryptoContainer *cc);
