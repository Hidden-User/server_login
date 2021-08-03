#define _CRT_SECURE_NO_WARNINGS
#include "server.h"
#include "stdio.h"
#include "stdlib.h"

#pragma comment (lib, "ws2_32.lib")

char buffer[1024];

DWORD WINAPI _main(LPVOID lpUF) {
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];
	SOCKET soc = NULL;
	SOCKET listener;
	sockaddr_in soc_addr;
	userFrame *duf = (userFrame*)lpUF;
	int counter = 0;

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
	soc_addr.sin_port = htons(SERVERPORT);
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
			//danger!!! ->
			serverStart(-1);
			return 0;
		}

		if (counter >= MAX_CLIENTS) {
			recv(soc, (char*)&ib, sizeof(int), 0);
			ib = USER_ACCESS_DENY;
			send(soc, (char*)&ib, sizeof(int), 0);
			closesocket(soc);
			continue;
		}

		//getnameinfo((sockaddr*)&listener, sizeof(listener), host, sizeof(host), service, sizeof(service), 0);

		recv(soc, (char*)&ib, sizeof(int), 0);
		switch (ib)
		{
		case SERVER_SET_CONNECTION:
			_ib = searchFreeUF(duf);
			if (_ib >= MAX_CLIENTS) {

			}
			duf[_ib].talk = soc;
			duf[_ib].share = NULL;
			duf[_ib].suf = duf;
			duf[_ib].pt = CreateThreadpoolWork((PTP_WORK_CALLBACK)_user2, &(duf[_ib]), NULL);
			//duf[_ib].pt = CreateThreadpoolWork((PTP_WORK_CALLBACK)_user2, &duf, NULL);

			if (duf[_ib].pt == NULL) {
				printf("CreateThreadpoolWork error: %d", GetLastError());
				serverStart(-1);
				exit(-1);
			}

			SubmitThreadpoolWork(duf[_ib].pt);
			//duf[_ib].tt = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_user, &(duf[_ib]), 0, NULL);

			//if (duf[_ib].tt == NULL) {
			//	printf("CreateThread error: %d\n", GetLastError());
			//	serverStart(-1);
			//	//sqlite3_close(db);
			//	exit(-1);
			//}

			_ib = USER_OK;

			send(soc, (char*)&_ib, sizeof(int), 0);

			counter++;

			break;
		case SERVER_ADD_CONNECTION:

			recv(soc, (char*)&_ib, sizeof(int), 0);

			if (verifyUID(duf, _ib)) {
				duf[searchUID(duf, _ib)].share = soc;
				_ib = USER_OK;
			}
			else {
				_ib = USER_ACCESS_DENY;
			}

			send(soc, (char*)&_ib, sizeof(int), 0);

			if (_ib == USER_ACCESS_DENY) {
				closesocket(soc);
			}

			break;
		default:
			break;
		}

	}
	return 0;
}


void share(userFrame* lpUF, uuid room)
{
	int strl = strlen(buffer) + 1;

	for (int t = 0; t < MAX_CLIENTS; t++) {
		//if (lpUF[t].id == ignore) {
		//	continue;
		//}
		if (lpUF[t].chat != room) {
			continue;
		}
		if (lpUF[t].share == 0) {
			continue;
		}
		send(lpUF[t].share, (char*)&strl, sizeof(int), 0);
		send(lpUF[t].share, buffer, strl, 0);
	}
}

void roomMSG(userFrame *lpUF, uuid room, char* lcb) {
	int ib = strlen(lcb);
	for (int t = 0; t < MAX_CLIENTS; t++) {
		if (lpUF[t].chat != room) {
			continue;
		}
		if (lpUF[t].share == 0) {
			continue;
		}
		send(lpUF[t].share, (char*)&ib, sizeof(int), 0);
		send(lpUF[t].share, lcb, ib, 0);
	}
}

