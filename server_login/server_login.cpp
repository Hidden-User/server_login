#define _CRT_SECURE_NO_WARNINGS
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "sqlite3.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include "connection.h"
#include "crypto.h"

#define MAX_CLIENTS 10000

HANDLE g_m;
SOCKET users[MAX_CLIENTS];
SOCKET users_share[MAX_CLIENTS];
int UIDs[MAX_CLIENTS];
HANDLE process[MAX_CLIENTS];
unsigned countOfUsers;

WSADATA wsad;
SOCKET listener;
sockaddr_in soc_addr;

#pragma comment (lib, "ws2_32.lib")

const char _sql_open[] = "CREATE TABLE IF NOT EXISTS UserList (UID INTEGER PRIMARY KEY AUTOINCREMENT, Name TEXT, Password TEXT)";
const char _sql_insert[] = "INSERT INTO UserList (Name, Password) values ('%s', '%s');";
const char _sql_search[] = "SELECT UID FROM UserList WHERE Name LIKE '%s' AND Password LIKE '%s'";
const char _sql_search2[] = "SELECT UID FROM UserList WHERE Name LIKE '%s'";
char buffer[1024];

sqlite3 *db = NULL;
char *err = NULL;
int specVal;

int callback1(void*, int, char**, char**);
int callback2(void*, int, char**, char**);
int callback3(void*, int, char**, char**);

bool tryLogin(MyStruct*);
int reg(MyStruct*);

DWORD WINAPI _user(LPVOID);

void _main();
void share(int ignore);

int main()
{
	char cb[128];
	//MyStruct mst;
	//CryptoContainer cc;
	HANDLE main_thread;
	DWORD thr_id;

	if (FAILED(WSAStartup(0x0202, &wsad))) {
		return -1;
	}

	//cryptoInit(&cc);

	// SQLite init
	if (sqlite3_open("db.db", &db)) {
		sqlite3_close(db);
		return 1;
	}
	else if (sqlite3_exec(db, _sql_open, 0, 0, &err)) {
		sqlite3_close(db);
		return 1;
	}

	//for (int t = 0; t < MAX_CLIENTS; t++) {
	//	sprintf(mst.name, "user%d", t);
	//	encryptPasswordMD5(mst.pass, mst.name, &cc);
	//	reg(&mst);
	//}

	g_m = CreateMutex(NULL, FALSE, NULL);

	if (g_m == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		exit(-1);
	}

	main_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_main, NULL, 0, &thr_id);

	while (true)
	{
		gets_s(cb, sizeof(cb));
		if (strcmp(cb, "/stop") == 0) {
			WSACleanup();
		}
	}
	
	for (int t = 0; t < countOfUsers; t++) {
		closesocket(users[t]);
		CloseHandle(process[t]);
	}

	CloseHandle(main_thread);
	CloseHandle(g_m);

	sqlite3_close(db);
	return 0;

}

void _main() {
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];
	SOCKET soc = NULL;

	int ib = 0;
	int _ib;

	// server connection init
	listener = socket(AF_INET, SOCK_STREAM, 0);

	if (listener == INVALID_SOCKET) {
		ib = WSAGetLastError();
		printf("socket rc: %d", ib);
		exit(-1);
	}

	soc_addr.sin_family = AF_INET;
	soc_addr.sin_port = htons(49999);
	soc_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	ib = bind(listener, (SOCKADDR*)&soc_addr, sizeof(soc_addr));
	if (ib != 0) {
		printf("bind code: %d", ib);
		exit(-1);
	}
	ib = listen(listener, SOMAXCONN);
	if (ib != 0) {
		printf("listen code: %d", ib);
		exit(-1);
	}

	while (true)
	{
		ib = sizeof(soc_addr);
		soc = accept(listener, (SOCKADDR*)&soc_addr, &ib);

		if (soc == INVALID_SOCKET) {
			ib = WSAGetLastError();
			printf("soc rc: %d", ib);
			sqlite3_close(db);
			exit(-1);
		}

		//getnameinfo((sockaddr*)&listener, sizeof(listener), host, sizeof(host), service, sizeof(service), 0);

		recv(soc, (char*)&ib, sizeof(int), 0);
		switch (ib)
		{
		case SERVER_SET_CONNECTION:
			users[countOfUsers] = soc;
			users_share[countOfUsers] = NULL;
			process[countOfUsers] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_user, NULL, 0, NULL);

			if (process[countOfUsers] == NULL) {
				printf("CreateThread error: %d\n", GetLastError());
				sqlite3_close(db);
				exit(-1);
			}

			_ib = USER_OK;

			send(soc, (char*)&_ib, sizeof(int), 0);

			break;
		case SERVER_ADD_CONNECTION:

			recv(soc, (char*)&_ib, sizeof(int), 0);
			ib = _ib;

			for (int t = 0; t < countOfUsers; t++) {
				if (UIDs[t] == _ib) {
					users_share[t] = soc;
					_ib = 0;
					break;
				}
			}

			if (ib != _ib) {
				_ib = USER_OK;
			}
			else {
				_ib = USER_ACCESS_DENY;
			}

			send(soc, (char*)&_ib, sizeof(int), 0);

			break;
		default:
			break;
		}

	}

}

