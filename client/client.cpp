#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "stdio.h"
#include "crypto.h"
#include "../server_login/connection.h"

#pragma comment (lib, "ws2_32.lib")

const char server_addr[] = "127.0.0.1";
const char server_port[] = "49999";
char buffer[1024];

HANDLE g_m;
HANDLE v_users[10000];
DWORD v_thr_id[10000];

WSADATA wsad;
SOCKET soc;
int my_id = 0;
sockaddr_in soc_addr;

CryptoContainer cc;

DWORD WINAPI listen(LPVOID);
DWORD WINAPI virtualUser(LPVOID);
unsigned v_id;

int main()
{
	char cb[1024];
	MyStruct mst;
	HANDLE listener = NULL;
	DWORD thr_id;
	int ib;
	addrinfo *ai, _ai;
	bool _work = false;

	if (cryptoInit(&cc)) {
		printf("Crypt init error\n");
		Sleep(10000);
		return -1;
	}

	if (FAILED(WSAStartup(0x0202, &wsad))) {
		printf("WSAStartup error\n");
		Sleep(10000);
		return -1;
	}

	g_m = CreateMutex(NULL, FALSE, NULL);

	if (g_m == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		Sleep(10000);
		exit(-1);
	}

	for (int t = 0; t < 200; t++) {
		v_users[t] = CreateThread(NULL, 0, virtualUser, NULL, 0, &(v_thr_id[t]));
		
		Sleep(20);
	
		if (v_users[t] == 0) {
			printf("Create thread error %d", GetLastError());
			Sleep(10000);
		}
	}
	if (WaitForSingleObject(g_m, INFINITE) != WAIT_OBJECT_0) {
		Sleep(10000);
		exit(-1);
	}
	ZeroMemory(&_ai, sizeof(_ai));
	_ai.ai_family = AF_INET;
	_ai.ai_socktype = SOCK_STREAM;
	_ai.ai_protocol = IPPROTO_TCP;

	ib = getaddrinfo(server_addr, server_port, &_ai, &ai);

	if (ib != 0) {
		printf("getaddrinfo rc: %d", ib);
		WSACleanup();
		return 0;
	}

	soc = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

	if (soc == INVALID_SOCKET) {
		ib = WSAGetLastError();
		printf("socket rc: %d", ib);
		Sleep(100000);
		return -1;
	}

	if (SOCKET_ERROR == (connect(soc, ai->ai_addr, (int)ai->ai_addrlen))) {
		printf("Internal error");
		ib = WSAGetLastError();
		printf("connect rc: %d", ib);
		Sleep(100000);
		return -1;
	}

	ib = SERVER_SET_CONNECTION;
	send(soc, (char*)&ib, sizeof(int), 0);

	recv(soc, (char*)&ib, sizeof(int), 0);

	if (ib != USER_OK) {

	}

	_work = true;
	if (!ReleaseMutex(g_m)) {
		Sleep(10000);
		exit(-1);
	}
	while (_work)
	{
		//scanf_s("%s", cb, sizeof(cb));
		gets_s(cb, sizeof(cb));
		if (strcmp(cb, "/exit") == 0) {
			closesocket(soc);
			WSACleanup();
			return 0;
		}
		if (strcmp(cb, "/login") == 0) {
			if (my_id != 0) {
				printf("you logged!\n");
			}
			scanf_s("%s", cb, sizeof(cb));
			sprintf(mst.name, "%s", cb);
			scanf_s("%s", cb, sizeof(cb));
			getchar();
			//sprintf(mst.pass, "%s", cb);
			encryptPasswordMD5(mst.pass, cb, &cc);

			ib = SERVER_LOGIN;

			send(soc, (char*)&ib, sizeof(int), 0);

			send(soc, (char*)&mst, sizeof(MyStruct), 0);

			recv(soc, (char*)&ib, sizeof(int), 0);

			if (ib == USER_LOGIN_SUCCES) {
				recv(soc, (char*)&my_id, sizeof(int), 0);
				listener = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)listen, NULL, 0, &thr_id);
				printf("login success\n");
			}
			else {
				printf("login deny\n");
			}


			continue;
		}
		if (strcmp(cb, "/logout") == 0) {
			if (my_id == 0) {
				continue;
			}

			ib = SERVER_LOGOUT;

			send(soc, (char*)&ib, sizeof(int), 0);

			recv(soc, (char*)&ib, sizeof(int), 0);

			if (ib != USER_OK) {
				printf("logout error\n");
			}

			my_id = 0;

			CloseHandle(listener);

			continue;
		}
		if (strcmp(cb, "/reg") == 0) {
			if (my_id != 0) {
				printf("you logged!\n");
			}
			scanf_s("%s", cb, sizeof(cb));
			sprintf(mst.name, "%s", cb);
			scanf_s("%s", cb, sizeof(cb));
			getchar();
			//sprintf(mst.pass, "%s", cb);
			encryptPasswordMD5(mst.pass, cb, &cc);

			ib = SERVER_REGISTER;

			send(soc, (char*)&ib, sizeof(int), 0);

			send(soc, (char*)&mst, sizeof(MyStruct), 0);

			recv(soc, (char*)&ib, sizeof(int), 0);

			if (ib == USER_REGISTER_SUCCESS) {
				recv(soc, (char*)&my_id, sizeof(int), 0);
				listener = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)listen, NULL, 0, &thr_id);
				printf("register success\n");
			}
			else {
				printf("register deny\n");
			}


			continue;
		}

		if (my_id == 0) {
			printf("You not logged!\n");
			continue;
		}

		ib = SERVER_MESSAGE;

		send(soc, (char*)&ib, sizeof(int), 0);

		ib = strlen(cb) + 1;

		send(soc, (char*)&ib, sizeof(int), 0);

		send(soc, cb, ib, 0);

		recv(soc, (char*)&ib, sizeof(int), 0);

		if (ib != USER_OK) {

		}

	}

	if (listener != NULL) {
		CloseHandle(listener);
	}

	cryptoRelease(&cc);
}

