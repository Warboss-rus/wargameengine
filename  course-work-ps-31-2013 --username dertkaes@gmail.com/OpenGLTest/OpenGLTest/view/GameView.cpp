#include "GameView.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL\freeglut.h>
#include <string>
#include "..\SelectionTools.h"
#include "..\UI\UICheckBox.h"
#include "..\controller\CommandHandler.h"
#include "..\LUA\LUARegisterFunctions.h"
#include "..\LUA\LUARegisterUI.h"
#include "..\LUA\LUARegisterObject.h"
#include "..\model\ObjectGroup.h"
#include "..\LogWriter.h"
#include "..\ThreadPool.h"
#include "..\Module.h"

using namespace std;

shared_ptr<CGameView> CGameView::m_instanse = NULL;
bool CGameView::m_visible = true;

weak_ptr<CGameView> CGameView::GetIntanse()
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
	DisableShadowMap();
	CTextureManager::FreeInstance();
	CCommandHandler::FreeInstance();
	CGameModel::FreeInstance();
}

void CGameView::CreateTable(float width, float height, std::string const& texture)
{
	m_table.reset(new CTable(width, height, texture));
}

void CGameView::CreateSkybox(double size, std::string const& textureFolder)
{
	m_skybox.reset(new CSkyBox(size, size, size, textureFolder));
}

CGameView::CGameView(void)
{
	m_gameModel = CGameModel::GetIntanse();
	m_ui.reset(new CUIElement());
}

void CGameView::OnTimer(int value)
{
	if(m_visible) glutPostRedisplay();
	glutTimerFunc(1, OnTimer, 0);
}

void CGameView::OnChangeState(int state)
{
	CGameView::m_visible = (state == GLUT_VISIBLE);
}

void CGameView::Init()
{
	setlocale(LC_ALL, ""); 
	int argc = 0;
	char* argv[] = {""};
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("GLUT test");
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.01f);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glutDisplayFunc(CGameView::OnDrawScene);
	glutTimerFunc(1, OnTimer, 0);
	glutReshapeFunc(&OnReshape);
	glutKeyboardFunc(&CInput::OnKeyboard);
	glutSpecialFunc(&CInput::OnSpecialKeyPress);
	glutMouseFunc(&CInput::OnMouse);
	glutMotionFunc(&CInput::OnMouseMove);
	glutPassiveMotionFunc(&CInput::OnPassiveMouseMove);
	glutMotionFunc(&CInput::OnMouseMove);
	glutCloseFunc(&CGameView::FreeInstance);
	glutWindowStatusFunc(OnChangeState);

	glewInit();
	m_vertexLightning = false;
	m_shadowMap = false;
	memset(m_lightPosition, 0, sizeof(float)* 3);
	m_updateTime = 0;

	m_lua.reset(new CLUAScript());
	RegisterFunctions(*m_lua.get());
	RegisterUI(*m_lua.get());
	RegisterObject(*m_lua.get());
	m_lua->RunScript(sModule::script);

	glutMainLoop();
}

void CGameView::OnDrawScene()
{
	CGameView::GetIntanse().lock()->DrawShadowMap();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CGameView::GetIntanse().lock()->Update();
	glutSwapBuffers();
}

void CGameView::DrawUI() const
{
	glEnable(GL_BLEND);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT),0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	m_ui->Draw();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glDisable(GL_BLEND);
}

void CGameView::DrawBoundingBox()
{
	std::shared_ptr<IObject> object = m_gameModel.lock()->GetSelectedObject();
	if(object)
	{
		if (CGameModel::IsGroup(object.get()))
		{
			CObjectGroup * group = (CObjectGroup *)object.get();
			for(unsigned int i = 0; i < group->GetCount(); ++i)
			{
				object = group->GetChild(i);
				if(object)
				{
					m_modelManager.GetBoundingBox(object->GetPathToModel())->Draw(object->GetX(), 
						object->GetY(), object->GetZ(), object->GetRotation());
				}
			}
		}
		else
		{
			m_modelManager.GetBoundingBox(object->GetPathToModel())->Draw(object->GetX(), 
				object->GetY(), object->GetZ(), object->GetRotation());
		}
	}
}

