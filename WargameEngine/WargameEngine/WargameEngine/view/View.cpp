#include "View.h"
#include "IWindow.h"
#include "../controller/Controller.h"
#include "../model/ObjectGroup.h"
#include "../LogWriter.h"
#include "../ThreadPool.h"
#include "../Module.h"
#include "../UI/UIElement.h"
#include "../UI/UITheme.h"
#include "IImageReader.h"
#include "IModelReader.h"
#include "../Utils.h"
#include "ITextWriter.h"
#include "ISoundPlayer.h"
#include "Material.h"
#include "PerfomanceMeter.h"

using namespace std;
using namespace placeholders;

namespace wargameEngine
{
namespace view
{

static const string g_controllerTag = "controller";

View::View(IWindow& window, ISoundPlayer& soundPlayer, ITextWriter& textWriter, ThreadPool& threadPool, AsyncFileProvider& asyncFileProvider,
	vector<unique_ptr<IImageReader>>& imageReaders, vector<unique_ptr<IModelReader>>& modelReaders, model::IBoundingBoxManager & boundingManager)
	: m_window(window)
	, m_renderer(m_window.GetRenderer())
	, m_viewHelper(m_window.GetViewHelper())
	, m_input(m_window.GetInput())
	, m_soundPlayer(soundPlayer)
	, m_textWriter(textWriter)
	, m_threadPool(threadPool)
	, m_boundingManager(boundingManager)
	, m_ui(m_textWriter)
	, m_modelManager(m_boundingManager, asyncFileProvider)
	, m_textureManager(m_viewHelper, asyncFileProvider)
{
	m_viewHelper.SetTextureManager(m_textureManager);
	for (auto& reader : imageReaders)
	{
		m_textureManager.RegisterImageReader(std::move(reader));
	}
	for (auto& reader : modelReaders)
	{
		m_modelManager.RegisterModelReader(std::move(reader));
	}
	setlocale(LC_ALL, "");
	setlocale(LC_NUMERIC, "english");

	m_ui.SetTheme(make_shared<ui::UITheme>());

	m_window.DoOnDrawScene([this]() {
		m_viewHelper.ClearBuffers(true, true);
		Update();
	});
	m_window.DoOnResize([this](int width, int height) {
		m_ui.Resize(height, width);
		for (auto& viewport : m_viewports)
		{
			viewport.Resize(width, height);
		}
	});
	m_window.DoOnShutdown([this] {
		m_threadPool.CancelAll();
		m_textWriter.Reset();
	});

#ifdef _DEBUG
	//m_physicsEngine->EnableDebugDraw(*m_renderer);
#endif
}

View::~View()
{
	m_viewports.clear();
}

void View::Init(model::Model& model, controller::Controller& controller)
{
	m_ui.ClearChildren();
	m_viewports.clear();
	int width, height;
	m_window.GetWindowSize(width, height);
	m_viewports.emplace_back(0, 0, width, height, 60.0f, m_viewHelper, m_input, true, true);
	m_viewports.front().GetCamera().SetLimits(100.0f, 100.0f, 100.0f, 0.5f, 2.8f);

	m_model = &model;
	m_controller = &controller;
	
	ClearResources();
	InitLandscape();
	InitInput();
	m_viewports.front().GetCamera().AttachToKeyboardMouse();
	m_soundPlayer.Init();
}

void View::InitLandscape()
{
	m_model->GetLandscape().DoOnUpdated([this]() {
		m_tableBuffer.reset();
	});
}

void View::WindowCoordsToWorldCoords(int windowX, int windowY, float & worldX, float & worldY, float worldZ)
{
	CVector3f start, end;
	WindowCoordsToWorldVector(windowX, windowY, start, end);
	float a = (worldZ - start.z) / (end.z - start.z);
	worldX = a * (end.x - start.x) + start.x;
	worldY = a * (end.y - start.y) + start.y;
}

void View::WindowCoordsToWorldVector(int x, int y, CVector3f & start, CVector3f & end)
{
	for (auto& viewport : m_viewports)
	{
		if (viewport.PointIsInViewport(x, y))
		{
			m_viewHelper.WindowCoordsToWorldVector(viewport, x, y, start, end);
			return;
		}
	}
}

void View::InitInput()
{
	m_input.Reset();
	//UI
	m_input.DoOnLMBDown([this](int x, int y) {
		return m_ui.LeftMouseButtonDown(x, y);
	}, 0);
	m_input.DoOnLMBUp([this](int x, int y) {
		return m_ui.LeftMouseButtonUp(x, y);
	}, 0);
	m_input.DoOnCharacter([this](wchar_t key) {
		return m_ui.OnCharacterInput(key);
	}, 0);
	m_input.DoOnKeyDown([this](VirtualKey key, int) {
		return m_ui.OnKeyPress(key, m_input.GetModifiers());
	}, 0);
	m_input.DoOnMouseMove([this](int x, int y, int /*dx*/, int /*dy*/) {
		m_ui.OnMouseMove(x, y);
		return false;
	}, 9);
	//Ruler
	m_input.DoOnLMBDown([this](int x, int y) {
		float wx, wy;
		WindowCoordsToWorldCoords(x, y, wx, wy);
		if (m_ruler.IsVisible())
		{
			m_ruler.Hide();
		}
		else
		{
			if (m_ruler.IsEnabled())
			{
				m_ruler.SetBegin(wx, wy);
				return true;
			}
		}
		return false;
	}, 2);
	m_input.DoOnLMBUp([this](int x, int y) {
		float wx, wy;
		WindowCoordsToWorldCoords(x, y, wx, wy);
		m_ruler.SetEnd(wx, wy);
		return false;
	}, 2);
	m_input.DoOnRMBDown([this](int, int) {
		if (m_ruler.IsVisible())
		{
			m_ruler.Hide();
		}
		return false;
	}, 2);
	m_input.DoOnMouseMove([this](int x, int y, int /*dx*/, int /*dy*/) {
		float wx, wy;
		WindowCoordsToWorldCoords(x, y, wx, wy);
		if (m_ruler.IsEnabled())
		{
			m_ruler.SetEnd(wx, wy);
		}
		return false;
	}, 2);
	//Game Controller
	m_input.DoOnLMBDown([this](int x, int y) {
		CVector3f begin, end;
		WindowCoordsToWorldVector(x, y, begin, end);
		bool result = m_controller->OnLeftMouseDown(begin, end, m_input.GetModifiers());
		auto object = m_model->GetSelectedObject();
		if (result && object)
		{
			m_ruler.SetBegin(object->GetX(), object->GetY());
		}
		return result;
	}, 5, g_controllerTag);
	m_input.DoOnLMBUp([this](int x, int y) {
		CVector3f begin, end;
		WindowCoordsToWorldVector(x, y, begin, end);
		bool result = m_controller->OnLeftMouseUp(begin, end, m_input.GetModifiers());
		if (result && !m_ruler.IsEnabled())
		{
			m_ruler.Hide();
		}
		m_ruler.Disable();
		return result;
	}, 5, g_controllerTag);
	m_input.DoOnMouseMove([this](int x, int y, int /*dx*/, int /*dy*/) {
		CVector3f begin, end;
		WindowCoordsToWorldVector(x, y, begin, end);
		bool result = m_controller->OnMouseMove(begin, end, m_input.GetModifiers());
		auto object = m_model->GetSelectedObject();
		if (result && object)
		{
			m_ruler.SetEnd(object->GetX(), object->GetY());
		}
		return result;
	}, 5, g_controllerTag);
	m_input.DoOnRMBDown([this](int x, int y) {
		CVector3f begin, end;
		WindowCoordsToWorldVector(x, y, begin, end);
		return m_controller->OnRightMouseDown(begin, end, m_input.GetModifiers());
	}, 5, g_controllerTag);
	m_input.DoOnRMBUp([this](int x, int y) {
		CVector3f begin, end;
		WindowCoordsToWorldVector(x, y, begin, end);
		return m_controller->OnRightMouseUp(begin, end, m_input.GetModifiers());
	}, 5, g_controllerTag);
	m_input.DoOnGamepadButtonStateChange([this](int gamepadIndex, int buttonIndex, bool newState) {
		return m_controller->OnGamepadButtonStateChange(gamepadIndex, buttonIndex, newState);
	}, 5, g_controllerTag);
	m_input.DoOnGamepadAxisChange([this](int gamepadIndex, int axisIndex, double horizontal, double vertical) {
		return m_controller->OnGamepadAxisChange(gamepadIndex, axisIndex, horizontal, vertical);
	}, 5, g_controllerTag);
}

void View::DrawUI()
{
	m_renderer.SetColor(0, 0, 0);
	m_viewHelper.DrawIn2D([this] {
		m_ui.Draw(m_renderer);
	});
}

void DrawBBox(model::Bounding::Box const& bbox, model::IBaseObject const& object, IRenderer & renderer, float scale)
{
	renderer.UnbindTexture();
	renderer.PushMatrix();
	renderer.Translate(object.GetCoords());
	renderer.Rotate(object.GetRotations());
	renderer.Scale(scale);
	CVector3f min = bbox.max;
	CVector3f max = bbox.min;
	renderer.SetColor(0, 0, 255);
	renderer.RenderArrays(IRenderer::RenderMode::LineLoop, { min, { min[0], max[1], min[2] }, { min[0], max[1], max[2] }, { min[0], min[1], max[2] } }, {}, {});//Left
	renderer.RenderArrays(IRenderer::RenderMode::LineLoop, { min, { min[0], min[1], max[2] }, { max[0], min[1], max[2] }, { max[0], min[1], min[2] } }, {}, {});//Back
	renderer.RenderArrays(IRenderer::RenderMode::LineLoop, { CVector3f(max[0], min[1], min[2]), { max[0], max[1], min[2] }, max, { max[0], min[1], max[2] } }, {}, {});//Right
	renderer.RenderArrays(IRenderer::RenderMode::LineLoop, { CVector3f(min[0], max[1], min[2]), { min[0], max[1], max[2] }, max, { max[0], max[1], min[2] } }, {}, {}); //Front
	renderer.SetColor(0, 0, 0);
	renderer.PopMatrix();
}

void View::DrawBoundingBox()
{
	shared_ptr<model::IObject> object = m_model->GetSelectedObject();
	if (object)
	{
		if (object->IsGroup())
		{
			model::ObjectGroup * group = (model::ObjectGroup *)object.get();
			for (size_t i = 0; i < group->GetCount(); ++i)
			{
				object = group->GetChild(i);
				if (object)
				{
					auto bbox = m_boundingManager.GetBounding(object->GetPathToModel());
					DrawBBox(bbox.GetBox(), *object, m_renderer, bbox.scale);
				}
			}
		}
		else
		{
			auto bbox = m_boundingManager.GetBounding(object->GetPathToModel());
			DrawBBox(bbox.GetBox(), *object, m_renderer, bbox.scale);
		}
	}
}

void View::Update()
{
	PerfomanceMeter::Reset();
	m_threadPool.Update();
	m_controller->Update();
	auto& defaultCamera = m_viewports.front().GetCamera();
	m_soundPlayer.SetListenerPosition(defaultCamera.GetPosition(), defaultCamera.GetDirection());
	m_soundPlayer.Update();
	CollectMeshes();
	SortMeshes();
	for (auto it = m_viewports.rbegin(); it != m_viewports.rend(); ++it)
	{
		auto& viewport = *it;
		viewport.Bind();
		m_viewHelper.EnableDepthTest(false, false);
		if (m_skybox && !viewport.IsDepthOnly())
		{
			auto& camera = viewport.GetCamera();
			m_skybox->Draw(m_viewHelper, -camera.GetPosition(), camera.GetScale());
		}
		m_viewHelper.EnableBlending(!viewport.IsDepthOnly());
		DrawMeshes(m_viewHelper, viewport);
		//RunOcclusionQueries(m_model->GetAllBaseObjects(), viewport, m_viewHelper);
		m_viewHelper.EnableDepthTest(false, false);
		if (!viewport.IsDepthOnly())
		{
			DrawBoundingBox();
			DrawRuler(viewport, m_viewHelper);
			//m_physicsEngine.Draw();
		}
		if (viewport.DrawUI())
		{
			DrawUI();
		}
		m_viewHelper.EnableDepthTest(true, true);
		viewport.Unbind();
	}
	m_viewHelper.DrawIn2D([this] {
		PerfomanceMeter::ReportFrameEnd();
		m_renderer.SetColor(255, 255, 0);
		m_textWriter.PrintText(m_renderer, 1, 16, "times.ttf", 16, L"FPS" + std::to_wstring(PerfomanceMeter::GetFps()));
		m_textWriter.PrintText(m_renderer, 1, 34, "times.ttf", 16, L"V" + std::to_wstring(PerfomanceMeter::GetVerticesDrawn()));
		m_textWriter.PrintText(m_renderer, 1, 52, "times.ttf", 16, L"P" + std::to_wstring(PerfomanceMeter::GetPolygonsDrawn()));
		m_textWriter.PrintText(m_renderer, 1, 70, "times.ttf", 16, L"DC" + std::to_wstring(PerfomanceMeter::GetDrawCalls()));
		m_renderer.SetColor(0, 0, 0);
	});
}

void View::DrawRuler(IViewport& viewport, IViewHelper& renderer)
{
	if (m_ruler.IsVisible())
	{
		m_renderer.SetColor(255, 255, 0);
		m_renderer.RenderArrays(IRenderer::RenderMode::Lines, { m_ruler.GetBegin(),m_ruler.GetEnd() }, {}, {});
		m_renderer.SetColor(255, 255, 255);
		DrawText3D(m_ruler.GetEnd(), ToWstring(m_ruler.GetDistance(), 2), viewport, renderer);
		m_renderer.SetColor(0, 0, 0);
	}
}

bool IsOutsideFrustum(IViewHelper& viewHelper, IViewport& viewport, const model::IBaseObject* obj)
{
	int x(-1), y(-1);
	viewHelper.WorldCoordsToWindowCoords(viewport, obj->GetCoords(), x, y);
	return x < viewport.GetX() || x > viewport.GetX() + viewport.GetWidth() ||
		y < viewport.GetY() || y > viewport.GetY() + viewport.GetHeight();
}

void View::CollectMeshes()
{
	m_meshesToDraw.clear();
	m_nonDepthTestMeshes.clear();
	CollectTableMeshes();
	auto notVisibleInFrustum = [this](const model::IBaseObject* obj) {
		for (auto& viewport : m_viewports)
		{
			if (!viewport.NeedsFrustumCulling() || !IsOutsideFrustum(m_viewHelper, viewport, obj))
			{
				return false;
			}
		}
		return true;
	};
	std::vector<model::IBaseObject*> objects = m_model->GetAllBaseObjects();
	m_meshesToDraw.reserve(objects.size() * 10);
	objects.erase(std::remove_if(objects.begin(), objects.end(), notVisibleInFrustum), objects.end());
	for (auto* object : objects)
	{
		m_renderer.PushMatrix();
		m_renderer.Translate(object->GetCoords());
		m_renderer.Rotate(object->GetRotations());
		model::IObject* fullObject = object->GetFullObject();
		m_modelManager.GetModelMeshes(object->GetPathToModel(), m_renderer, m_textureManager, fullObject, m_meshesToDraw);
		if (fullObject)
		{
			size_t secondaryModels = fullObject->GetSecondaryModelsCount();
			for (size_t j = 0; j < secondaryModels; ++j)
			{
				m_modelManager.GetModelMeshes(fullObject->GetSecondaryModel(j), m_renderer, m_textureManager, nullptr, m_meshesToDraw);
			}
		}
		m_renderer.PopMatrix();
	}
}

void View::DrawMeshes(IViewHelper& renderer, Viewport& currentViewport)
{
	const bool shadowOnly = currentViewport.IsDepthOnly();
	renderer.EnableDepthTest(true, true);
	auto& shaderManager = renderer.GetShaderManager();
	if (!shadowOnly)
	{
		if (m_shaderProgram)shaderManager.PushProgram(*m_shaderProgram);
		currentViewport.SetUpShadowMap();
		auto& lights = m_model->GetLights();
		size_t lightsCount = lights.size();
		renderer.SetNumberOfLights(lightsCount);
		for (size_t i = 0; i < lightsCount; ++i)
		{
			auto& light = lights[i];
			renderer.SetUpLight(i, light.GetPosition(), light.GetAmbient(), light.GetDiffuse(), light.GetSpecular());
		}
		static const std::string eyePosKey = "viewPos";
		CVector3f viewPos = currentViewport.GetCamera().GetPosition();
		shaderManager.SetUniformValue(eyePosKey, 3, 1, viewPos.ptr());
	}

	//Draw
	DrawMeshesList(renderer, m_meshesToDraw, shadowOnly);
	if (!shadowOnly && m_shaderProgram)
	{
		shaderManager.PopProgram();
	}
	if (!shadowOnly)
	{
		renderer.EnableDepthTest(true, false);
		DrawMeshesList(renderer, m_nonDepthTestMeshes, shadowOnly);
	}

	if (!shadowOnly)
	{
		for (size_t i = 0; i < m_model->GetProjectileCount(); i++)
		{
			model::Projectile const& projectile = m_model->GetProjectile(i);
			renderer.PushMatrix();
			renderer.Translate(projectile.GetCoords());
			renderer.Rotate(projectile.GetRotations());
			if (projectile.GetParticle())
				m_particles.Draw(*projectile.GetParticle(), renderer);
			renderer.PopMatrix();
		}
		for (size_t i = 0; i < m_model->GetParticleCount(); ++i)
		{
			model::ParticleEffect const& effect = m_model->GetParticleEffect(i);
			m_particles.Draw(effect, renderer);
		}
	}
}

void View::DrawMeshesList(IViewHelper &renderer, const std::vector<DrawableMesh>& list, bool shadowOnly)
{
	auto& shaderManager = renderer.GetShaderManager();
	renderer.UnbindTexture();
	ICachedTexture* texture = nullptr;
	Material* material = nullptr;
	Matrix4F prevMatrix;

	for (const DrawableMesh& mesh : list)
	{
		if (mesh.shader && !shadowOnly)
		{
			shaderManager.PushProgram(*mesh.shader);
		}
		if (texture != mesh.texturePtr && !shadowOnly)
		{
			texture = mesh.texturePtr;
			if (texture)
			{
				renderer.SetTexture(*texture);
			}
		}
		if (material != mesh.material && !shadowOnly)
		{
			material = mesh.material;
			if (material)
			{
				renderer.SetMaterial(mesh.material->ambient, mesh.material->diffuse, mesh.material->specular, mesh.material->shininess);
			}
			else
			{
				constexpr float empty[] = { 0.0f, 0.0f, 0.0f, 1.0f };
				renderer.SetMaterial(empty, empty, empty, 0.0f);
			}
			if (material && !material->bumpMap.empty())
			{
				auto texturePtr = m_textureManager.GetTexturePtr(material->bumpMap);
				m_renderer.SetTexture(*texturePtr, IRenderer::TextureSlot::Bump);
			}
			if (material && !material->specularMap.empty())
			{
				auto texturePtr = m_textureManager.GetTexturePtr(material->specularMap);
				m_renderer.SetTexture(*texturePtr, IRenderer::TextureSlot::Specular);
			}
		}
		if (!texture && mesh.material)
		{
			m_renderer.SetColor(mesh.material->diffuse);
		}
		if (mesh.modelMatrix != prevMatrix)
		{
			m_renderer.SetModelMatrix(mesh.modelMatrix);
			prevMatrix = mesh.modelMatrix;
		}
		if (mesh.skeleton)
		{
			shaderManager.SetUniformValue("joints", 16, mesh.skeleton->size() / 16, mesh.skeleton->data());
		}

		auto buffer = mesh.buffer;
		std::unique_ptr<IVertexBuffer> tempBuffer;
		if (!buffer && mesh.tempBuffer)
		{
			tempBuffer = renderer.CreateVertexBuffer((const float*)mesh.tempBuffer->vertices.data(), (const float*)mesh.tempBuffer->normals.data(), (const float*)mesh.tempBuffer->texCoords, mesh.tempBuffer->vertices.size());
			if (mesh.tempBuffer->indexes && mesh.tempBuffer->indexesCount > 0 && mesh.indexed)
			{
				renderer.SetIndexBuffer(*tempBuffer, mesh.tempBuffer->indexes, mesh.tempBuffer->indexesCount);
			}
			buffer = tempBuffer.get();
		}

		if (mesh.indexed)
		{
			renderer.DrawIndexed(*buffer, mesh.count, mesh.start);
		}
		else
		{
			renderer.Draw(*buffer, mesh.count, mesh.start);
		}
		if (!texture && mesh.material)
		{
			m_renderer.SetColor(0, 0, 0);
		}

		if (mesh.skeleton)
		{
			shaderManager.SetUniformValue("joints", 16, 0, (const float*)nullptr);
		}
		if (mesh.shader && !shadowOnly)
		{
			shaderManager.PopProgram();
		}
	}
	m_renderer.SetModelMatrix(Matrix4F());
}

void View::CollectTableMeshes()
{
	if (!m_tableBuffer)
	{
		model::Landscape const& landscape = m_model->GetLandscape();
		float x1 = -landscape.GetWidth() / 2.0f;
		float x2 = landscape.GetWidth() / 2.0f;
		float y1 = -landscape.GetDepth() / 2.0f;
		float y2 = landscape.GetDepth() / 2.0f;
		float xstep = landscape.GetWidth() / (landscape.GetPointsPerWidth() - 1);
		float ystep = landscape.GetDepth() / (landscape.GetPointsPerDepth() - 1);
		vector<CVector3f> vertex;
		vector<CVector2f> texCoord;
		for (float x = x1; x <= x2 - xstep; x += xstep)
		{
			for (float y = y1; y <= y2 - ystep; y += ystep)
			{
				texCoord.push_back({ (x + x2) / landscape.GetHorizontalTextureScale(), (y + y2) / landscape.GetVerticalTextureScale() });
				vertex.push_back({ x, y, landscape.GetHeight(x, y) });

				texCoord.push_back({ (x + x2 + xstep) / landscape.GetHorizontalTextureScale(), (y + y2) / landscape.GetVerticalTextureScale() });
				vertex.push_back({ x + xstep, y, landscape.GetHeight(x + xstep, y) });

				texCoord.push_back({ (x + x2) / landscape.GetHorizontalTextureScale(), (y + y2 + ystep) / landscape.GetVerticalTextureScale() });
				vertex.push_back({ x, y + ystep, landscape.GetHeight(x, y + ystep) });

				texCoord.push_back({ (x + x2 + xstep) / landscape.GetHorizontalTextureScale(), (y + y2) / landscape.GetVerticalTextureScale() });
				vertex.push_back({ x + xstep, y, landscape.GetHeight(x + xstep, y) });

				texCoord.push_back({ (x + x2) / landscape.GetHorizontalTextureScale(), (y + y2 + ystep) / landscape.GetVerticalTextureScale() });
				vertex.push_back({ x, y + ystep, landscape.GetHeight(x, y + ystep) });

				texCoord.push_back({ (x + x2 + xstep) / landscape.GetHorizontalTextureScale(), (y + y2 + ystep) / landscape.GetVerticalTextureScale() });
				vertex.push_back({ x + xstep, y + ystep, landscape.GetHeight(x + xstep, y + ystep) });
			}
		}
		m_tableBuffer = m_renderer.CreateVertexBuffer(vertex.data()->ptr(), nullptr, &texCoord.data()->x, vertex.size());
		m_tableBufferSize = vertex.size();
	}
	model::Landscape const& landscape = m_model->GetLandscape();
	m_meshesToDraw.push_back({nullptr, m_textureManager.GetTexturePtr(landscape.GetTexture()), nullptr, m_tableBuffer.get(), Matrix4F(), nullptr, nullptr, 0, m_tableBufferSize, false});

	for (size_t i = 0; i < landscape.GetNumberOfDecals(); ++i)
	{
		model::Decal const& decal = landscape.GetDecal(i);
		m_renderer.SetTexture(decal.texture);
		m_renderer.PushMatrix();
		m_renderer.Translate(CVector3f(decal.x, decal.y, 0.0f));
		m_renderer.Rotate(decal.rotation, CVector3f(0.0f, 0.0f, 1.0f));
		Matrix4F mat = m_renderer.GetModelMatrix();
		m_renderer.PopMatrix();
		std::vector<CVector3f> vertices = {
			CVector3f(-decal.width / 2, -decal.depth / 2, landscape.GetHeight(decal.x - decal.width / 2, decal.y - decal.depth / 2) + 0.001f),
			{ -decal.width / 2, decal.depth / 2, landscape.GetHeight(decal.x - decal.width / 2, decal.y + decal.depth / 2) + 0.001f },
			{ decal.width / 2, -decal.depth / 2, landscape.GetHeight(decal.x + decal.width / 2, decal.y - decal.depth / 2) + 0.001f },
			{ -decal.width / 2, decal.depth / 2, landscape.GetHeight(decal.x - decal.width / 2, decal.y + decal.depth / 2) + 0.001f },
			{ decal.width / 2, -decal.depth / 2, landscape.GetHeight(decal.x + decal.width / 2, decal.y - decal.depth / 2) + 0.001f },
			{ decal.width / 2, decal.depth / 2, landscape.GetHeight(decal.x + decal.width / 2, decal.y + decal.depth / 2) + 0.001f }
		};
		static const CVector2f texCoords[] = { CVector2f(0.0f, 0.0f), { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f },{ 1.0f, 0.0f },{ 1.0f, 1.0f } };
		auto temp = std::make_shared<TempMeshBuffer>(TempMeshBuffer{ vertices, std::vector<CVector3f>(), texCoords, nullptr, 0 });
		m_nonDepthTestMeshes.push_back(DrawableMesh{nullptr, m_textureManager.GetTexturePtr(decal.texture), nullptr, nullptr, mat, std::move(temp), nullptr, 0, 6, false});
	}
}

void View::CreateSkybox(float size, const Path& textureFolder)
{
	m_skybox = std::make_unique<SkyBox>(size, size, size, textureFolder, m_textureManager);
}

ModelManager& View::GetModelManager()
{
	return m_modelManager;
}

ui::IUIElement * View::GetUI()
{
	return &m_ui;
}

ISoundPlayer& View::GetSoundPlayer()
{
	return m_soundPlayer;
}

TranslationManager& View::GetTranslationManager()
{
	return m_translationManager;
}

Ruler& View::GetRuler()
{
	return m_ruler;
}

IRenderer& View::GetRenderer()
{
	return m_renderer;
}

void View::NewShaderProgram(const Path& vertex, const Path& fragment, const Path& geometry)
{
	m_shaderProgram = m_renderer.GetShaderManager().NewProgram(vertex, fragment, geometry);
}

IShaderProgram const& View::GetShaderProgram() const
{
	return *m_shaderProgram;
}

void View::ResizeWindow(int height, int width)
{
	m_window.ResizeWindow(width, height);
}

Viewport& View::CreateShadowMapViewport(int size, float angle, CVector3f const& lightPosition)
{
	m_viewports.emplace_back(0, 0, size, size, angle, m_viewHelper, m_input, false, false);
	auto& shadowMapViewport = m_viewports.back();
	shadowMapViewport.AttachNewTexture(IRenderer::CachedTextureType::Depth, static_cast<int>(IRenderer::TextureSlot::ShadowMap));
	shadowMapViewport.GetCamera().Set(lightPosition);
	shadowMapViewport.SetPolygonOffset(true, 2.0f, 500.0f);
	shadowMapViewport.SetClippingPlanes(3.0, 300.0);
	return shadowMapViewport;
}

void View::DisableShadowMap(Viewport& viewport)
{
	auto shadowMapViewport = viewport.GetShadowViewport();
	for (auto it = m_viewports.begin(); it != m_viewports.end(); ++it)
	{
		if (&*it == shadowMapViewport)
		{
			m_viewports.erase(it);
			break;
		}
	}
	viewport.SetShadowViewport(nullptr);
}

void View::EnableMSAA(bool enable, int level)
{
	m_window.EnableMultisampling(enable, level);
}

float View::GetMaxAnisotropy() const
{
	return m_viewHelper.GetMaximumAnisotropyLevel();
}

void View::SetAnisotropyLevel(float level)
{
	m_textureManager.SetAnisotropyLevel(level);
}

void View::ClearResources()
{
	m_modelManager.Reset();
	m_textureManager.Reset();
	m_tableBuffer.reset();
}

void View::SetWindowTitle(wstring const& title)
{
	m_window.SetTitle(title + L" - Wargame Engine");
}

ThreadPool& View::GetThreadPool()
{
	return m_threadPool;
}

IViewHelper& View::GetViewHelper()
{
	return m_viewHelper;
}

IInput& View::GetInput()
{
	return m_input;
}

ParticleSystem& View::GetParticleSystem()
{
	return m_particles;
}

size_t View::GetViewportCount() const
{
	return m_viewports.size();
}

Viewport& View::GetViewport(size_t index /*= 0*/)
{
	return m_viewports[index];
}

Viewport& View::AddViewport(Viewport&& viewport)
{
	m_viewports.emplace_back(std::move(viewport));
	return m_viewports.back();
}

void View::RemoveViewport(IViewport * viewportPtr)
{
	for (auto it = m_viewports.begin(); it != m_viewports.end(); ++it)
	{
		if (&*it == viewportPtr)
		{
			m_viewports.erase(it);
			return;
		}
	}
}

void View::Preload(const Path& image)
{
	if (!image.empty())
	{
		m_viewHelper.ClearBuffers(true, true);
		m_viewHelper.DrawIn2D([this, &image] {
			m_renderer.SetTexture(image);
			int width = 640;
			int height = 480;
			m_window.GetWindowSize(width, height);
			m_renderer.RenderArrays(IRenderer::RenderMode::TriangleStrip, { CVector2i(0, 0), { 0, height }, { width, 0 }, { width, height } }, { CVector2f(0.0f, 0.0f), { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } });
			//glutSwapBuffers();
		});
	}
	size_t countObjects = m_model->GetObjectCount();
	for (size_t i = 0; i < countObjects; i++)
	{
		shared_ptr<const model::IObject> object = m_model->Get3DObject(i);
		m_modelManager.LoadIfNotExist(object->GetPathToModel(), m_textureManager);
	}
}

void View::DrawText3D(CVector3f const& pos, wstring const& text, IViewport& viewport, IViewHelper& renderer)
{
	renderer.DrawIn2D([&] {
		int x, y;
		renderer.WorldCoordsToWindowCoords(viewport, pos, x, y);
		m_textWriter.PrintText(renderer, x, y, "times.ttf", 24, text);
	});
}

bool View::EnableVRMode(bool enable, bool mirrorToScreen)
{
	auto viewportFactory = [this](unsigned int width, unsigned int height) {
		Viewport first(0, 0, width, height, 65.0f, m_viewHelper, m_input, false, false);
		first.AttachNewTexture(IRenderer::CachedTextureType::RGBA);
		Viewport second(0, 0, width, height, 65.0f, m_viewHelper, m_input, false, false);
		second.AttachNewTexture(IRenderer::CachedTextureType::RGBA);
		return std::pair<IViewport&, IViewport&>(AddViewport(std::move(first)), AddViewport(std::move(second)));
	};
	return m_window.EnableVRMode(enable, viewportFactory);
}

void View::AddParticleEffect(const Path& effectPath, CVector3f const& position, float scale, size_t maxParticles /*= 1000u*/)
{
	m_model->AddParticleEffect(m_particles.GetParticleUpdater(effectPath), effectPath, position, scale, maxParticles);
}

void View::SetSkyboxShaders(const Path& vertex, const Path& fragment)
{
	m_skybox->SetShaders(vertex, fragment, m_renderer.GetShaderManager());
}

void View::PreloadModel(const Path& model)
{
	m_modelManager.LoadIfNotExist(model, m_textureManager);
}

void View::EnableGPUSkinning(bool enable)
{
	m_modelManager.EnableGPUSkinning(enable);
}

std::vector<float> GetBoundingVertices(const model::Bounding& bounding)
{
	std::vector<float> result;
	if (bounding.type == model::Bounding::eType::Compound)
	{
		for (auto& child : bounding.GetCompound().items)
		{
			auto childVertices = GetBoundingVertices(child);
			result.insert(result.end(), childVertices.begin(), childVertices.end());
		}
	}
	if (bounding.type == model::Bounding::eType::Box)
	{
		auto& box = bounding.GetBox();
		auto min = box.min;
		auto max = box.max;
		std::array<CVector3f, 24> vertices = {
			//left
			min,{ min[0], max[1], min[2] },{ min[0], max[1], max[2] },
			{ min[0], max[1], min[2] },{ min[0], max[1], max[2] },{ min[0], min[1], max[2] },
			//back
			min,{ min[0], min[1], max[2] },{ max[0], min[1], max[2] },
			{ min[0], min[1], max[2] },{ max[0], min[1], max[2] },{ max[0], min[1], min[2] },
			//right
			CVector3f(max[0], min[1], min[2]),{ max[0], max[1], min[2] }, max,
			{ max[0], max[1], min[2] }, max,{ max[0], min[1], max[2] },
			//front
			CVector3f(min[0], max[1], min[2]),{ min[0], max[1], max[2] }, max,
			{ min[0], max[1], max[2] }, max,{ max[0], max[1], min[2] }
		};
		for (auto& vec : vertices)
		{
			result.push_back(vec.x);
			result.push_back(vec.y);
			result.push_back(vec.z);
		}
	}
	return result;
}

void View::RunOcclusionQueries(std::vector<model::IBaseObject *> objects, Viewport &currentViewport, IViewHelper& renderer)
{
	renderer.EnableColorWrite(false, false);
	renderer.UnbindTexture();
	for (auto object : objects)
	{
		if (IsOutsideFrustum(renderer, currentViewport, object))
			continue;
		auto& query = currentViewport.GetOcclusionQuery(object);
		auto it = m_boundingCache.find(object->GetPathToModel());
		if (it == m_boundingCache.end())
		{
			auto bounding = m_boundingManager.GetBounding(object->GetPathToModel());
			auto vertices = GetBoundingVertices(bounding);
			auto buffer = renderer.CreateVertexBuffer(vertices.data(), nullptr, nullptr, vertices.size() / 3, false);
			it = m_boundingCache.emplace(std::make_pair(object->GetPathToModel(), std::make_pair(std::move(buffer), vertices.size() / 3))).first;
		}
		renderer.PushMatrix();
		renderer.Translate(object->GetCoords());
		renderer.Rotate(object->GetRotations());
		query.Query([&renderer, it] {
			renderer.Draw(*it->second.first, it->second.second);
		});
		renderer.PopMatrix();
	}
	renderer.EnableColorWrite(true, true);
}

bool MeshComparator(const DrawableMesh& first, const DrawableMesh& second) {
	return std::tie(first.shader, first.texturePtr, first.buffer, first.material)
		< std::tie(second.shader, second.texturePtr, second.buffer, second.material);
};

void View::SortMeshes()
{
	std::sort(m_meshesToDraw.begin(), m_meshesToDraw.end(), MeshComparator);
	std::sort(m_nonDepthTestMeshes.begin(), m_nonDepthTestMeshes.end(), MeshComparator);
}

}
}