#include "GameWindowDirectX.h"
#include "InputDirectX.h"
#include <Windows.h>
#include <d3d11.h>
#include <atlcomcli.h>
#include "DirectXRenderer.h"
#include <DirectXMath.h>

#pragma comment (lib, "d3d11.lib")

using namespace DirectX;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}

struct CGameWindowDirectX::Impl
{
public:
	Impl()
	{
		CreateMainWindow();

		InitDirect3D();

		m_renderer = std::make_unique<CDirectXRenderer>(m_dev, m_devcon, m_hWnd);
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
				// translate keystroke messages into the right format
				TranslateMessage(&msg);

				// send the message to the WindowProc function
				DispatchMessage(&msg);

				m_input->ProcessEvent(msg.message, msg.wParam, msg.lParam);

				// check to see if it's time to quit
				if (msg.message == WM_QUIT)
				{
					ReleaseDirect3D();
					return;
				}
			}

			//Render next frame
			m_onDraw();

			m_swapchain->Present(0, 0);
		}
	}

	void DoOnDrawScene(std::function<void() > const& handler)
	{
		m_onDraw = handler;
	}

	void DoOnResize(std::function<void(int, int) > const& handler)
	{
		m_onResize = handler;
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
		IDXGIOutput* pOutput;
		m_swapchain->GetFullscreenState(&fullscreen, &pOutput);
		m_swapchain->SetFullscreenState(!fullscreen, pOutput);
		pOutput->Release();
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
		wc.lpszClassName = "WindowClass1";

		// register the window class
		RegisterClassEx(&wc);

		RECT wr = { 0, 0, 600, 600 };    // set the size, but not the position
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

															  // create the window and use the result as the handle
		m_hWnd = CreateWindowEx(NULL,
			"WindowClass1",    // name of the window class
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
		scd.SampleDesc.Count = 1;                               // how many multisamples
		scd.Windowed = TRUE;                                    // windowed/full-screen mode
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		// create a device, device context and swap chain using the information in the scd struct
		D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			D3D11_CREATE_DEVICE_DEBUG,
			NULL,
			NULL,
			D3D11_SDK_VERSION,
			&scd,
			&m_swapchain,
			&m_dev,
			NULL,
			&m_devcon);

		// get the address of the back buffer
		ID3D11Texture2D *pBackBuffer;
		m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		ID3D11RenderTargetView* backBuffer;
		m_dev->CreateRenderTargetView(pBackBuffer, NULL, &backBuffer);
		pBackBuffer->Release();
		// set the render target as the back buffer
		m_devcon->OMSetRenderTargets(1, &backBuffer, NULL);
		backBuffer->Release();

		// Set the viewport
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = 600;
		viewport.Height = 600;
		viewport.MaxDepth = 1.0f;

		m_devcon->RSSetViewports(1, &viewport);
	}

	void ReleaseDirect3D()
	{
		m_swapchain->SetFullscreenState(FALSE, NULL);
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