#include "window.h"
#include "dx11.h"
#include <stdexcept>
#include <imgui.h>
#include <imgui_internal.h>
#include "imgui/backends/imgui_impl_dx11.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "render.h"
#include "interactions.h"

DX::Graphics gfx;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
LRESULT WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if ( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) )
		return true;

	switch ( msg )
	{
		case WM_SIZE:
			if ( wParam != SIZE_MINIMIZED )
				DX::OnResize( gfx, (UINT)LOWORD( lParam ), (UINT)HIWORD( lParam ) );
			return 0;

		case WM_SYSCOMMAND:
			if ( ( wParam & 0xfff0 ) == SC_KEYMENU )
				return 0;
			break;

		case WM_DESTROY:
			::PostQuitMessage( 0 );
			return 0;

	}
	return ::DefWindowProc( hWnd, msg, wParam, lParam );
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	try
	{
		// make da window
		HWND hwnd = Window::Create( 1920, 1080, hInstance, "Test", WndProc );
		gfx = DX::Setup( 1920, 1080, hwnd );

		Render::Setup( hwnd, gfx );
		Interactions::SetupTobii( );

		MSG msg = { 0 };
		bool windowOpen = true;

		while ( WM_QUIT != msg.message )
		{
			if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			
			DX::ClearBackBuffer( gfx );
			Render::NewFrame( );
			ImGui::NewFrame( );
			Interactions::DoUI( );
			ImGui::Render( );
			Render::RenderFrame( );

			DX::Present( gfx );
			Interactions::UpdateTobii( );

			// hacky click though solution
			if ( GetAsyncKeyState( VK_HOME ) & 1 )
			{
				windowOpen = !windowOpen;
				Interactions::ShowWindow( windowOpen );
				Window::EnableClickThough( hwnd, windowOpen );
			}
		}
	}
	catch ( const std::exception& error )
	{
		MessageBoxA( NULL, error.what( ), "Runtime Error", 0 );
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}