userFrame * initUserFrames(unsigned count, const char * dataBaseName)
{
	userFrame* uf;

	uf = (userFrame*)malloc(count * sizeof(userFrame));

	if (sqlite3_open(dataBaseName, &(uf->db))) {
		sqlite3_close(uf->db);
		free(uf);
		return NULL;
	}

	uf->err = (char**)malloc(sizeof(void*));

	if (REQSQL(_sql_open, uf)) {
		sqlite3_close(uf->db);
		free(uf->err);
		free(uf);
		return NULL;
	}

	uf->mm = CreateMutex(NULL, FALSE, NULL);
	if (uf->mm == NULL) {
		sqlite3_close(uf->db);
		free(uf->err);
		free(uf);
		return NULL;
	}

	uf->tp = CreateThreadpool(NULL);
	if (uf->tp == NULL) {
		sqlite3_close(uf->db);
		free(uf->err);
		CloseHandle(uf->mm);
		free(uf);
	}
	SetThreadpoolThreadMaximum(uf->tp, MAX_CLIENTS);
	SetThreadpoolThreadMinimum(uf->tp, 2);

	uf->cc = (CryptoContainer*)malloc(sizeof(CryptoContainer));
	cryptoInitToAES(uf->cc);

	for (int t = 0; t < count; t++) {
		uf[t].db = uf->db;
		uf[t].mm = uf->mm;
		uf[t].err = uf->err;
		uf[t].cc = uf->cc;
		uf[t].tp = uf->tp;
		uf[t].id = 0;
		uf[t].chat = -1;
		uf[t].share = 0;
		uf[t].talk = 0;
		//uf[t].tt = NULL;
		uf[t].pt = NULL;
	}

	return uf;
}

void releaseUserFrames(userFrame ** uf)
{
	userFrame *duf = *uf;

	sqlite3_close(duf->db);
	CloseHandle(duf->mm);
	for (int t = 0; t < MAX_CLIENTS; t++) {
		if (*(duf[t].err) != NULL) {
			sqlite3_free(*(duf[t].err));
		}
		if (duf[t].talk) {
			closesocket(duf->talk);
		}
		if (duf[t].share) {
			closesocket(duf->share);
		}
		//if (duf[t].tt) {
		//	CloseHandle(duf[t].tt);
		//}
		if (duf[t].pt) {
			CloseThreadpoolWork(duf[t].pt);
		}
	}
	free(duf->err);
	CloseThreadpool(duf->tp);
	free(duf);
	uf = NULL;

}

void closeUserFrame(userFrame* uf) {
	uf->chat = -1;
	uf->id = 0;
	closesocket(uf->talk);
	uf->talk = 0;
	closesocket(uf->share);
	uf->share = 0;
	CloseThreadpoolWork(uf->pt);
}

int serverStart(int code)
{
	static userFrame *duf;
	static HANDLE main_thread;
	static DWORD thr_id;
	static WSADATA wsad;
	static int _wsaInit = 0;

	if (code == 0) {
		return _wsaInit;
	}

	if (code < 0) {
		if (_wsaInit == 0) {
			return 0;
		}
		if (duf != NULL) {
			releaseUserFrames(&duf);
		}
		CloseHandle(main_thread);
		WSACleanup();
		_wsaInit = 0;
		return 0;
	}

	if (_wsaInit == 0) {
		if (FAILED(WSAStartup(0x0202, &wsad))) {
			_wsaInit = -1;
			return _wsaInit;
		}
	}
	else {
		if (_wsaInit < 0) {
			return -1;
		}
	}

	duf = initUserFrames(MAX_CLIENTS, "db.db");

	if (duf == NULL) {
		if (_wsaInit > 0) {
			WSACleanup();
		}
		_wsaInit = -1;
		return -1;
	}

	main_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_main, duf, 0, &thr_id);

	if (main_thread == NULL) {
		releaseUserFrames(&duf);
		if (_wsaInit > 0) {
			WSACleanup();
			_wsaInit = 0;
		}
		return 1;
	}

	return 0;
}

int callback1(void *, int, char **, char **)
{
	return 0;
}

int callback2(void *v, int argc, char **, char **)// login
{
	((int*)v)[0] = argc;
	return 0;
}

int callback3(void *, int, char **, char **)
{
	return 0;
}

