#include "GameWindowDirectX.h"
#include "InputDirectX.h"
#include <Windows.h>
#include <d3d11.h>
#include <atlcomcli.h>
#include "DirectXRenderer.h"
#include <DirectXMath.h>
#include "..\LogWriter.h"
#include <thread>

using namespace DirectX;

struct CGameWindowDirectX::Impl
{
	static Impl * g_instance;
public:
	Impl()
	{
		g_instance = this;
		CreateMainWindow();

		InitDirect3D();

		m_renderer = std::make_unique<CDirectXRenderer>(m_dev, m_devcon, m_hWnd);
	}

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		// sort through and find what code to run for the message given
		switch (message)
		{
			// this message is read when the window is closed
		case WM_DESTROY:
		{
			// close the application entirely
			PostQuitMessage(0);
			return 0;
		} break;
		case WM_SIZE:
		{
			g_instance->OnSize(lParam);
		}break;
		}

		if (g_instance->m_input)
		{
			if (g_instance->m_input->ProcessEvent(message, wParam, lParam))
			{
				return TRUE;
			}
		}

		// Handle any messages the switch statement didn't
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	void LaunchMainLoop()
	{
		// enter the main loop:

		// this struct holds Windows event messages
		MSG msg;

		while (true)
		{
			// Check to see if any messages are waiting in the queue
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (msg.message == WM_QUIT)
			{
				ReleaseDirect3D();
/*#ifdef _DEBUG
				CComPtr<ID3D11Debug> debugDev;
				m_dev->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDev));
				debugDev->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif*/
				return;
			}

			//Render next frame
			m_onDraw();

			m_swapchain->Present(0, 0);

			std::this_thread::yield();
		}
	}

	void DoOnDrawScene(std::function<void() > const& handler)
	{
		m_onDraw = handler;
	}

	void DoOnResize(std::function<void(int, int) > const& handler)
	{
		m_onResize = handler;
		m_onResize(600, 600);
	}

	void DoOnShutdown(std::function<void() > const& handler)
	{
		m_onShutdown = handler;
	}

	void ResizeWindow(int width, int height)
	{
		RECT wr = { 0, 0, width, height };
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
		::SetWindowPos(m_hWnd, NULL, 0, 0, wr.right - wr.left, wr.bottom - wr.top, SWP_NOZORDER | SWP_NOMOVE);
	}

	void SetTitle(std::string const& title)
	{
		SetWindowText(m_hWnd, title.c_str());
	}

	void ToggleFullscreen()
	{
		BOOL fullscreen;
		CComPtr<IDXGIOutput> pOutput;
		m_swapchain->GetFullscreenState(&fullscreen, &pOutput);
		m_swapchain->SetFullscreenState(!fullscreen, pOutput);
	}

	IInput& ResetInput()
	{
		m_input = std::make_unique<CInputDirectX>(m_hWnd);
		return *m_input;
	}

	void EnableMultisampling(bool enable, int level /*= 1.0f*/)
	{
		DXGI_SWAP_CHAIN_DESC scd;
		m_swapchain->GetDesc(&scd);
		scd.SampleDesc.Count = enable ? static_cast<UINT>(level) : 1;
		//Recreate swap chain?
	}

	IRenderer& GetRenderer()
	{
		return *m_renderer;
	}

	IViewHelper& GetViewHelper()
	{
		return *m_renderer;
	}

