#include "GameView.h"
#include <GL\glut.h>
#include "GlutInitializer.h"
#include <string>
#include "..\SelectionTools.h"
#include "..\UI.h"

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

void CreateSpaceMarine()
{
	std::shared_ptr<IObject> pObject(new C3DObject("SpaceMarine.obj", 0.0, 0.0, 0.0)); 
	CGameModel::GetIntanse().lock()->AddObject(pObject);
}

void DeleteObject()
{
	CGameModel::GetIntanse().lock()->DeleteSelectedObject();
}

void RollDices()
{
	RollDice(10, 6, false);
}

void Ruler()
{
	CInput::EnableRuler();
}

CGameView::CGameView(void)
{
	m_gameModel = CGameModel::GetIntanse();
	m_table.reset(new CTable(30.0f, 15.0f, "sand.bmp"));
	m_skybox.reset(new CSkyBox(0.0, 0.0, 0.0, 60.0, 60.0, 60.0, "skybox"));
	CUI::GetIntanse().lock()->CreateButton("Button1", 10, 10, 30, 100, "Create", CreateSpaceMarine);
	CUI::GetIntanse().lock()->CreateButton("Button2", 130, 10, 30, 100, "Delete", DeleteObject);
	CUI::GetIntanse().lock()->CreateButton("Button3", 250, 10, 30, 100, "Roll 10D6", RollDices);
	CUI::GetIntanse().lock()->CreateButton("Button4", 380, 10, 30, 100, "Ruler", Ruler);
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
	if(m_gameModel.lock()->GetSelectedObjectBoundingBox()) DrawSelectionBox(m_gameModel.lock()->GetSelectedObjectBoundingBox());
	m_ruler.Draw();
	CUI::GetIntanse().lock()->Draw();
}
CGameView::~CGameView(void)
{
}

void CGameView::DrawObjects(void)
{
	unsigned long countObjects = m_gameModel.lock()->GetObjectCount();
	for (unsigned long i = 0; i < countObjects; i++)
	{
		std::shared_ptr<const IObject> object = m_gameModel.lock()->Get3DObject(i);
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
	m_gameModel.lock()->TrySelectObject(start, end);
}

void CGameView::RulerBegin(int x, int y)
{
	m_ruler.SetBegin(x, y);
}

void CGameView::RulerEnd(int x, int y)
{
	m_ruler.SetEnd(x, y);
	OnDrawScene();
}

void CGameView::RulerHide()
{
	m_ruler.Hide();
}

void CGameView::TryMoveSelectedObject(int x, int y)
{
	double worldX, worldY;
	WindowCoordsToWorldCoords(x, y, worldX, worldY);
	if(worldX > m_table.get()->GetWidth() / 2 || worldX < -m_table.get()->GetWidth() / 2 || worldY > m_table.get()->GetHeight() / 2 || worldY < -m_table.get()->GetHeight() / 2) return;
	m_gameModel.lock()->MoveSelectedObjectTo(worldX, worldY);
}