#include "GameWindowGLUT.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "InputGLUT.h"
#include "OpenGLRenderer.h"
#include <stdexcept>

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

void CGameWindowGLUT::OnShutdown()
{
	if (g_instance->m_onShutdown)
	{
		g_instance->m_onShutdown;
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
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.01f);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutDisplayFunc(&CGameWindowGLUT::OnDrawScene);
	glutTimerFunc(1, OnTimer, 0);
	glutReshapeFunc(&OnReshape);
	glutKeyboardFunc(&CInputGLUT::OnKeyboard);
	glutKeyboardUpFunc(&CInputGLUT::OnKeyboardUp);
	glutSpecialFunc(&CInputGLUT::OnSpecialKeyPress);
	glutSpecialUpFunc(&CInputGLUT::OnSpecialKeyRelease);
	glutMouseFunc(&CInputGLUT::OnMouse);
	glutMotionFunc(&CInputGLUT::OnMouseMove);
	glutPassiveMotionFunc(&CInputGLUT::OnPassiveMouseMove);
	glutMotionFunc(&CInputGLUT::OnMouseMove);
	glutCloseFunc(&CGameWindowGLUT::OnShutdown);
	glutWindowStatusFunc(OnChangeState);

	glewInit();

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

void CGameWindowGLUT::SetTitle(std::string const& title)
{
	glutSetWindowTitle(title.c_str());
}

void CGameWindowGLUT::ToggleFullscreen()
{
	glutFullScreenToggle();
}

void CGameWindowGLUT::EnableMultisampling(bool enable, int /*level*/)
{
	if (GLEW_ARB_multisample)
	{
		if (enable)
			glEnable(GL_MULTISAMPLE_ARB);
		else
			glDisable(GL_MULTISAMPLE_ARB);
	}
	else
	{
		throw std::runtime_error("MSAA is not supported");
	}
}

