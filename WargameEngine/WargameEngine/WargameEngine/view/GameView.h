#include <memory>
#include <functional>
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
#include "../AsyncFileProvider.h"
#include "../ThreadPool.h"
#include "../Module.h"
#include "TextureManager.h"
#include "IViewport.h"

class IScriptHandler;
class INetSocket;
class IImageReader;
class IPhysicsEngine;

struct sGameViewContext
{
	~sGameViewContext();
	std::unique_ptr<IGameWindow> window;
	std::unique_ptr<ISoundPlayer> soundPlayer;
	std::unique_ptr<ITextWriter> textWriter;
	std::unique_ptr<IPhysicsEngine> physicsEngine;
	std::function<std::unique_ptr<IScriptHandler>()> scriptHandlerFactory;
	std::function<std::unique_ptr<INetSocket>()> socketFactory;
	sModule module;
	std::wstring workingDir;
	std::vector<std::unique_ptr<IImageReader>> imageReaders;
	std::vector<std::unique_ptr<IModelReader>> modelReaders;
};

class CGameView
{
public:
	CGameView(sGameViewContext * context);
	~CGameView();
	void Init(sModule const& module);
	void CreateSkybox(double size, std::wstring const& textureFolder);
	IUIElement * GetUI() const;
	CModelManager& GetModelManager();
	ISoundPlayer& GetSoundPlayer();
	CTranslationManager& GetTranslationManager();
	CRuler& GetRuler();
	IRenderer& GetRenderer();
	IShaderManager & GetShaderManager();
	CAsyncFileProvider& GetAsyncFileProvider();
	ThreadPool& GetThreadPool();
	IViewHelper& GetViewHelper();
	IInput& GetInput();
	CParticleSystem& GetParticleSystem();

	size_t GetViewportCount() const;
	IViewport& GetViewport(size_t index = 0);
	IViewport& AddViewport(std::unique_ptr<IViewport> && viewport);
	void RemoveViewport(IViewport * viewport);

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
	void SetWindowTitle(std::wstring const& title);
	void Preload(std::wstring const& image);
	void LoadModule(std::wstring const& module);
	void EnableLight(size_t index, bool enable = true);
	void SetLightColor(size_t index, LightningType type, float * values);
	bool EnableVRMode(bool enable, bool mirrorToScreen = true);
	void AddParticleEffect(std::wstring const& effectPath, CVector3f const& position, float scale, size_t maxParticles = 1000u);
private:
	void DrawTable(bool shadowOnly = false);
	void DrawUI();
	void Update();
	void DrawRuler();
	void DrawObjects(bool shadowOnly);
	void DrawStaticObjects(bool shadowOnly);
	void DrawBoundingBox();
	void SetUpShadowMapDraw();

	void InitLandscape();

	void InitInput();
	void DrawText3D(CVector3d const& pos, std::wstring const& text);
	void WindowCoordsToWorldCoords(int windowX, int windowY, double & worldX, double & worldY, double worldZ = 0);
	void WindowCoordsToWorldVector(int x, int y, CVector3d & start, CVector3d & end);
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
	std::vector<std::unique_ptr<IViewport>> m_viewports;
	std::unique_ptr<CSkyBox> m_skybox;
	std::unique_ptr<IUIElement> m_ui;
	std::unique_ptr<ITextWriter> m_textWriter;
	std::unique_ptr<IPhysicsEngine> m_physicsEngine;
	ThreadPool m_threadPool;
	CAsyncFileProvider m_asyncFileProvider;
	CModelManager m_modelManager;
	CTextureManager m_textureManager;
	CParticleSystem m_particles;
	CRuler m_ruler;
	CTranslationManager m_translationManager;
	std::function<std::unique_ptr<IScriptHandler>()> m_scriptHandlerFactory;
	std::function<std::unique_ptr<INetSocket>()> m_socketFactory;
	std::unique_ptr<IDrawingList> m_tableList;
	std::unique_ptr<IDrawingList> m_tableListShadow;
	bool m_vertexLightning;
	IViewport * m_shadowMapViewport;
	IViewport * m_currentViewport;
	CVector3d m_lightPosition;
};