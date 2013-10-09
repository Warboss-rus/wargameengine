#include "GameView.h"
#include <GL\glut.h>
#include "GlutInitializer.h"
#include <string>
#include "..\SelectionTools.h"

using namespace std;

std::shared_ptr<CGameView> CGameView::m_instanse = NULL;

std::weak_ptr<CGameView> CGameView::GetIntanse()
{
	if (!m_instanse.get())
	{
		m_instanse.reset(new CGameView());
		m_instanse->Init();
	}
	std::weak_ptr<CGameView> pView(m_instanse);

	return pView;
}


void CGameView::OnIdle()
{
	glutPostRedisplay();
}

CGameView::CGameView(void)
{
	m_gameModel = CGameModel::GetIntanse();
	m_table.reset(new CTable(30.0f, 15.0f, "sand.bmp"));
	m_skybox.reset(new CSkyBox(0.0, 0.0, 0.0, 60.0, 60.0, 60.0, "skybox"));
}

void CGameView::Init()
{
	int argc = 0;
	char* argv[] = {""};
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("GLUT test");
	glutDisplayFunc(&OnDrawScene);
	glutIdleFunc(&OnIdle);
	glutReshapeFunc(&OnReshape);
	glEnable(GL_NORMALIZE);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	m_input.reset(new CInput());
	glutMainLoop();
}

void CGameView::OnDrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CGameView::GetIntanse().lock()->Update();
	glutSwapBuffers();
}

void CGameView::Update()
{
	m_camera.Update();
	m_skybox->Draw();
	m_table->Draw();
	DrawObjects();
	if(m_selectedObject.get()) DrawSelectionBox(m_selectedObject.get());
	ruler.Draw();
}
CGameView::~CGameView(void)
{
}

void CGameView::DrawObjects(void)
{
	unsigned long countObjects = m_gameModel.lock()->GetObjectCount();
	for (unsigned long i = 0; i < countObjects; i++)
	{
		IObject const* object = m_gameModel.lock()->Get3DObject(i);
		glPushMatrix();
		glTranslated(object->GetX(), object->GetY(), 0);
		glRotated(object->GetRotation(), 0.0, 0.0, 1.0);
		m_modelManager.DrawModel(object->GetPathToModel());
		glPopMatrix();
	}
}

void CGameView::OnReshape(int width, int height) 
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble aspect = (GLdouble)width / (GLdouble)height;
	gluPerspective(60, aspect, 0.5, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

void CGameView::FreeInstance()
{
	m_instanse.reset();
}

void CGameView::CameraZoomIn()
{
	m_camera.ZoomIn();
}

void CGameView::CameraZoomOut()
{
	m_camera.ZoomOut();
}

void CGameView::CameraRotate(int rotZ, int rotX)
{
	m_camera.Rotate((double)rotZ / 10, (double)rotX / 5);
}

void CGameView::CameraReset()
{
	m_camera.Reset();
}

void CGameView::CameraTranslateLeft()
{
	m_camera.Translate(CCamera::TRANSLATE, 0.0);
}

void CGameView::CameraTranslateRight()
{
	m_camera.Translate(-CCamera::TRANSLATE, 0.0);
}

void CGameView::CameraTranslateDown()
{
	m_camera.Translate(0.0, CCamera::TRANSLATE);
}

void CGameView::CameraTranslateUp()
{
	m_camera.Translate(0.0, -CCamera::TRANSLATE);
}

void CGameView::SelectObject(int x, int y)
{
	double start[3];
	double end[3];
	WindowCoordsToWorldVector(x, y, start[0], start[1], start[2], end[0], end[1], end[2]);
	//m_selectedObject.reset(m_gameModel.lock()->Get3DObjectIntersectingRay(start, end));
}

void CGameView::RulerBegin(int x, int y)
{
	ruler.SetBegin(x, y);
}

void CGameView::RulerEnd(int x, int y)
{
	ruler.SetEnd(x, y);
	char str[10];
	double distance = ruler.GetDistance();
	sprintf(str, "%0.2f", distance);
	OnDrawScene();
	MessageBoxA(NULL, str, "Distance", 0);
	ruler.Hide();
}