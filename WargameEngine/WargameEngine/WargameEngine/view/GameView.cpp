#include "GameView.h"
#include <string>
#include <cstring>
#include "Matrix4.h"
#include "../controller/GameController.h"
#include "../model/ObjectGroup.h"
#include "../LogWriter.h"
#include "../ThreadPool.h"
#include "../Module.h"
#include "../Ruler.h"
#include "../OSSpecific.h"
#include "TextWriter.h"
#include "CameraStrategy.h"
#include "../UI/UIElement.h"
#include "../SoundPlayerFMod.h"
#ifdef DIRECTX

#else
#include "OpenGLRenderer.h"
#define RENDERER_CLASS COpenGLRenderer
#endif
#ifdef DIRECTX

#elif GLFW

#else
#include "InputGLUT.h"
#include "GameWindowGLUT.h"
#define WINDOW_CLASS CGameWindowGLUT
#endif

using namespace std;
using namespace placeholders;

shared_ptr<CGameView> CGameView::m_instanse = NULL;

weak_ptr<CGameView> CGameView::GetInstance()
{
	if (!m_instanse.get())
	{
		m_instanse.reset(new CGameView());
		m_instanse->Init();
	}
	weak_ptr<CGameView> pView(m_instanse);

	return pView;
}

void CGameView::FreeInstance()
{
	m_instanse.reset();
}

CGameView::~CGameView()
{
	ThreadPool::CancelAll();
	DisableShadowMap();
}

CGameView::CGameView(void)
	: m_renderer(make_unique<RENDERER_CLASS>())
	, m_viewHelper(dynamic_cast<IViewHelper*>(m_renderer.get()))
	, m_shaderManager(m_renderer->CreateShaderManager())
	, m_textWriter(make_unique<CTextWriter>(*m_renderer))
	, m_particles(*m_renderer)
	, m_gameModel(make_unique<CGameModel>())
	, m_modelManager(*m_renderer, *m_gameModel)
{
	m_ui = make_unique<CUIElement>(*m_renderer);
	m_ui->SetTheme(make_shared<CUITheme>(CUITheme::defaultTheme));
}

void CGameView::Init()
{
	setlocale(LC_ALL, ""); 
	setlocale(LC_NUMERIC, "english");

	m_window = make_unique<WINDOW_CLASS>();
	
	m_vertexLightning = false;
	m_shadowMap = false;
	m_camera = make_unique<CCameraStrategy>(0.0, 0.0, 2.8, 0.5);
	m_tableList = 0;
	m_tableListShadow = 0;

	m_gameController = make_unique<CGameController>(*m_gameModel);
	m_gameController->Init();
	m_soundPlayer = std::make_unique<CSoundPlayerFMod>();
	m_soundPlayer->Init();

	InitInput();

	m_window->DoOnDrawScene([this] {
		DrawShadowMap();
		m_viewHelper->ClearBuffers(true, true);
		Update();
	});
	m_window->DoOnResize([this](int width, int height) {m_ui->Resize(height, width);});
	m_window->DoOnShutdown(FreeInstance);

	m_window->Init();
}

void CGameView::WindowCoordsToWorldCoords(int windowX, int windowY, double & worldX, double & worldY, double worldZ)
{
	CVector3d start, end;
	m_viewHelper->WindowCoordsToWorldVector(windowX, windowY, start, end);
	double a = (worldZ - start.z) / (end.z - start.z);
	worldX = a * (end.x - start.x) + start.x;
	worldY = a * (end.y - start.y) + start.y;
}

static const string g_controllerTag = "controller";

