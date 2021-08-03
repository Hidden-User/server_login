#include "client.h"
#include <WinSock2.h>
#include <ws2tcpip.h>

using namespace System;
using namespace System::Threading;


int createWindow(HINSTANCE hInstance)
{
	const wchar_t wcn[] = L"Client";
	const wchar_t wn[] = L"Client";
	int wW = 1280;
	int wH = 800;

	::WNDCLASS wndcl;
	wndcl.style = CS_VREDRAW | CS_HREDRAW;
	wndcl.lpfnWndProc = &wndProc;
	wndcl.cbClsExtra = 0;
	wndcl.cbWndExtra = 0;
	wndcl.hInstance = hInstance;
	wndcl.hIcon = NULL;
	wndcl.hCursor = NULL;
	wndcl.hbrBackground = reinterpret_cast <HBRUSH> (COLOR_BTNFACE + 1);
	wndcl.lpszMenuName = NULL;
	wndcl.lpszClassName = wcn;
	::RegisterClass(&wndcl);



	HWND mainWindow = ::CreateWindow(
		wcn,
		wn,
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wW,
		wH,
		NULL,
		NULL,
		hInstance,
		0);

	::ShowWindow(mainWindow, SW_SHOW);

	::MSG message;
	while (::GetMessageA(&message, 0, 0, 0)) {
		switch (message.message) {
		case WM_QUIT:
			break;
		default:
			::TranslateMessage(&message);
			::DispatchMessage(&message);
			break;
		}
	}
	return 0;
}

int createAuthWind(HINSTANCE hInstance)
{
	const wchar_t wcn[] = L"Auth";
	const wchar_t wn[] = L"Auth";
	int wW = 1280;
	int wH = 800;

	::WNDCLASS wndcl;
	wndcl.style = CS_VREDRAW | CS_HREDRAW;
	wndcl.lpfnWndProc = &wndProc;
	wndcl.cbClsExtra = 0;
	wndcl.cbWndExtra = 0;
	wndcl.hInstance = hInstance;
	wndcl.hIcon = NULL;
	wndcl.hCursor = NULL;
	wndcl.hbrBackground = reinterpret_cast <HBRUSH> (COLOR_BTNFACE + 1);
	wndcl.lpszMenuName = NULL;
	wndcl.lpszClassName = wcn;
	::RegisterClass(&wndcl);



	HWND mainWindow = ::CreateWindow(
		wcn,
		wn,
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wW,
		wH,
		NULL,
		NULL,
		hInstance,
		0);

	::ShowWindow(mainWindow, SW_SHOW);

	::MSG message;
	while (::GetMessageA(&message, 0, 0, 0)) {
		switch (message.message) {
		case WM_QUIT:
			break;
		default:
			::TranslateMessage(&message);
			::DispatchMessage(&message);
			break;
		}
	}
	return 0;
}

