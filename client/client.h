#pragma once
#include "stdio.h"
#include "string.h"
#include "../server_login/connection.h"
#include "crypto.h"


public value class session {
public:
	//MyStruct mst;
	HANDLE mm;
	SOCKET s_send;
	SOCKET s_recv;
	uuid id;
	uuid room;
	CryptoContainer2 cc;
};

int createClient(int code, char* userName, HINSTANCE hInstance);

void createWin();

int connect(int code);

int _send(int code, char *msg);

LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
