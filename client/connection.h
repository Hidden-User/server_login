#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>

#define SERVER_LOGIN 1
#define SERVER_LOGOUT 2
#define SERVER_REGISTER 3
#define SERVER_MESSAGE 4
#define SERVER_CHANGE_ROOM 5

#define SERVER_ADD_CONNECTION 0xf0
#define SERVER_SET_CONNECTION 0x0f

#define USER_OK 0
#define USER_ACCESS_DENY 1
#define USER_LOGIN_SUCCES 2
#define USER_LOGIN_DENY 3
#define USER_REGISTER_SUCCESS 4
#define USER_REGISTER_DENY 5

#define SKIPDWP res = DefWindowProc(hWnd, message, wParam, lParam);

typedef int uuid;

const char server_addr[] = "127.0.0.1";
const char server_port[] = "49999";

struct MyStruct
{
	char name[128];
	char pass[128];
	char buff[32];
};