void CGameView::Update()
{
	if (m_updateTime > 0)
	{
		m_updateTime--;
		if (m_updateTime == 0 && m_socket)
		{
			std::vector<char> result = GetState();
			m_socket->SendData(&result[0], result.size());//1 For full dump
			m_updateTime = 1000;
		}
	}
	
	if (m_socket)
	{
		char data[65536];
		int result = m_socket->RecieveData(data, 65536);
		if (result > 0)
		{
			if (data[0] == 0)
				MessageBoxA(NULL, data + 1, "Recieved message", 0);
			else if (data[0] == 1)
			{
				CLogWriter::WriteLine("SettingState.");
				SetState(data + 1);
			}
		}
		if (result == 0)
		{
			m_socket.reset();
		}
	}
	if (m_updateCallback) m_updateCallback();
	if (m_singleCallback)
	{
		m_singleCallback();
		m_singleCallback = std::function<void()>();
	}
	CThreadPool::Update();
	m_camera.Update();
	if(m_skybox) m_skybox->Draw(m_camera.GetTranslationX(), m_camera.GetTranslationY(), 0, m_camera.GetScale());
	DrawObjects();
	DrawBoundingBox();
	m_ruler.Draw();
	DrawUI();
}

void CGameView::DrawObjects(void)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	m_shader.BindProgram();
	if (m_shadowMap) SetUpShadowMapDraw();
	if (m_table) m_table->Draw();
	if (m_vertexLightning)
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_LIGHTING);
	}
	unsigned long countObjects = m_gameModel.lock()->GetObjectCount();
	for (unsigned long i = 0; i < countObjects; i++)
	{
		shared_ptr<const IObject> object = m_gameModel.lock()->Get3DObject(i);
		glPushMatrix();
		glTranslated(object->GetX(), object->GetY(), 0);
		glRotated(object->GetRotation(), 0.0, 0.0, 1.0);
		m_modelManager.DrawModel(object->GetPathToModel(), &object->GetHiddenMeshes());
		glPopMatrix();
	}
	m_shader.UnBindProgram();
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glDisable(GL_DEPTH_TEST);
}

void CGameView::SetUpShadowMapDraw()
{
	// Сохраняем матрицы, они нам нужны для вычисления освещения
	// Инвертированная матрица используется в расчёте матрицы источника света
	float cameraModelViewMatrix[16];
	float cameraInverseModelViewMatrix[16];
	float lightMatrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, cameraModelViewMatrix);
	gluInvertMatrix(cameraModelViewMatrix, cameraInverseModelViewMatrix);

	// Вычисляем матрицу источника света
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.5, 0.5, 0.5); // + 0.5
	glScalef(0.5, 0.5, 0.5); // * 0.5
	glMultMatrixf(m_lightProjectionMatrix);
	glMultMatrixf(m_lightModelViewMatrix);
	glMultMatrixf(cameraInverseModelViewMatrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, lightMatrix);
	glPopMatrix();

	m_shader.SetUniformMatrix4("lightMatrix", 1, lightMatrix);
}

