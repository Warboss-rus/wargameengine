#include "GameView.h"
#include "gl.h"
#include <string>
#include "..\SelectionTools.h"
#include "..\UI\UICheckBox.h"
#include "..\controller\CommandHandler.h"
#include "..\LUA\LUARegisterFunctions.h"
#include "..\LUA\LUARegisterUI.h"
#include "..\LUA\LUARegisterObject.h"
#include "..\model\ObjectGroup.h"

using namespace std;

shared_ptr<CGameView> CGameView::m_instanse = NULL;

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

void CGameView::CreateTable(float width, float height, std::string const& texture)
{
	m_table.reset(new CTable(width, height, texture));
}

void CGameView::CreateSkybox(float size, std::string const& textureFolder)
{
	m_skybox.reset(new CSkyBox(size, size, size, textureFolder));
}

CGameView::CGameView(void)
{
	m_gameModel = CGameModel::GetIntanse();
	m_ui.reset(new CUIElement());

	m_light.SetPosition(float3(10, 10, 10));
	m_light.SetDiffuseIntensity(1, 1, 1, 10);
	m_light.SetAmbientIntensity(0.5f, 0.5f, 0.5f, 1.0f);
	m_light.SetSpecularIntensity(1, 1, 1, 1);
}

void CGameView::OnTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(10, OnTimer, 0);
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
	glEnable(GL_NORMALIZE);
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.01f);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LIGHT0);
	
	glutDisplayFunc(CGameView::OnDrawScene);
	glutTimerFunc(10, OnTimer, 0);
	glutReshapeFunc(&OnReshape);
	glutKeyboardFunc(&CInput::OnKeyboard);
	glutSpecialFunc(&CInput::OnSpecialKeyPress);
	glutMouseFunc(&CInput::OnMouse);
	glutMotionFunc(&CInput::OnMouseMove);
	glutPassiveMotionFunc(&CInput::OnPassiveMouseMove);
	glutMotionFunc(&CInput::OnMouseMove);

	m_lua.reset(new CLUAScript());
	RegisterFunctions(*m_lua.get());
	RegisterUI(*m_lua.get());
	RegisterObject(*m_lua.get());
	try
	{
		m_lua->RunScript("main.lua");
	}
	catch(std::exception e)
	{
		MessageBoxA(NULL, e.what(), "LUA Error", 0);
	}

	InitShaders();
	glutMainLoop();
}

void CGameView::InitShaders()
{
	// Загружаем шейдеры
	CShaderLoader loader;
	CShader vertexShader =
		loader.LoadShader(GL_VERTEX_SHADER, L"per_pixel.vsh");
	CShader fragmentShader =
		loader.LoadShader(GL_FRAGMENT_SHADER, L"per_pixel.fsh");

	// Создаем программы и присоединяем к ней шейдеры
	m_program.Create();
	m_program.AttachShader(vertexShader);
	m_program.AttachShader(fragmentShader);

	// Компилируем шейдеры
	CShaderCompiler compiler;
	compiler.CompileShader(vertexShader);
	compiler.CompileShader(fragmentShader);
	compiler.CheckStatus();

	// Компонуем программу
	CProgramLinker linker;
	linker.LinkProgram(m_program);
	linker.CheckStatus();

	CProgramInfo programInfo(m_program);

	CTextureManager::GetInstance()->SetShaderVarLocation(m_program.GetUniformLocation("texture"));
}

void CGameView::OnDrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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
	if(m_updateCallback) m_updateCallback();
	if(m_singleCallback)
	{
		m_singleCallback();
		m_singleCallback = std::function<void()>();
		Sleep(50);
	}
	m_camera.Update();
	if(m_skybox) m_skybox->Draw(m_camera.GetTranslationX(), m_camera.GetTranslationY(), 0, m_camera.GetScale());
	glEnable(GL_DEPTH_TEST);

	glPushMatrix();
	  float3 pos = m_light.GetPosition();
	  glTranslatef(pos.x, pos.y, pos.z);
	  float4 lightColor = m_light.GetColor();
	  glColor3f(lightColor.x, lightColor.y, lightColor.z);
	  glutWireSphere(0.1, 100, 100);
	glPopMatrix();
	
	if(m_table) m_table->Draw();
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glUseProgram(m_program);

	glActiveTexture(GL_TEXTURE0);
	m_light.SetLight(GL_LIGHT0);
	DrawObjects();	
	
	glUseProgram(0);
	//glCullFace(GL_BACK);

	glDisable(GL_DEPTH_TEST);
	DrawBoundingBox();
	m_ruler.Draw();
	DrawUI();
}

