#include "GameView.h"
#include <GL\glut.h>
#include <string>
#include "..\SelectionTools.h"
#include "..\UI\UICheckBox.h"
#include "..\controller\CommandHandler.h"
#include "..\LUA\LUARegisterFunctions.h"
#include "..\LUA\LUARegisterUI.h"
#include "..\LUA\LUARegisterObject.h"
#include "..\ObjectGroup.h"

using namespace std;

shared_ptr<CGameView> CGameView::m_instanse = NULL;

weak_ptr<CGameView> CGameView::GetIntanse()
{
	if (!m_instanse.get())
	{
		m_instanse.reset(new CGameView());
		m_instanse->Init();
	}
	weak_ptr<CGameView> pView(m_instanse);

	return pView;
}

void CGameView::CreateTable(float width, float height, std::string const& texture)
{
	m_table.reset(new CTable(width, height, texture));
}

void CGameView::CreateSkybox(double size, std::string const& textureFolder)
{
	m_skybox.reset(new CSkyBox(size, size, size, textureFolder));
}

CGameView::CGameView(void)
{
	m_gameModel = CGameModel::GetIntanse();
	m_ui.reset(new CUIElement());
}

void CGameView::OnTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(10, OnTimer, 0);
}

void CGameView::Init()
{
	int argc = 0;
	char* argv[] = {""};
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("GLUT test");
	glEnable(GL_NORMALIZE);
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.01f);
	
	glutDisplayFunc(CGameView::OnDrawScene);
	glutTimerFunc(10, OnTimer, 0);
	glutReshapeFunc(&OnReshape);
	glutKeyboardFunc(&CInput::OnKeyboard);
	glutSpecialFunc(&CInput::OnSpecialKeyPress);
	glutMouseFunc(&CInput::OnMouse);
	glutMotionFunc(&CInput::OnMouseMove);
	glutPassiveMotionFunc(&CInput::OnPassiveMouseMove);
	glutMotionFunc(&CInput::OnMouseMove);

	m_lua.reset(new CLUAScript());
	RegisterFunctions(*m_lua.get());
	RegisterUI(*m_lua.get());
	RegisterObject(*m_lua.get());
	m_lua->RunScript("main.lua");
	
	glutMainLoop();
}

void CGameView::OnDrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	CGameView::GetIntanse().lock()->Update();
	glutSwapBuffers();
}

void CGameView::DrawUI() const
{
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT),0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	m_ui->Draw();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void CGameView::DrawBoundingBox()
{
	std::shared_ptr<IObject> object = m_gameModel.lock()->GetSelectedObject();
	if(object)
	{
		if (CGameModel::IsGroup(object))
		{
			CObjectGroup * group = (CObjectGroup *)object.get();
			for(unsigned int i = 0; i < group->GetCount(); ++i)
			{
				object = group->GetChild(i);
				m_modelManager.GetBoundingBox(object->GetPathToModel())->Draw(object->GetX(), 
					object->GetY(), object->GetZ(), object->GetRotation());
			}
		}
		else
		{
			m_modelManager.GetBoundingBox(object->GetPathToModel())->Draw(object->GetX(), 
				object->GetY(), object->GetZ(), object->GetRotation());
		}
	}
}

void CGameView::Update()
{
	m_camera.Update();
	if(m_skybox) m_skybox->Draw(m_camera.GetTranslationX(), m_camera.GetTranslationY(), 0, m_camera.GetScale());
	if(m_table) m_table->Draw();
	glEnable(GL_DEPTH_TEST);
	DrawObjects();
	glDisable(GL_DEPTH_TEST);
	DrawBoundingBox();
	m_ruler.Draw();
	DrawUI();
}

void CGameView::DrawObjects(void)
{
	unsigned long countObjects = m_gameModel.lock()->GetObjectCount();
	for (unsigned long i = 0; i < countObjects; i++)
	{
		shared_ptr<const IObject> object = m_gameModel.lock()->Get3DObject(i);
		glPushMatrix();
		glTranslated(object->GetX(), object->GetY(), 0);
		glRotated(object->GetRotation(), 0.0, 0.0, 1.0);
		m_modelManager.DrawModel(object->GetPathToModel(), object->GetHiddenMeshes());
		glPopMatrix();
	}
}