int client(int code, MyStruct* user, HINSTANCE hInstance)
{
	static session ses;
	HWND wnd = NULL;
	MSG msg;
	BOOL bRet = 0;

	if (user != NULL) {
		
		ses.mst = user;

		if (code < 0) {
			return 0;
		}
	}

	if (code < 0) {
		//close all
	}

	

	while (bRet = GetMessage(&msg, wnd, 0, 0))
	{
		// auth window
		if (bRet == -1) {
			break;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		if (ses.mst != NULL) {
			::CloseWindow(wnd);
		}
		
	}



	return 0;
}

//LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//LRESULT CALLBACK regProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT res = FALSE;
	Message^ mes;
	Client^ cl = Client::self;
	static HWND textBox1, textBox2;
	static HWND button;
	static HWND rooms[5];
	int ib, _ib;

	switch (message)
	{
	default:
		SKIPDWP
			break;
	case WM_CLOSE:
		SKIPDWP
			break;
	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	case WM_CREATE:

		textBox1 = CreateWindow(L"STATIC", L"", WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 60, 1000, 650, hWnd, (HMENU)1, NULL, NULL);
		cl->hWnds[0] = textBox1;
		textBox2 = CreateWindow(L"EDIT", L"", WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 710, 1000, 20, hWnd, (HMENU)2, NULL, NULL);

		button = CreateWindow(L"BUTTON", L"send", WS_VISIBLE | WS_CHILD | WS_BORDER, 1020, 710, 70, 20, hWnd, (HMENU)3, NULL, NULL);
		rooms[0] = CreateWindow(L"BUTTON", L"Room 1", WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 10, 190, 20, hWnd, (HMENU)4, NULL, NULL);
		rooms[1] = CreateWindow(L"BUTTON", L"Room 2", WS_BORDER | WS_CHILD | WS_VISIBLE, 210, 10, 190, 20, hWnd, (HMENU)5, NULL, NULL);
		rooms[2] = CreateWindow(L"BUTTON", L"Room 3", WS_BORDER | WS_CHILD | WS_VISIBLE, 410, 10, 190, 20, hWnd, (HMENU)6, NULL, NULL);
		rooms[3] = CreateWindow(L"BUTTON", L"Room 4", WS_BORDER | WS_CHILD | WS_VISIBLE, 610, 10, 190, 20, hWnd, (HMENU)7, NULL, NULL);
		rooms[4] = CreateWindow(L"BUTTON", L"Room 5", WS_BORDER | WS_CHILD | WS_VISIBLE, 810, 10, 190, 20, hWnd, (HMENU)8, NULL, NULL);

		break;
	case WM_COMMAND:

		if (HIWORD(wParam)) {
			res = DefWindowProc(hWnd, message, wParam, lParam);
		}
		else {
			mes = gcnew Message();
			for (int t = 0; t < 5; t++) {
				_ib = 0;
				if ((HWND)lParam == rooms[t]) {
					mes->type = SERVER_CHANGE_ROOM;
					mes->addition = (void*)t;
					_ib = 1;
				}
				if (_ib) {
					break;
				}
			}

			if (_ib) {
				ThreadPool::QueueUserWorkItem(gcnew WaitCallback((Client::self)->Send), mes);
				break;
			}

			if ((HWND)lParam == button) {

				mes->type = SERVER_MESSAGE;

				mes->msgLen = GetWindowTextLengthA(textBox2) + 1;

				mes->message = (char*)malloc(mes->msgLen);

				GetWindowTextA(textBox2, mes->message, mes->msgLen);

				SetWindowTextA(textBox2, "");

				res = FALSE;
			}
			else {
				SKIPDWP
			}
			ThreadPool::QueueUserWorkItem(gcnew WaitCallback((Client::self)->Send), mes);
		}

		res = DefWindowProc(hWnd, message, wParam, lParam);
		break;

		break;
	}

	return res;
}

LRESULT CALLBACK regProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char cb[512];
	LRESULT res;
	static HWND t1, t2, t3;
	static HWND reg, log, can;
	Message^ mes;
	int ib;

	switch (message)
	{
	default:
		SKIPDWP
			break;
	case WM_CLOSE:
		SKIPDWP
			break;
	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	case WM_CREATE:
		t1 = CreateWindow(L"EDIT", L"Name", WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 10, 500, 20, hWnd, (HMENU)1, NULL, NULL);
		t2 = CreateWindow(L"EDIT", L"Password", WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 40, 500, 20, hWnd, (HMENU)2, NULL, NULL);
		//t3 = CreateWindow(L"STATIC", L"", WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 40, 500, 20, hWnd, (HMENU)2, NULL, NULL);

		reg = CreateWindow(L"BUTTON", L"Register", WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 70, 60, 20, hWnd, (HMENU)4, NULL, NULL);
		log = CreateWindow(L"BUTTON", L"Login", WS_BORDER | WS_CHILD | WS_VISIBLE, 400, 70, 50, 20, hWnd, (HMENU)5, NULL, NULL);
		can = CreateWindow(L"BUTTON", L"Cancel", WS_BORDER | WS_CHILD | WS_VISIBLE, 460, 70, 50, 20, hWnd, (HMENU)6, NULL, NULL);

		res = NULL;

		break;
	case WM_COMMAND:

		if (HIWORD(wParam)) {
			res = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
		/*
		if ((HWND)lParam == can) {
			exit(0);
		}
		*/
		mes = gcnew Message();

		if ((HWND)lParam == reg) {
			mes->type = SERVER_REGISTER;
		}
		else if ((HWND)lParam == log) {
			mes->type = SERVER_LOGIN;
		}
		else {
			exit(-1);
		}

		GetWindowTextA(t1, cb, 128);

		GetWindowTextA(t2, &(cb[128]), 128);

		SetWindowTextA(t1, "");

		SetWindowTextA(t2, "");

		mes->addition = new MyStruct;

		memcpy(mes->addition, cb, 256);

		ThreadPool::QueueUserWorkItem(gcnew WaitCallback((Client::self)->Send), mes);

		res = NULL;
		break;
	}

	return res;
}