DWORD WINAPI listen(LPVOID) {
	SOCKET _soc;
	addrinfo *ai, _ai;
	char lcb[256];
	int ib;
	int _ib;

	ZeroMemory(&_ai, sizeof(_ai));
	_ai.ai_family = AF_INET;
	_ai.ai_socktype = SOCK_STREAM;
	_ai.ai_protocol = IPPROTO_TCP;

	ib = getaddrinfo(server_addr, server_port, &_ai, &ai);

	if (ib != 0) {
		printf("getaddrinfo rc: %d", ib);
		WSACleanup();
		exit(-1);
	}

	_soc = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

	if (_soc == INVALID_SOCKET) {
		ib = WSAGetLastError();
		printf("socket rc: %d", ib);
		exit(-1);
	}

	if (SOCKET_ERROR == (connect(_soc, ai->ai_addr, (int)ai->ai_addrlen))) {
		printf("Internal error");
		ib = WSAGetLastError();
		printf("connect rc: %d", ib);
		exit(-1);
	}

	ib = SERVER_ADD_CONNECTION;
	send(_soc, (char*)&ib, sizeof(int), 0);
	send(_soc, (char*)&my_id, sizeof(int), 0);

	recv(_soc, (char*)&ib, sizeof(int), 0);

	if (ib != USER_OK) {

	}

	while (true)
	{
		_ib = recv(_soc, (char*)&ib, sizeof(int), 0);
		if (_ib == 0) {
			closesocket(_soc);
			return 0;
		}
		if (_ib == -1) {
			closesocket(_soc);
			return -1;
		}
		recv(_soc, lcb, ib, 0);
		//printf(lcb);
	}

}

DWORD WINAPI virtualUser(LPVOID) {
	SOCKET _soc;
	addrinfo *ai, _ai;
	HANDLE listener = NULL;
	DWORD thr_id;
	MyStruct mst;
	int ib;
	int _ib;
	int l_id = 0;

	if (WaitForSingleObject(g_m, INFINITE) != WAIT_OBJECT_0) {
		Sleep(10000);
		exit(-1);
	}

	ZeroMemory(&_ai, sizeof(_ai));
	_ai.ai_family = AF_INET;
	_ai.ai_socktype = SOCK_STREAM;
	_ai.ai_protocol = IPPROTO_TCP;

	ib = getaddrinfo(server_addr, server_port, &_ai, &ai);

	if (ib != 0) {
		printf("getaddrinfo rc: %d", ib);
		WSACleanup();
		Sleep(10000);
		exit(-1);
	}

	_soc = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

	if (_soc == INVALID_SOCKET) {
		ib = WSAGetLastError();
		printf("socket rc: %d", ib);
		Sleep(10000);
		exit(-1);
	}

	if (SOCKET_ERROR == (connect(_soc, ai->ai_addr, (int)ai->ai_addrlen))) {
		printf("Internal error");
		ib = WSAGetLastError();
		printf("connect rc: %d", ib);
		Sleep(10000);
		exit(-1);
	}

	ib = SERVER_SET_CONNECTION;
	send(soc, (char*)&ib, sizeof(int), 0);

	recv(soc, (char*)&ib, sizeof(int), 0);

	if (ib != USER_OK) {

	}

	sprintf(mst.name, "user%d", v_id++);
	encryptPasswordMD5(mst.pass, mst.name, &cc);

	ib = SERVER_LOGIN;

	send(soc, (char*)&ib, sizeof(int), 0);

	send(soc, (char*)&mst, sizeof(MyStruct), 0);

	recv(soc, (char*)&ib, sizeof(int), 0);

	if (ib == USER_LOGIN_SUCCES) {
		recv(soc, (char*)&l_id, sizeof(int), 0);
		listener = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)listen, NULL, 0, &thr_id);
	}
	else {
		printf("login deny\n");
		if (!ReleaseMutex(g_m)) {
			Sleep(10000);
			exit(-1);
		}
		return 0;
	}

	if (!ReleaseMutex(g_m)) {
		Sleep(10000);
		exit(-1);
	}

	while (true)
	{

	}

}