void share(int ignore)
{
	int strl = strlen(buffer) + 1;

	for (int t = 0; t < countOfUsers; t++) {
		if (t == ignore) {
			continue;
		}
		send(users_share[t], (char*)&strl, sizeof(int), 0);
		send(users_share[t], buffer, strl, 0);
	}
}

int callback1(void *, int, char **, char **)
{
	return 0;
}

int callback2(void *v, int argc, char **, char **)// login
{
	specVal = argc;
	return 0;
}

int callback3(void *, int, char **, char **)
{
	return 0;
}

bool tryLogin(MyStruct* mst) {
	int ib;

	sprintf(buffer, _sql_search, mst->name, mst->pass);

	specVal = 0;

	ib = sqlite3_exec(db, buffer, callback2, 0, &err);

	if (ib) {
		sqlite3_close(db);
		printf("login error\n");
		exit(-1);
	}

	if (specVal == 1) {
		return true;
	}
	else if (specVal > 1) {
		printf("login: err eq names\n");
	}
	
	return false;
}

int reg(MyStruct* mst) {
	int ib;

	sprintf(buffer, _sql_search2, mst->name);

	specVal = 0;

	ib = sqlite3_exec(db, buffer, callback2, 0, &err);

	if (ib) {
		sqlite3_close(db);
		printf("reg error\n");
		exit(-1);
	}

	if (specVal) {
		return 1;
	}

	sprintf(buffer, _sql_insert, mst->name, mst->pass);

	ib = sqlite3_exec(db, buffer, 0, 0, &err);

	if (ib) {
		sqlite3_close(db);
		printf("reg 2 error\n");
		exit(-1);
	}

	return 0;

}

DWORD WINAPI _user(LPVOID) {
	unsigned my_index = countOfUsers++;
	SOCKET soc = users[my_index];
	char cb[256];
	MyStruct user;
	int _ib;
	int ib;
	int uid;
	bool _work;
	bool logged = false;

	_work = true;

	while (_work)
	{
		recv(soc, (char*)&_ib, sizeof(int), 0);

		if (WaitForSingleObject(g_m, INFINITE) != WAIT_OBJECT_0) {
			exit(-1);
		}

		switch (_ib)
		{
		case SERVER_LOGIN:
			recv(soc, (char*)&user, sizeof(MyStruct), 0);

			printf("user try login: %s | %s\n", user.name, user.pass);

			if (logged) {
				ib = USER_LOGIN_DENY;
				send(soc, (char*)&ib, sizeof(int), 0);
			}

			if (tryLogin(&user)) {
				ib = USER_LOGIN_SUCCES;
				send(soc, (char*)&ib, sizeof(int), 0);
				do
				{
					uid = rand();
					while (true)
					{
						_random_1:
						for (int t = 0; t < countOfUsers; t++) {
							if (uid == UIDs[t]) {
								uid = rand();
								goto _random_1;
							}
						}
						break;
					}
				} while (uid == 0);
				send(soc, (char*)&uid, sizeof(int), 0);
				UIDs[my_index] = uid;
				logged = true;
			}
			else {
				ib = USER_LOGIN_DENY;
				send(soc, (char*)&ib, sizeof(int), 0);
			}


			break;
		case SERVER_LOGOUT:

			ib = USER_OK;

			send(soc, (char*)&ib, sizeof(int), 0);

			uid = 0;
			UIDs[my_index] = 0;

			logged = false;

			break;
		case SERVER_REGISTER:

			recv(soc, (char*)&user, sizeof(MyStruct), 0);

			printf("new user: %s | %s\n", user.name, user.pass);

			if (logged) {
				ib = USER_REGISTER_DENY;
				send(soc, (char*)&ib, sizeof(int), 0);
			}

			if (reg(&user)) {
				ib = USER_REGISTER_DENY;
				send(soc, (char*)&ib, sizeof(int), 0);
			}
			else {
				do
				{
					uid = rand();
					while (true)
					{
					_random_2:
						for (int t = 0; t < countOfUsers; t++) {
							if (uid == UIDs[t]) {
								uid = rand();
								goto _random_2;
							}
						}
						break;
					}
				} while (uid == 0);
				ib = USER_REGISTER_SUCCESS;

				send(soc, (char*)&ib, sizeof(int), 0);
				send(soc, (char*)&uid, sizeof(int), 0);
				UIDs[my_index] = uid;

				logged = true;
			}

			break;
		case SERVER_MESSAGE:

			recv(soc, (char*)&ib, sizeof(int), 0);
			recv(soc, cb, ib, 0);

			if (!logged) {
				ib = USER_ACCESS_DENY;
				send(soc, (char*)&ib, sizeof(int), 0);
				break;
			}

			sprintf(buffer, "%s: %s\n", user.name, cb);
			printf(buffer);
			//printf("%s: %s\n", user.name, cb);

			ib = USER_OK;

			send(soc, (char*)&ib, sizeof(int), 0);

			share(my_index);

			break;
		default:
			break;
		}

		if (!ReleaseMutex(g_m)) {
			exit(-1);
		}

	}
}