//LRESULT CALLBACK authProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
//	char cb[512];
//	LRESULT res;
//	HWND t1, t2, t3;
//	HWND reg, log, can;
//	Message^ mes;
//	int ib;
//
//	switch (message)
//	{
//	default:
//		SKIPDWP
//		break;
//	case WM_CLOSE:
//		SKIPDWP
//		break;
//	case WM_DESTROY:
//
//		PostQuitMessage(0);
//		break;
//	case WM_CREATE:
//		t1 = CreateWindow(L"EDIT", L"Name", WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 10, 500, 20, hWnd, (HMENU)1, NULL, NULL);
//		t2 = CreateWindow(L"EDIT", L"Password", WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 40, 500, 20, hWnd, (HMENU)2, NULL, NULL);
//		//t3 = CreateWindow(L"STATIC", L"", WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 40, 500, 20, hWnd, (HMENU)2, NULL, NULL);
//
//		reg = CreateWindow(L"BUTTON", L"Register", WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 70, 60, 20, hWnd, (HMENU)4, NULL, NULL);
//		log = CreateWindow(L"BUTTON", L"Login", WS_BORDER | WS_CHILD | WS_VISIBLE, 400, 70, 50, 20, hWnd, (HMENU)5, NULL, NULL);
//		can = CreateWindow(L"BUTTON", L"Cancel", WS_BORDER | WS_CHILD | WS_VISIBLE, 460, 70, 50, 20, hWnd, (HMENU)6, NULL, NULL);
//
//		res = NULL;
//
//		break;
//	case WM_COMMAND:
//
//		if (HIWORD(wParam)) {
//			res = DefWindowProc(hWnd, message, wParam, lParam);
//			break;
//		}
//		/*
//		if ((HWND)lParam == can) {
//			exit(0);
//		}
//		*/
//		mes = gcnew Message();
//
//		if ((HWND)lParam == reg) {
//			mes->type = SERVER_REGISTER;
//		}
//		else if ((HWND)lParam == log) {
//			mes->type = SERVER_LOGIN;
//		}
//		else {
//			exit(-1);
//		}
//
//		ib = GetWindowTextLengthA(t1) + 1;
//
//		if (ib > 128) {
//			ib = 128;
//		}
//
//		GetWindowTextA(t1, cb, 128);
//
//		ib = GetWindowTextLengthA(t2) + 1;
//
//		if (ib > 128) {
//			ib = 128;
//		}
//
//		GetWindowTextA(t2, &(cb[128]), 128);
//
//		mes->addition = new MyStruct;
//
//		memcpy(mes->addition, cb, 256);
//
//		ThreadPool::QueueUserWorkItem(gcnew WaitCallback(mes->exec), mes);
//
//		res = NULL;
//		break;
//	}
//
//	return res;
//}

//void Message::exec()
//{
//	char* cb;
//	int ib;
//	this->mm.WaitOne();
//
//	switch (this->type)
//	{
//	case SERVER_LOGIN:
//	case SERVER_REGISTER:
//		cb = (char*)malloc(sizeof(MyStruct) + 16);
//
//		this->cc.encrypt((BYTE*)cb, (char*)this->addition, 256);
//
//		ib = this->type;
//
//		send(this->soc, (char*)&ib, sizeof(int), 0);
//
//		send(this->soc, cb, sizeof(MyStruct) + 16, 0);
//
//		recv(this->soc, (char*)&ib, sizeof(int), 0);
//
//		if (ib != USER_OK) {
//
//			break;
//		}
//
//		recv(this->soc, (char*)&ib, sizeof(int), 0);
//		break;
//	case SERVER_LOGOUT:
//		ib = this->type;
//
//		send(this->soc, (char*)&ib, sizeof(int), 0);
//		
//		recv(this->soc, (char*)&ib, sizeof(int), 0);
//
//		if (ib != USER_OK) {
//			exit(-1);
//		}
//		break;
//	case SERVER_MESSAGE:
//		ib = this->type;
//		
//		send(this->soc, (char*)&ib, sizeof(int), 0);
//
//		ib = this->msgLen;
//
//		send(this->soc, (char*)&ib, sizeof(int), 0);
//
//		send(this->soc, this->message, ib, 0);
//
//		recv(this->soc, (char*)&ib, sizeof(int), 0);
//
//		if (ib != USER_OK) {
//			exit(-1);
//		}
//
//		break;
//	case SERVER_CHANGE_ROOM:
//		ib = this->type;
//
//		send(this->soc, (char*)&ib, sizeof(int), 0);
//
//		ib = (int)this->addition;
//
//		send(this->soc, (char*)&ib, sizeof(int), 0);
//
//		recv(this->soc, (char*)&ib, sizeof(int), 0);
//
//		if (ib != USER_OK) {
//
//			exit(-1);
//		}
//
//		break;
//	default:
//		break;
//	}
//
//	this->mm.ReleaseMutex();
//	//throw gcnew System::NotImplementedException();
//}

