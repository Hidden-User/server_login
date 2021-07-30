#define _CRT_SECURE_NO_WARNINGS
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "server.h"
#include "crypto.h"

//#define MAX_CLIENTS 1000

//HANDLE g_m;
//SOCKET users[MAX_CLIENTS];
//SOCKET users_share[MAX_CLIENTS];
//int UIDs[MAX_CLIENTS];
//HANDLE process[MAX_CLIENTS];
//unsigned countOfUsers;



//sqlite3 *db = NULL;
//char *err = NULL;

int main()
{
	char cb[128];
	//MyStruct mst;
	//CryptoContainer cc;
	//userFrame *duf;
	//HANDLE main_thread;
	//DWORD thr_id;
	//WSADATA wsad;

	//if (FAILED(WSAStartup(0x0202, &wsad))) {
	//	return -1;
	//}

	//duf = initUserFrames(MAX_CLIENTS, "db.db");

	////cryptoInit(&cc);

	//// SQLite init
	//if (sqlite3_open("db.db", &(duf->db))) {
	//	sqlite3_close(db);
	//	return 1;
	//}//sqlite3_exec(db, _sql_open, 0, 0, &err)
	//if (REQSQL(_sql_open, duf)) {
	//	sqlite3_close(db);
	//	return 1;
	//}

	////for (int t = 0; t < MAX_CLIENTS; t++) {
	////	sprintf(mst.name, "user%d", t);
	////	encryptPasswordMD5(mst.pass, mst.name, &cc);
	////	reg(&mst);
	////}

	//if (duf->mm == NULL)
	//{
	//	printf("CreateMutex error: %d\n", GetLastError());
	//	exit(-1);
	//}

	//main_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_main, duf, 0, &thr_id);

	if (serverStart(1)) {
		printf("server not started\n");
		return -1;
	}

	while (true)
	{
		gets_s(cb, sizeof(cb));
		if (strcmp(cb, "/stop") == 0) {
			serverStart(-1);
			break;
			//WSACleanup();
		}
	}
	/*
	for (int t = 0; t < countOfUsers; t++) {
		closesocket(users[t]);
		CloseHandle(process[t]);
	}

	CloseHandle(main_thread);
	CloseHandle(g_m);

	sqlite3_close(db);*/
	return 0;

}


//void _main() {
//	char host[NI_MAXHOST];
//	char service[NI_MAXSERV];
//	SOCKET soc = NULL;
//
//	int ib = 0;
//	int _ib;
//
//	// server connection init
//	listener = socket(AF_INET, SOCK_STREAM, 0);
//
//	if (listener == INVALID_SOCKET) {
//		ib = WSAGetLastError();
//		printf("socket rc: %d", ib);
//		exit(-1);
//	}
//
//	soc_addr.sin_family = AF_INET;
//	soc_addr.sin_port = htons(49999);
//	soc_addr.sin_addr.S_un.S_addr = INADDR_ANY;
//
//	ib = bind(listener, (SOCKADDR*)&soc_addr, sizeof(soc_addr));
//	if (ib != 0) {
//		printf("bind code: %d", ib);
//		exit(-1);
//	}
//	ib = listen(listener, SOMAXCONN);
//	if (ib != 0) {
//		printf("listen code: %d", ib);
//		exit(-1);
//	}
//
//	while (true)
//	{
//		ib = sizeof(soc_addr);
//		soc = accept(listener, (SOCKADDR*)&soc_addr, &ib);
//
//		if (soc == INVALID_SOCKET) {
//			ib = WSAGetLastError();
//			printf("soc rc: %d", ib);
//			sqlite3_close(db);
//			exit(-1);
//		}
//
//		//getnameinfo((sockaddr*)&listener, sizeof(listener), host, sizeof(host), service, sizeof(service), 0);
//
//		recv(soc, (char*)&ib, sizeof(int), 0);
//		switch (ib)
//		{
//		case SERVER_SET_CONNECTION:
//			users[countOfUsers] = soc;
//			users_share[countOfUsers] = NULL;
//			process[countOfUsers] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_user, NULL, 0, NULL);
//
//			if (process[countOfUsers] == NULL) {
//				printf("CreateThread error: %d\n", GetLastError());
//				sqlite3_close(db);
//				exit(-1);
//			}
//
//			_ib = USER_OK;
//
//			send(soc, (char*)&_ib, sizeof(int), 0);
//
//			break;
//		case SERVER_ADD_CONNECTION:
//
//			recv(soc, (char*)&_ib, sizeof(int), 0);
//			ib = _ib;
//
//			for (int t = 0; t < countOfUsers; t++) {
//				if (UIDs[t] == _ib) {
//					users_share[t] = soc;
//					_ib = 0;
//					break;
//				}
//			}
//
//			if (ib != _ib) {
//				_ib = USER_OK;
//			}
//			else {
//				_ib = USER_ACCESS_DENY;
//			}
//
//			send(soc, (char*)&_ib, sizeof(int), 0);
//
//			break;
//		default:
//			break;
//		}
//
//	}
//
//}

