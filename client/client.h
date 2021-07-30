#pragma once
#include "stdio.h"
#include "string.h"
#include "crypto.h"

int connect(int code);

int _send(int code, char ^msg);

LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