Client::Client()
{
	this->wnd = NULL;
	this->logWnd = NULL;
	this->hWnds = NULL;
	this->msg = NULL;

	this->s_send = NULL;
	this->s_recv = NULL;
	this->id = 0;
	this->room = -1;
	this->message = NULL;
	this->addition = NULL;

	if (this->self == nullptr) {
		this->self = this;
	}

	this->ts = nullptr;
	this->th = nullptr;
	this->rts = nullptr;
	this->rth = nullptr;

	//throw gcnew System::NotImplementedException();
}

Client::~Client()
{
	// ???
	//throw gcnew System::NotImplementedException();
}

void Client::Start()
{
	WSADATA wsad;
	addrinfo *ai, _ai;
	char lcb[256];
	char *lcb2;
	int ib;
	int _ib;
	
	this->hWnds = new HWND[6];
	this->msg = new MSG;
	this->message = new char[512];

	this->cc.initAES();

	if (FAILED(WSAStartup(0x0202, &wsad))) {
		throw 1;
	}

	

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

	this->s_send = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

	if (this->s_send == INVALID_SOCKET) {
		ib = WSAGetLastError();
		printf("socket rc: %d", ib);
		exit(-1);
	}

	if (SOCKET_ERROR == (connect(this->s_send, ai->ai_addr, (int)ai->ai_addrlen))) {
		printf("Internal error");
		ib = WSAGetLastError();
		printf("connect rc: %d", ib);
		exit(-1);
	}

	ib = SERVER_SET_CONNECTION;
	ib = send(this->s_send, (char*)&ib, sizeof(int), 0);

	//printf("%d\n");

	recv(this->s_send, (char*)&ib, sizeof(int), 0);

	if (ib != USER_OK) {
		exit(0);
	}

	//this->Work(nullptr);

	//throw gcnew System::NotImplementedException();
	//ThreadPool::QueueUserWorkItem(gcnew WaitCallback(this->Work));

	this->ts = gcnew ThreadStart(this, &Client::Work);
	this->th = gcnew Thread(ts);
	(this->th)->Start();
}

void Client::Stop()
{

	CloseWindow(this->wnd);
	closesocket(this->s_recv);
	closesocket(this->s_send);
	this->cc.Release();
	//throw gcnew System::NotImplementedException();
}

void Client::Work()
{
	//Client^ cl = Client::self;
	MSG _msg;
	BOOL bRet = 0;

	this->CreateWindowS();

	while (bRet = GetMessage(this->msg, this->logWnd, 0, 0))
	{
		::TranslateMessage(this->msg);
		::DispatchMessage(this->msg);

		this->mm.WaitOne();

		if (this->id != 0) {
			::CloseWindow(this->logWnd);
			this->mm.ReleaseMutex();
			break;
		}

		this->mm.ReleaseMutex();
	}

	this->rts = gcnew ThreadStart(this, &Client::Recv);
	this->rth = gcnew Thread(rts);
	(this->rth)->Start();
	//ThreadPool::QueueUserWorkItem(gcnew WaitCallback( Client::Recv));

	while (bRet = ::GetMessage(this->msg, this->wnd, 0, 0))
	{
		::TranslateMessage(this->msg);
		::DispatchMessage(this->msg);

		if (this->id == 0) {
			this->Stop();
		}

	}

}

void Client::CreateWindowS()
{
	const wchar_t* windowName = L"Client";
	const wchar_t *windowClassName = L"Client";
	const wchar_t* logName = L"Auth";
	const wchar_t* logClassName = L"Auth";
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HCURSOR hCur = LoadCursor(hInstance, IDC_ARROW);
	LOGBRUSH logBr = { BS_SOLID, RGB(0,0,0), 0 };
	HBRUSH hbrBackGr = CreateBrushIndirect(&logBr);
	WNDCLASSEXA wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = wndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = hCur;
	wc.hbrBackground = hbrBackGr;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = (LPCSTR)windowClassName;
	wc.hIconSm = NULL;

	RegisterClassExA(&wc);

	this->wnd = CreateWindowA(
		(LPCSTR)windowClassName,
		(LPCSTR)windowName,
		WS_OVERLAPPEDWINDOW,
		50,
		50,
		1280,
		800,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!this->wnd) {
		throw - 1;
	}

	ShowWindow(this->wnd, SW_SHOW);

	wc.lpfnWndProc = regProc;
	wc.lpszClassName = (LPCSTR)logClassName;

	RegisterClassExA(&wc);

	this->logWnd = CreateWindowA(
		(LPCSTR)logClassName,
		(LPCSTR)logName,
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		540,
		200,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!this->logWnd) {
		CloseWindow(this->wnd);
		throw - 1;
	}

	ShowWindow(this->logWnd, SW_SHOW);
}

