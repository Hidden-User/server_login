#include "crypto.h"

typedef struct {
	BLOBHEADER header;
	DWORD len;
	BYTE key[16];
}AesBlob128;

int cryptoInitToMD5(CryptoContainer * cc)
{
	if (!CryptAcquireContext(&(cc->hProv), NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		return GetLastError();
	}

	return 0;
}

int cryptoInitToAES(CryptoContainer * cc)
{
	if (!CryptAcquireContext(&(cc->hProv), NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
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


int encryptSession(BYTE * rb, char * ins, int ins_size, CryptoContainer * cc)
{
	AesBlob128 aes;
	int kBytes = 16;
	HCRYPTKEY hKey = NULL;
	int blocks = ins_size / kBytes;
	DWORD len;
	bool last;

	if (ins_size%kBytes) {
		blocks++;
	}

	aes.header.aiKeyAlg = CALG_AES_128;
	aes.header.bType = PLAINTEXTKEYBLOB;
	aes.header.bVersion = CUR_BLOB_VERSION;
	aes.header.reserved = 0;
	aes.len = kBytes;
	memcpy(aes.key, openKey, 16);

	if (!CryptImportKey(cc->hProv, (BYTE*)&aes, sizeof(AesBlob128), NULL, 0, &hKey)) {
		return GetLastError();
	}

	memcpy(rb, ins, ins_size);

	for (int t = 0; t < blocks; t++) {
		last = t + 1 == blocks;

		len = 16;

		if (0 == CryptEncrypt(hKey, NULL, last, 0, rb + (kBytes * t), &len, 32)) {
			len = GetLastError();
			return len;
		}

	}

	CryptDestroyKey(hKey);
	return 0;
}

int decryptSession(BYTE * rb, BYTE* in, int in_size, CryptoContainer* cc)
{
	AesBlob128 aes;
	int kBytes = 16;
	HCRYPTKEY hKey = NULL;
	int blocks = in_size / kBytes;
	DWORD len;
	bool last;

	if (in_size%kBytes) {
		blocks++;
	}

	aes.header.aiKeyAlg = CALG_AES_128;
	aes.header.bType = PLAINTEXTKEYBLOB;
	aes.header.bVersion = CUR_BLOB_VERSION;
	aes.header.reserved = 0;
	aes.len = kBytes;
	memcpy(aes.key, openKey, 16);

	if (!CryptImportKey(cc->hProv, (BYTE*)&aes, sizeof(AesBlob128), NULL, 0, &hKey)) {
		return GetLastError();
	}

	memcpy(rb, in, in_size);

	for (int t = 0; t < blocks; t++) {
		last = t + 1 == blocks;

		len = 16;

		if (!CryptDecrypt(hKey, NULL, last, 0, rb + (kBytes * t), &len)) {
			return GetLastError();
		}

	}

	CryptDestroyKey(hKey);
}

void cryptoRelease(CryptoContainer * cc)
{
	CryptReleaseContext(cc->hProv, 0);
}
