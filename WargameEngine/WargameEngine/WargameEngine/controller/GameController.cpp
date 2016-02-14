#include "GameController.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "ScriptRegisterFunctions.h"
#include "../model/ObjectGroup.h"
#include "../model/Object.h"
#include "../view/IInput.h"
#include "../LogWriter.h"
#include "../view/GameView.h"
#include "../MemoryStream.h"

CGameController::CGameController(CGameModel& model, std::unique_ptr<IScriptHandler> && scriptHandler)
	:m_model(model), m_scriptHandler(std::move(scriptHandler))
{
}

void CGameController::Init(CGameView & view, std::function<std::unique_ptr<INetSocket>()> const& socketFactory, std::string const& scriptPath)
{
	m_commandHandler = std::make_unique<CCommandHandler>();
	m_network = std::make_unique<CNetwork>(*this, *m_commandHandler, m_model, socketFactory);
	m_commandHandler->DoOnNewCommand([this] (ICommand * command){
		if (m_network->IsConnected())
		{
			m_network->SendAction(*command);
		}
	});

	RegisterModelFunctions(*m_scriptHandler, m_model);
	RegisterViewFunctions(*m_scriptHandler, view);
	RegisterControllerFunctions(*m_scriptHandler, *this, view.GetAsyncFileProvider(), view.GetThreadPool());
	RegisterUI(*m_scriptHandler, view.GetUI(), view.GetTranslationManager());
	RegisterObject(*m_scriptHandler, *this, m_model, view.GetModelManager());
	m_scriptHandler->RunScript(scriptPath);
}

void CGameController::Update()
{
	m_network->Update();
	if (m_updateCallback) m_updateCallback();
	if (m_singleCallback)
	{
		m_singleCallback();
		m_singleCallback = std::function<void()>();
	}
	m_model.Update();
}

CVector3d CGameController::RayToPoint(CVector3d const& begin, CVector3d const& end, double z)
{
	CVector3d result;
	double a = (z - begin.z) / (end.z - begin.z);
	result.x = a * (end.x - begin.x) + begin.x;
	result.y = a * (end.y - begin.y) + begin.y;
	result.z = z;
	return result;
}

bool CGameController::OnLeftMouseDown(CVector3d const& begin, CVector3d const& end, int modifiers)
{
	SelectObject(begin, end, modifiers & IInput::MODIFIER_SHIFT);
	auto selected = m_model.GetSelectedObject();
	if (!selected)//selection rectangle
	{
		auto point = RayToPoint(begin, end);
		m_selectionRectangleBegin = std::make_unique<CVector2d>(point.x, point.y);
	}
	else
	{
		m_selectedObjectBeginCoords = std::make_unique<CVector3d>(selected->GetCoords());
	}
	return true;
}

bool CGameController::OnLeftMouseUp(CVector3d const& begin, CVector3d const& end, int)
{
	auto selected = m_model.GetSelectedObject();
	auto pos = RayToPoint(begin, end);
	if (m_lmbCallback && m_lmbCallback(GetNearestObject(begin, end), "Object", pos.x, pos.y, pos.z))
	{
		m_selectedObjectBeginCoords.reset();
		m_selectionRectangleBegin.reset();
		return true;
	}
	if (selected)
	{
		pos = RayToPoint(begin, end, m_selectedObjectCapturePoint.z);
		pos.z = 0;
		TryMoveSelectedObject(selected, pos);
		if (m_selectedObjectBeginCoords)
		{
			MoveObject(selected, selected->GetX() - m_selectedObjectBeginCoords->x, selected->GetY() - m_selectedObjectBeginCoords->y);
		}
	}
	else//needs a fix
	{
		if (m_selectionRectangleBegin)
		{
			SelectObjectGroup(m_selectionRectangleBegin->x, m_selectionRectangleBegin->y, pos.x, pos.y);
		}
	}
	m_selectedObjectBeginCoords.reset();
	m_selectionRectangleBegin.reset();
	return true;
}

