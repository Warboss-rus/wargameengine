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
#include "..\LUA\LUAScriptHandler.h"

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
	std::shared_ptr<IUIElement> m_ui;
	std::shared_ptr<CLUAScript> m_lua;

	CGameView(void);
	CGameView(CGameView const&){};
	CGameView& operator=(const CGameView&){};
	void DrawUI() const;
public:
	static std::weak_ptr<CGameView> GetIntanse();
	void Update();
	void DrawObjects(void);
	void DrawBoundingBox();
	void Init();

	void CreateTable(float width, float height, std::string const& texture);
	void CreateSkybox(double size, std::string const& textureFolder);
	void SetUI(IUIElement * ui);
	IUIElement * GetUI() const;

	void SelectObject(int x, int y);
	void SelectObjectGroup(int beginX, int beginY, int endX, int endY);
	void CameraSetLimits(double maxTransX, double maxTransY, double maxScale, double minScale);
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
	
	static void OnDrawScene();
	static void OnReshape(int width, int height);
	static void OnTimer(int value);

	static void FreeInstance();
};