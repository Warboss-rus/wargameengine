#include "Input.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "GameView.h"
#include "../Ruler.h"
#include "SelectionTools.h"
#include "../controller/GameController.h"

bool CInput::m_isLMBDown = false;
bool CInput::m_isRMBDown = false;
bool CInput::m_ruler = false;
double CInput::startX = 0.0;
double CInput::startY = 0.0;
double CInput::m_oldRotation = 0.0;
int CInput::startWindowX = -1;
int CInput::startWindowY = -1;
std::function<void(std::shared_ptr<IObject>, std::string const&, double, double, double)> CInput::m_LMBclickCallback;
std::function<void(std::shared_ptr<IObject>, std::string const&, double, double, double)> CInput::m_RMBclickCallback;
bool CInput::m_disableDefaultLMB = false;
bool CInput::m_disableDefaultRMB = false;

void SelectObject(int x, int y, bool shiftPressed, bool noCallback = false)
{
	double start[3];
	double end[3];
	WindowCoordsToWorldVector(x, y, start[0], start[1], start[2], end[0], end[1], end[2]);
	CGameController::GetInstance().lock()->SelectObject(start, end, shiftPressed, noCallback);
}

void CInput::OnMouse(int button, int state, int x, int y)
{
	std::shared_ptr<CGameModel> model = CGameModel::GetInstance().lock();
	switch(button)
	{
	case GLUT_LEFT_BUTTON: 
		if (state == GLUT_DOWN)
		{
			m_isLMBDown = true;
			if(CGameView::GetInstance().lock()->GetUI()->LeftMouseButtonDown(x, y)) return;
			if(m_ruler)
			{
				double worldX, worldY;
				WindowCoordsToWorldCoords(x, y, worldX, worldY);
				CRuler::SetBegin(worldX, worldY);
			}
			else
			{
				CRuler::Hide();
				if (!m_disableDefaultLMB)
				{
					SelectObject(x, y, glutGetModifiers() == GLUT_ACTIVE_SHIFT);
					std::shared_ptr<IObject> obj = model->GetSelectedObject();
					if (obj)//drag object
					{
						startX = obj->GetX();
						startY = obj->GetY();
						CRuler::SetBegin(startX, startY);
					}
					else//selection rectangle
					{
						startWindowX = x;
						startWindowY = y;
					}
				}
			}
		}
		else
		{
			if (!m_isLMBDown) break;
			m_isLMBDown = false;
			if (CGameView::GetInstance().lock()->GetUI()->LeftMouseButtonUp(x, y)) return;
			std::shared_ptr<IObject> obj = model->GetSelectedObject();
			if(!m_ruler && obj && !m_disableDefaultLMB)
			{
				double worldX, worldY;
				const CVector3d * capturePoint = CGameController::GetInstance().lock()->GetCapturePoint();
				WindowCoordsToWorldCoords(x, y, worldX, worldY, capturePoint->z);
				CGameController::GetInstance().lock()->TryMoveSelectedObject(obj, worldX, worldY, 0.0);
				double newX = obj->GetX();
				double newY = obj->GetY();
				CGameController::GetInstance().lock()->MoveObject(obj, newX - startX, newY - startY);
				startX = -1.0;
				startY = -1.0;
				CRuler::Hide();
			}
			if (!obj && !m_disableDefaultLMB)
			{
				//needs to be fixed
				double beginWorldX, beginWorldY, endWorldX, endWorldY;
				WindowCoordsToWorldCoords(startWindowX, startWindowY, beginWorldX, beginWorldY);
				WindowCoordsToWorldCoords(x, y, endWorldX, endWorldY);
				CGameController::GetInstance().lock()->SelectObjectGroup(beginWorldX, beginWorldY, endWorldX, endWorldY);
				startWindowX = -1;
				startWindowY = -1;
			}
			if (m_LMBclickCallback)
			{
				double worldX, worldY;
				WindowCoordsToWorldCoords(x, y, worldX, worldY);
				std::shared_ptr<IObject> prev = model->GetSelectedObject();
				SelectObject(x, y, false, true);
				m_LMBclickCallback(model->GetSelectedObject(), "Object", worldX, worldY, 0.0);
				model->SelectObject(prev);
			}
			m_ruler = false;
		}break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			m_isRMBDown = true;
			if (!m_disableDefaultRMB)
			{
				std::shared_ptr<IObject> prev = model->GetSelectedObject();
				SelectObject(x, y, false);
				WindowCoordsToWorldCoords(x, y, startX, startY);
				IObject * object = model->GetSelectedObject().get();
				if (!object) model->SelectObject(prev);
				m_oldRotation = (object) ? object->GetRotation() : 0;
			}
		}
		else
		{
			if (!m_isRMBDown) break;
			m_isRMBDown = false;
			std::shared_ptr<IObject> object = model->GetSelectedObject();
			if (object && !m_disableDefaultRMB)
			{
				double worldX, worldY;
				WindowCoordsToWorldCoords(x, y, worldX, worldY);
				double rot = model->GetSelectedObject()->GetRotation();
				double rotation = 90 + (atan2(worldY - startY, worldX - startX) * 180 / 3.1417);
				if (sqrt((worldX - startX) * (worldX - startX) + (worldY - startY) * (worldY - startY)) > 0.2)
					model->GetSelectedObject()->Rotate(rotation - rot);
				CGameController::GetInstance().lock()->RotateObject(object, object->GetRotation() - m_oldRotation);
			}
			if (m_RMBclickCallback)
			{
				double worldX, worldY;
				WindowCoordsToWorldCoords(x, y, worldX, worldY);
				std::shared_ptr<IObject> prev = model->GetSelectedObject();
				SelectObject(x, y, false, true);
				std::shared_ptr<IObject> current = model->GetSelectedObject();
				model->SelectObject(prev);
				m_RMBclickCallback(current, "Object", worldX, worldY, 0.0);
			}
			startX = 0;
			startY = 0;
		}break;
	case SCROLL_UP:
		if (state == GLUT_UP)
		{
			CGameView::GetInstance().lock()->GetCamera()->OnMouseWheelUp();
		}break;
	case SCROLL_DOWN:
		if (state == GLUT_UP)
		{
			CGameView::GetInstance().lock()->GetCamera()->OnMouseWheelDown();
		}break;
	}
}

