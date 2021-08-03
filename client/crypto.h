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

typedef struct {
	BLOBHEADER header;
	DWORD len;
	BYTE key[16];
}AesBlob128;

public value class CryptoContainer2 {
private:
	HCRYPTPROV hProv;
	int type;

	int encryptAES(BYTE * rb, char * ins, int ins_size) {
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

		if (!CryptImportKey(this->hProv, (BYTE*)&aes, sizeof(AesBlob128), NULL, 0, &hKey)) {
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

public:

	void setHPROV(HCRYPTPROV hpr) {
		this->hProv = hpr;
	}

	int initAES() {
		HCRYPTPROV hpr;

		if (this->type != NULL) {
			return 0;
		}

		if (!::CryptAcquireContext(&hpr, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
			return ::GetLastError();
		}

		this->hProv = hpr;
		this->type = 1;

		return 0;
	}

	int initMD5() {

		return -1;
	}

	void Release() {
		CryptReleaseContext(this->hProv, 0);
	}

	int encrypt(BYTE * rb, char * ins, int ins_size) {
		if (this->type == NULL) {
			return -1;
		}
		if (this->type > 0) {
			return encryptAES(rb, ins, ins_size);
		}
		else {
			return -1;
		}
	}

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