int tryLogin(userFrame* uf, MyStruct* mst) {
	int ib;
	int _ib;

	sprintf(buffer, _sql_search, mst->name, mst->pass);

	ib = sqlite3_exec(uf->db, buffer, callback2, &_ib, uf->err);

	if (ib) {
		return -1;
	}

	if (_ib == 1) {
		return 0;
	}

	return 1;
}

int reg(userFrame* uf, MyStruct* mst) {
	int ib;
	int _ib = 0;

	sprintf(buffer, _sql_search2, mst->name);

	ib = sqlite3_exec(uf->db, buffer, callback2, &_ib, uf->err);

	if (ib) {
		return -1;
	}

	if (_ib) {
		return 1;
	}

	sprintf(buffer, _sql_insert, mst->name, mst->pass);

	ib = sqlite3_exec(uf->db, buffer, 0, 0, uf->err);

	if (ib) {
		return -1;
	}

	return 0;

}

DWORD WINAPI _user(LPVOID lpUF) {
	//unsigned my_index = countOfUsers++;
	//SOCKET soc = users[my_index];
	userFrame *uf = (userFrame*)lpUF;
	userFrame *suf = uf->suf;
	char cb[512];
	char cb2[512];
	MyStruct user;
	int _ib;
	int ib;
	uuid uid;
	bool _work;
	bool logged = false;

	_work = true;

	while (_work)
	{
		recv(uf->talk, (char*)&_ib, sizeof(int), 0);

		if (WaitForSingleObject(uf->mm, INFINITE) != WAIT_OBJECT_0) {
			printf("src 382\n");
			exit(-1);
		}

		switch (_ib)
		{
		case SERVER_LOGIN:
			recv(uf->talk, cb, sizeof(MyStruct) + 16, 0);

			//printf("user try login: %s | %s\n", user.name, user.pass);

			if (logged) {
				ib = USER_LOGIN_DENY;
				send(uf->talk, (char*)&ib, sizeof(int), 0);
			}

			decryptSession((BYTE*)cb2, (BYTE*)cb, 256, uf->cc);

			memcpy(&user, cb2, sizeof(MyStruct));

			ib = tryLogin(uf, &user);

			if (ib) {
				ib = USER_LOGIN_DENY;
				send(uf->talk, (char*)&ib, sizeof(int), 0);
				if (ib == 1) {
					printf("Database have equals usernames: %s\n", user.name);
				}
				else {
					serverStart(-1);
				}
			}
			else {
				ib = USER_LOGIN_SUCCES;
				send(uf->talk, (char*)&ib, sizeof(int), 0);
				do
				{
					_ib = rand();
					while (true)
					{
					_random_1:
						for (int t = 0; t < MAX_CLIENTS; t++) {
							if (_ib == suf[t].id) {
								_ib = rand();
								goto _random_1;
							}
						}
						break;
					}
				} while (_ib == 0);
				send(uf->talk, (char*)&_ib, sizeof(int), 0);
				uf->id = _ib;
				logged = true;
			}


			break;
		case SERVER_LOGOUT:

			ib = USER_OK;

			send(uf->talk, (char*)&ib, sizeof(int), 0);

			uf->id = 0;

			logged = false;

			break;
		case SERVER_REGISTER:

			recv(uf->talk, cb, sizeof(MyStruct) + 16, 0);

			//printf("new user: %s | %s\n", user.name, user.pass);

			if (logged) {
				ib = USER_REGISTER_DENY;
				send(uf->talk, (char*)&ib, sizeof(int), 0);
			}

			decryptSession((BYTE*)cb2, (BYTE*)cb, 256, uf->cc);

			memcpy(&user, cb2, sizeof(MyStruct));

			ib = reg(uf, &user);

			if (ib) {
				ib = USER_REGISTER_DENY;
				send(uf->talk, (char*)&ib, sizeof(int), 0);
				if (ib < 0) {
					serverStart(-1);
				}
			}
			else {
				do
				{
					_ib = rand();
					while (true)
					{
					_random_2:
						for (int t = 0; t < MAX_CLIENTS; t++) {
							if (_ib == suf[t].id) {
								_ib = rand();
								goto _random_2;
							}
						}
						break;
					}
				} while (_ib == 0);
				ib = USER_REGISTER_SUCCESS;

				send(uf->talk, (char*)&ib, sizeof(int), 0);
				send(uf->talk, (char*)&_ib, sizeof(int), 0);
				uf->id = _ib;

				logged = true;
			}

			break;
		case SERVER_MESSAGE:


			recv(uf->talk, (char*)&ib, sizeof(int), 0);
			recv(uf->talk, cb, ib, 0);

			if (!logged) {
				ib = USER_ACCESS_DENY;
				send(uf->talk, (char*)&ib, sizeof(int), 0);
				break;
			}

			sprintf(buffer, "%s: %s\n", user.name, cb);
			printf(buffer);
			//printf("%s: %s\n", user.name, cb);

			ib = USER_OK;

			send(uf->talk, (char*)&ib, sizeof(int), 0);

			share(suf, uf->chat);

			break;
		default:
			break;
		}

		if (!ReleaseMutex(uf->mm)) {
			printf("src 528\n");
			exit(-1);
		}

	}
	return 0;
}