bool HasModifier(int modifier)
{
	return (glutGetModifiers() & modifier) != 0;
}

void CInput::OnKeyboard(unsigned char key, int /*x*/, int /*y*/)
{
	if (CGameView::GetInstance().lock()->GetUI()->OnKeyPress(key) || CGameView::GetInstance().lock()->GetCamera()->OnKeyPress(key))
		return;
	if(key == 13 && glutGetModifiers() == GLUT_ACTIVE_ALT)
	{
		CGameView::GetInstance().lock()->ToggleFullscreen();
	}
	CGameController::GetInstance().lock()->OnKeyPress(key, HasModifier(GLUT_ACTIVE_SHIFT), HasModifier(GLUT_ACTIVE_CTRL), HasModifier(GLUT_ACTIVE_ALT));
}

int SpecialToKeyCode(int special)
{
	if(special >= GLUT_KEY_F1 && special <= GLUT_KEY_F12)
	{
		return 112 + special - GLUT_KEY_F1;
	}
	if(special >= GLUT_KEY_LEFT && special <= GLUT_KEY_DOWN)
	{
		return 37 + special - GLUT_KEY_LEFT;
	}
	if(special == GLUT_KEY_PAGE_UP || special == GLUT_KEY_PAGE_DOWN)
	{
		return 33 + special - GLUT_KEY_PAGE_UP;
	}
	if(special == GLUT_KEY_END)
	{
		return 35;
	}
	if (special == GLUT_KEY_HOME)
	{
		return 36;
	}
	if (special == GLUT_KEY_INSERT)
	{
		return 45;
	}
	return 0;
}

void CInput::OnSpecialKeyPress(int key, int /*x*/, int /*y*/)
{
	if (CGameView::GetInstance().lock()->GetUI()->OnSpecialKeyPress(key))
		return;
	int keycode = SpecialToKeyCode(key);
	if (CGameView::GetInstance().lock()->GetCamera()->OnKeyPress(keycode))
		return;
	CGameController::GetInstance().lock()->OnKeyPress(static_cast<unsigned char>(keycode), HasModifier(GLUT_ACTIVE_SHIFT), HasModifier(GLUT_ACTIVE_CTRL), HasModifier(GLUT_ACTIVE_ALT));
}

void CInput::OnPassiveMouseMove(int x, int y)
{
	static int prevMouseX = 320;
	static int prevMouseY = 240;
	static bool just_warped = false;
	if(just_warped) 
	{
        just_warped = false;
        return;
    }
	CGameView::GetInstance().lock()->GetCamera()->OnMouseMove(x - prevMouseX, prevMouseY - y, m_isLMBDown, m_isRMBDown, HasModifier(GLUT_ACTIVE_SHIFT), HasModifier(GLUT_ACTIVE_CTRL), HasModifier(GLUT_ACTIVE_ALT));
	if (CGameView::GetInstance().lock()->GetCamera()->HidePointer())
	{
		glutSetCursor(GLUT_CURSOR_NONE);
		glutWarpPointer(prevMouseX, prevMouseY);
		just_warped = true;
	}
	else
	{
		glutSetCursor(GLUT_CURSOR_INHERIT);
		prevMouseX = x;
		prevMouseY = y;
	}
}

void CInput::OnMouseMove(int x, int y)
{
	if (m_isLMBDown)
	{
		if(m_ruler)
		{
			double worldX, worldY;
			WindowCoordsToWorldCoords(x, y, worldX, worldY);
			CRuler::SetEnd(worldX, worldY);
		}
		else
		{
			std::shared_ptr<IObject> object = CGameModel::GetInstance().lock()->GetSelectedObject();
			if(object)
			{
				double worldX, worldY;
				const CVector3d * capturePoint = CGameController::GetInstance().lock()->GetCapturePoint();
				WindowCoordsToWorldCoords(x, y, worldX, worldY, capturePoint->z);
				CGameController::GetInstance().lock()->TryMoveSelectedObject(object, worldX, worldY, 0.0);
				CRuler::SetEnd(object->GetX(), object->GetY());
			}
		}
	}
	if(m_isRMBDown)
	{
		if(CGameModel::GetInstance().lock()->GetSelectedObject())
		{
			double worldX, worldY;
			WindowCoordsToWorldCoords(x, y, worldX, worldY);
			double rot = CGameModel::GetInstance().lock()->GetSelectedObject()->GetRotation();
			double rotation = 90 + (atan2(worldY-startY,worldX-startX)*180/3.1417);
			if(sqrt((worldX - startX) * (worldX - startX) + (worldY - startY) * (worldY - startY)) > 0.2)
				CGameModel::GetInstance().lock()->GetSelectedObject()->Rotate(rotation-rot);
		}
	}
}

void CInput::SetLMBCallback(MouseCallback const& callback, bool disableDefault)
{
	m_LMBclickCallback = callback;
	m_disableDefaultLMB = disableDefault;
}

void CInput::SetRMBCallback(MouseCallback const& callback, bool disableDefault)
{
	m_RMBclickCallback = callback;
	m_disableDefaultRMB = disableDefault;
}