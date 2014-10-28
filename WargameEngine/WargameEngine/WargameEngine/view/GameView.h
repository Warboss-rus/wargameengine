//#pragma once

#include <memory>
#include "../model/Object.h"
#include "ModelManager.h"
#include "../model/GameModel.h"
#include "Table.h"
#include "SkyBox.h"
#include "Camera.h"
#include "Input.h"
#include "../UI/UIElement.h"
#include "ShaderManager.h"
#include "../NetSocket.h"

class CGameView
{
private:
	CModelManager m_modelManager;
	std::weak_ptr<CGameModel> m_gameModel;

	static std::shared_ptr<CGameView> m_instanse;
	std::shared_ptr<CTable> m_table;
	std::shared_ptr<CSkyBox> m_skybox;
	CCamera m_camera;
	std::shared_ptr<IUIElement> m_ui;
	
	CShaderManager m_shader;
	std::shared_ptr<CNetSocket> m_socket;

	CGameView(void);
	CGameView(CGameView const&){};
	CGameView& operator=(const CGameView&){};

	callback(m_updateCallback);
	callback(m_singleCallback);
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

	void DrawUI() const;
	void Update();
	void DrawObjects(void);
	void DrawBoundingBox();
	void DrawShadowMap();
	void SetUpShadowMapDraw();
	void Init();
public:
	static std::weak_ptr<CGameView> GetInstance();
	~CGameView();

	void CreateTable(float width, float height, std::string const& texture);
	void CreateSkybox(double size, std::string const& textureFolder);
	IUIElement * GetUI() const;

	void SelectObject(int x, int y, bool shiftPressed);
	CCamera * GetCamera();
	void TryMoveSelectedObject(std::shared_ptr<IObject> object, int x, int y);
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
	void ToggleFullscreen() const;

	void DrawLine(double beginX, double beginY, double beginZ, double endX, double endY, double endZ, unsigned char colorR, unsigned char colorG, unsigned char colorB) const;
	void DrawLineLoop(double * points, unsigned int size, unsigned char colorR, unsigned char colorG, unsigned char colorB) const;
	void DrawText3D(double x, double y, double z, std::string const& text);
	
	void Save(std::string const& filename);
	void Load(std::string const& filename);

	void SetUpdateCallback(callback(onUpdate));
	void SetSingleCallback(callback(onSingleUpdate));
	
	static void OnDrawScene();
	static void OnReshape(int width, int height);
	static void OnTimer(int value);
	static void OnChangeState(int state);

	static void FreeInstance();
};