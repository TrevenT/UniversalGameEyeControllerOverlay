#include "window.h"
#include <stdexcept>

namespace Window
{
	constexpr const char* const WindowClassName = "WindowClassName";

	HWND Create( LONG width, LONG height, HINSTANCE& instance, LPCSTR title, WNDPROC wndProc )
	{
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof( WNDCLASSEX );
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = wndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = instance;
		wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
		wcex.hbrBackground = (HBRUSH)CreateSolidBrush( RGB( 0, 0, 0 ) );
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = WindowClassName;
		wcex.hIcon = nullptr;
		wcex.hIconSm = nullptr;

		if ( !RegisterClassExA( &wcex ) )
			throw std::runtime_error( "Failed to register window!" );

		// Get the desktop resolution
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow( );
		GetWindowRect( hDesktop, &desktop );

		// Create the window
		RECT rc = { 0, 0, width, height };
		HWND window = CreateWindowEx( WS_EX_TOPMOST | WS_EX_TRANSPARENT, wcex.lpszClassName, title, WS_POPUP, rc.left, rc.top, ( rc.right - rc.left ), ( rc.bottom - rc.top ), NULL, NULL, instance, NULL );
		
		if ( !window )
			throw std::runtime_error( "Failed to create window!" );

		// Show the window
		//SetWindowPos( window, HWND_TOPMOST, 0, 0, width, height, ( SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE ) & ~SWP_NOSIZE );
		SetLayeredWindowAttributes( window, RGB( 0, 0, 0 ), 255, ULW_COLORKEY | LWA_ALPHA );
		//SetWindowLong( window, GWL_EXSTYLE, GetWindowLong( window, GWL_EXSTYLE ) |   );
		ShowWindow( window, SW_SHOW );
		UpdateWindow( window );

		return window;
	}

	void EnableClickThough( HWND hwnd, bool enabled )
	{
		if ( enabled )
			SetWindowLong( hwnd, GWL_EXSTYLE, GetWindowLong( hwnd, GWL_EXSTYLE ) & ~( WS_EX_LAYERED ) );
		else				
			SetWindowLong( hwnd, GWL_EXSTYLE, GetWindowLong( hwnd, GWL_EXSTYLE ) | WS_EX_LAYERED );
	}

	void Destroy( HWND& window, HINSTANCE& instance )
	{
		DestroyWindow( window );
		window = NULL;

		UnregisterClassA( WindowClassName, instance );
	}
}