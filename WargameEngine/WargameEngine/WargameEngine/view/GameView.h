//#pragma once

#include <memory>
#include "../model/3dObject.h"
#include "ModelManager.h"
#include "../model/GameModel.h"
#include "Table.h"
#include "SkyBox.h"
#include "Camera.h"
#include "Input.h"
#include "../Ruler.h"
#include "../UI/UIElement.h"
#include "../LUA/LUAScriptHandler.h"
#include "ShaderManager.h"
#include "../NetSocket.h"

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
	std::shared_ptr<CNetSocket> m_socket;

	CGameView(void);
	CGameView(CGameView const&){};
	CGameView& operator=(const CGameView&){};
	void DrawUI() const;

	callback(m_selectionCallback);
	callback(m_updateCallback);
	callback(m_singleCallback);
	callback(m_stateRecievedCallback);
	std::function<void(const char*)>m_stringRecievedCallback;
	bool m_vertexLightning;
	bool m_shadowMap;
	unsigned int m_shadowMapTexture;
	unsigned int m_shadowMapFBO;
	int m_shadowMapSize;
	float m_lightProjectionMatrix[16];
	float m_lightModelViewMatrix[16];
	float m_lightPosition[3];
	float m_shadowAngle;
	float m_anisoptropy;
	static bool m_visible;
	unsigned int m_updateTime;

	int m_netRecievedSize;
	int m_netTotalSize;
	char * m_netData;
public:
	static std::weak_ptr<CGameView> GetIntanse();
	~CGameView();

	void Update();
	void DrawObjects(void);
	void DrawBoundingBox();
	void DrawShadowMap();
	void SetUpShadowMapDraw();
	void Init();

	void CreateTable(float width, float height, std::string const& texture);
	void CreateSkybox(double size, std::string const& textureFolder);
	void SetUI(IUIElement * ui);
	IUIElement * GetUI() const;

	std::shared_ptr<IObject> GetNearestObject(int x, int y);
	void SelectObject(int x, int y, bool shiftPressed);
	void SelectObjectGroup(int beginX, int beginY, int endX, int endY);
	bool IsObjectInteresectSomeObjects(std::shared_ptr<IObject> object);
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
	void EnableVertexLightning();
	void DisableVertexLightning();
	void EnableShadowMap(int size, float angle);
	void DisableShadowMap();
	void SetLightPosition(int index, float* pos);
	void EnableMSAA() const;
	void DisableMSAA() const;
	static float GetMaxAnisotropy();
	float GetAnisotropyLevel() const;
	void SetAnisotropy(float maxAnisotropy);
	void ClearResources();
	void SetWindowTitle(std::string const& title) const;
	const CShaderManager * GetShaderManager() const;
	void Preload(std::string const& image);
	void LoadModule(std::string const& module);
	void ResetLUA();

	void NetHost(unsigned short port);
	void NetClient(std::string const& ip, unsigned short port);
	void NetSendMessage(std::string const& message);
	
	void Save(std::string const& filename);
	void Load(std::string const& filename);

	void SetSelectionCallback(callback(onSelect));
	void SetUpdateCallback(callback(onUpdate));
	void SetSingleCallback(callback(onSingleUpdate));
	void SetStateRecievedCallback(callback(onStateRecieved));
	void SetStringRecievedCallback(std::function<void(const char*)> onStringRecieved);
	
	static void OnDrawScene();
	static void OnReshape(int width, int height);
	static void OnTimer(int value);
	static void OnChangeState(int state);

	static void FreeInstance();
};