void CGameView::DrawObjects(void)
{
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

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
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void CGameView::OnReshape(int width, int height) 
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat aspect = (GLfloat)width / (GLfloat)height;
	gluPerspective(60, aspect, 0.5, 100.0);
	glMatrixMode(GL_MODELVIEW);
	CGameView::GetIntanse().lock()->m_ui->Resize(height, width);
}

void CGameView::FreeInstance()
{
	m_instanse.reset();
}

void CGameView::CameraSetLimits(float maxTransX, float maxTransY, float maxScale, float minScale)
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
	m_camera.Rotate((float)rotZ / 10, (float)rotX / 5);
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
	float beginWorldX, beginWorldY, endWorldX, endWorldY;
	WindowCoordsToWorldCoords(beginX, beginY, beginWorldX, beginWorldY);
	WindowCoordsToWorldCoords(endX, endY, endWorldX, endWorldY);
	float minX = (beginWorldX < endWorldX)?beginWorldX:endWorldX;
	float maxX = (beginWorldX > endWorldX)?beginWorldX:endWorldX;
	float minY = (beginWorldY < endWorldY)?beginWorldY:endWorldY;
	float maxY = (beginWorldY > endWorldY)?beginWorldY:endWorldY;
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
	float minDistance = 10000000.0;
	CGameModel * model = CGameModel::GetIntanse().lock().get();
	float start[3];
	float end[3];
	WindowCoordsToWorldVector(x, y, start[0], start[1], start[2], end[0], end[1], end[2]);
	for(unsigned long i = 0; i < model->GetObjectCount(); ++i)
	{
		std::shared_ptr<IObject> object = model->Get3DObject(i);
		if(m_modelManager.GetBoundingBox(object->GetPathToModel())->IsIntersectsRay(start, end, object->GetX(), object->GetY(), object->GetZ(), object->GetRotation(), m_selectedObjectCapturePoint))
		{
			float distance = sqrt(object->GetX() * object->GetX() + object->GetY() * object->GetY() + object->GetZ() * object->GetZ());
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

void CGameView::RulerBegin(float x, float y)
{
	m_ruler.SetBegin(x, y);
}

void CGameView::RulerEnd(float x, float y)
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
	float worldX, worldY;
	WindowCoordsToWorldCoords(x, y, worldX, worldY, m_selectedObjectCapturePoint.z);

	float3 oldCords(object->GetCoords());
	if (m_table->isCoordsOnTable(worldX, worldY))
	{
		object->SetCoords(worldX - m_selectedObjectCapturePoint.x, worldY - m_selectedObjectCapturePoint.y, 0);
		if (IsObjectInteresectSomeObjects(object))
		{
			object->SetCoords(oldCords);
		}
	}
}

bool CGameView::IsObjectInteresectSomeObjects(std::shared_ptr<IObject> current)
{
	CGameModel * model = CGameModel::GetIntanse().lock().get();
	std::shared_ptr<IBounding> curBox = m_modelManager.GetBoundingBox(current->GetPathToModel());
	float3 curPos( current->GetX(), current->GetY(), current->GetZ() );
	float curAngle = current->GetRotation();
	for (unsigned long i = 0; i < model->GetObjectCount(); ++i)
	{
		std::shared_ptr<IObject> object = model->Get3DObject(i);
		std::shared_ptr<IBounding> bounding = m_modelManager.GetBoundingBox( object->GetPathToModel() );
		float3 pos( object->GetX(), object->GetY(), object->GetZ() );
		float angle = object->GetRotation();
		if (current != object && IsInteresect( curBox.get(), curPos, curAngle, bounding.get(), pos, angle ) )
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