VOID WINAPI _user2(PTP_CALLBACK_INSTANCE, LPVOID lpUF) {
	//unsigned my_index = countOfUsers++;
	//SOCKET soc = users[my_index];
	userFrame *uf = (userFrame*)lpUF;
	userFrame *suf = uf->suf;
	char cb[512];
	char cb2[512];
	MyStruct user;
	int _ib;
	int ib;
	uuid uid;
	bool _work;
	bool logged = false;

	_work = true;

	while (_work)
	{
		ib = recv(uf->talk, (char*)&_ib, sizeof(int), 0);

		if (ib == 0) {
			switch (WSAGetLastError())
			{
			default:
				break;
			case WSAENETDOWN:
				//serverStart(-1);
				//exit(-1);
				closeUserFrame(uf);
				return;
				break;
			case WSAENOTCONN:
			case WSAENETRESET:
			case WSAENOTSOCK:
			case WSAESHUTDOWN:
			case WSAEMSGSIZE:
			case WSAEINVAL:
			case WSAETIMEDOUT:
			case WSAECONNRESET:
			case WSAECONNABORTED:
				closeUserFrame(uf);
				_work = false;
				continue;
				break;
			}
		}

		if (ib == SOCKET_ERROR) {
			switch (WSAGetLastError())
			{
			default:
				break;
			case WSAENETDOWN:
				//serverStart(-1);
				//exit(-1);
				closeUserFrame(uf);
				return;
				break;
			case WSAENOTCONN:
			case WSAENETRESET:
			case WSAENOTSOCK:
			case WSAESHUTDOWN:
			case WSAEMSGSIZE:
			case WSAEINVAL:
			case WSAETIMEDOUT:
			case WSAECONNRESET:
			case WSAECONNABORTED:
				closeUserFrame(uf);
				_work = false;
				continue;
				break;
			}
		}

		if (WaitForSingleObject(uf->mm, INFINITE) != WAIT_OBJECT_0) {
			printf("src 557\n");
			exit(-1);
		}

		switch (_ib)
		{
		case SERVER_LOGIN:
			recv(uf->talk, cb, sizeof(MyStruct) + 16, 0);

			//printf("user try login: %s | %s\n", user.name, user.pass);

			if (logged) {
				ib = USER_LOGIN_DENY;
				send(uf->talk, (char*)&ib, sizeof(int), 0);
			}

			decryptSession((BYTE*)cb2, (BYTE*)cb, 256, uf->cc);

			memcpy(&user, cb2, sizeof(MyStruct));

			ib = tryLogin(uf, &user);

			if (ib) {
				ib = USER_LOGIN_DENY;
				send(uf->talk, (char*)&ib, sizeof(int), 0);
				if (ib == 1) {
					printf("Database have equals usernames: %s\n", user.name);
				}
				else {
					serverStart(-1);
				}
			}
			else {
				ib = USER_LOGIN_SUCCES;
				send(uf->talk, (char*)&ib, sizeof(int), 0);
				do
				{
					_ib = rand();
					while (true)
					{
					_random_1:
						for (int t = 0; t < MAX_CLIENTS; t++) {
							if (_ib == suf[t].id) {
								_ib = rand();
								goto _random_1;
							}
						}
						break;
					}
				} while (_ib == 0);
				send(uf->talk, (char*)&_ib, sizeof(int), 0);
				uf->id = _ib;
				logged = true;
			}


			break;
		case SERVER_LOGOUT:

			ib = USER_OK;

			send(uf->talk, (char*)&ib, sizeof(int), 0);

			uf->id = 0;

			logged = false;

			break;
		case SERVER_REGISTER:

			recv(uf->talk, cb, sizeof(MyStruct) + 16, 0);

			//printf("new user: %s | %s\n", user.name, user.pass);

			if (logged) {
				ib = USER_REGISTER_DENY;
				send(uf->talk, (char*)&ib, sizeof(int), 0);
			}

			decryptSession((BYTE*)cb2, (BYTE*)cb, 256, uf->cc);

			memcpy(&user, cb2, sizeof(MyStruct));

			ib = reg(uf, &user);

			if (ib) {
				ib = USER_REGISTER_DENY;
				send(uf->talk, (char*)&ib, sizeof(int), 0);
				if (ib < 0) {
					serverStart(-1);
				}
			}
			else {
				do
				{
					_ib = rand();
					while (true)
					{
					_random_2:
						for (int t = 0; t < MAX_CLIENTS; t++) {
							if (_ib == suf[t].id) {
								_ib = rand();
								goto _random_2;
							}
						}
						break;
					}
				} while (_ib == 0);
				ib = USER_REGISTER_SUCCESS;

				send(uf->talk, (char*)&ib, sizeof(int), 0);
				send(uf->talk, (char*)&_ib, sizeof(int), 0);
				uf->id = _ib;

				logged = true;
			}

			break;
		case SERVER_CHANGE_ROOM:

			recv(uf->talk, (char*)&ib, sizeof(int), 0);

			if (!logged) {
				ib = USER_ACCESS_DENY;
				send(uf->talk, (char*)&ib, sizeof(int), 0);
				break;
			}

			if (uf->chat == ib) {
				// leave

				sprintf(buffer, "User: '%s' leave from room %d\n", user.name, uf->chat + 1);

				share(suf, uf->chat);

				uf->chat = -1;
				break;
			}

			if (uf->chat != -1) {

				sprintf(buffer, "User: '%s' leave from room %d\n", user.name, uf->chat + 1);

				share(suf, uf->chat);
				// leave
			}

			sprintf(buffer, "User: '%s' join to room %d\n", user.name, ib + 1);

			uf->chat = ib;

			share(suf, uf->chat);

			ib = USER_OK;

			send(uf->talk, (char*)&ib, sizeof(int), 0);

			break;
		case SERVER_MESSAGE:


			recv(uf->talk, (char*)&ib, sizeof(int), 0);
			recv(uf->talk, cb, ib, 0);

			if (!logged) {
				ib = USER_ACCESS_DENY;
				send(uf->talk, (char*)&ib, sizeof(int), 0);
				break;
			}

			if (uf->chat == -1) {
				ib = USER_ACCESS_DENY;
				send(uf->talk, (char*)&ib, sizeof(int), 0);
				break;
			}

			sprintf(buffer, "%s: %s\n", user.name, cb);
			printf(buffer);
			//printf("%s: %s\n", user.name, cb);

			ib = USER_OK;

			send(uf->talk, (char*)&ib, sizeof(int), 0);

			share(suf, uf->chat);

			break;
		default:
			break;
		}

		if (!ReleaseMutex(uf->mm)) {
			printf("src 703\n");
			exit(-1);
		}

	}
}


int searchFreeUF(userFrame* uf) {
	for (int t = 0; t < MAX_CLIENTS; t++) {
		if (uf[t].id == 0) {
			return t;
		}
	}
	return -1;
}

int searchUID(userFrame* uf, uuid id) {
	for (int t = 0; t < MAX_CLIENTS; t++) {
		if (uf[t].id == id) {
			return t;
		}
	}
	return -1;
}

bool verifyUID(userFrame* uf, uuid id) {
	for (int t = 0; t < MAX_CLIENTS; t++) {
		if (uf[t].id == id) {
			return true;
		}
	}
	return false;
}
