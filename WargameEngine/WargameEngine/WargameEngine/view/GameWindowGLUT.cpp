#include "GameWindowGLUT.h"
#include <GL/freeglut.h>
#include "InputGLUT.h"
#include "OpenGLRenderer.h"
#include <stdexcept>
#include "../Utils.h"
#include "../LogWriter.h"

static CGameWindowGLUT* g_instance = nullptr;
bool CGameWindowGLUT::m_visible = true;

void CGameWindowGLUT::OnTimer(int value)
{
	if (m_visible) glutPostRedisplay();
	glutTimerFunc(1, OnTimer, value);
}

void CGameWindowGLUT::OnChangeState(int state)
{
	m_visible = (state == GLUT_VISIBLE);
}

void CGameWindowGLUT::OnDrawScene()
{
	if (g_instance->m_onDraw)
	{
		g_instance->m_onDraw();
	}
	glutSwapBuffers();
}

void CGameWindowGLUT::OnReshape(int width, int height)
{
	if (g_instance->m_onResize)
	{
		g_instance->m_onResize(width, height);
	}
}

void CGameWindowGLUT::OnShutdown()
{
	if (g_instance->m_onShutdown)
	{
		g_instance->m_onShutdown();
	}
}

IInput& CGameWindowGLUT::ResetInput()
{
	m_input = std::make_unique<CInputGLUT>();
	return *m_input;
}

IRenderer& CGameWindowGLUT::GetRenderer()
{
	return *m_renderer;
}

IViewHelper& CGameWindowGLUT::GetViewHelper()
{
	return *m_renderer;
}

CGameWindowGLUT::CGameWindowGLUT()
{
	g_instance = this;
	int argc = 0;
	char* argv[] = { "" };
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(600, 600);
	glutCreateWindow("WargameEngine");
	

	glutDisplayFunc(&CGameWindowGLUT::OnDrawScene);
	glutTimerFunc(1, OnTimer, 0);
	glutReshapeFunc(&OnReshape);
	glutKeyboardFunc(&CInputGLUT::OnKeyboard);
	glutKeyboardUpFunc(&CInputGLUT::OnKeyboardUp);
	glutSpecialFunc(&CInputGLUT::OnSpecialKeyPress);
	glutSpecialUpFunc(&CInputGLUT::OnSpecialKeyRelease);
	glutMouseFunc(&CInputGLUT::OnMouse);
	glutMotionFunc(&CInputGLUT::MouseMoveCallback);
	glutPassiveMotionFunc(&CInputGLUT::OnPassiveMouseMove);
	glutMotionFunc(&CInputGLUT::MouseMoveCallback);
	glutCloseFunc(&CGameWindowGLUT::OnShutdown);
	glutWindowStatusFunc(OnChangeState);
	glutJoystickFunc(&CInputGLUT::OnJoystick, 10);

	m_renderer = std::make_unique<COpenGLRenderer>();
}

CGameWindowGLUT::~CGameWindowGLUT()
{
}

void CGameWindowGLUT::LaunchMainLoop()
{
	glutMainLoop();
}

void CGameWindowGLUT::DoOnDrawScene(std::function<void()> const& handler)
{
	m_onDraw = handler;
}

void CGameWindowGLUT::DoOnResize(std::function<void(int, int)> const& handler)
{
	m_onResize = handler;
}

void CGameWindowGLUT::DoOnShutdown(std::function<void()> const& handler)
{
	m_onShutdown = handler;
}

void CGameWindowGLUT::ResizeWindow(int width, int height)
{
	glutReshapeWindow(width, height);
}

void CGameWindowGLUT::SetTitle(std::wstring const& title)
{
	glutSetWindowTitle(WStringToUtf8(title).c_str());
}

void CGameWindowGLUT::ToggleFullscreen()
{
	glutFullScreenToggle();
}

void CGameWindowGLUT::EnableMultisampling(bool enable, int /*level*/)
{
	m_renderer->EnableMultisampling(enable);
}

void CGameWindowGLUT::EnableVRMode(bool /*show*/)
{
	LogWriter::WriteLine("GameWindowGLUT does not support VR mode, use GameWindowVR instead");
}

