#pragma once

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

typedef int uuid;

struct MyStruct
{
	char name[128];
	char pass[128];
};
