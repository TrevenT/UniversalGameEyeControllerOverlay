#pragma once
#include <Windows.h>

namespace Window
{
	HWND Create( LONG width, LONG height, HINSTANCE& instance, LPCSTR title, WNDPROC winProc );
	void EnableClickThough( HWND hwnd, bool enabled );
	void Destroy( HWND& window, HINSTANCE& instance );
}