#include "GameView.h"
#include <GL\glut.h>
#include <string>
#include "..\SelectionTools.h"
#include "..\UI\UIListBox.h"
#include "..\UI\UICheckBox.h"
#include "..\UI\UIEdit.h"
#include "..\CommandHandler.h"

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

void CGameView::CreateSpaceMarine()
{
	CCommandHandler handler;
	CUIListBox * listbox = (CUIListBox *)m_ui.GetChildByName("ListBox1");
	handler.AddNewCreateObject(listbox->GetSelectedItem(), 0.0, 0.0, 0.0);
}

void NewSpaceMarine()
{
	CGameView::GetIntanse().lock()->CreateSpaceMarine();
}

void DeleteObject()
{
	CGameModel::GetIntanse().lock()->DeleteSelectedObject();
}

void RollXDX()
{
	CGameView::GetIntanse().lock()->RollDices();
}

void SetDicePanelVisibility()
{
	CGameView::GetIntanse().lock()->DisplayDicePanel();
}

void CGameView::RollDices()
{
	CUIListBox * listbox = (CUIListBox *)m_ui.GetChildByName("Panel1")->GetChildByName("ListBox2");
	CUIEdit * edit = (CUIEdit *)m_ui.GetChildByName("Panel1")->GetChildByName("Edit1");
	CUICheckBox * checkbox = (CUICheckBox *)m_ui.GetChildByName("Panel1")->GetChildByName("CheckBox1");
	RollDice(atoi(edit->GetText().c_str()), atoi(listbox->GetSelectedItem().c_str()), checkbox->GetState());
}

void CGameView::DisplayDicePanel()
{
	m_ui.GetChildByName("Panel1")->SetVisible(!m_ui.GetChildByName("Panel1")->GetVisible());
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
	vector<string> items;;
	items.push_back("SpaceMarine.obj");
	items.push_back("CSM.obj");
	items.push_back("rhino.obj");
	m_ui.AddNewListBox("ListBox1", 10, 10, 30, 200, items);
	m_ui.AddNewButton("Button1", 220, 10, 30, 80, "Create", NewSpaceMarine);
	m_ui.AddNewButton("Button2", 310, 10, 30, 80, "Delete", DeleteObject);
	m_ui.AddNewButton("Button3", 400, 10, 30, 100, "Roll Dices", SetDicePanelVisibility);
	m_ui.AddNewButton("Button4", 510, 10, 30, 80, "Ruler", Ruler);

	IUIElement * panel = m_ui.AddNewPanel("Panel1", 390, 40, 160, 120);
	panel->SetVisible(false);
	panel->AddNewStaticText("Label1", 5, 10, 30, 50, "Count");
	panel->AddNewStaticText("Label2", 5, 50, 30, 50, "Faces");
	panel->AddNewButton("Button5", 30, 120, 30, 60, "Roll", RollXDX);
	panel->AddNewCheckBox("CheckBox1", 5, 80, 30, 100, "Group", false);
	panel->AddNewEdit("Edit1", 65, 10, 30, 50, "1");
	items.clear();
	items.push_back("6");
	items.push_back("3");
	items.push_back("12");
	items.push_back("20");
	panel->AddNewListBox("ListBox2", 65, 50, 30, 50, items);
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
	m_ui.Draw();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void CGameView::Update()
{
	glEnable(GL_TEXTURE_2D);
	m_camera.Update();
	m_skybox->Draw();
	m_table->Draw();
	glEnable(GL_DEPTH_TEST);
	DrawObjects();
	const IObject * object = m_gameModel.lock()->GetSelectedObject().get();
	if(m_gameModel.lock()->GetSelectedObjectModel() != "") m_modelManager.GetBoundingBox(m_gameModel.lock()->GetSelectedObjectModel())->Draw(
		object->GetX(), object->GetY(), object->GetZ(), object->GetRotation());
	glDisable(GL_DEPTH_TEST);
	m_ruler.Draw();
	DrawUI();
}

CGameView::~CGameView(void)
{
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
		m_modelManager.DrawModel(object->GetPathToModel());
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
	long selectedObject = -1;
	double minDistance = 10000000.0;
	CGameModel * model = CGameModel::GetIntanse().lock().get();
	double start[3];
	double end[3];
	WindowCoordsToWorldVector(x, y, start[0], start[1], start[2], end[0], end[1], end[2]);
	for(unsigned long i = 0; i < model->GetObjectCount(); ++i)
	{
		shared_ptr<const IObject> object = model->Get3DObject(i);
		double direction[3] = {end[0] - start[0], end[1] - start[1], end[2] - start[2]};
		if(m_modelManager.GetBoundingBox(object->GetPathToModel())->IsIntersectsRay(start, direction, object->GetX(), object->GetY(), object->GetZ(), object->GetRotation()))
		{
			double distance = sqrt(object->GetX() * object->GetX() + object->GetY() * object->GetY() + 
				object->GetZ() * object->GetZ());
			if(distance < minDistance)
			{
				selectedObject = i;
				minDistance = distance;
			}
		}
	}
	m_gameModel.lock()->SelectObjectByIndex(selectedObject);
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
	double worldX, worldY;
	WindowCoordsToWorldCoords(x, y, worldX, worldY);
	if(worldX > m_table.get()->GetWidth() / 2 || worldX < -m_table.get()->GetWidth() / 2 || worldY > m_table.get()->GetHeight() / 2 || worldY < -m_table.get()->GetHeight() / 2) return;
	m_gameModel.lock()->MoveSelectedObjectTo(worldX, worldY);
}

bool CGameView::UILeftMouseButtonDown(int x, int y)
{
	return m_ui.LeftMouseButtonDown(x, y);
}

bool CGameView::UILeftMouseButtonUp(int x, int y)
{
	 return m_ui.LeftMouseButtonUp(x, y);
}

bool CGameView::UIKeyPress(unsigned char key)
{
	return m_ui.OnKeyPress(key);
}

bool CGameView::UISpecialKeyPress(int key)
{
	return m_ui.OnSpecialKeyPress(key);
}