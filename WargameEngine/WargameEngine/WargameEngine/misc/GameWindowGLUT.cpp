#include "GameWindow.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Input.h"

static CGameWindow* g_instance = nullptr;
bool CGameWindow::m_visible = true;

void CGameWindow::OnTimer(int value)
{
	if (m_visible) glutPostRedisplay();
	glutTimerFunc(1, OnTimer, value);
}

void CGameWindow::OnChangeState(int state)
{
	m_visible = (state == GLUT_VISIBLE);
}

void CGameWindow::OnDrawScene()
{
	if (g_instance->m_onDraw)
	{
		g_instance->m_onDraw();
	}
	glutSwapBuffers();
}

void CGameWindow::OnReshape(int width, int height)
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

void CGameWindow::OnShutdown()
{
	if (g_instance->m_onShutdown)
	{
		g_instance->m_onShutdown;
	}
}

void CGameWindow::Init()
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

	glutDisplayFunc(&CGameWindow::OnDrawScene);
	glutTimerFunc(1, OnTimer, 0);
	glutReshapeFunc(&OnReshape);
	glutKeyboardFunc(&CInput::OnKeyboard);
	glutKeyboardUpFunc(&CInput::OnKeyboardUp);
	glutSpecialFunc(&CInput::OnSpecialKeyPress);
	glutSpecialUpFunc(&CInput::OnSpecialKeyRelease);
	glutMouseFunc(&CInput::OnMouse);
	glutMotionFunc(&CInput::OnMouseMove);
	glutPassiveMotionFunc(&CInput::OnPassiveMouseMove);
	glutMotionFunc(&CInput::OnMouseMove);
	glutCloseFunc(&CGameWindow::OnShutdown);
	glutWindowStatusFunc(OnChangeState);

	glewInit();

	glutMainLoop();
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
	glutReshapeWindow(width, height);
}

void CGameWindow::SetTitle(std::string const& title)
{
	glutSetWindowTitle(title.c_str());
}

void CGameWindow::ToggleFullscreen()
{
	glutFullScreenToggle();
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
		glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, -1, 1);
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

