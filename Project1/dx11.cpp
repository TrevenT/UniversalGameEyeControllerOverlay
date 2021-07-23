#include "dx11.h"
#include <stdexcept>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )

namespace DX
{
	Graphics Setup( int screenWidth, int screenHeight, HWND hwnd )
	{
		IDXGIFactory* factory;
		IDXGIAdapter* adapter;

		HRESULT result = CreateDXGIFactory( __uuidof( IDXGIFactory ), (void**)&factory );
		if ( FAILED( result ) )
		{
			throw std::runtime_error( "Failed to create dx factory!" );
		}

		result = factory->EnumAdapters( 0, &adapter );
		if ( FAILED( result ) )
		{
			throw std::runtime_error( "Failed to enum adapters!" );
		}

		Graphics gfx;

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory( &swapChainDesc, sizeof( swapChainDesc ) );
		swapChainDesc.BufferCount = 1; 
		swapChainDesc.BufferDesc.Width = screenWidth;
		swapChainDesc.BufferDesc.Height = screenHeight;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = true;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		result = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
			D3D11_SDK_VERSION, &swapChainDesc, &gfx.swapChain, &gfx.device, NULL, &gfx.context );

		if ( FAILED( result ) )
		{
			throw std::runtime_error( "Failed to create swap chain and device!" );
		}


		ID3D11Texture2D* pBackBuffer;
		gfx.swapChain->GetBuffer( 0, IID_PPV_ARGS( &pBackBuffer ) );

		if ( !pBackBuffer )
			throw std::runtime_error( "Backbuffer was NULL" );

		gfx.device->CreateRenderTargetView( pBackBuffer, NULL, &gfx.renderTargetView );
		pBackBuffer->Release( );

		return gfx;
	}

	void OnResize( Graphics& gfx, unsigned int w, unsigned int h )
	{
		if ( gfx.device != NULL )
		{
			if ( gfx.renderTargetView )
			{
				gfx.renderTargetView->Release( );
				gfx.renderTargetView = NULL;
			}

			gfx.swapChain->ResizeBuffers( 0, w, h, DXGI_FORMAT_UNKNOWN, 0 );

			ID3D11Texture2D* pBackBuffer;
			gfx.swapChain->GetBuffer( 0, IID_PPV_ARGS( &pBackBuffer ) );

			if ( !pBackBuffer )
				throw std::runtime_error( "Backbuffer was NULL" );

			gfx.device->CreateRenderTargetView( pBackBuffer, NULL, &gfx.renderTargetView );
			pBackBuffer->Release( );
		}
	}

	void ClearBackBuffer( Graphics& gfx )
	{
		const float clear_color_with_alpha[4] = { 0,0,0,0 };
		gfx.context->OMSetRenderTargets( 1, &gfx.renderTargetView, NULL );
		gfx.context->ClearRenderTargetView( gfx.renderTargetView, clear_color_with_alpha );
	}
	void Present( Graphics& gfx )
	{
		gfx.swapChain->Present( 1, 0 );
	}
}