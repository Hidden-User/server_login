#pragma once
#include "sqlite3.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include "connection.h"
#include "crypto.h"

#ifndef MAX_CLIENTS
#define MAX_CLIENTS 1000
#endif // !MAX_CLIENTS

#ifndef SERVERPORT
#define SERVERPORT 49999
#endif // !SERVERPORT

struct userFrame
{
	SOCKET talk;
	SOCKET share;
	sqlite3 *db;
	char** err;
	uuid id;
	uuid chat;
	PTP_WORK pt;
	//HANDLE tt;
	HANDLE mm;
	CryptoContainer* cc;
	userFrame* suf;
	PTP_POOL tp;
};

//#define REQSQL(req, sql, meth) sqlite3_exec(sql->db, req, meth, NULL, sql->err)
#define REQSQL(req, sql) sqlite3_exec(sql->db, req, NULL, NULL, sql->err)

const char _sql_open[] = "CREATE TABLE IF NOT EXISTS UserList (UID INTEGER PRIMARY KEY AUTOINCREMENT, Name TEXT, Password TEXT)";
const char _sql_insert[] = "INSERT INTO UserList (Name, Password) values ('%s', '%s');";
const char _sql_search[] = "SELECT UID FROM UserList WHERE Name LIKE '%s' AND Password LIKE '%s'";
const char _sql_search2[] = "SELECT UID FROM UserList WHERE Name LIKE '%s'";

userFrame* initUserFrames(unsigned count, const char * dataBaseName);
void releaseUserFrames(userFrame **uf);

int serverStart(int code);

int callback1(void*, int, char**, char**);
int callback2(void*, int, char**, char**);
int callback3(void*, int, char**, char**);

int tryLogin(userFrame*, MyStruct*);
int reg(userFrame*, MyStruct*);

DWORD WINAPI _user(LPVOID);
VOID WINAPI _user2(PTP_CALLBACK_INSTANCE, LPVOID lpUF);

DWORD WINAPI _main(LPVOID);
//void _main();
void share(userFrame* lpUF, uuid room);

int searchFreeUF(userFrame*);
int searchUID(userFrame*, uuid);
bool verifyUID(userFrame*, uuid);
