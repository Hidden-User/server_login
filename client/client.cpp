#include "client.h"
#include <WinSock2.h>
#include <ws2tcpip.h>


session *sess;

int connect(int code, HINSTANCE hInstance)
{
	HANDLE window;
	WSADATA wsad;
	session s;

	if (cryptoInitToAES2(s.cc)) {
		return -1;
	}

	if (FAILED(WSAStartup(0x0202, &wsad))) {
		return -1;
	}



	return 0;
}

int createClient(int code, char * userName, HINSTANCE hInstance)
{
	static session ses;




	return 0;
}

int _send(int code, char *msg)
{
	

	return 0;
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT res = FALSE;




}