void CGameView::DrawShadowMap()
{
	if (!m_shadowMap) return;
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, m_shadowMapSize, m_shadowMapSize);
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0, 500.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(m_shadowAngle, 1.0, 3.0, 300.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(m_lightPosition[0], m_lightPosition[1], m_lightPosition[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Сохраняем эти матрицы, они нам понадобятся для расчёта матрицы источника света
	glGetFloatv(GL_PROJECTION_MATRIX, m_lightProjectionMatrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, m_lightModelViewMatrix);

	unsigned long countObjects = m_gameModel.lock()->GetObjectCount();
	for (unsigned long i = 0; i < countObjects; i++)
	{
		shared_ptr<const IObject> object = m_gameModel.lock()->Get3DObject(i);
		glPushMatrix();
		glTranslated(object->GetX(), object->GetY(), 0);
		glRotated(object->GetRotation(), 0.0, 0.0, 1.0);
		m_modelManager.DrawModel(object->GetPathToModel(), &object->GetHiddenMeshes(), true);
		glPopMatrix();
	}

	glDisable(GL_POLYGON_OFFSET_FILL);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();
}

void CGameView::OnReshape(int width, int height) 
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble aspect = (GLdouble)width / (GLdouble)height;
	gluPerspective(60, aspect, 0.5, 100.0);
	glMatrixMode(GL_MODELVIEW);
	CGameView::GetIntanse().lock()->m_ui->Resize(height, width);
}

void CGameView::CameraSetLimits(double maxTransX, double maxTransY, double maxScale, double minScale)
{
	m_camera.SetLimits(maxTransX, maxTransY, maxScale, minScale);
}

void CGameView::CameraZoomIn()
{
	m_camera.ZoomIn();
}

void CGameView::CameraZoomOut()
{
	m_camera.ZoomOut();
}

void CGameView::CameraRotate(int rotZ, int rotX)
{
	m_camera.Rotate((double)rotZ / 10, (double)rotX / 5);
}

void CGameView::CameraReset()
{
	m_camera.Reset();
}

void CGameView::CameraTranslateLeft()
{
	m_camera.Translate(CCamera::TRANSLATE, 0.0);
}

void CGameView::CameraTranslateRight()
{
	m_camera.Translate(-CCamera::TRANSLATE, 0.0);
}

void CGameView::CameraTranslateDown()
{
	m_camera.Translate(0.0, CCamera::TRANSLATE);
}

void CGameView::CameraTranslateUp()
{
	m_camera.Translate(0.0, -CCamera::TRANSLATE);
}

void CGameView::SelectObjectGroup(int beginX, int beginY, int endX, int endY)//Works only for Z = 0 plane and select object only if its center is within selection rectangle, needs better algorithm
{
	double beginWorldX, beginWorldY, endWorldX, endWorldY;
	WindowCoordsToWorldCoords(beginX, beginY, beginWorldX, beginWorldY);
	WindowCoordsToWorldCoords(endX, endY, endWorldX, endWorldY);
	double minX = (beginWorldX < endWorldX)?beginWorldX:endWorldX;
	double maxX = (beginWorldX > endWorldX)?beginWorldX:endWorldX;
	double minY = (beginWorldY < endWorldY)?beginWorldY:endWorldY;
	double maxY = (beginWorldY > endWorldY)?beginWorldY:endWorldY;
	CObjectGroup* group = new CObjectGroup();
	CGameModel * model = CGameModel::GetIntanse().lock().get();
	for(unsigned long i = 0; i < model->GetObjectCount(); ++i)
	{
		shared_ptr<IObject> object = model->Get3DObject(i);
		if(object->GetX() > minX && object->GetX() < maxX && object->GetY() > minY && object->GetY() < maxY && object->IsSelectable())
		{
			group->AddChildren(object);
		}
	}
	switch(group->GetCount())
	{
	case 0:
		{
			model->SelectObject(NULL);
			delete group;
		}break;
	case 1:
		{
			model->SelectObject(group->GetChild(0));
			delete group;
		}break;
	default:
		{
			model->SelectObject(std::shared_ptr<IObject>(group));
		}break;
	}
	if(m_selectionCallback) m_selectionCallback();
}

shared_ptr<IObject> CGameView::GetNearestObject(int x, int y)
{
	std::shared_ptr<IObject> selectedObject = NULL;
	double minDistance = 10000000.0;
	CGameModel * model = CGameModel::GetIntanse().lock().get();
	double start[3];
	double end[3];
	WindowCoordsToWorldVector(x, y, start[0], start[1], start[2], end[0], end[1], end[2]);
	for(unsigned long i = 0; i < model->GetObjectCount(); ++i)
	{
		std::shared_ptr<IObject> object = model->Get3DObject(i);
		if (!object) continue;
		std::shared_ptr<IBounding> bounding = m_modelManager.GetBoundingBox(object->GetPathToModel());
		if (!bounding) continue;
		if(bounding->IsIntersectsRay(start, end, object->GetX(), object->GetY(), object->GetZ(), object->GetRotation(), m_selectedObjectCapturePoint))
		{
			double distance = sqrt(object->GetX() * object->GetX() + object->GetY() * object->GetY() + object->GetZ() * object->GetZ());
			if(distance < minDistance)
			{
				selectedObject = object;
				minDistance = distance;
				m_selectedObjectCapturePoint.x -= selectedObject->GetX();
				m_selectedObjectCapturePoint.y -= selectedObject->GetY();
				m_selectedObjectCapturePoint.z -= selectedObject->GetZ();
			}
		}
	}
	return selectedObject;
}

void CGameView::SelectObject(int x, int y, bool shiftPressed)
{
	std::shared_ptr<IObject> selectedObject = GetNearestObject(x, y);
	if(selectedObject && !selectedObject->IsSelectable())
	{
		return;
	}
	std::shared_ptr<IObject> object = m_gameModel.lock()->GetSelectedObject();
	if(CGameModel::IsGroup(object.get()))
	{
		CObjectGroup * group = (CObjectGroup *)object.get();
		if(shiftPressed)
		{
			if(group->ContainsChildren(selectedObject))
			{
				group->RemoveChildren(selectedObject);
				if(group->GetCount() == 1)//Destroy group
				{
					m_gameModel.lock()->SelectObject(group->GetChild(0));
				}
			}
			else
			{
				group->AddChildren(selectedObject);
			}
		}
		else
		{
			if(!group->ContainsChildren(selectedObject))
			{
				m_gameModel.lock()->SelectObject(selectedObject);
			}
			else
			{
				group->SetCurrent(selectedObject);
			}
		}
	}
	else
	{
		if(shiftPressed && object != NULL)
		{
			CObjectGroup * group = new CObjectGroup();
			group->AddChildren(object);
			group->AddChildren(selectedObject);
			m_gameModel.lock()->SelectObject(std::shared_ptr<IObject>(group));
		}
		else
		{
			m_gameModel.lock()->SelectObject(selectedObject);
		}
	}
	if(m_selectionCallback) m_selectionCallback();
}

void CGameView::RulerBegin(double x, double y)
{
	m_ruler.SetBegin(x, y);
}

void CGameView::RulerEnd(double x, double y)
{
	m_ruler.SetEnd(x, y);
}

void CGameView::RulerHide()
{
	m_ruler.Hide();
}

void CGameView::TryMoveSelectedObject(int x, int y)
{
	std::shared_ptr<IObject> object = m_gameModel.lock()->GetSelectedObject();
	if (!object)
	{
		return;
	}
	double worldX, worldY;
	WindowCoordsToWorldCoords(x, y, worldX, worldY, m_selectedObjectCapturePoint.z);
	CVector3d old(object->GetCoords());
	if (m_table->isCoordsOnTable(worldX, worldY))
	{
		object->SetCoords(worldX - m_selectedObjectCapturePoint.x, worldY - m_selectedObjectCapturePoint.y, 0);
	}
	if (IsObjectInteresectSomeObjects(object))
	{
		object->SetCoords(old);
	}
}

bool CGameView::IsObjectInteresectSomeObjects(std::shared_ptr<IObject> current)
{
	CGameModel * model = CGameModel::GetIntanse().lock().get();
	std::shared_ptr<IBounding> curBox = m_modelManager.GetBoundingBox(current->GetPathToModel());
	if (!curBox) return false;
	CVector3d curPos(current->GetCoords());
	double curAngle = current->GetRotation();
	for (unsigned long i = 0; i < model->GetObjectCount(); ++i)
	{
		std::shared_ptr<IObject> object = model->Get3DObject(i);
		if (!object) continue;
		std::shared_ptr<IBounding> bounding = m_modelManager.GetBoundingBox(object->GetPathToModel());
		if (!bounding) continue;
		CVector3d pos(object->GetCoords());
		double angle = object->GetRotation();
		if (current != object && IsInteresect(curBox.get(), curPos, curAngle, bounding.get(), pos, angle))
		{
			return true;
		}
	}
	return false;
}

bool CGameView::UILeftMouseButtonDown(int x, int y)
{
	return m_ui->LeftMouseButtonDown(x, y);
}

bool CGameView::UILeftMouseButtonUp(int x, int y)
{
	return m_ui->LeftMouseButtonUp(x, y);
}

bool CGameView::UIKeyPress(unsigned char key)
{
	return m_ui->OnKeyPress(key);
}

bool CGameView::UISpecialKeyPress(int key)
{
	return m_ui->OnSpecialKeyPress(key);
}

void CGameView::SetUI(IUIElement * ui)
{
	m_ui.reset(ui);
}

IUIElement * CGameView::GetUI() const
{
	return m_ui.get();
}

void CGameView::SetSelectionCallback(callback(onSelect))
{
	m_selectionCallback = onSelect;
}

void CGameView::SetUpdateCallback(callback(onUpdate))
{
	m_updateCallback = onUpdate;
}

void CGameView::SetSingleCallback(callback(onSingleUpdate))
{
	m_singleCallback = onSingleUpdate;
}

void CGameView::ResizeWindow(int height, int width)
{
	glutReshapeWindow(width, height);
}

void CGameView::NewShaderProgram(std::string const& vertex, std::string const& fragment, std::string const& geometry)
{
	m_shader.NewProgram(vertex, fragment, geometry);
}

void CGameView::EnableVertexLightning()
{ 
	m_vertexLightning = true;
	glEnable(GL_NORMALIZE);
}
void CGameView::DisableVertexLightning()
{ 
	m_vertexLightning = false;
	glDisable(GL_NORMALIZE);
}

void CGameView::EnableShadowMap(int size, float angle)
{
	if (m_shadowMap) return;
	if (!GLEW_ARB_depth_buffer_float)
	{
		CLogWriter::WriteLine("GL_ARB_depth_buffer_float is not supported, shadow maps cannot be enabled");
		return;
	}
	if (!GLEW_EXT_framebuffer_object)
	{
		CLogWriter::WriteLine("GL_EXT_framebuffer_object is not supported, shadow maps cannot be enabled");
		return;
	}
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &m_shadowMapTexture);
	glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size, size,
		0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE, 0);
	glGenFramebuffers(1, &m_shadowMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
	glDrawBuffer(GL_NONE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMapTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		CLogWriter::WriteLine("Cannot enable shadowmaps. Error creating framebuffer.");
		glDeleteTextures(1, &m_shadowMapTexture);
		return;
	}
	m_shadowMap = true;
	m_shadowMapSize = size;
	m_shadowAngle = angle;
	
}