void CGameView::InitInput()
{
	m_input = make_unique<CInputGLUT>();
	m_camera->SetInput(*m_input);
	//UI
	m_input->DoOnLMBDown([this](int x, int y) {
		return m_ui->LeftMouseButtonDown(x, y);
	}, 0);
	m_input->DoOnLMBUp([this](int x, int y) {
		return m_ui->LeftMouseButtonUp(x, y);
	}, 0);
	m_input->DoOnCharacter([this](unsigned int key) {
		return m_ui->OnCharacterInput(key);
	}, 0);
	m_input->DoOnKeyDown([this](int key, int modifiers) {
		return m_ui->OnKeyPress(key, modifiers);
	}, 0);
	m_input->DoOnMouseMove([this](int x, int y) {
		m_ui->OnMouseMove(x, y);
		return false;
	}, 9);
	//Ruler
	m_input->DoOnLMBDown([this](int x, int y) {
		double wx, wy;
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
	m_input->DoOnLMBUp([this](int x, int y) {
		double wx, wy;
		WindowCoordsToWorldCoords(x, y, wx, wy);
		m_ruler.SetEnd(wx, wy);
		return false;
	}, 2);
	m_input->DoOnRMBDown([this](int, int) {
		if (m_ruler.IsVisible())
		{
			m_ruler.Hide();
		}
		return false;
	}, 2);
	m_input->DoOnMouseMove([this](int x, int y) {
		double wx, wy;
		WindowCoordsToWorldCoords(x, y, wx, wy);
		if (m_ruler.IsEnabled())
		{
			m_ruler.SetEnd(wx, wy);
		}
		return false;
	}, 2);
	//Game Controller
	m_input->DoOnLMBDown([this](int x, int y) {
		CVector3d begin, end;
		m_viewHelper->WindowCoordsToWorldVector(x, y, begin, end);
		bool result = m_gameController->OnLeftMouseDown(begin, end, m_input->GetModifiers());
		auto object = m_gameModel->GetSelectedObject();
		if (result && object)
		{
			m_ruler.SetBegin(object->GetX(), object->GetY());
		}
		return result;
	}, 5, g_controllerTag);
	m_input->DoOnLMBUp([this](int x, int y) {
		CVector3d begin, end;
		m_viewHelper->WindowCoordsToWorldVector(x, y, begin, end);
		bool result = m_gameController->OnLeftMouseUp(begin, end, m_input->GetModifiers());
		if (result && !m_ruler.IsEnabled())
		{
			m_ruler.Hide();
		}
		m_ruler.Disable();
		return result;
	}, 5, g_controllerTag);
	m_input->DoOnMouseMove([this](int x, int y) {
		CVector3d begin, end;
		m_viewHelper->WindowCoordsToWorldVector(x, y, begin, end);
		bool result = m_gameController->OnMouseMove(begin, end, m_input->GetModifiers());
		auto object = m_gameModel->GetSelectedObject();
		if (result && object)
		{
			m_ruler.SetEnd(object->GetX(), object->GetY());
		}
		return result;
	}, 5, g_controllerTag);
	m_input->DoOnRMBDown([this](int x, int y) {
		CVector3d begin, end;
		m_viewHelper->WindowCoordsToWorldVector(x, y, begin, end);
		return m_gameController->OnRightMouseDown(begin, end, m_input->GetModifiers());
	}, 5, g_controllerTag);
	m_input->DoOnRMBUp([this](int x, int y) {
		CVector3d begin, end;
		m_viewHelper->WindowCoordsToWorldVector(x, y, begin, end);
		return m_gameController->OnRightMouseUp(begin, end, m_input->GetModifiers());
	}, 5, g_controllerTag);
}

void CGameView::DrawUI()
{
	m_window->Enter2DMode();
	m_ui->Draw();
	m_window->Leave2DMode();
}

void DrawBBox(IBounding* ibox, double x, double y, double z, double rotation, IRenderer & renderer)
{
	if (dynamic_cast<CBoundingCompound*>(ibox) != NULL)
	{
		CBoundingCompound * bbox = (CBoundingCompound *)ibox;
		for (size_t i = 0; i < bbox->GetChildCount(); ++i)
		{
			DrawBBox(bbox->GetChild(i), x, y, z, rotation, renderer);
		}
		return;
	}
	CBoundingBox * bbox = (CBoundingBox *)ibox;
	if (!bbox) return;
	renderer.PushMatrix();
	renderer.Translate(x, y, z);
	renderer.Rotate(rotation, 0.0, 0.0, 1.0);
	renderer.Scale(bbox->GetScale());
	renderer.SetColor(0.0f, 0.0f, 255.0f);
	const double * min = bbox->GetMin();
	const double * max = bbox->GetMax();
	renderer.RenderArrays(RenderMode::LINE_LOOP, { CVector3d(min[0], min[1], min[2]), { min[0], max[1], min[2] }, { min[0], max[1], max[2] }, { min[0], min[1], max[2] } }, {}, {});//Left
	renderer.RenderArrays(RenderMode::LINE_LOOP, { CVector3d(min[0], min[1], min[2]), { min[0], min[1], max[2] }, { max[0], min[1], max[2] }, { max[0], min[1], min[2] } }, {}, {});//Back
	renderer.RenderArrays(RenderMode::LINE_LOOP, { CVector3d(max[0], min[1], min[2]), { max[0], max[1], min[2] }, { max[0], max[1], max[2] }, { max[0], min[1], max[2] } }, {}, {});//Right
	renderer.RenderArrays(RenderMode::LINE_LOOP, { CVector3d(min[0], max[1], min[2]), { min[0], max[1], max[2] }, { max[0], max[1], max[2] }, { max[0], max[1], min[2] } }, {}, {}); //Front
	renderer.SetColor(255.0f, 255.0f, 255.0f);
	renderer.PopMatrix();
}

void CGameView::DrawBoundingBox()
{
	shared_ptr<IObject> object = m_gameModel->GetSelectedObject();
	if(object)
	{
		if (CGameModel::IsGroup(object.get()))
		{
			CObjectGroup * group = (CObjectGroup *)object.get();
			for(size_t i = 0; i < group->GetCount(); ++i)
			{
				object = group->GetChild(i);
				if(object)
				{
					auto bbox = m_gameModel->GetBoundingBox(object->GetPathToModel());
					if (bbox)
					{
						DrawBBox(bbox.get(), object->GetX(), object->GetY(), object->GetZ(), object->GetRotation(), *m_renderer);
					}
				}
			}
		}
		else
		{
			auto bbox = m_gameModel->GetBoundingBox(object->GetPathToModel());
			if(bbox) DrawBBox(bbox.get(), object->GetX(), object->GetY(), object->GetZ(), object->GetRotation(), *m_renderer);
		}
	}
}

void CGameView::Update()
{
	ThreadPool::Update();
	CVector3d position = m_camera->GetPosition();
	CVector3d direction = m_camera->GetDirection();
	CVector3d up = m_camera->GetUpVector();
	m_soundPlayer->SetListenerPosition(position, direction);
	m_soundPlayer->Update();
	if (m_skybox) m_skybox->Draw(-direction[0], -direction[1], -direction[2], m_camera->GetScale());
	m_renderer->ResetViewMatrix();
	m_renderer->LookAt(position, direction, up);
	m_gameController->Update();
	DrawObjects();
	DrawBoundingBox();
	DrawRuler();
	DrawUI();
}

void CGameView::DrawRuler()
{
	if (m_ruler.IsVisible())
	{
		m_renderer->SetColor(255.0f, 255.0f, 0.0f);
		m_renderer->RenderArrays(RenderMode::LINES, { m_ruler.GetBegin(),m_ruler.GetEnd() }, {}, {});
		m_renderer->SetColor(255.0f, 255.0f, 255.0f);
		char str[10];
		sprintf(str, "%0.2f", m_ruler.GetDistance());
		DrawText3D(m_ruler.GetEnd(), str);
	}
}

void CGameView::ResetTable()
{
	m_tableList.reset();
	m_tableListShadow.reset();
}

void CGameView::DrawTable(bool shadowOnly)
{	
	auto list = m_renderer->CreateDrawingList([this, shadowOnly] {
		CLandscape const& landscape = m_gameModel->GetLandscape();
		double x1 = -landscape.GetWidth() / 2.0;
		double x2 = landscape.GetWidth() / 2.0;
		double y1 = -landscape.GetDepth() / 2.0;
		double y2 = landscape.GetDepth() / 2.0;
		double xstep = landscape.GetWidth() / (landscape.GetPointsPerWidth() - 1);
		double ystep = landscape.GetDepth() / (landscape.GetPointsPerDepth() - 1);
		m_renderer->SetTexture(landscape.GetTexture());
		unsigned int k = 0;
		for (double x = x1; x <= x2 - xstep; x += xstep)
		{
			vector<CVector3d> vertex;
			vector<CVector2d> texCoord;
			for (double y = y1; y <= y2; y += ystep, k++)
			{
				texCoord.push_back({ (x + x2) / landscape.GetHorizontalTextureScale(), (y + y2) / landscape.GetVerticalTextureScale() });
				vertex.push_back({ x, y, landscape.GetHeight(k) });
				texCoord.push_back({ (x + x2 + xstep) / landscape.GetHorizontalTextureScale(), (y + y2) / landscape.GetVerticalTextureScale() });
				vertex.push_back({ x + xstep, y, landscape.GetHeight(k + 1) });
			}
			m_renderer->RenderArrays(RenderMode::TRIANGLE_STRIP, vertex, {}, texCoord);
		}
		m_renderer->SetTexture("");
		if (!shadowOnly)//Down't draw decals because they don't cast shadows
		{
			for (size_t i = 0; i < landscape.GetNumberOfDecals(); ++i)
			{
				sDecal const& decal = landscape.GetDecal(i);
				m_renderer->SetTexture(decal.texture);
				m_renderer->PushMatrix();
				m_renderer->Translate(decal.x, decal.y, 0.0);
				m_renderer->Rotate(decal.rotation, 0.0, 0.0, 1.0);
				m_renderer->RenderArrays(RenderMode::TRIANGLE_STRIP, {
					CVector3d(-decal.width / 2, -decal.depth / 2, landscape.GetHeight(decal.x - decal.width / 2, decal.y - decal.depth / 2) + 0.0001),
					{ -decal.width / 2, decal.depth / 2, landscape.GetHeight(decal.x - decal.width / 2, decal.y + decal.depth / 2) + 0.0001 },
					{ decal.width / 2, -decal.depth / 2, landscape.GetHeight(decal.x + decal.width / 2, decal.y - decal.depth / 2) + 0.0001 },
					{ decal.width / 2, decal.depth / 2, landscape.GetHeight(decal.x + decal.width / 2, decal.y + decal.depth / 2) + 0.0001 }
					}, {},{ CVector2d(0.0, 0.0), { 0.0, 1.0 }, { 1.0, 0.0 }, { 1.0, 1.0 } });
				m_renderer->PopMatrix();
			}
		}
		m_renderer->SetTexture("");
	});
	if (shadowOnly)
	{
		m_tableListShadow = move(list);
	}
	else
	{
		m_tableList = move(list);
	}
}

void CGameView::DrawObjects(void)
{
	m_viewHelper->EnableDepthTest(true);
	m_viewHelper->EnableBlending(true);
	m_shaderManager->BindProgram();
	if (m_vertexLightning)
	{
		m_viewHelper->EnableVertexLightning(true);
	}
	if (m_shadowMap) SetUpShadowMapDraw();
	if (!m_tableList) DrawTable(false);
	m_tableList->Draw();
	DrawStaticObjects(false);
	size_t countObjects = m_gameModel->GetObjectCount();
	for (size_t i = 0; i < countObjects; i++)
	{
		shared_ptr<IObject> object = m_gameModel->Get3DObject(i);
		m_renderer->PushMatrix();
		m_renderer->Translate(object->GetX(), object->GetY(), 0.0);
		m_renderer->Rotate(object->GetRotation(), 0.0, 0.0, 1.0);
		m_modelManager.DrawModel(object->GetPathToModel(), object, false);
		size_t secondaryModels = object->GetSecondaryModelsCount();
		for (size_t j = 0; j < secondaryModels; ++j)
		{
			m_modelManager.DrawModel(object->GetSecondaryModel(j), object, false);
		}
		m_renderer->PopMatrix();
	}
	m_shaderManager->UnBindProgram();
	m_viewHelper->EnableBlending(false);
	m_viewHelper->EnableVertexLightning(false);
	for (size_t i = 0; i < m_gameModel->GetProjectileCount(); i++)
	{
		CProjectile const& projectile = m_gameModel->GetProjectile(i);
		m_renderer->PushMatrix();
		m_renderer->Translate(projectile.GetX(), projectile.GetY(), projectile.GetZ());
		m_renderer->Rotate(projectile.GetRotation(), 0.0, 0.0, 1.0);
		if (!projectile.GetPathToModel().empty())
			m_modelManager.DrawModel(projectile.GetPathToModel(), nullptr, false);
		if (!projectile.GetParticle().empty())
			m_particles.DrawEffect(projectile.GetParticle(), projectile.GetTime());
		m_renderer->PopMatrix();
	}
	m_particles.DrawParticles();
	m_viewHelper->EnableDepthTest(false);
}

void CGameView::DrawStaticObjects(bool shadowOnly)
{
	auto& landscape = m_gameModel->GetLandscape();
	for (size_t i = 0; i < landscape.GetStaticObjectCount(); i++)
	{
		CStaticObject const& object = landscape.GetStaticObject(i);
		if (!shadowOnly || object.CastsShadow())
		{
			m_renderer->PushMatrix();
			m_renderer->Translate(object.GetX(), object.GetY(), object.GetZ());
			m_renderer->Rotate(object.GetRotation(), 0.0, 0.0, 1.0);
			m_modelManager.DrawModel(object.GetPathToModel(), nullptr, shadowOnly);
			m_renderer->PopMatrix();
		}
	}
}

void CGameView::SetUpShadowMapDraw()
{
	Matrix4F cameraModelViewMatrix;
	m_renderer->GetViewMatrix(cameraModelViewMatrix);

	Matrix4F cameraInverseModelViewMatrix = cameraModelViewMatrix.Invert();

	Matrix4F lightMatrix;
	lightMatrix.Scale(0.5f);
	lightMatrix.Translate(0.5, 0.5, 0.5);
	lightMatrix *= m_lightProjectionMatrix;
	lightMatrix *= m_lightModelViewMatrix;
	lightMatrix *= cameraInverseModelViewMatrix;

	m_shaderManager->SetUniformMatrix4("lightMatrix", 1, lightMatrix);
}

void CGameView::DrawShadowMap()
{
	if (!m_shadowMap) return;
	m_viewHelper->EnableDepthTest(true);
	m_viewHelper->SetUpViewport(m_lightPosition, { 0.0, 0.0, 0.0 }, m_shadowMapSize, m_shadowMapSize, m_shadowAngle, 3.0, 300.0);
	m_shadowMapFBO->Bind();
	m_viewHelper->ClearBuffers(false, true);
	m_viewHelper->EnablePolygonOffset(true, 2.0f, 500.0f);

	m_renderer->GetViewMatrix(m_lightModelViewMatrix);
	m_viewHelper->GetProjectionMatrix(m_lightProjectionMatrix);

	if (!m_tableListShadow) DrawTable(true);
	m_tableListShadow->Draw();

	size_t countObjects = m_gameModel->GetObjectCount();
	for (size_t i = 0; i < countObjects; i++)
	{
		shared_ptr<IObject> object = m_gameModel->Get3DObject(i);
		if (!object->CastsShadow()) continue;
		m_renderer->PushMatrix();
		m_renderer->Translate(object->GetX(), object->GetY(), object->GetZ());
		m_renderer->Rotate(object->GetRotation(), 0.0, 0.0, 1.0);
		m_modelManager.DrawModel(object->GetPathToModel(), object, true);
		size_t secondaryModels = object->GetSecondaryModelsCount();
		for (size_t j = 0; j < secondaryModels; ++j)
		{
			m_modelManager.DrawModel(object->GetSecondaryModel(j), object, true);
		}
		m_renderer->PopMatrix();
	}

	m_viewHelper->EnablePolygonOffset(false);
	m_shadowMapFBO->UnBind();
	m_viewHelper->RestoreViewport();
	m_viewHelper->EnableDepthTest(false);
}

void CGameView::CreateSkybox(double size, string const& textureFolder)
{
	m_skybox.reset(new CSkyBox(size, size, size, textureFolder, *m_renderer));
}

CGameController& CGameView::GetController()
{
	return *m_gameController;
}

CGameModel& CGameView::GetModel()
{
	return *m_gameModel;
}

void CGameView::ResetController()
{
	m_input->DeleteAllSignalsByTag(g_controllerTag);
	m_gameController.reset();
	m_gameModel = make_unique<CGameModel>();
	m_gameController = make_unique<CGameController>(*m_gameModel);
}

ICamera * CGameView::GetCamera()
{
	return m_camera.get();
}

void CGameView::SetCamera(ICamera * camera)
{
	m_camera.reset(camera);
	m_camera->SetInput(*m_input);
}

CModelManager& CGameView::GetModelManager()
{
	return m_modelManager;
}

IUIElement * CGameView::GetUI() const
{
	return m_ui.get();
}

CParticleSystem& CGameView::GetParticleSystem()
{
	return m_particles;
}

CTextWriter& CGameView::GetTextWriter()
{
	return *m_textWriter;
}

ISoundPlayer& CGameView::GetSoundPlayer()
{
	return *m_soundPlayer;
}

CTranslationManager& CGameView::GetTranslationManager()
{
	return m_translationManager;
}

CRuler& CGameView::GetRuler()
{
	return m_ruler;
}

IRenderer& CGameView::GetRenderer()
{
	return *m_renderer;
}

void CGameView::ResizeWindow(int height, int width)
{
	m_window->ResizeWindow(width, height);
}

void CGameView::NewShaderProgram(string const& vertex, string const& fragment, string const& geometry)
{
	m_shaderManager->NewProgram(vertex, fragment, geometry);
}

void CGameView::EnableVertexLightning(bool enable)
{ 
	m_vertexLightning = enable;
}

void CGameView::EnableShadowMap(int size, float angle)
{
	if (m_shadowMap) return;
	
	m_viewHelper->ActivateTextureSlot(TextureSlot::eShadowMap);
	m_shadowMapTexture = m_renderer->CreateTexture(NULL, size, size, CachedTextureType::DEPTH);
	try
	{
		m_shadowMapFBO = m_viewHelper->CreateFramebuffer();
	}
	catch (std::runtime_error const& e)
	{
		LogWriter::WriteLine(string(e.what()) + ", shadow maps cannot be enabled");
		m_shadowMapTexture.reset();
		return;
	}
	m_shadowMapFBO->Bind();
	//glDrawBuffer(GL_NONE);
	try
	{
		m_shadowMapFBO->AssignTexture(*m_shadowMapTexture, CachedTextureType::DEPTH);
	}
	catch (std::runtime_error const& e)
	{
		LogWriter::WriteLine(string("Cannot enable shadowmaps. ") + e.what());
		m_shadowMapFBO.reset();
		m_shadowMapTexture.reset();
		return;
	}
	m_shadowMapFBO->UnBind();
	m_viewHelper->ActivateTextureSlot(TextureSlot::eDiffuse);
	m_shadowMap = true;
	m_shadowMapSize = size;
	m_shadowAngle = angle;
}

void CGameView::DisableShadowMap()
{
	if (!m_shadowMap) return;
	m_shadowMapTexture.reset();
	m_shadowMapFBO.reset();
	m_shadowMap = false;
}

void CGameView::SetLightPosition(int index, float* pos)
{
	m_viewHelper->SetLightPosition(index, pos);
	if (index == 0) m_lightPosition = {pos[0], pos[1], pos[2]};
}

void CGameView::EnableMSAA(bool enable, int level)
{
	try
	{
		m_window->EnableMultisampling(enable, level);
	}
	catch (std::runtime_error const& e)
	{
		LogWriter::WriteLine(e.what());
	}
}

float CGameView::GetMaxAnisotropy() const
{
	return m_viewHelper->GetMaximumAnisotropyLevel();
}

void CGameView::ClearResources()
{
	m_modelManager = CModelManager(*m_renderer, *m_gameModel);
	((COpenGLRenderer&)*m_renderer).GetTextureManager().Reset();
	if (m_skybox)
	{
		m_skybox->ResetList();
	}
	ResetTable();
}

void CGameView::SetWindowTitle(string const& title)
{
	m_window->SetTitle(title + " - Wargame Engine");
}

IShaderManager const& CGameView::GetShaderManager() const
{
	return *m_shaderManager;
}

void CGameView::Preload(string const& image)
{
	if (!image.empty())
	{
		m_viewHelper->ClearBuffers(true, true);
		m_window->Enter2DMode();
		m_renderer->SetTexture(image);
		float width = 640.0f;//glutGet(GLUT_WINDOW_WIDTH);
		float height = 480.0f;//glutGet(GLUT_WINDOW_HEIGHT);
		m_renderer->RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector2f(0.0f, 0.0f), { 0.0f, height }, { width, 0.0f }, { width, height } }, { CVector2f(0.0f, 0.0f), { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } });
		//glutSwapBuffers();
		m_window->Leave2DMode();
	}
	size_t countObjects = m_gameModel->GetObjectCount();
	for (size_t i = 0; i < countObjects; i++)
	{
		shared_ptr<const IObject> object = m_gameModel->Get3DObject(i);
		m_modelManager.LoadIfNotExist(object->GetPathToModel());
	}
	m_renderer->SetTexture("");
}

void CGameView::LoadModule(string const& module)
{
	ThreadPool::CancelAll();
	sModule::Load(module);
	ChangeWorkingDirectory(sModule::folder);
	m_vertexLightning = false;
	m_shadowMap = false;
	m_lightPosition = CVector3d();
	ThreadPool::QueueCallback([this]() {
		ResetController();
		ClearResources();
		m_ui->ClearChildren();
		GetController().Init();
		InitInput();
	});
}

void CGameView::ToggleFullscreen() const 
{
	m_window->ToggleFullscreen();
}

void CGameView::DrawText3D(CVector3d const& pos, string const& text)
{
	m_window->Enter2DMode();
	int x, y;
	m_viewHelper->WorldCoordsToWindowCoords(pos, x, y);
	m_textWriter->PrintText(x, y, "times.ttf", 24, text);
	m_window->Leave2DMode();
}

void CGameView::EnableLight(size_t index, bool enable)
{
	m_viewHelper->EnableLight(index, enable);
}

void CGameView::SetLightColor(size_t index, LightningType type, float * values)
{
	m_viewHelper->SetLightColor(index, type, values);
}

void CGameView::EnableGPUSkinning(bool enable)
{
	m_modelManager.EnableGPUSkinning(enable);
}