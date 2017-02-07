#include <memory>
#include <functional>
#include "IRenderer.h"
#include "ISoundPlayer.h"
#include "IInput.h"
#include "../UI/UIElement.h"
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
#include "ViewportBase.h"

class IScriptHandler;
class INetSocket;
class IImageReader;
class IPhysicsEngine;
class IOcclusionQuery;

struct sGameViewContext
{
	~sGameViewContext();
	std::unique_ptr<IGameWindow> window;
	std::unique_ptr<ISoundPlayer> soundPlayer;
	std::unique_ptr<ITextWriter> textWriter;
	std::unique_ptr<IPhysicsEngine> physicsEngine;
	std::function<std::unique_ptr<IScriptHandler>()> scriptHandlerFactory;
	std::function<std::unique_ptr<INetSocket>()> socketFactory;
	std::vector<std::unique_ptr<IImageReader>> imageReaders;
	std::vector<std::unique_ptr<IModelReader>> modelReaders;
	sModule module;
};

class CGameView
{
public:
	CGameView(sGameViewContext * context);
	~CGameView();
	void Init(sModule const& module);
	void CreateSkybox(float size, std::wstring const& textureFolder);
	IUIElement * GetUI();
	CModelManager& GetModelManager();
	ISoundPlayer& GetSoundPlayer();
	CTranslationManager& GetTranslationManager();
	CRuler& GetRuler();
	IRenderer& GetRenderer();
	void NewShaderProgram(std::wstring const& vertex, std::wstring const& fragment, std::wstring const& geometry);
	IShaderProgram const& GetShaderProgram() const;
	CAsyncFileProvider& GetAsyncFileProvider();
	ThreadPool& GetThreadPool();
	IViewHelper& GetViewHelper();
	IInput& GetInput();
	CParticleSystem& GetParticleSystem();

	size_t GetViewportCount() const;
	CViewportBase& GetViewport(size_t index = 0);
	CViewportBase& AddViewport(std::unique_ptr<CViewportBase> && viewport);
	CViewportBase& CreateShadowMapViewport(int size, float angle, CVector3f const& lightPosition);
	void RemoveViewport(IViewport * viewport);

	void ResizeWindow(int height, int width);
	void EnableGPUSkinning(bool enable);
	void DisableShadowMap(CViewportBase& viewport);
	void EnableMSAA(bool enable, int level = 1.0f);
	float GetMaxAnisotropy() const;
	void SetAnisotropyLevel(float level);
	void ClearResources();
	void SetWindowTitle(std::wstring const& title);
	void Preload(std::wstring const& image);
	void LoadModule(std::wstring const& module);
	bool EnableVRMode(bool enable, bool mirrorToScreen = true);
	void AddParticleEffect(std::wstring const& effectPath, CVector3f const& position, float scale, size_t maxParticles = 1000u);
	void SetSkyboxShaders(std::wstring const& vertex, std::wstring const& fragment);
private:
	void DrawTable(bool shadowOnly = false);
	void DrawUI();
	void Update();
	void DrawRuler();
	void DrawObjects(bool shadowOnly);
	void DrawBoundingBox();
	void InitLandscape();
	void InitInput();
	void DrawText3D(CVector3f const& pos, std::wstring const& text);
	void WindowCoordsToWorldCoords(int windowX, int windowY, float & worldX, float & worldY, float worldZ = 0);
	void WindowCoordsToWorldVector(int x, int y, CVector3f & start, CVector3f & end);
	CGameView(CGameView const&) = delete;
	CGameView& operator=(const CGameView&) = delete;

	std::unique_ptr<IGameWindow> m_window;
	IRenderer * m_renderer;
	IViewHelper * m_viewHelper;
	IInput * m_input;
	std::unique_ptr<CGameModel> m_gameModel;
	std::unique_ptr<CGameController> m_gameController;
	std::unique_ptr<IShaderProgram> m_shaderProgram;
	std::unique_ptr<ISoundPlayer> m_soundPlayer;
	std::unique_ptr<ITextWriter> m_textWriter;
	std::unique_ptr<IPhysicsEngine> m_physicsEngine;
	std::vector<std::unique_ptr<CViewportBase>> m_viewports;
	std::unique_ptr<CSkyBox> m_skybox;
	CUIElement m_ui;
	ThreadPool m_threadPool;
	CAsyncFileProvider m_asyncFileProvider;
	CModelManager m_modelManager;
	CTextureManager m_textureManager;
	CParticleSystem m_particles;
	CRuler m_ruler;
	CTranslationManager m_translationManager;
	std::function<std::unique_ptr<IScriptHandler>()> m_scriptHandlerFactory;
	std::function<std::unique_ptr<INetSocket>()> m_socketFactory;
	std::unique_ptr<IVertexBuffer> m_tableBuffer;
	size_t m_tableBufferSize;
	CViewportBase * m_currentViewport;
	long long m_lastFrameTime = 0;
};