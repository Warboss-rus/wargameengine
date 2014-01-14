#pragma once

#include <memory>
#include "../model/3dObject.h"
#include "ModelManager.h"
#include "../model/GameModel.h"
#include "Table.h"
#include "SkyBox.h"
#include "Camera.h"
#include "Input.h"
#include "..\Ruler.h"
#include "..\UI\UIElement.h"
#include "..\LUA\LUAScriptHandler.h"
#include "shader/OmniLight.h"

#include "shader/ShaderLoader.h"
#include "shader/ShaderCompiler.h"
#include "shader/ProgramLinker.h"
#include "shader/ProgramInfo.h"
#include "shader/Shaders.h"

class CGameView
{
private:
	std::shared_ptr<IObject> m_selectedObject;
	CVector3d m_selectedObjectCapturePoint;
	CModelManager m_modelManager;
	std::weak_ptr<CGameModel> m_gameModel;

	static std::shared_ptr<CGameView> m_instanse;
	std::shared_ptr<CTable> m_table;
	std::shared_ptr<CSkyBox> m_skybox;
	COmniLight m_light;
	CCamera m_camera;
	CRuler m_ruler;
	std::shared_ptr<IUIElement> m_ui;
	std::shared_ptr<CLUAScript> m_lua;

	CProgram m_program;

	CGameView(void);
	CGameView(CGameView const&){};
	CGameView& operator=(const CGameView&){};
	void DrawUI() const;
	void InitShaders();

	callback(m_selectionCallback);
	callback(m_updateCallback);
	callback(m_singleCallback);
public:
	static std::weak_ptr<CGameView> GetIntanse();
	void Update();
	void DrawObjects(void);
	void DrawBoundingBox();
	void Init();

	void CreateTable(float width, float height, std::string const& texture);
	void CreateSkybox(float size, std::string const& textureFolder);
	void SetUI(IUIElement * ui);
	IUIElement * GetUI() const;

	std::shared_ptr<IObject> GetNearestObject(int x, int y);
	void SelectObject(int x, int y, bool shiftPressed);
	void SelectObjectGroup(int beginX, int beginY, int endX, int endY);
	bool IsObjectInteresectSomeObjects(std::shared_ptr<IObject> object);
	void CameraSetLimits(float maxTransX, float maxTransY, float maxScale, float minScale);
	void CameraZoomIn();
	void CameraZoomOut();
	void CameraRotate(int rotZ, int rotX);
	void CameraReset();
	void CameraTranslateLeft();
	void CameraTranslateRight();
	void CameraTranslateDown();
	void CameraTranslateUp();
	void RulerBegin(float x, float y);
	void RulerEnd(float x, float y);
	void RulerHide();
	void TryMoveSelectedObject(int x, int y);
	bool UILeftMouseButtonDown(int x, int y);
	bool UILeftMouseButtonUp(int x, int y);
	bool UIKeyPress(unsigned char key);
	bool UISpecialKeyPress(int key);
	CModelManager* GetModelManager() { return &m_modelManager; }
	void ResizeWindow(int height, int width);

	void SetSelectionCallback(callback(onSelect));
	void SetUpdateCallback(callback(onUpdate));
	void SetSingleCallback(callback(onSingleUpdate));
	
	static void OnDrawScene();
	static void OnReshape(int width, int height);
	static void OnTimer(int value);

	static void FreeInstance();
};