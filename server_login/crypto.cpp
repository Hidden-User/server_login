#include "crypto.h"

int cryptoInit(CryptoContainer * cc)
{
	if (!CryptAcquireContext(&(cc->hProv), NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		return GetLastError();
	}

	return 0;
}

int encryptPasswordMD5(char * rs, char * ins, CryptoContainer * cc)
{
	BYTE rgbHash[MD5LEN];
	DWORD cbHash = 0;

	if (!CryptCreateHash(cc->hProv, CALG_MD5, 0, 0, &(cc->hHash))) {
		return GetLastError();
	}

	if (!CryptHashData(cc->hHash, (BYTE*)ins, strlen(ins), 0)) {
		return GetLastError();
	}

	cbHash = MD5LEN;

	if (!CryptGetHashParam(cc->hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
		return GetLastError();
	}

	for (int t = 0; t < cbHash; t++) {
		rs[t * 2] = rgbDigits[rgbHash[t] >> 4];
		rs[t * 2 + 1] = rgbDigits[rgbHash[t] & 0xf];
	}
	rs[cbHash * 2] = '\0';
	
	CryptDestroyHash(cc->hHash);

	return 0;
}

void cryptoRelease(CryptoContainer * cc)
{
	CryptReleaseContext(cc->hProv, 0);
}
