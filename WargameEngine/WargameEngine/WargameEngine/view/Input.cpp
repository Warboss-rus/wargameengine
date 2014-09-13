#include "Input.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "GameView.h"
#include "../SelectionTools.h"
#include "../controller/CommandHandler.h"

bool CInput::m_isLMBDown = false;
bool CInput::m_isRMBDown = false;
bool CInput::m_ruler = false;
double CInput::startX = 0;
double CInput::startY = 0;
double CInput::m_oldRotation = 0;
std::map<CInput::sKeyBind, std::function<void()>> CInput::m_keyBindings;

void CInput::OnMouse(int button, int state, int x, int y)
{
	switch(button)
	{
	case GLUT_LEFT_BUTTON: 
		if (state == GLUT_DOWN)
		{
			m_isLMBDown = true;
			if(CGameView::GetInstance().lock()->UILeftMouseButtonDown(x, y)) return;
			if(m_ruler)
			{
				double worldX, worldY;
				WindowCoordsToWorldCoords(x, y, worldX, worldY);
				CGameView::GetInstance().lock()->RulerBegin(worldX, worldY);
			}
			else
			{
				CGameView::GetInstance().lock()->RulerHide();
				CGameView::GetInstance().lock()->SelectObject(x, y, glutGetModifiers() == GLUT_ACTIVE_SHIFT);
				std::shared_ptr<IObject> obj = CGameModel::GetInstance().lock()->GetSelectedObject();
				if(obj)//drag object
				{
					startX = obj->GetX();
					startY = obj->GetY();
					CGameView::GetInstance().lock()->RulerBegin(startX, startY);
				}
				else//selection rectangle
				{
					startX = x;
					startY = y;
				}
			}
		}
		else
		{
			if (!m_isLMBDown) break;
			m_isLMBDown = false;
			if(CGameView::GetInstance().lock()->UILeftMouseButtonUp(x, y)) return;
			std::shared_ptr<IObject> obj = CGameModel::GetInstance().lock()->GetSelectedObject();
			if(!m_ruler && obj)
			{
				double worldX, worldY;
				WindowCoordsToWorldCoords(x, y, worldX, worldY);
				CGameView::GetInstance().lock()->TryMoveSelectedObject(obj, x, y);
				CCommandHandler::GetInstance().lock()->AddNewMoveObject(obj, worldX - startX, worldY - startY);
				startX = -1;
				startY = -1;
				CGameView::GetInstance().lock()->RulerHide();
			}
			if(!obj)
			{
				CGameView::GetInstance().lock()->SelectObjectGroup(startX, startY, x, y);
			}
			m_ruler = false;
		}break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			m_isRMBDown = true;
			CGameView::GetInstance().lock()->SelectObject(x, y, false);
			WindowCoordsToWorldCoords(x, y, startX, startY);
			IObject * object = CGameModel::GetInstance().lock()->GetSelectedObject().get();
			m_oldRotation = (object)?object->GetRotation():0;
		}
		else
		{
			if (!m_isLMBDown) break;
			m_isRMBDown = false;
			std::shared_ptr<IObject> object = CGameModel::GetInstance().lock()->GetSelectedObject();
			if (object)
			{
				double worldX, worldY;
				WindowCoordsToWorldCoords(x, y, worldX, worldY);
				double rot = CGameModel::GetInstance().lock()->GetSelectedObject()->GetRotation();
				double rotation = 90 + (atan2(worldY - startY, worldX - startX) * 180 / 3.1417);
				if (sqrt((worldX - startX) * (worldX - startX) + (worldY - startY) * (worldY - startY)) > 0.2)
					CGameModel::GetInstance().lock()->GetSelectedObject()->Rotate(rotation - rot);
				CCommandHandler::GetInstance().lock()->AddNewRotateObject(object, object->GetRotation() - m_oldRotation);
			}
				
			startX = 0;
			startY = 0;
		}break;
	case SCROLL_UP:
		if (state == GLUT_UP)
		{
			CGameView::GetInstance().lock()->CameraZoomIn();
		}break;
	case SCROLL_DOWN:
		if (state == GLUT_UP)
		{
			CGameView::GetInstance().lock()->CameraZoomOut();
		}break;
	}
}

void CInput::OnKeyboard(unsigned char key, int x, int y)
{
	if(CGameView::GetInstance().lock()->UIKeyPress(key))
		return;
	switch(key)
	{
		case BACKSPACE_BUTTON_ID:
		{
			CGameView::GetInstance().lock()->CameraReset();
		} break;
		case 13:
		{
			if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			{
				CGameView::GetInstance().lock()->ToggleFullscreen();
			}
		}
	}
	sKeyBind keybind(key, glutGetModifiers() == GLUT_ACTIVE_SHIFT, glutGetModifiers() == GLUT_ACTIVE_CTRL, glutGetModifiers() == GLUT_ACTIVE_ALT);
	if(m_keyBindings.find(keybind) != m_keyBindings.end())
	{
		m_keyBindings[keybind]();
	}
}

void CInput::OnSpecialKeyPress(int key, int x, int y)
{
    if(CGameView::GetInstance().lock()->UISpecialKeyPress(key))
		return;
	switch (key) 
	{
		case GLUT_KEY_LEFT:
		{
			CGameView::GetInstance().lock()->CameraTranslateLeft();
		}
		break;
		case GLUT_KEY_RIGHT:
		{
			CGameView::GetInstance().lock()->CameraTranslateRight();
		}
		break;
		case GLUT_KEY_DOWN:
		{
			CGameView::GetInstance().lock()->CameraTranslateDown();
		}
		break;
		case GLUT_KEY_UP:
		{
			CGameView::GetInstance().lock()->CameraTranslateUp();
		}
		break;
	}
}

void CInput::OnPassiveMouseMove(int x, int y)
{
	static int prevMouseX;
	static int prevMouseY;
	static bool just_warped = false;
	if(just_warped) 
	{
        just_warped = false;
        return;
    }
	if(glutGetModifiers() == GLUT_ACTIVE_ALT)
	{
		glutSetCursor(GLUT_CURSOR_NONE);
		glutWarpPointer(prevMouseX, prevMouseY);
		CGameView::GetInstance().lock()->CameraRotate(x - prevMouseX, prevMouseY - y);
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
			CGameView::GetInstance().lock()->RulerEnd(worldX, worldY);
		}
		else
		{
			std::shared_ptr<IObject> object = CGameModel::GetInstance().lock()->GetSelectedObject();
			if(object)
			{
				CGameView::GetInstance().lock()->TryMoveSelectedObject(object, x, y);
				CGameView::GetInstance().lock()->RulerEnd(object->GetX(), object->GetY());
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

void CInput::BindKey(unsigned char key, bool shift, bool ctrl, bool alt, std::function<void()> const& func)
{
	sKeyBind keybind(key, shift, ctrl, alt);
	if(func)
	{
		m_keyBindings[keybind] = func;
	}
	else
	{
		if(m_keyBindings.find(keybind) != m_keyBindings.end())
		{
			m_keyBindings.erase(keybind);
		}
	}
}