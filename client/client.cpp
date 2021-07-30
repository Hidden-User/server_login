#include "client.h"
#include <WinSock2.h>
#include <ws2tcpip.h>


int connect(int code)
{
	HANDLE window;
	CryptoContainer cc;
	WSADATA wsad;

	if (cryptoInitToAES(&cc)) {
		return -1;
	}

	if (FAILED(WSAStartup(0x0202, &wsad))) {
		return -1;
	}



	return 0;
}

int _send(int code, char ^ msg)
{
	HANDLE mm = NULL;
	if (mm == NULL) {
		mm = CreateMutex(NULL, FALSE, NULL);
	}



	return 0;
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT res = FALSE;




}