void CGameView::OnReshape(int width, int height) 
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble aspect = (GLdouble)glutGet(GLUT_WINDOW_WIDTH) / (GLdouble)glutGet(GLUT_WINDOW_HEIGHT);
	gluPerspective(60, aspect, 0.5, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

void CGameView::FreeInstance()
{
	m_instanse.reset();
}

void CGameView::CameraSetLimits(double maxTransX, double maxTransY, double maxScale, double minScale)
{
	m_camera.SetLimits(maxTransX, maxTransY, maxScale, minScale);
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

void CGameView::SelectObjectGroup(int beginX, int beginY, int endX, int endY)//Works only for Z = 0 plane and select object only if its center is within selection rectangle, needs better algorithm
{
	double beginWorldX, beginWorldY, endWorldX, endWorldY;
	WindowCoordsToWorldCoords(beginX, beginY, beginWorldX, beginWorldY);
	WindowCoordsToWorldCoords(endX, endY, endWorldX, endWorldY);
	double minX = (beginWorldX < endWorldX)?beginWorldX:endWorldX;
	double maxX = (beginWorldX > endWorldX)?beginWorldX:endWorldX;
	double minY = (beginWorldY < endWorldY)?beginWorldY:endWorldY;
	double maxY = (beginWorldY > endWorldY)?beginWorldY:endWorldY;
	CObjectGroup* group = new CObjectGroup();
	CGameModel * model = CGameModel::GetIntanse().lock().get();
	for(unsigned long i = 0; i < model->GetObjectCount(); ++i)
	{
		shared_ptr<IObject> object = model->Get3DObject(i);
		if(object->GetX() > minX && object->GetX() < maxX && object->GetY() > minY && object->GetY() < maxY)
		{
			group->AddChildren(object);
		}
	}
	switch(group->GetCount())
	{
	case 0:
		{
			model->SelectObject(NULL);
			delete group;
		}break;
	case 1:
		{
			model->SelectObject(group->GetChild(0));
			delete group;
		}break;
	default:
		{
			model->SelectObject(std::shared_ptr<IObject>(group));
		}break;
	}

}

void CGameView::SelectObject(int x, int y)
{
	shared_ptr<IObject> selectedObject = NULL;
	double minDistance = 10000000.0;
	CGameModel * model = CGameModel::GetIntanse().lock().get();
	double start[3];
	double end[3];
	WindowCoordsToWorldVector(x, y, start[0], start[1], start[2], end[0], end[1], end[2]);
	for(unsigned long i = 0; i < model->GetObjectCount(); ++i)
	{
		shared_ptr<IObject> object = model->Get3DObject(i);
		double direction[3] = {end[0] - start[0], end[1] - start[1], end[2] - start[2]};
		if(m_modelManager.GetBoundingBox(object->GetPathToModel())->IsIntersectsRay(start, direction, object->GetX(), object->GetY(), object->GetZ(), object->GetRotation()))
		{
			double distance = sqrt(object->GetX() * object->GetX() + object->GetY() * object->GetY() + 
				object->GetZ() * object->GetZ());
			if(distance < minDistance)
			{
				selectedObject = object;
				minDistance = distance;
			}
		}
	}
	if (selectedObject)
	{
		double worldX, worldY;
		WindowCoordsToWorldCoords(x, y, worldX, worldY);
		m_selectedObjectCapturePoint.x = worldX - selectedObject->GetX();
		m_selectedObjectCapturePoint.y = worldY - selectedObject->GetY();
	}
	m_gameModel.lock()->SelectObject(selectedObject);
}

void CGameView::RulerBegin(int x, int y)
{
	double worldX, worldY;
	WindowCoordsToWorldCoords(x, y, worldX, worldY);
	m_ruler.SetBegin(worldX, worldY);
}

void CGameView::RulerEnd(int x, int y)
{
	double worldX, worldY;
	WindowCoordsToWorldCoords(x, y, worldX, worldY);
	m_ruler.SetEnd(worldX, worldY);
}

void CGameView::RulerHide()
{
	m_ruler.Hide();
}

void CGameView::TryMoveSelectedObject(int x, int y)
{
	std::shared_ptr<IObject> object = m_gameModel.lock()->GetSelectedObject();
	if (!object)
	{
		return;
	}
	double worldX, worldY;
	WindowCoordsToWorldCoords(x, y, worldX, worldY);
	if (m_table->isCoordsOnTable(worldX, worldY))
	{
		object->Move(worldX - object->GetX() - m_selectedObjectCapturePoint.x, worldY - object->GetY() - m_selectedObjectCapturePoint.y, 0);
		//SetCursorPos(object->GetX(), object->GetY());
	}
}

bool CGameView::UILeftMouseButtonDown(int x, int y)
{
	return m_ui->LeftMouseButtonDown(x, y);
}

bool CGameView::UILeftMouseButtonUp(int x, int y)
{
	return m_ui->LeftMouseButtonUp(x, y);
}

bool CGameView::UIKeyPress(unsigned char key)
{
	return m_ui->OnKeyPress(key);
}

bool CGameView::UISpecialKeyPress(int key)
{
	return m_ui->OnSpecialKeyPress(key);
}

void CGameView::SetUI(IUIElement * ui)
{
	m_ui.reset(ui);
}

IUIElement * CGameView::GetUI() const
{
	return m_ui.get();
}