void CGameView::DisableShadowMap()
{
	if (!m_shadowMap) return;
	glDeleteTextures(1, &m_shadowMapTexture);
	glDeleteFramebuffersEXT(1, &m_shadowMapFBO);
	m_shadowMap = false;
}

void CGameView::SetLightPosition(int index, float* pos)
{
	glLightfv(GL_LIGHT0 + index, GL_POSITION, pos);
	if(index == 0) memcpy(m_lightPosition, pos, sizeof(float)* 3);
}

void CGameView::NetHost(unsigned short port)
{
	m_socket.reset(new CNetSocket(port));
	m_updateTime = 1000;
}

void CGameView::NetClient(std::string const& ip, unsigned short port)
{
	m_socket.reset(new CNetSocket(ip.c_str(), port));
}

void CGameView::NetSendMessage(std::string const& message)
{
	char * data = new char[message.size() + 2];
	data[0] = 0;//0 for text message
	memcpy(&data[1], message.c_str(), message.size() + 1);
	if (m_socket) m_socket->SendData(data, message.size() + 2);
	else CLogWriter::WriteLine("Net error. Socket is not yet created or connection has been lost.");
	delete [] data;
}

std::vector<char> PackProperties(std::map<std::string, std::string> const&properties)
{
	std::vector<char> result;
	result.resize(4);
	*((unsigned int*)&result[0]) = properties.size();
	for (auto i = properties.begin(); i != properties.end(); ++i)
	{
		unsigned int begin = result.size();
		result.resize(begin + 10 + i->first.size() + i->second.size());
		*((unsigned int*)&result[begin]) = i->first.size() + 1;
		memcpy(&result[begin + 4], i->first.c_str(), i->first.size() + 1);
		begin += i->first.size() + 5;
		*((unsigned int*)&result[begin]) = i->second.size() + 1;
		memcpy(&result[begin + 4], i->second.c_str(), i->second.size() + 1);
	}
	return result;
}

