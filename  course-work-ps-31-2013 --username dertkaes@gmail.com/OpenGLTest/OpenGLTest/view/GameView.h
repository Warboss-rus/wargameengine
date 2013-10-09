#pragma once

#include <memory>
#include "GlutInitializer.h"
#include "../3dObject.h"
#include "ModelManager.h"
#include "../model/GameModel.h"
#include "Table.h"
#include "SkyBox.h"
#include "Camera.h"
#include "Input.h"

class CGameView
{
private:
	std::shared_ptr<IObject> m_selectedObject;
	CModelManager m_modelManager;
	std::weak_ptr<CGameModel> m_gameModel;

	static std::shared_ptr<CGameView> m_instanse;
	std::shared_ptr<CTable> m_table;
	std::shared_ptr<CSkyBox> m_skybox;
	std::shared_ptr<CInput> m_input;
	CCamera m_camera;

	CGameView(void);
	CGameView(CGameView const&){};
	CGameView& operator=(const CGameView&){};
public:
	~CGameView(void);
	static std::weak_ptr<CGameView> GetIntanse();
	void Update();
	void DrawObjects(void);
	void Init();

	IObject* CGameView::SelectObject(int x, int y);
	void DrawSelectionBox(IObject * object);
	void CameraZoomIn();
	void CameraZoomOut();
	void CameraTranslate(double transX, double transY);
	void CameraRotate(double rotZ, double rotX);
	void CameraReset();
	void CameraTranslateLeft();
	void CameraTranslateRight();
	void CameraTranslateDown();
	void CameraTranslateUp();

	static void OnDrawScene();
	static void OnReshape(int width, int height);
	static void OnIdle();

	static void FreeInstance();
};

