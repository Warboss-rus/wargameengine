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
#include "..\Ruler.h"

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
	CRuler ruler;

	CGameView(void);
	CGameView(CGameView const&){};
	CGameView& operator=(const CGameView&){};
public:
	~CGameView(void);
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

	static void OnDrawScene();
	static void OnReshape(int width, int height);
	static void OnIdle();

	static void FreeInstance();
};

