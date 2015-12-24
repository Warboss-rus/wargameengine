#include <memory>
#include <functional>
#include "ICamera.h"
#include "IRenderer.h"
#include "ISoundPlayer.h"
#include "IInput.h"
#include "../UI/IUI.h"
#include "../controller/GameController.h"
#include "../model/GameModel.h"
#include "IGameWindow.h"
#include "IViewHelper.h"
#include "ITextWriter.h"
#include "ModelManager.h"
#include "ParticleSystem.h"
#include "SkyBox.h"
#include "../Ruler.h"
#include "TranslationManager.h"

class IScriptHandler;
class INetSocket;

struct sGameViewContext
{
	std::unique_ptr<IGameWindow> window;
	std::unique_ptr<ISoundPlayer> soundPlayer;
	std::unique_ptr<ITextWriter> textWriter;
	std::function<std::unique_ptr<IScriptHandler>()> scriptHandlerFactory;
	std::function<std::unique_ptr<INetSocket>()> socketFactory;
};

class CGameView
{
public:
	CGameView(sGameViewContext * context);
	~CGameView();
	void CreateSkybox(double size, std::string const& textureFolder);
	IUIElement * GetUI() const;
	void SetCamera(ICamera * camera);
	CModelManager& GetModelManager();
	CParticleSystem& GetParticleSystem();
	ISoundPlayer& GetSoundPlayer();
	CTranslationManager& GetTranslationManager();
	CRuler& GetRuler();
	IRenderer& GetRenderer();
	IShaderManager & GetShaderManager();
	void ResizeWindow(int height, int width);
	void EnableVertexLightning(bool enable);
	void EnableGPUSkinning(bool enable);
	void EnableShadowMap(int size, float angle);
	void DisableShadowMap();
	void SetLightPosition(int index, float* pos);
	void EnableMSAA(bool enable, int level = 1.0f);
	float GetMaxAnisotropy() const;
	void SetAnisotropyLevel(float level);
	void ClearResources();
	void SetWindowTitle(std::string const& title);
	void Preload(std::string const& image);
	void LoadModule(std::string const& module);
	void EnableLight(size_t index, bool enable = true);
	void SetLightColor(size_t index, LightningType type, float * values);
	
private:
	void DrawTable(bool shadowOnly = false);
	void DrawUI();
	void Update();
	void DrawRuler();
	void DrawObjects(void);
	void DrawStaticObjects(bool shadowOnly);
	void DrawBoundingBox();
	void DrawShadowMap();
	void SetUpShadowMapDraw();
	void Init();

	void InitLandscape();

	void InitInput();
	void ResetController();
	void DrawText3D(CVector3d const& pos, std::string const& text);
	void WindowCoordsToWorldCoords(int windowX, int windowY, double & worldX, double & worldY, double worldZ = 0);
	CGameView(CGameView const&) = delete;
	CGameView& operator=(const CGameView&) = delete;

	std::unique_ptr<IGameWindow> m_window;
	IRenderer * m_renderer;
	IViewHelper * m_viewHelper;
	IInput * m_input;
	std::unique_ptr<CGameModel> m_gameModel;
	std::unique_ptr<CGameController> m_gameController;
	std::unique_ptr<IShaderManager> m_shaderManager;
	std::unique_ptr<ISoundPlayer> m_soundPlayer;
	std::unique_ptr<ICamera> m_camera;
	std::unique_ptr<CSkyBox> m_skybox;
	std::unique_ptr<IUIElement> m_ui;
	std::unique_ptr<ITextWriter> m_textWriter;
	CModelManager m_modelManager;
	CParticleSystem m_particles;
	CRuler m_ruler;
	CTranslationManager m_translationManager;
	std::function<std::unique_ptr<IScriptHandler>()> m_scriptHandlerFactory;
	std::function<std::unique_ptr<INetSocket>()> m_socketFactory;

	bool m_vertexLightning;
	bool m_shadowMap;
	std::unique_ptr<ICachedTexture> m_shadowMapTexture;
	std::unique_ptr<IFrameBuffer> m_shadowMapFBO;
	int m_shadowMapSize;
	float m_lightProjectionMatrix[16];
	float m_lightModelViewMatrix[16];
	CVector3d m_lightPosition;
	float m_shadowAngle;

	std::unique_ptr<IDrawingList> m_tableList;
	std::unique_ptr<IDrawingList> m_tableListShadow;
};