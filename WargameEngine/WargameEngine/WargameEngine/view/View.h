#pragma once
#include "../UI/UIElement.h"
#include "ModelManager.h"
#include "ParticleSystem.h"
#include "Ruler.h"
#include "SkyBox.h"
#include "TextureManager.h"
#include "TranslationManager.h"
#include "Viewport.h"
#include <memory>
#include <string>
#include "DrawableMesh.h"
#include "TextWriter.h"

namespace wargameEngine
{
class ThreadPool;
class AsyncFileProvider;
class ITextRasterizer;

namespace model
{
class Model;
class IBoundingBoxManager;
}
namespace controller
{
class Controller;
}

namespace view
{
class IWindow;
class ISoundPlayer;

class View
{
public:
	View(IWindow& window, ISoundPlayer& soundPlayer, ITextRasterizer& textRasterizer, ThreadPool& threadPool, AsyncFileProvider& asyncFileProvider,
		std::vector<std::unique_ptr<IImageReader>>& imageReaders, std::vector<std::unique_ptr<IModelReader>>& modelReaders, model::IBoundingBoxManager & boundingManager);

	void Init(model::Model& model, controller::Controller& controller);
	void CreateSkybox(float size, const Path& textureFolder);
	ui::IUIElement* GetUI();
	ModelManager& GetModelManager();
	ISoundPlayer& GetSoundPlayer();
	TranslationManager& GetTranslationManager();
	Ruler& GetRuler();
	IRenderer& GetRenderer();
	ThreadPool& GetThreadPool();
	IViewHelper& GetViewHelper();
	IInput& GetInput();
	ParticleSystem& GetParticleSystem();
	TextWriter& GetTextWriter();

	size_t GetViewportCount() const;
	Viewport& GetViewport(size_t index = 0);
	Viewport& AddViewport(std::unique_ptr<Viewport>&& viewport);
	Viewport& CreateShadowMapViewport(int size, float angle, CVector3f const& lightPosition);
	void RemoveViewport(IViewport* viewport);

	void ResizeWindow(int height, int width);
	void EnableGPUSkinning(bool enable);
	void DisableShadowMap(Viewport& viewport);
	void EnableMSAA(bool enable, int level = 1.0f);
	float GetMaxAnisotropy() const;
	void SetAnisotropyLevel(float level);
	void ClearResources();
	void SetWindowTitle(std::wstring const& title);
	void Preload(const Path& image);
	bool EnableVRMode(bool enable, bool mirrorToScreen = true);
	void AddParticleEffect(const Path& effectPath, CVector3f const& position, float scale, size_t maxParticles = 1000u);
	void SetSkyboxShaders(const Path& vertex, const Path& fragment);
	void PreloadModel(const Path& model);

private:
	void CollectTableMeshes();
	void DrawUI();
	void Update();
	void DrawRuler(IViewport& viewport, IViewHelper& renderer);
	void CollectMeshes();
	void SortMeshes();
	void DrawMeshes(IViewHelper& renderer, Viewport& currentViewport);
	void DrawMeshesList(IViewHelper &renderer, const MeshList& list, bool shadowOnly);
	void RunOcclusionQueries(std::vector<model::IBaseObject *> objects, Viewport &currentViewport, IViewHelper& renderer);
	void DrawBoundingBox();
	void InitLandscape();
	void InitInput();
	void DrawText3D(CVector3f const& pos, std::wstring const& text, IViewport& viewport, IViewHelper& renderer);
	void WindowCoordsToWorldCoords(int windowX, int windowY, float& worldX, float& worldY, float worldZ = 0);
	void WindowCoordsToWorldVector(int x, int y, CVector3f& start, CVector3f& end);
	View(View const&) = delete;
	View& operator=(const View&) = delete;

	IWindow& m_window;
	IRenderer& m_renderer;
	IViewHelper& m_viewHelper;
	IInput& m_input;
	ISoundPlayer& m_soundPlayer;
	TextWriter m_textWriter;
	ThreadPool& m_threadPool;
	model::IBoundingBoxManager & m_boundingManager;

	ModelManager m_modelManager;
	TextureManager m_textureManager;
	ParticleSystem m_particles;
	Ruler m_ruler;
	TranslationManager m_translationManager;

	model::Model* m_model;
	controller::Controller* m_controller;
	ui::UIElement m_ui;
	std::vector<std::unique_ptr<Viewport>> m_viewports;
	std::unique_ptr<SkyBox> m_skybox;
	std::unique_ptr<IVertexBuffer> m_tableBuffer;
	size_t m_tableBufferSize = 0;
	MeshList m_meshesToDraw;
	MeshList m_nonDepthTestMeshes;
	std::unordered_map<Path, std::pair<std::unique_ptr<IVertexBuffer>, size_t>> m_boundingCache;
};
}
}