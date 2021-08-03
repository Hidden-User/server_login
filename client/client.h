#pragma once
#include "stdio.h"
#include "string.h"
#include "connection.h"
#include "crypto.h"

value class Message {
private:
	//static SOCKET soc = NULL;
	//static System::Threading::Mutex mm;
	//static CryptoContainer2 cc;
public:
	char* message;
	int type;
	int msgLen;
	void* addition;

	//void exec();
};

public ref class Client {
public:
	HWND *hWnds;
	static Client^ self = nullptr;

	Client();

	~Client();

	void Start();

	void Stop();

	static void Send(Object^ stateInfo);

private:
	HWND wnd;
	HWND logWnd;
	MSG *msg;

	SOCKET s_send;
	SOCKET s_recv;
	CryptoContainer2 cc;
	uuid id;
	uuid room;
	int type;
	char* message;
	int msgLen;
	void* addition;

	System::Threading::ThreadStart^ ts;
	System::Threading::Thread^ th;
	System::Threading::ThreadStart^ rts;
	System::Threading::Thread^ rth;

	System::Threading::Mutex mm;


	void Work();

	void CreateWindowS();

	void Recv();

	//static LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//static LRESULT regProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

public value class session {
public:
	MyStruct* mst;
	HANDLE mm;
	SOCKET s_send;
	SOCKET s_recv;
	uuid id;
	uuid room;
	CryptoContainer2 cc;
};


int createWindow(HINSTANCE hInstance);

int createAuthWind(HINSTANCE hInstance);

int client(int code, MyStruct* user, HINSTANCE hInstance);

LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK authProc(HWND, UINT, WPARAM, LPARAM);