std::vector<char> CGameView::GetState() const
{
	std::vector<char> result;
	result.resize(5);
	result[0] = 1;
	unsigned int count = m_gameModel.lock()->GetObjectCount();
	*((unsigned int*)&result[1]) = count;
	for (unsigned int i = 0; i < count; ++i)
	{
		IObject * object = m_gameModel.lock()->Get3DObject(i).get();
		std::vector<char> current;
		std::string path = object->GetPathToModel();
		current.resize(36+path.size() + 1, 0);
		*((double*)&current[0]) = object->GetX();
		*((double*)&current[8]) = object->GetY();
		*((double*)&current[16]) = object->GetZ();
		*((double*)&current[24]) = object->GetRotation();
		*((unsigned int*)&current[32]) = path.size() + 1;
		memcpy(&current[36], path.c_str(), path.size() + 1);
		std::vector<char> properties = PackProperties(object->GetAllProperties());
		current.insert(current.end(), properties.begin(), properties.end());
		result.insert(result.end(), current.begin(), current.end());
	}
	std::vector<char> globalProperties = PackProperties(m_gameModel.lock()->GetAllProperties());
	result.insert(result.end(), globalProperties.begin(), globalProperties.end());
	return result;
}

void CGameView::SetState(char* data)
{
	unsigned int count = *(unsigned int*)&data[0];
	unsigned int current = 4;
	CGameModel * model = CGameModel::GetIntanse().lock().get();
	model->Clear();
	for (unsigned int i = 0; i < count; ++i)
	{
		double x = *((double*)&data[current]);
		double y = *((double*)&data[current + 8]);
		double z = *((double*)&data[current + 16]);
		double rotation = *((double*)&data[current + 24]);
		unsigned int pathSize = *((unsigned int*)&data[current + 32]);
		char * path = new char[pathSize];
		memcpy(path, &data[current + 36], pathSize);
		IObject* object = new C3DObject(path, x, y, rotation);
		model->AddObject(std::shared_ptr<IObject>(object));
		delete[] path;
		current += 36 + pathSize;
		unsigned int propertiesCount = *((unsigned int*)&data[current]);
		current += 4;
		for (unsigned int i = 0; i < propertiesCount; ++i)
		{
			unsigned int firstSize = *((unsigned int*)&data[current]);
			char * first = new char[firstSize];
			memcpy(first, &data[current + 4], firstSize);
			current += firstSize + 4;
			unsigned int secondSize = *((unsigned int*)&data[current]);
			char * second = new char[secondSize];
			memcpy(second, &data[current + 4], secondSize);
			current += secondSize + 4;
			object->SetProperty(first, second);
		}
	}
	unsigned int globalPropertiesCount = *((unsigned int*)&data[current]);
	current += 4;
	for (unsigned int i = 0; i < globalPropertiesCount; ++i)
	{
		unsigned int firstSize = *((unsigned int*)&data[current]);
		char * first = new char[firstSize];
		memcpy(first, &data[current + 4], firstSize);
		current += firstSize + 4;
		unsigned int secondSize = *((unsigned int*)&data[current]);
		char * second = new char[secondSize];
		memcpy(second, &data[current + 4], secondSize);
		current += secondSize + 4;
		model->SetProperty(first, second);
	}
}

void CGameView::Save(std::string const& filename)
{
	FILE* file = fopen(filename.c_str(), "wb");
	std::vector<char> state = GetState();
	fwrite(&state[0], 1, state.size(), file);
	fclose(file);
}

void CGameView::Load(std::string const& filename)
{
	FILE* file = fopen(filename.c_str(), "rb");
	if (!file)
	{
		CLogWriter::WriteLine("LoadState. Cannot find file " + filename);
		return;
	}
	fseek(file, 0L, SEEK_END);
	unsigned int size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	char* data = new char[size];
	fread(data, 1, size, file);
	fclose(file);
	SetState(data+1);
	delete[] data;
}