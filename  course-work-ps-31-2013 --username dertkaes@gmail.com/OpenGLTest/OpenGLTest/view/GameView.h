#pragma once

#include <memory>
#include "../3dObject.h"
#include "ModelManager.h"
#include "../model/GameModel.h"
#include "Table.h"
#include "SkyBox.h"
#include "Camera.h"
#include "Input.h"
#include "..\Ruler.h"
#include "..\UI\UIElement.h"

class CGameView
{
private:
	std::shared_ptr<IObject> m_selectedObject;
	CModelManager m_modelManager;
	std::weak_ptr<CGameModel> m_gameModel;

	static std::shared_ptr<CGameView> m_instanse;
	std::shared_ptr<CTable> m_table;
	std::shared_ptr<CSkyBox> m_skybox;
	CCamera m_camera;
	CRuler m_ruler;
	CUIElement m_ui;

	CGameView(void);
	CGameView(CGameView const&){};
	CGameView& operator=(const CGameView&){};
	void DrawUI() const;
public:
	static std::weak_ptr<CGameView> GetIntanse();
	void Update();
	void DrawObjects(void);
	void Init();

	void SelectObject(int x, int y);
	void CameraZoomIn();
	void CameraZoomOut();
	void CameraRotate(int rotZ, int rotX);
	void CameraReset();
	void CameraTranslateLeft();
	void CameraTranslateRight();
	void CameraTranslateDown();
	void CameraTranslateUp();
	void RulerBegin(int x, int y);
	void RulerEnd(int x, int y);
	void RulerHide();
	void TryMoveSelectedObject(int x, int y);
	bool UILeftMouseButtonDown(int x, int y);
	bool UILeftMouseButtonUp(int x, int y);
	bool UIKeyPress(unsigned char key);
	bool UISpecialKeyPress(int key);
	void CreateSpaceMarine();
	void RollDices();
	void DisplayDicePanel();
	
	static void OnDrawScene();
	static void OnReshape(int width, int height);
	static void OnTimer(int value);

	static void FreeInstance();
};