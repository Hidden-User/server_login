#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include "client.h"



#include <WinSock2.h>
#include <ws2tcpip.h>


#pragma comment (lib, "ws2_32.lib")

const char server_addr[] = "127.0.0.1";
const char server_port[] = "49999";
const char uc[] = "It is not a command!";
char buffer[1024];

HANDLE g_m;
HANDLE v_users[10000];
DWORD v_thr_id[10000];

HWND hWnd;
HWND textBox1;
HWND textBox2;
HWND button;
HWND rooms[5];
HINSTANCE hInstance;
MSG msg;

const wchar_t* windowName = L"Client";
const wchar_t *windowClassName = L"Client";

WSADATA wsad;
SOCKET soc;
int my_id = 0;
int my_id2 = 0;
int curr_room = -1;
sockaddr_in soc_addr;

CryptoContainer cc;

DWORD WINAPI listen(LPVOID);
DWORD WINAPI listen2(LPVOID);
DWORD WINAPI virtualUser(LPVOID);
unsigned v_id;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void createWindow() {
	hInstance = GetModuleHandle(NULL);
	HCURSOR hCur = LoadCursor(hInstance, IDC_ARROW);

	LOGBRUSH logBr = { BS_SOLID, RGB(0,0,0), 0 };
	HBRUSH hbrBackGr = CreateBrushIndirect(&logBr);

	WNDCLASSEXA wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = WndProc;
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

	hWnd = CreateWindowA(
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

	if (!hWnd) {
		throw 0;
	}

	ShowWindow(hWnd, SW_SHOW);

}
DWORD WINAPI wndThread(LPVOID);
DWORD WINAPI sendToServer(LPVOID);

[System::STAThread]
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

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


int main()
{
	char cb[1024];
	MyStruct mst;
	HANDLE listener = NULL;
	DWORD thr_id;
	HANDLE window = NULL;
	DWORD window_id;
	int ib;
	addrinfo *ai, _ai;
	bool _work = false;

	window = CreateThread(NULL, 0, wndThread, NULL, NULL, &window_id);

	if (cryptoInitToAES(&cc)) {
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

	//for (int t = 0; t < 1500; t++) {
	//	v_users[t] = CreateThread(NULL, 1024, virtualUser, NULL, 0, &(v_thr_id[t]));
	//	
	//	Sleep(20);
	//
	//	if (v_users[t] == 0) {
	//		printf("Create thread error %d", GetLastError());
	//		return 1;
	//		//Sleep(10000);
	//	}
	//}
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

	if (SOCKET_ERROR == (connect, (soc, ai->ai_addr, (int)ai->ai_addrlen))) {
		printf("Internal error");
		ib = WSAGetLastError();
		printf("connect rc: %d", ib);
		Sleep(100000);
		return -1;
	}

	ib = SERVER_SET_CONNECTION;
	ib = send(soc, (char*)&ib, sizeof(int), 0);

	//printf("%d\n");

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
			sprintf(mst.pass, "%s", cb);
			//encryptPasswordMD5(mst.pass, cb, &cc);

			ib = SERVER_LOGIN;

			send(soc, (char*)&ib, sizeof(int), 0);

			//memcpy(buffer, &mst, sizeof(MyStruct));
			encryptSession((BYTE*)buffer, (char*)&mst, sizeof(MyStruct), &cc);
			send(soc, buffer, sizeof(MyStruct) + 16, 0);

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
			sprintf(mst.pass, "%s", cb);
			//encryptPasswordMD5(mst.pass, cb, &cc);

			ib = SERVER_REGISTER;

			send(soc, (char*)&ib, sizeof(int), 0);

			encryptSession((BYTE*)buffer, (char*)&mst, sizeof(MyStruct), &cc);
			send(soc, buffer, sizeof(MyStruct) + 16, 0);

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

	_soc = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

	if (_soc == INVALID_SOCKET) {
		ib = WSAGetLastError();
		printf("socket rc: %d", ib);
		exit(-1);
	}

	if (SOCKET_ERROR == (connect, (_soc, ai->ai_addr, (int)ai->ai_addrlen))) {
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
		printf("server denay add connection");
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
		//SendMessage(textBox1, EM_SETSEL, -1, -1);
		//SendMessage(textBox1, EM_REPLACESEL, 0, (LPARAM)lcb);
		_ib = GetWindowTextLengthA(textBox1);

		lcb2 = (char*)malloc(ib + _ib);

		GetWindowTextA(textBox1, lcb2, _ib);

		if (_ib > 0) {
			lcb2[_ib - 1] = '\n';
		}

		memcpy(&(lcb2[_ib]), lcb, ib);

		SetWindowTextA(textBox1, lcb2);

		free(lcb2);

		printf(lcb);
	}

}

DWORD WINAPI listen2(LPVOID) {
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

	if (SOCKET_ERROR == (connect, (_soc, ai->ai_addr, (int)ai->ai_addrlen))) {
		printf("Internal error");
		ib = WSAGetLastError();
		printf("connect rc: %d", ib);
		exit(-1);
	}

	ib = SERVER_ADD_CONNECTION;
	send(_soc, (char*)&ib, sizeof(int), 0);
	send(_soc, (char*)&my_id2, sizeof(int), 0);

	recv(_soc, (char*)&ib, sizeof(int), 0);

	if (ib != USER_OK) {
		printf("server denay add connection");
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
		printf(lcb);
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

	if (SOCKET_ERROR == (connect, (_soc, ai->ai_addr, (int)ai->ai_addrlen))) {
		printf("Internal error");
		ib = WSAGetLastError();
		printf("connect rc: %d", ib);
		Sleep(10000);
		exit(-1);
	}

	ib = SERVER_SET_CONNECTION;
	send(_soc, (char*)&ib, sizeof(int), 0);

	recv(_soc, (char*)&ib, sizeof(int), 0);

	if (ib != USER_OK) {

	}

	sprintf(mst.name, "user%d", v_id++);
	encryptPasswordMD5(mst.pass, mst.name, &cc);

	ib = SERVER_LOGIN;

	send(_soc, (char*)&ib, sizeof(int), 0);

	send(_soc, (char*)&mst, sizeof(MyStruct), 0);

	recv(_soc, (char*)&ib, sizeof(int), 0);

	if (ib == USER_LOGIN_SUCCES) {
		recv(_soc, (char*)&my_id2, sizeof(int), 0);
		listener = CreateThread(NULL, 1024, (LPTHREAD_START_ROUTINE)listen2, NULL, 0, &thr_id);
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

	Sleep(-1);

}

DWORD WINAPI wndThread(LPVOID) {
	BOOL bRet = 0;

	createWindow();

	while (bRet = GetMessage(&msg, nullptr, 0, 0))
	{
		if (-1 == bRet) break;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	exit(0);

	return 0;
}

#define SKIPDWP res = DefWindowProc(hWnd, message, wParam, lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//HWND button;
	char cb[1024];
	int ib;
	int _ib;
	LRESULT res = NULL;
	switch (message)
	{
	default:
		res = DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_CLOSE:
		SKIPDWP
			break;
	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	case WM_CREATE:
		textBox1 = CreateWindow(L"STATIC", L"", WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 60, 1000, 650, hWnd, (HMENU)1, NULL, NULL);
		textBox2 = CreateWindow(L"EDIT", L"", WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 710, 1000, 20, hWnd, (HMENU)2, NULL, NULL);

		button = CreateWindow(L"BUTTON", L"send", WS_VISIBLE | WS_CHILD | WS_BORDER, 1020, 710, 70, 20, hWnd, (HMENU)3, NULL, NULL);
		rooms[0] = CreateWindow(L"BUTTON", L"Room 1", WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 10, 190, 20, hWnd, (HMENU)4, NULL, NULL);
		rooms[1] = CreateWindow(L"BUTTON", L"Room 2", WS_BORDER | WS_CHILD | WS_VISIBLE, 210, 10, 190, 20, hWnd, (HMENU)5, NULL, NULL);
		rooms[2] = CreateWindow(L"BUTTON", L"Room 3", WS_BORDER | WS_CHILD | WS_VISIBLE, 410, 10, 190, 20, hWnd, (HMENU)6, NULL, NULL);
		rooms[3] = CreateWindow(L"BUTTON", L"Room 4", WS_BORDER | WS_CHILD | WS_VISIBLE, 610, 10, 190, 20, hWnd, (HMENU)7, NULL, NULL);
		rooms[4] = CreateWindow(L"BUTTON", L"Room 5", WS_BORDER | WS_CHILD | WS_VISIBLE, 810, 10, 190, 20, hWnd, (HMENU)8, NULL, NULL);
		break;

	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_RETURN:



			break;
		default:

			res = TRUE;

			break;
		}

		break;
	case WM_COMMAND:

		if (HIWORD(wParam)) {
			res = DefWindowProc(hWnd, message, wParam, lParam);
		}
		else {

			for (int t = 0; t < 5; t++) {
				_ib = 0;
				if ((HWND)lParam == rooms[t]) {
					ib = SERVER_CHANGE_ROOM;
					send(soc, (char*)&ib, sizeof(int), 0);
					ib = t;
					send(soc, (char*)&ib, sizeof(int), 0);
					recv(soc, (char*)&ib, sizeof(int), 0);
					if (ib != USER_OK) {

					}
					if (t == curr_room) {
						curr_room = -1;
					}
					else {
						curr_room = t;
					}
					_ib = 1;
				}
				if (_ib) {
					break;
				}
			}

			if (_ib) {
				break;
			}

			if ((HWND)lParam == button) {

				//ib = GetWindowTextLengthA(textBox2) + 1;
				//GetWindowTextA(textBox2, cb, ib);
				//
				//if (cb[0] == '/') {
				//	
				//}

				ib = SERVER_MESSAGE;

				send(soc, (char*)&ib, sizeof(int), 0);

				ib = GetWindowTextLengthA(textBox2) + 1;

				send(soc, (char*)&ib, sizeof(int), 0);

				GetWindowTextA(textBox2, cb, ib);

				send(soc, cb, ib, 0);

				recv(soc, (char*)&ib, sizeof(int), 0);

				SetWindowTextA(textBox2, "");

				if (ib != USER_OK) {

				}

				res = FALSE;
			}
			else {
				SKIPDWP
					//res = DefWindowProc(hWnd, message, wParam, lParam);
			}

			//if (hWnd == button) {
			//	res = FALSE;
			//}
			//else {
			//	res = DefWindowProc(hWnd, message, wParam, lParam);
			//}
		}

		res = DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return res;
}

DWORD WINAPI sendToServer(LPVOID) {
	return 0;
}
