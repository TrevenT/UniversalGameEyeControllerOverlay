#pragma once
#include <Windows.h>
#include <d3d11.h>

namespace DX
{
	struct Graphics
	{
		ID3D11Device* device;
		ID3D11DeviceContext* context;
		IDXGISwapChain* swapChain;
		ID3D11RenderTargetView* renderTargetView;
	};

	Graphics Setup( int screenWidth, int screenHeight, HWND hwnd );
	void OnResize( Graphics&, unsigned int, unsigned int );
	void ClearBackBuffer( Graphics& gfx );
	void Present( Graphics& gfx );
}