	void OnSize(LPARAM lParam)
	{
		if (m_swapchain)
		{
			m_swapchain->ResizeBuffers(1, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
		}
		if (m_dev)
		{
			CComPtr<ID3D11Texture2D> pBackBuffer;
			m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			CComPtr<ID3D11RenderTargetView> backBuffer;
			m_dev->CreateRenderTargetView(pBackBuffer, NULL, &backBuffer);
			CComPtr<ID3D11DepthStencilView> pDepthStencilView;
			CreateDepthBuffer(LOWORD(lParam), HIWORD(lParam), &pDepthStencilView);
			m_devcon->OMSetRenderTargets(1, &backBuffer.p, pDepthStencilView);
		}
		if (m_devcon)
		{
			D3D11_VIEWPORT viewport;
			unsigned int num = 1;
			m_devcon->RSGetViewports(&num, &viewport);
			viewport.Width = LOWORD(lParam);
			viewport.Height = HIWORD(lParam);
			m_devcon->RSSetViewports(num, &viewport);
		}
		if (m_renderer)
		{
			m_renderer->OnResize();
		}
		if (m_onResize)
		{
			m_onResize(LOWORD(lParam), HIWORD(lParam));
		}
	}
private:
	void CreateMainWindow()
	{
		// this struct holds information for the window class
		WNDCLASSEX wc;

		// clear out the window class for use
		ZeroMemory(&wc, sizeof(WNDCLASSEX));

		// fill in the struct with the needed information
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.lpszClassName = "WargameEngineWnd";

		// register the window class
		RegisterClassEx(&wc);

		RECT wr = { 0, 0, 600, 600 };    // set the size, but not the position
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

															  // create the window and use the result as the handle
		m_hWnd = CreateWindowEx(NULL,
			"WargameEngineWnd",    // name of the window class
			"WargameEngine",   // title of the window
			WS_OVERLAPPEDWINDOW,    // window style
			300,    // x-position of the window
			300,    // y-position of the window
			wr.right - wr.left,    // width of the window
			wr.bottom - wr.top,    // height of the window
			NULL,    // we have no parent window, NULL
			NULL,    // we aren't using menus, NULL
			NULL,    // application handle
			NULL);    // used with multiple windows, NULL

					  // display the window on the screen
		ShowWindow(m_hWnd, SW_SHOW);
	}

	void InitDirect3D()
	{
		DXGI_SWAP_CHAIN_DESC scd;

		// clear out the struct for use
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

		// fill the swap chain description struct
		scd.BufferCount = 1;                                    // one back buffer
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
		scd.OutputWindow = m_hWnd;                                // the window to be used
		scd.SampleDesc.Count = 4;                               // how many multisamples
		scd.Windowed = TRUE;                                    // windowed/full-screen mode
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		// create a device, device context and swap chain using the information in the scd struct
		HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
#ifdef _DEBUG
			D3D11_CREATE_DEVICE_DEBUG,
#else
			0,
#endif
			NULL,
			NULL,
			D3D11_SDK_VERSION,
			&scd,
			&m_swapchain,
			&m_dev,
			NULL,
			&m_devcon);

		if (FAILED(hr))
		{
			LogWriter::WriteLine("DirectX error: Cannot create Swapchain");
		}

		// get the address of the back buffer
		CComPtr<ID3D11Texture2D> pBackBuffer;
		m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		CComPtr<ID3D11RenderTargetView> backBuffer;
		hr = m_dev->CreateRenderTargetView(pBackBuffer, NULL, &backBuffer);
		if (FAILED(hr))
		{
			LogWriter::WriteLine("DirectX error: Cannot create backbuffer");
		}
		pBackBuffer = NULL;
		CComPtr<ID3D11DepthStencilView> pDepthStencilView;
		CreateDepthBuffer(600, 600, &pDepthStencilView);
		m_devcon->OMSetRenderTargets(1, &backBuffer.p, pDepthStencilView);
		
		backBuffer = NULL;
		pDepthStencilView = NULL;

		// Set the viewport
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.Width = 600;
		viewport.Height = 600;
		viewport.MaxDepth = 1.0f;

		m_devcon->RSSetViewports(1, &viewport);

		D3D11_RASTERIZER_DESC rasterizerState;
		ZeroMemory(&rasterizerState, sizeof(D3D11_RASTERIZER_DESC));

		rasterizerState.AntialiasedLineEnable = TRUE;
		rasterizerState.CullMode = D3D11_CULL_NONE; // D3D11_CULL_FRONT or D3D11_CULL_NONE D3D11_CULL_BACK
		rasterizerState.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_SOLID  D3D11_FILL_WIREFRAME
		rasterizerState.DepthBias = 0;
		rasterizerState.DepthBiasClamp = 0.0f;
		rasterizerState.DepthClipEnable = FALSE;
		rasterizerState.FrontCounterClockwise = FALSE;
		rasterizerState.MultisampleEnable = TRUE;
		rasterizerState.ScissorEnable = FALSE;
		rasterizerState.SlopeScaledDepthBias = 0.0f;

		CComPtr<ID3D11RasterizerState> pRasterState;
		hr = m_dev->CreateRasterizerState(&rasterizerState, &pRasterState);
		if (FAILED(hr))
		{
			LogWriter::WriteLine("DirectX error: Cannot create rasterizer state");
		}
		m_devcon->RSSetState(pRasterState);
	}

