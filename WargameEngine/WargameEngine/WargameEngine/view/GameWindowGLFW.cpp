#include <GL/glew.h>
#include "GameWindowGLFW.h"
#include "InputGLFW.h"

static CGameWindowGLFW* g_instance = nullptr;
bool CGameWindowGLFW::m_visible = true;

void CGameWindowGLFW::OnChangeState(GLFWwindow * /*window*/, int state)
{
	m_visible = (state == GLFW_VISIBLE);
}

void CGameWindowGLFW::OnReshape(GLFWwindow * /*window*/, int width, int height)
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

void CGameWindowGLFW::OnShutdown(GLFWwindow * window)
{
	if (g_instance->m_onShutdown)
	{
		g_instance->m_onShutdown;
	}
	glfwDestroyWindow(window);
	g_instance->m_window = nullptr;
}

void CGameWindowGLFW::Init()
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

void CGameWindowGLFW::CreateNewWindow(GLFWmonitor * monitor /*= NULL*/)
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

	glfwSetWindowSizeCallback(m_window, &OnReshape);
	glfwSetKeyCallback(m_window, &CInputGLFW::OnKeyboard);
	glfwSetCharCallback(m_window, &CInputGLFW::OnCharacter);
	glfwSetMouseButtonCallback(m_window, &CInputGLFW::OnMouse);
	glfwSetCursorPosCallback(m_window, &CInputGLFW::OnMouseMove);
	glfwSetScrollCallback(m_window, &CInputGLFW::OnScroll);
	glfwSetWindowCloseCallback(m_window, &CGameWindowGLFW::OnShutdown);
	glfwSetWindowIconifyCallback(m_window, &OnChangeState);
}

void CGameWindowGLFW::DoOnDrawScene(std::function<void()> const& handler)
{
	m_onDraw = handler;
}

void CGameWindowGLFW::DoOnResize(std::function<void(int, int)> const& handler)
{
	m_onResize = handler;
}

void CGameWindowGLFW::DoOnShutdown(std::function<void()> const& handler)
{
	m_onShutdown = handler;
}

void CGameWindowGLFW::ResizeWindow(int width, int height)
{
	glfwSetWindowSize(m_window, width, height);
}

void CGameWindowGLFW::SetTitle(std::string const& title)
{
	glfwSetWindowTitle(m_window, title.c_str());
}

void CGameWindowGLFW::ToggleFullscreen()
{
	CreateNewWindow(glfwGetPrimaryMonitor());
}

void CGameWindowGLFW::Enter2DMode()
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

void CGameWindowGLFW::Leave2DMode()
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

void CGameWindowGLFW::EnableMultisampling(bool enable, int level /*= 1.0f*/)
{
	if (enable)
		glEnable(GL_MULTISAMPLE);
	else
		glDisable(GL_MULTISAMPLE);
	glfwWindowHint(GLFW_SAMPLES, level);
}

IInput& CGameWindowGLFW::GetInput()
{
	return *m_input;
}

void CGameWindowGLFW::ResetInput()
{
	m_input = std::make_unique<CInputGLFW>(m_window);
}