bool CGameController::OnRightMouseDown(CVector3d const& begin, CVector3d const& end, int)
{
	auto prev = m_model.GetSelectedObject();
	SelectObject(begin, end, false);
	auto object = m_model.GetSelectedObject();
	if (!object) m_model.SelectObject(prev);
	m_selectedObjectPrevRotation = (object) ? object->GetRotation() : 0;
	m_rotationPosBegin = std::make_unique<CVector3d>(RayToPoint(begin, end));
	return !!object;
}

bool CGameController::OnRightMouseUp(CVector3d const& begin, CVector3d const& end, int)
{
	auto object = m_model.GetSelectedObject();
	double rot = object ? object->GetRotation() : 0.0;
	auto point = RayToPoint(begin, end);
	if (m_rmbCallback && m_rmbCallback(GetNearestObject(begin, end), "Object", point.x, point.y, point.z))
	{
		m_rotationPosBegin.reset();
		return true;
	}
	bool result = false;
	if (m_rotationPosBegin && object)
	{
		double rotation = 90 + (atan2(point.y - m_rotationPosBegin->y, point.x - m_rotationPosBegin->x) * 180 / M_PI);
		if (sqrt((point.x - m_rotationPosBegin->x) * (point.x - m_rotationPosBegin->x) + (point.y - m_rotationPosBegin->y) * (point.y - m_rotationPosBegin->y)) > 0.2)
		{
			object->Rotate(rotation - rot);
			result = true;
		}
		RotateObject(object, object->GetRotation() - m_selectedObjectPrevRotation);
	}
	m_rotationPosBegin.reset();
	return result;
}

bool CGameController::OnMouseMove(CVector3d const& begin, CVector3d const& end, int)
{
	auto selected = m_model.GetSelectedObject();
	if (selected && m_selectedObjectBeginCoords)
	{
		auto pos = RayToPoint(begin, end, m_selectedObjectCapturePoint.z);
		pos.z = 0;
		TryMoveSelectedObject(selected, pos);
	}
	if (selected && m_rotationPosBegin)
	{
		double rot = selected->GetRotation();
		auto point = RayToPoint(begin, end);
		double rotation = 90 + (atan2(point.y - m_rotationPosBegin->y, point.x - m_rotationPosBegin->x) * 180 / M_PI);
		if (sqrt((point.x - m_rotationPosBegin->x) * (point.x - m_rotationPosBegin->x) + (point.y - m_rotationPosBegin->y) * (point.y - m_rotationPosBegin->y)) > 0.2)
			m_model.GetSelectedObject()->Rotate(rotation - rot);
	}
	return false;
}

void CGameController::SelectObjectGroup(double beginX, double beginY, double endX, double endY)
{
	double minX = (beginX < endX) ? beginX : endX;
	double maxX = (beginX > endX) ? beginX : endX;
	double minY = (beginY < endY) ? beginY : endY;
	double maxY = (beginY > endY) ? beginY : endY;
	auto group = std::make_shared<CObjectGroup>(m_model);
	for (unsigned long i = 0; i < m_model.GetObjectCount(); ++i)
	{
		std::shared_ptr<IObject> object = m_model.Get3DObject(i);
		if (object->GetX() > minX && object->GetX() < maxX && object->GetY() > minY && object->GetY() < maxY && object->IsSelectable())
		{
			group->AddChildren(object);
		}
	}
	switch (group->GetCount())
	{
	case 0:
	{
		m_model.SelectObject(NULL);
	}break;
	case 1:
	{
		m_model.SelectObject(group->GetChild(0));
	}break;
	default:
	{
		m_model.SelectObject(group);
	}break;
	}
	if (m_selectionCallback) m_selectionCallback();
}