	void CreateDepthBuffer(unsigned int width, unsigned int height, ID3D11DepthStencilView ** buffer)
	{
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		depthBufferDesc.Width = width;
		depthBufferDesc.Height = height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 4;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		CComPtr<ID3D11Texture2D> pDepthStencilBuffer;
		m_dev->CreateTexture2D(&depthBufferDesc, NULL, &pDepthStencilBuffer);

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = depthBufferDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		
		m_dev->CreateDepthStencilView(pDepthStencilBuffer, &depthStencilViewDesc, buffer);
	}

	void ReleaseDirect3D()
	{
		m_swapchain->SetFullscreenState(FALSE, NULL);
		m_renderer.reset();
		m_devcon = NULL;
		m_swapchain = NULL;
	}

	std::unique_ptr<CInputDirectX> m_input;
	std::unique_ptr<CDirectXRenderer> m_renderer;
	std::function<void()> m_onDraw;
	std::function<void(int, int)> m_onResize;
	std::function<void()> m_onShutdown;

	HWND m_hWnd;
	CComPtr<IDXGISwapChain> m_swapchain;             // the pointer to the swap chain interface
	CComPtr<ID3D11Device> m_dev;                     // the pointer to our Direct3D device interface
	CComPtr<ID3D11DeviceContext> m_devcon;           // the pointer to our Direct3D device context
};

CGameWindowDirectX::Impl* CGameWindowDirectX::Impl::g_instance = nullptr;

CGameWindowDirectX::CGameWindowDirectX()
	:m_pImpl(std::make_unique<Impl>())
{
}

CGameWindowDirectX::~CGameWindowDirectX()
{
}

void CGameWindowDirectX::LaunchMainLoop()
{
	m_pImpl->LaunchMainLoop();
}

void CGameWindowDirectX::DoOnDrawScene(std::function<void() > const& handler)
{
	m_pImpl->DoOnDrawScene(handler);
}

void CGameWindowDirectX::DoOnResize(std::function<void(int, int) > const& handler)
{
	m_pImpl->DoOnResize(handler);
}

void CGameWindowDirectX::DoOnShutdown(std::function<void() > const& handler)
{
	m_pImpl->DoOnShutdown(handler);
}

void CGameWindowDirectX::ResizeWindow(int width, int height)
{
	m_pImpl->ResizeWindow(width, height);
}

void CGameWindowDirectX::SetTitle(std::string const& title)
{
	m_pImpl->SetTitle(title);
}

void CGameWindowDirectX::ToggleFullscreen()
{
	m_pImpl->ToggleFullscreen();
}

IInput& CGameWindowDirectX::ResetInput()
{
	return m_pImpl->ResetInput();
}

IRenderer& CGameWindowDirectX::GetRenderer()
{
	return m_pImpl->GetRenderer();
}

IViewHelper& CGameWindowDirectX::GetViewHelper()
{
	return m_pImpl->GetViewHelper();
}

void CGameWindowDirectX::EnableMultisampling(bool enable, int level /*= 1.0f*/)
{
	m_pImpl->EnableMultisampling(enable, level);
}