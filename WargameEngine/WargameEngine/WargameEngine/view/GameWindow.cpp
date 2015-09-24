#include <GL/glew.h>
#include "GameWindow.h"
#include "Input.h"

static CGameWindow* g_instance = nullptr;
bool CGameWindow::m_visible = true;

void CGameWindow::OnChangeState(GLFWwindow * /*window*/, int state)
{
	m_visible = (state == GLFW_VISIBLE);
}

void CGameWindow::OnReshape(GLFWwindow * /*window*/, int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble aspect = (GLdouble)width / (GLdouble)height;
	gluPerspective(60, aspect, 0.5, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	if (g_instance->m_onResize)
	{
		g_instance->m_onResize(width, height);
	}
}

void CGameWindow::OnShutdown(GLFWwindow * window)
{
	if (g_instance->m_onShutdown)
	{
		g_instance->m_onShutdown;
	}
	glfwDestroyWindow(window);
	g_instance->m_window = nullptr;
}

void CGameWindow::Init()
{
	g_instance = this;

	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 16);

	CreateNewWindow();
	
	glewInit();

	//glfwSwapInterval(1);

	while (m_window && !glfwWindowShouldClose(m_window))
	{
		if (m_visible)
		{
			if (g_instance->m_onDraw)
			{
				g_instance->m_onDraw();
			}
			glfwSwapBuffers(g_instance->m_window);
		}
		glfwPollEvents();
	}
	glfwTerminate();
}

void CGameWindow::CreateNewWindow(GLFWmonitor * monitor /*= NULL*/)
{
	if (m_window)
	{
		glfwDestroyWindow(m_window);
	}
	m_window = glfwCreateWindow(600, 600, "WargameEngine", monitor, NULL);
	glfwMakeContextCurrent(m_window);
	//glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.01f);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetFramebufferSizeCallback(m_window, &OnReshape);
	glfwSetKeyCallback(m_window, &CInput::OnKeyboard);
	glfwSetCharCallback(m_window, &CInput::OnCharacter);
	glfwSetMouseButtonCallback(m_window, &CInput::OnMouse);
	glfwSetCursorPosCallback(m_window, &CInput::OnMouseMove);
	glfwSetScrollCallback(m_window, &CInput::OnScroll);
	glfwSetWindowCloseCallback(m_window, &CGameWindow::OnShutdown);
	glfwSetWindowIconifyCallback(m_window, &OnChangeState);
}

void CGameWindow::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CGameWindow::DoOnDrawScene(std::function<void()> const& handler)
{
	m_onDraw = handler;
}

void CGameWindow::DoOnResize(std::function<void(int, int)> const& handler)
{
	m_onResize = handler;
}

void CGameWindow::DoOnShutdown(std::function<void()> const& handler)
{
	m_onShutdown = handler;
}

void CGameWindow::ResizeWindow(int width, int height)
{
	glfwSetWindowSize(m_window, width, height);
}

void CGameWindow::SetTitle(std::string const& title)
{
	glfwSetWindowTitle(m_window, title.c_str());
}

void CGameWindow::ToggleFullscreen()
{
	CreateNewWindow(glfwGetPrimaryMonitor());
}

void CGameWindow::Enter2DMode()
{
	if (!m_2dMode)
	{
		m_2dMode = true;
		glEnable(GL_BLEND);
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);
		glOrtho(0, width, height, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
}

void CGameWindow::Leave2DMode()
{
	if (m_2dMode)
	{
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glDisable(GL_BLEND);
		m_2dMode = false;
	}
}

IInput& CGameWindow::GetInput()
{
	return *m_input;
}

void CGameWindow::ResetInput()
{
	m_input = std::make_unique<CInput>(m_window);
}

