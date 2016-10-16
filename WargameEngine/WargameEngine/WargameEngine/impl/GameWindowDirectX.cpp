#include "GameWindowDirectX.h"
#include "InputDirectX.h"
#include <Windows.h>
#include "DirectXRenderer.h"
#include "..\LogWriter.h"
#include <thread>

struct CGameWindowDirectX::Impl
{
	static Impl * g_instance;
public:
	Impl()
	{
		g_instance = this;
		CreateMainWindow();

		m_renderer = std::make_unique<CDirectXRenderer>(m_hWnd);
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
				m_renderer.reset();
				return;
			}

			m_input->UpdateControllers();

			//Render next frame
			m_onDraw();

			m_renderer->Present();

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

	void SetTitle(std::wstring const& title)
	{
		SetWindowTextW(m_hWnd, title.c_str());
	}

	void ToggleFullscreen()
	{
		m_renderer->ToggleFullscreen();
	}

	IInput& ResetInput()
	{
		m_input = std::make_unique<CInputDirectX>(m_hWnd);
		return *m_input;
	}

	void EnableMultisampling(bool enable, int level /*= 1.0f*/)
	{
		m_renderer->EnableMultisampling(enable, level);
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
		if (m_renderer)
		{
			m_renderer->OnResize(LOWORD(lParam), HIWORD(lParam));
		}
		if (m_onResize)
		{
			m_onResize(LOWORD(lParam), HIWORD(lParam));
		}
	}

	void GetWindowSize(int& width, int& height)
	{
		RECT rect;
		GetClientRect(m_hWnd, &rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
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

	std::unique_ptr<CInputDirectX> m_input;
	std::unique_ptr<CDirectXRenderer> m_renderer;
	std::function<void()> m_onDraw;
	std::function<void(int, int)> m_onResize;
	std::function<void()> m_onShutdown;

	HWND m_hWnd;
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

void CGameWindowDirectX::SetTitle(std::wstring const& title)
{
	m_pImpl->SetTitle(title);
}

void CGameWindowDirectX::ToggleFullscreen()
{
	m_pImpl->ToggleFullscreen();
}

bool CGameWindowDirectX::EnableVRMode(bool /*show*/, VRViewportFactory const&)
{
	throw std::runtime_error("GameWindowDirectX does not support VR mode, use GameWindowVR instead");
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

void CGameWindowDirectX::GetWindowSize(int& width, int& height)
{
	m_pImpl->GetWindowSize(width, height);
}
