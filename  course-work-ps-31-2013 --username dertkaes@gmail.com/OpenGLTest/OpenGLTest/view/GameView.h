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
#include "ShaderManager.h"

class CGameView
{
private:
	CVector3d m_selectedObjectCapturePoint;
	CModelManager m_modelManager;
	std::weak_ptr<CGameModel> m_gameModel;

	static std::shared_ptr<CGameView> m_instanse;
	std::shared_ptr<CTable> m_table;
	std::shared_ptr<CSkyBox> m_skybox;
	CCamera m_camera;
	CRuler m_ruler;
	std::shared_ptr<IUIElement> m_ui;
	std::shared_ptr<CLUAScript> m_lua;
	CShaderManager m_shader;

	CGameView(void);
	CGameView(CGameView const&){};
	CGameView& operator=(const CGameView&){};
	void DrawUI() const;

	callback(m_selectionCallback);
	callback(m_updateCallback);
	callback(m_singleCallback);
	bool m_vertexLightning;
public:
	static std::weak_ptr<CGameView> GetIntanse();
	~CGameView();

	void Update();
	void DrawObjects(void);
	void DrawBoundingBox();
	void Init();

	void CreateTable(float width, float height, std::string const& texture);
	void CreateSkybox(double size, std::string const& textureFolder);
	void SetUI(IUIElement * ui);
	IUIElement * GetUI() const;

	std::shared_ptr<IObject> GetNearestObject(int x, int y);
	void SelectObject(int x, int y, bool shiftPressed);
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
	void RulerBegin(double x, double y);
	void RulerEnd(double x, double y);
	void RulerHide();
	void TryMoveSelectedObject(int x, int y);
	bool UILeftMouseButtonDown(int x, int y);
	bool UILeftMouseButtonUp(int x, int y);
	bool UIKeyPress(unsigned char key);
	bool UISpecialKeyPress(int key);
	CModelManager* GetModelManager() { return &m_modelManager; }
	void ResizeWindow(int height, int width);
	void NewShaderProgram(std::string const& vertex = "", std::string const& fragment = "", std::string const& geometry = "");
	void EnableVertexLightning() { m_vertexLightning = true; }
	void DisableVertexLightning() { m_vertexLightning = false; }

	void SetSelectionCallback(callback(onSelect));
	void SetUpdateCallback(callback(onUpdate));
	void SetSingleCallback(callback(onSingleUpdate));
	
	static void OnDrawScene();
	static void OnReshape(int width, int height);
	static void OnTimer(int value);

	static void FreeInstance();
};