void Client::Send(Object^ stateInfo)
{
	Message^ mes = dynamic_cast<Message^> (stateInfo);
	Client^ cl = Client::self;
	int ib;

	cl->mm.WaitOne();

	switch (mes->type)
	{
	default:
		break;
	case SERVER_LOGIN:
	case SERVER_REGISTER:
		ib = mes->type;

		send(cl->s_send, (char*)&ib, sizeof(int), 0);

		cl->cc.encrypt((BYTE*)cl->message, (char*)mes->addition, 256);

		delete mes->addition;

		send(cl->s_send, cl->message, 272, 0);

		recv(cl->s_send, (char*)&ib, sizeof(int), 0);

		if (ib % 2) {

			break;
		}

		recv(cl->s_send, (char*)&ib, sizeof(int), 0);

		cl->id = ib;

		break;

	case SERVER_LOGOUT:

		ib = mes->type;

		send(cl->s_send, (char*)&ib, sizeof(int), 0);

		recv(cl->s_send, (char*)&ib, sizeof(int), 0);

		if (ib != USER_OK) {

			break;
		}

	case SERVER_MESSAGE:

		ib = mes->type;

		send(cl->s_send, (char*)&ib, sizeof(int), 0);

		ib = mes->msgLen;

		send(cl->s_send, (char*)&ib, sizeof(int), 0);

		send(cl->s_send, mes->message, ib, 0);

		free(mes->message);

		recv(cl->s_send, (char*)&ib, sizeof(int), 0);

		if (ib != USER_OK) {
			// error
		}

		break;

	case SERVER_CHANGE_ROOM:

		ib = mes->type;

		send(cl->s_send, (char*)&ib, sizeof(int), 0);

		ib = (int)mes->addition;

		send(cl->s_send, (char*)&ib, sizeof(int), 0);

		recv(cl->s_send, (char*)&ib, sizeof(int), 0);

		if (ib != USER_OK) {

			break;
		}

		if (cl->room == (int)mes->addition) {
			cl->room = -1;
		}
		else {
			cl->room = (int)mes->addition;
		}

	}

	cl->mm.ReleaseMutex();
}

void Client::Recv()
{
	//Client^ cl = Client::self;
	addrinfo *ai, _ai;
	char lcb[256];
	char *lcb2;
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

	this->s_recv = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

	if (this->s_recv == INVALID_SOCKET) {
		ib = WSAGetLastError();
		printf("socket rc: %d", ib);
		exit(-1);
	}

	if (SOCKET_ERROR == (connect(this->s_recv, ai->ai_addr, (int)ai->ai_addrlen))) {
		printf("Internal error");
		ib = WSAGetLastError();
		printf("connect rc: %d", ib);
		exit(-1);
	}

	ib = SERVER_ADD_CONNECTION;
	send(this->s_recv, (char*)&ib, sizeof(int), 0);
	ib = this->id;
	send(this->s_recv, (char*)&ib, sizeof(int), 0);

	recv(this->s_recv, (char*)&ib, sizeof(int), 0);

	if (ib != USER_OK) {
		printf("server denay add connection");
	}

	while (true)
	{
		_ib = recv(this->s_recv, (char*)&ib, sizeof(int), 0);
		if (_ib == 0) {
			closesocket(this->s_recv);
			this->s_recv = 0;
			return;
		}
		if (_ib == -1) {
			closesocket(this->s_recv);
			this->s_recv = 0;
			return;
		}
		recv(this->s_recv, lcb, ib, 0);
		//SendMessage(textBox1, EM_SETSEL, -1, -1);
		//SendMessage(textBox1, EM_REPLACESEL, 0, (LPARAM)lcb);
		_ib = GetWindowTextLengthA(this->hWnds[0]);

		lcb2 = (char*)malloc(ib + _ib);

		GetWindowTextA(this->hWnds[0], lcb2, _ib);

		if (_ib > 0) {
			lcb2[_ib - 1] = '\n';
		}

		memcpy(&(lcb2[_ib]), lcb, ib);

		SetWindowTextA(this->hWnds[0], lcb2);

		free(lcb2);

		printf(lcb);
	}

}