std::shared_ptr<IObject> CGameController::GetNearestObject(const double * start, const double * end)
{
	std::shared_ptr<IObject> selectedObject = NULL;
	double minDistance = 10000000.0;
	for (unsigned long i = 0; i < m_model.GetObjectCount(); ++i)
	{
		std::shared_ptr<IObject> object = m_model.Get3DObject(i);
		if (!object) continue;
		std::shared_ptr<IBounding> bounding = m_model.GetBoundingBox(object->GetPathToModel());
		if (!bounding) continue;
		if (bounding->IsIntersectsRay(start, end, object->GetX(), object->GetY(), object->GetZ(), object->GetRotation(), m_selectedObjectCapturePoint))
		{
			double distance = sqrt(object->GetX() * object->GetX() + object->GetY() * object->GetY() + object->GetZ() * object->GetZ());
			if (distance < minDistance)
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

void CGameController::SelectObject(const double * begin, const double * end, bool add, bool noCallback /*= false*/)
{
	std::shared_ptr<IObject> selectedObject = GetNearestObject(begin, end);
	if (selectedObject && !selectedObject->IsSelectable())
	{
		return;
	}
	std::shared_ptr<IObject> object = m_model.GetSelectedObject();
	if (CGameModel::IsGroup(object.get()))
	{
		CObjectGroup * group = (CObjectGroup *)object.get();
		if (add)
		{
			if (group->ContainsChildren(selectedObject))
			{
				group->RemoveChildren(selectedObject);
				if (group->GetCount() == 1)//Destroy group
				{
					m_model.SelectObject(group->GetChild(0));
				}
			}
			else
			{
				group->AddChildren(selectedObject);
			}
		}
		else
		{
			if (!group->ContainsChildren(selectedObject))
			{
				m_model.SelectObject(selectedObject);
			}
			else
			{
				group->SetCurrent(selectedObject);
			}
		}
	}
	else
	{
		if (add && object && selectedObject)
		{
			CObjectGroup * group = new CObjectGroup(m_model);
			group->AddChildren(object);
			group->AddChildren(selectedObject);
			m_model.SelectObject(std::shared_ptr<IObject>(group));
		}
		else
		{
			m_model.SelectObject(selectedObject);
		}
	}
	if (m_selectionCallback && !noCallback) m_selectionCallback();
}

bool CGameController::IsObjectInteresectSomeObjects(std::shared_ptr<IObject> current)
{
	std::shared_ptr<IBounding> curBox = m_model.GetBoundingBox(current->GetPathToModel());
	if (!curBox) return false;
	CVector3d curPos(current->GetCoords());
	double curAngle = current->GetRotation();
	for (size_t i = 0; i < m_model.GetObjectCount(); ++i)
	{
		std::shared_ptr<IObject> object = m_model.Get3DObject(i);
		if (!object) continue;
		std::shared_ptr<IBounding> bounding = m_model.GetBoundingBox(object->GetPathToModel());
		if (!bounding) continue;
		CVector3d pos(object->GetCoords());
		double angle = object->GetRotation();
		CObjectGroup * group = static_cast<CObjectGroup*>(current.get());
		bool isGroupChild = m_model.IsGroup(current.get()) && group->ContainsChildren(object);
		if (current != object && !isGroupChild && IsInteresect(curBox.get(), curPos, curAngle, bounding.get(), pos, angle))
		{
			return true;
		}
	}
	return false;
}

bool CGameController::TestRay(double *origin, double *dir, IObject * shooter, IObject* target)
{
	CVector3d coords;
	for (size_t i = 0; i < m_model.GetObjectCount(); ++i)
	{
		IObject * current = m_model.Get3DObject(i).get();
		if (current == shooter || current == target) continue;
		IBounding * box = m_model.GetBoundingBox(current->GetPathToModel()).get();
		if (!box) continue;
		if (box->IsIntersectsRay(origin, dir, current->GetX(), current->GetY(), current->GetZ(), current->GetRotation(), coords))
		{
			return false;
		}
	}
	return true;
}

int CGameController::BBoxlos(double origin[3], IBounding * target, IObject * shooter, IObject * targetObject)
{
	int result = 0;
	int total = 0;
	CBoundingBox * tarBox = dynamic_cast<CBoundingBox *>(target);
	if (!tarBox)
	{
		CBoundingCompound * compound = dynamic_cast<CBoundingCompound*>(target);
		if (compound)
		{
			for (size_t i = 0; i < compound->GetChildCount(); ++i)
			{
				result += BBoxlos(origin, compound->GetChild(i), shooter, targetObject);
			}
			result /= compound->GetChildCount();
		}
		total = 100;
	}
	else
	{
		double dir[3];
		for (dir[0] = tarBox->GetMin()[0] + targetObject->GetX(); dir[0] < tarBox->GetMax()[0] + targetObject->GetX(); dir[0] += (tarBox->GetMax()[0] - tarBox->GetMin()[0]) / 10.0 + 0.0001)
		{
			for (dir[1] = tarBox->GetMin()[1] + targetObject->GetY(); dir[1] < tarBox->GetMax()[1] + targetObject->GetY(); dir[1] += (tarBox->GetMax()[1] - tarBox->GetMin()[1]) / 10.0 + 0.0001)
			{
				for (dir[2] = tarBox->GetMin()[2] + targetObject->GetZ(); dir[2] < tarBox->GetMax()[2] + targetObject->GetZ(); dir[2] += (tarBox->GetMax()[2] - tarBox->GetMin()[2]) / 10.0 + 0.0001)
				{
					total++;
					if (TestRay(origin, dir, shooter, targetObject))
						result++;
				}
			}
		}
	}
	return result * 100 / total;
}

int CGameController::GetLineOfSight(IObject * shooter, IObject * target)
{
	if (!shooter || !target) return -1;
	IBounding * targetBound = m_model.GetBoundingBox(target->GetPathToModel()).get();
	double center[3] = { shooter->GetX(), shooter->GetY(), shooter->GetZ() + 2.0 };
	return BBoxlos(center, targetBound, shooter, target);
}

void CGameController::SetSelectionCallback(std::function<void()> const& onSelect)
{
	m_selectionCallback = onSelect;
}

void CGameController::PackProperties(std::map<std::string, std::string> const&properties, IWriteMemoryStream & stream)
{
	stream.WriteSizeT(properties.size());
	for (auto i = properties.begin(); i != properties.end(); ++i)
	{
		stream.WriteString(i->first);
		stream.WriteString(i->second);
	}
}

void CGameController::SerializeState(IWriteMemoryStream & stream, bool hasAdresses /*= false*/) const
{
	size_t count = m_model.GetObjectCount();
	stream.WriteSizeT(count);
	for (size_t i = 0; i < count; ++i)
	{
		IObject * object = m_model.Get3DObject(i).get();
		stream.WriteDouble(object->GetX());
		stream.WriteDouble(object->GetY());
		stream.WriteDouble(object->GetZ());
		stream.WriteDouble(object->GetRotation());
		stream.WriteString(object->GetPathToModel());
		if (hasAdresses)
		{
			stream.WritePointer(object);
		}
		PackProperties(object->GetAllProperties(), stream);
	}
	PackProperties(m_model.GetAllProperties(), stream);
}

void CGameController::LoadState(IReadMemoryStream & stream, bool hasAdresses)
{
	size_t count = stream.ReadSizeT();
	m_model.Clear();
	for (size_t i = 0; i < count; ++i)
	{
		double x = stream.ReadDouble();
		double y = stream.ReadDouble();
		double z = stream.ReadDouble();
		double rotation = stream.ReadDouble();
		std::string path = stream.ReadString();
		std::shared_ptr<IObject> object = std::shared_ptr<IObject>(new CObject(path, x, y, z, rotation));
		m_model.AddObject(object);
		if (hasAdresses)
		{
			unsigned int address = reinterpret_cast<unsigned int>(stream.ReadPointer());
			m_network->AddAddress(object, address);
		}
		unsigned int propertiesCount = stream.ReadSizeT();
		for (unsigned int j = 0; j < propertiesCount; ++j)
		{
			std::string first = stream.ReadString();
			std::string second = stream.ReadString();
			object->SetProperty(first, second);
		}
	}
	unsigned int globalPropertiesCount = stream.ReadSizeT();
	for (unsigned int i = 0; i < globalPropertiesCount; ++i)
	{
		std::string first = stream.ReadString();
		std::string second = stream.ReadString();
		m_model.SetProperty(first, second);
	}
}

void CGameController::Save(std::string const& filename)
{
	FILE* file = fopen(filename.c_str(), "wb");
	CWriteMemoryStream stream;
	SerializeState(stream);
	fwrite(stream.GetData(), 1, stream.GetSize(), file);
	fclose(file);
}

void CGameController::Load(std::string const& filename)
{
	FILE* file = fopen(filename.c_str(), "rb");
	if (!file)
	{
		LogWriter::WriteLine("LoadState. Cannot find file " + filename);
		return;
	}
	fseek(file, 0L, SEEK_END);
	unsigned int size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	std::unique_ptr<char> data(new char[size]);
	fread(data.get(), 1, size, file);
	fclose(file);
	CReadMemoryStream stream(data.get(), size);
	LoadState(stream);
	m_network->CallStateRecievedCallback();
}

void CGameController::TryMoveSelectedObject(std::shared_ptr<IObject> object, CVector3d const& pos)
{
	if (!object)
	{
		return;
	}
	double deltaX = pos.x - object->GetX();
	double deltaY = pos.y - object->GetY();
	double deltaZ = pos.z - object->GetZ();
	if (sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ) < 0.1)
	{
		return;
	}

	CVector3d old(object->GetCoords());
	if (m_model.GetLandscape().isCoordsOnTable(pos.x, pos.y))
	{
		object->SetCoords(pos.x - m_selectedObjectCapturePoint.x, pos.y - m_selectedObjectCapturePoint.y, pos.z);
	}
	if (IsObjectInteresectSomeObjects(object))
	{
		object->SetCoords(old);
	}
}

void CGameController::SetUpdateCallback(std::function<void()> const& onUpdate)
{
	m_updateCallback = onUpdate;
}

void CGameController::SetSingleCallback(std::function<void()> const& onSingleUpdate)
{
	m_singleCallback = onSingleUpdate;
}

void CGameController::SetLMBCallback(MouseButtonCallback const& callback)
{
	m_lmbCallback = callback;
}

void CGameController::SetRMBCallback(MouseButtonCallback const& callback)
{
	m_rmbCallback = callback;
}

void CGameController::BindKey(unsigned char key, bool shift, bool ctrl, bool alt, std::function<void()> const& func)
{
	sKeyBind keybind(key, shift, ctrl, alt);
	if (func)
	{
		m_keyBindings[keybind] = func;
	}
	else
	{
		if (m_keyBindings.find(keybind) != m_keyBindings.end())
		{
			m_keyBindings.erase(keybind);
		}
	}
}

bool CGameController::OnKeyPress(unsigned char key, bool shift, bool ctrl, bool alt)
{
	sKeyBind keybind(key, shift, ctrl, alt);
	if (m_keyBindings.find(keybind) != m_keyBindings.end())
	{
		m_keyBindings[keybind]();
		return true;
	}
	return false;
}

void CGameController::MoveObject(std::shared_ptr<IObject> obj, double deltaX, double deltaY)
{
	m_commandHandler->AddNewMoveObject(obj, deltaX, deltaY);
}

void CGameController::RotateObject(std::shared_ptr<IObject> obj, double deltaRot)
{
	m_commandHandler->AddNewRotateObject(obj, deltaRot);
}

std::shared_ptr<IObject> CGameController::CreateObject(std::string const& model, double x, double y, double rotation)
{
	std::shared_ptr<IObject> object = std::make_shared<CObject>(model, x, y, 0.0, rotation);
	m_commandHandler->AddNewCreateObject(object, m_model);
	m_network->AddAddressLocal(object);
	return object;
}

void CGameController::DeleteObject(std::shared_ptr<IObject> obj)
{
	m_commandHandler->AddNewDeleteObject(obj, m_model);
}

void CGameController::SetObjectProperty(std::shared_ptr<IObject> obj, std::string const& key, std::string const& value)
{
	m_commandHandler->AddNewChangeProperty(obj, key, value);
}

void CGameController::PlayObjectAnimation(std::shared_ptr<IObject> object, std::string const& animation, eAnimationLoopMode loopMode, float speed)
{
	m_commandHandler->AddNewPlayAnimation(object, animation, loopMode, speed);
}

void CGameController::ObjectGoTo(std::shared_ptr<IObject> object, double x, double y, double speed, std::string const& animation, float animationSpeed)
{
	m_commandHandler->AddNewGoTo(object, x, y, speed, animation, animationSpeed);
}

CCommandHandler & CGameController::GetCommandHandler()
{
	return *m_commandHandler;
}

CNetwork& CGameController::GetNetwork()
{
	return *m_network;
}

bool operator< (CGameController::sKeyBind const& one, CGameController::sKeyBind const& two)
{
	return one.key < two.key;
}