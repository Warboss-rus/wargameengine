#include "Controller.h"
#define _USE_MATH_DEFINES
#include "../LogWriter.h"
#include "../MemoryStream.h"
#include "../Utils.h"
#include "../model/Object.h"
#include "../model/ObjectGroup.h"
#include "../view/IInput.h"
#include "../view/View.h"
#include "MovementLimiter.h"
#include "ScriptRegisterFunctions.h"
#include <float.h>
#include <math.h>

namespace wargameEngine
{
namespace controller
{
Controller::Controller(model::Model& model, IScriptHandler& scriptHandler, IPhysicsEngine& physicsEngine, IPathfinding& pathFinder, model::IBoundingBoxManager& boundingManager)
	: m_model(model)
	, m_physicsEngine(physicsEngine)
	, m_boundingManager(boundingManager)
	, m_scriptHandler(scriptHandler)
	, m_pathFinder(pathFinder)
{
	m_model.DoOnObjectCreation(std::bind(&IPhysicsEngine::AddDynamicObject, &m_physicsEngine, std::placeholders::_1, 0.0));
	m_model.DoOnObjectRemove(std::bind(&IPhysicsEngine::RemoveObject, &m_physicsEngine, std::placeholders::_1));
	m_destroyThread = false;
}

Controller::~Controller()
{
	m_destroyThread = true;
	if (m_controllerThread.get_id() != std::thread::id())
	{
		m_controllerThread.join();
	}
}

void Controller::Init(view::View& view, std::function<std::unique_ptr<INetSocket>()> const& socketFactory, const Path& scriptPath, AsyncFileProvider& asyncFileProvider)
{
	m_network = std::make_unique<Network>(*this, m_commandHandler, m_model, socketFactory);
	m_commandHandler.DoOnNewCommand([this](ICommand* command) {
		if (m_network->IsConnected())
		{
			m_network->SendAction(*command);
		}
	});
	m_physicsEngine.Reset(m_boundingManager);

	m_scriptHandler.Reset();
	RegisterModelFunctions(m_scriptHandler, m_model);
	RegisterViewFunctions(m_scriptHandler, view, asyncFileProvider);
	RegisterControllerFunctions(m_scriptHandler, *this, asyncFileProvider, view.GetThreadPool());
	RegisterUI(m_scriptHandler, view.GetUI(), view.GetTranslationManager());
	RegisterObject(m_scriptHandler, *this, m_model, view.GetModelManager());
	RegisterViewport(m_scriptHandler, view);
	{
		auto lock = m_model.LockModel();
		m_scriptHandler.RunScript(scriptPath);
	}

	m_lastUpdateTime = std::chrono::high_resolution_clock::now();
}

void Controller::InitAsync(view::View& view, std::function<std::unique_ptr<INetSocket>()> const& socketFactory, const Path& scriptPath, AsyncFileProvider& asyncFileProvider)
{
	m_controllerThread = std::thread([this, &view, socketFactory, scriptPath, &asyncFileProvider] {
		Init(view, socketFactory, scriptPath, asyncFileProvider);
		auto lastUpdateTime = std::chrono::high_resolution_clock::now();
		while (!m_destroyThread)
		{
			Update();
			std::this_thread::sleep_until(lastUpdateTime + m_updatePeriod);
		}
	});
}

void Controller::Update()
{
	{
		std::unique_lock<std::mutex> lk(m_taskMutex);
		while (!m_tasks.empty())
		{
			auto task = m_tasks.front();
			m_tasks.pop_front();
			lk.unlock();
			task();
			lk.lock();
		}
	}
	m_network->Update();
	if (m_updateCallback)
		m_updateCallback();
	if (m_singleCallback)
	{
		m_singleCallback();
		m_singleCallback = std::function<void()>();
	}
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto delta = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastUpdateTime);
	m_lastUpdateTime = currentTime;
	for (auto& decorator : m_objectDecorators)
	{
		decorator.second->Update(delta);
	}
	m_model.Update(delta);
	m_physicsEngine.Update(delta);
}

CVector3f Controller::RayToPoint(CVector3f const& begin, CVector3f const& end, float z)
{
	CVector3f result;
	float a = (z - begin.z) / (end.z - begin.z);
	result.x = a * (end.x - begin.x) + begin.x;
	result.y = a * (end.y - begin.y) + begin.y;
	result.z = z;
	return result;
}

bool Controller::OnLeftMouseDown(CVector3f const& begin, CVector3f const& end, int modifiers)
{
	SelectObject(begin, end, modifiers & view::IInput::MODIFIER_SHIFT);
	auto selected = m_model.GetSelectedObject();
	if (!selected) //selection rectangle
	{
		auto point = RayToPoint(begin, end);
		m_selectionRectangleBegin = std::make_unique<CVector2d>(point.x, point.y);
	}
	else
	{
		m_selectedObjectBeginCoords = std::make_unique<CVector3f>(selected->GetCoords());
	}
	return selected.get() != nullptr;
}

bool Controller::OnLeftMouseUp(CVector3f const& begin, CVector3f const& end, int)
{
	auto selected = m_model.GetSelectedObject();
	auto pos = RayToPoint(begin, end);
	if (m_lmbCallback && m_lmbCallback(GetNearestObject(begin, end), L"Object", pos.x, pos.y, pos.z))
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
	else //needs a fix
	{
		if (m_selectionRectangleBegin)
		{
			SelectObjectGroup(m_selectionRectangleBegin->x, m_selectionRectangleBegin->y, pos.x, pos.y);
		}
	}
	m_selectedObjectBeginCoords.reset();
	m_selectionRectangleBegin.reset();
	return selected.get() != nullptr;
}

bool Controller::OnRightMouseDown(CVector3f const& begin, CVector3f const& end, int)
{
	auto prev = m_model.GetSelectedObject();
	SelectObject(begin, end, false);
	auto object = m_model.GetSelectedObject();
	if (!object)
		m_model.SelectObject(prev);
	m_selectedObjectPrevRotation = (object) ? object->GetRotation() : 0;
	m_rotationPosBegin = std::make_unique<CVector3f>(RayToPoint(begin, end));
	return !!object;
}

bool Controller::OnRightMouseUp(CVector3f const& begin, CVector3f const& end, int)
{
	auto object = m_model.GetSelectedObject();
	float rot = object ? object->GetRotation() : 0.0f;
	auto point = RayToPoint(begin, end);
	if (m_rmbCallback && m_rmbCallback(GetNearestObject(begin, end), L"Object", point.x, point.y, point.z))
	{
		m_rotationPosBegin.reset();
		return true;
	}
	bool result = false;
	if (m_rotationPosBegin && object)
	{
		float rotation = 90.0f + static_cast<float>(atan2(point.y - m_rotationPosBegin->y, point.x - m_rotationPosBegin->x) * 180.0f / (float)M_PI);
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

bool Controller::OnMouseMove(CVector3f const& begin, CVector3f const& end, int)
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
		float rot = selected->GetRotation();
		auto point = RayToPoint(begin, end);
		float rotation = 90 + static_cast<float>(atan2(point.y - m_rotationPosBegin->y, point.x - m_rotationPosBegin->x) * 180.0f / (float)M_PI);
		if (sqrt((point.x - m_rotationPosBegin->x) * (point.x - m_rotationPosBegin->x) + (point.y - m_rotationPosBegin->y) * (point.y - m_rotationPosBegin->y)) > 0.2)
			m_model.GetSelectedObject()->Rotate(rotation - rot);
	}
	return false;
}

bool Controller::OnGamepadButtonStateChange(int gamepadIndex, int buttonIndex, bool newState)
{
	m_onGamepadButton(gamepadIndex, buttonIndex, newState);
	return m_onGamepadButton;
}

bool Controller::OnGamepadAxisChange(int gamepadIndex, int axisIndex, double horizontal, double vertical)
{
	m_onGamepadAxis(gamepadIndex, axisIndex, horizontal, vertical);
	return m_onGamepadAxis;
}

void Controller::SelectObjectGroup(double beginX, double beginY, double endX, double endY)
{
	double minX = (beginX < endX) ? beginX : endX;
	double maxX = (beginX > endX) ? beginX : endX;
	double minY = (beginY < endY) ? beginY : endY;
	double maxY = (beginY > endY) ? beginY : endY;
	auto group = std::make_shared<model::ObjectGroup>(m_model);
	for (size_t i = 0; i < m_model.GetObjectCount(); ++i)
	{
		std::shared_ptr<model::IObject> object = m_model.Get3DObject(i);
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
	}
	break;
	case 1:
	{
		m_model.SelectObject(group->GetChild(0));
	}
	break;
	default:
	{
		m_model.SelectObject(group);
	}
	break;
	}
	if (m_selectionCallback)
		m_selectionCallback();
}

std::shared_ptr<model::IObject> Controller::GetNearestObject(const float* start, const float* end)
{
	auto result = m_physicsEngine.CastRay(CVector3f(start), CVector3f(end));
	if (result.success)
	{
		m_selectedObjectCapturePoint = result.hitPoint;
		return m_model.Get3DObject(result.object);
	}
	return nullptr;
}

void Controller::SelectObject(const float* begin, const float* end, bool add, bool noCallback /*= false*/)
{
	std::shared_ptr<model::IObject> selectedObject = GetNearestObject(begin, end);
	if (selectedObject && !selectedObject->IsSelectable())
	{
		return;
	}
	std::shared_ptr<model::IObject> object = m_model.GetSelectedObject();
	if (object && object->IsGroup())
	{
		model::ObjectGroup* group = (model::ObjectGroup*)object.get();
		if (add)
		{
			if (group->ContainsChildren(selectedObject))
			{
				group->RemoveChildren(selectedObject);
				if (group->GetCount() == 1) //Destroy group
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
			auto group = std::make_shared<model::ObjectGroup>(m_model);
			group->AddChildren(object);
			group->AddChildren(selectedObject);
			m_model.SelectObject(group);
		}
		else
		{
			m_model.SelectObject(selectedObject);
		}
	}
	if (m_selectionCallback && !noCallback)
		m_selectionCallback();
}

size_t Controller::BBoxlos(CVector3f const& origin, model::Bounding* target, model::IObject* shooter, model::IObject* targetObject)
{
	size_t result = 0;
	size_t total = 0;
	if (target->type == model::Bounding::eType::Compound)
	{
		model::Bounding::Compound compound = target->GetCompound();
		for (size_t i = 0; i < compound.items.size(); ++i)
		{
			result += BBoxlos(origin, &compound.items[i], shooter, targetObject);
		}
		result /= compound.items.size();
		total = 100;
	}
	else
	{
		model::Bounding::Box const& tarBox = target->GetBox();
		CVector3f dir;
		for (dir.x = tarBox.min[0] + targetObject->GetX(); dir.x < tarBox.max[0] + targetObject->GetX(); dir.x += (tarBox.max[0] - tarBox.min[0]) / 10.0f + 0.0001f)
		{
			for (dir.y = tarBox.min[1] + targetObject->GetY(); dir.y < tarBox.max[1] + targetObject->GetY(); dir.y += (tarBox.max[1] - tarBox.min[1]) / 10.0f + 0.0001f)
			{
				for (dir.z = tarBox.min[2] + targetObject->GetZ(); dir.z < tarBox.max[2] + targetObject->GetZ(); dir.z += (tarBox.max[2] - tarBox.min[2]) / 10.0f + 0.0001f)
				{
					total++;
					if (!m_physicsEngine.CastRay(origin, dir, { shooter, targetObject }).success)
						result++;
				}
			}
		}
	}
	return result * 100 / total;
}

size_t Controller::GetLineOfSight(model::IObject* shooter, model::IObject* target)
{
	if (!shooter || !target)
		return 0;
	model::Bounding targetBound = m_boundingManager.GetBounding(target->GetPathToModel());
	CVector3f center = shooter->GetCoords();
	center.z += 2.0f;
	return BBoxlos(center, &targetBound, shooter, target);
}

void Controller::SetSelectionCallback(std::function<void()> const& onSelect)
{
	m_selectionCallback = onSelect;
}

void Controller::PackProperties(std::unordered_map<std::wstring, std::wstring> const& properties, IWriteMemoryStream& stream)
{
	stream.WriteSizeT(properties.size());
	for (auto i = properties.begin(); i != properties.end(); ++i)
	{
		stream.WriteWString(i->first);
		stream.WriteWString(i->second);
	}
}

void Controller::SerializeState(IWriteMemoryStream& stream, bool hasAdresses /*= false*/) const
{
	size_t count = m_model.GetObjectCount();
	stream.WriteSizeT(count);
	for (size_t i = 0; i < count; ++i)
	{
		model::IObject* object = m_model.Get3DObject(i).get();
		stream.WriteFloat(object->GetX());
		stream.WriteFloat(object->GetY());
		stream.WriteFloat(object->GetZ());
		stream.WriteFloat(object->GetRotation());
		stream.WriteString(to_string(object->GetPathToModel()));
		if (hasAdresses)
		{
			stream.WritePointer(object);
		}
		PackProperties(object->GetAllProperties(), stream);
	}
	PackProperties(m_model.GetAllProperties(), stream);
}

void Controller::LoadState(IReadMemoryStream& stream, bool hasAdresses)
{
	size_t count = stream.ReadSizeT();
	m_model.Clear();
	for (size_t i = 0; i < count; ++i)
	{
		float x = stream.ReadFloat();
		float y = stream.ReadFloat();
		float z = stream.ReadFloat();
		float rotation = stream.ReadFloat();
		Path path = make_path(stream.ReadString());
		std::shared_ptr<model::IObject> object = std::make_shared<model::Object>(path, CVector3f{ x, y, z }, rotation);
		m_model.AddObject(object);
		if (hasAdresses)
		{
			m_network->AddAddress(object, stream.ReadPointer());
		}
		size_t propertiesCount = stream.ReadSizeT();
		for (size_t j = 0; j < propertiesCount; ++j)
		{
			std::wstring first = stream.ReadWString();
			std::wstring second = stream.ReadWString();
			object->SetProperty(first, second);
		}
	}
	size_t globalPropertiesCount = stream.ReadSizeT();
	for (size_t i = 0; i < globalPropertiesCount; ++i)
	{
		std::wstring first = stream.ReadWString();
		std::wstring second = stream.ReadWString();
		m_model.SetProperty(first, second);
	}
}

void Controller::Save(const Path& filename)
{
	WriteMemoryStream stream;
	SerializeState(stream);
	WriteFile(filename, stream.GetData(), stream.GetSize());
}

void Controller::Load(const Path& filename)
{
	std::vector<char> data = ReadFile(filename);
	ReadMemoryStream stream(data.data());
	LoadState(stream);
	m_network->CallStateRecievedCallback();
}

void Controller::TryMoveSelectedObject(std::shared_ptr<model::IObject> const& object, CVector3f const& pos)
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

	CVector3f old(object->GetCoords());
	if (m_model.GetLandscape().isCoordsOnTable(pos.x, pos.y))
	{
		object->SetCoords(pos.x - m_selectedObjectCapturePoint.x, pos.y - m_selectedObjectCapturePoint.y, pos.z);
	}
	if (m_physicsEngine.TestObject(object.get()))
	{
		object->SetCoords(old);
	}
}

void Controller::SetUpdateCallback(std::function<void()> const& onUpdate)
{
	m_updateCallback = onUpdate;
}

void Controller::SetSingleCallback(std::function<void()> const& onSingleUpdate)
{
	m_singleCallback = onSingleUpdate;
}

void Controller::SetLMBCallback(MouseButtonCallback const& callback)
{
	m_lmbCallback = callback;
}

void Controller::SetRMBCallback(MouseButtonCallback const& callback)
{
	m_rmbCallback = callback;
}

void Controller::SetGamepadButtonCallback(std::function<bool(int gamepadIndex, int buttonIndex, bool newState)> const& handler)
{
	m_onGamepadButton.Connect(handler);
}

void Controller::SetGamepadAxisCallback(std::function<bool(int gamepadIndex, int axisIndex, double horizontal, double vertical)> const& handler)
{
	m_onGamepadAxis.Connect(handler);
}

void Controller::BindKey(unsigned char key, bool shift, bool ctrl, bool alt, std::function<void()> const& func)
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

bool Controller::OnKeyPress(unsigned char key, bool shift, bool ctrl, bool alt)
{
	sKeyBind keybind(key, shift, ctrl, alt);
	if (m_keyBindings.find(keybind) != m_keyBindings.end())
	{
		m_keyBindings[keybind]();
		return true;
	}
	return false;
}

void Controller::MoveObject(std::shared_ptr<model::IObject> const& obj, float deltaX, float deltaY)
{
	m_commandHandler.AddNewMoveObject(obj, deltaX, deltaY);
}

void Controller::RotateObject(std::shared_ptr<model::IObject> const& obj, float deltaRot)
{
	m_commandHandler.AddNewRotateObject(obj, deltaRot);
}

std::shared_ptr<model::IObject> Controller::CreateObject(const Path& model, float x, float y, float rotation)
{
	std::shared_ptr<model::IObject> object = std::make_shared<model::Object>(model, CVector3f{ x, y, 0.0f }, rotation);
	m_commandHandler.AddNewCreateObject(object, m_model);
	m_network->AddAddressLocal(object);
	return object;
}

void Controller::DeleteObject(std::shared_ptr<model::IObject> const& obj)
{
	m_commandHandler.AddNewDeleteObject(obj, m_model);
}

void Controller::SetObjectProperty(std::shared_ptr<model::IObject> const& obj, std::wstring const& key, std::wstring const& value)
{
	m_commandHandler.AddNewChangeProperty(obj, key, value);
}

void Controller::PlayObjectAnimation(std::shared_ptr<model::IObject> const& object, std::string const& animation, model::AnimationLoop loopMode, float speed)
{
	m_commandHandler.AddNewPlayAnimation(object, animation, loopMode, speed);
}

void Controller::ObjectGoTo(std::shared_ptr<model::IObject> const& object, float x, float y, float speed, std::string const& animation, float animationSpeed)
{
	m_commandHandler.AddNewGoTo(GetDecorator(object), x, y, speed, animation, animationSpeed);
}

void Controller::ObjectMovePath(std::shared_ptr<model::IObject> const& object, const std::vector<MovePathNode>& path)
{
	GetDecorator(object)->MovePath(path);
}

void Controller::SetMovementLimiter(std::shared_ptr<model::IObject> const& object, std::unique_ptr<IMoveLimiter>&& limiter)
{
	GetDecorator(object)->SetLimiter(std::move(limiter));
}

CommandHandler& Controller::GetCommandHandler()
{
	return m_commandHandler;
}

Network& Controller::GetNetwork()
{
	return *m_network;
}

std::shared_ptr<ObjectDecorator> Controller::GetDecorator(std::shared_ptr<model::IObject> const& object)
{
	if (m_objectDecorators.find(object.get()) == m_objectDecorators.end())
	{
		m_objectDecorators[object.get()] = std::make_shared<ObjectDecorator>(object);
	}
	return m_objectDecorators[object.get()];
}

void Controller::QueueTask(std::function<void()> const& handler)
{
	std::unique_lock<std::mutex> lk(m_taskMutex);
	m_tasks.push_back(handler);
}

bool operator<(Controller::sKeyBind const& one, Controller::sKeyBind const& two)
{
	return one.key < two.key;
}

ObjectDecorator::ObjectDecorator(std::shared_ptr<model::IObject> const& object)
	: m_object(object)
	, m_goSpeed(0.0f)
{
	auto fixPosition = [this](CVector3f position, CVector3f rotation, const CVector3f& oldPosition, const CVector3f& oldRotation) {
		if (m_limiter && !m_limiter->FixPosition(position, rotation, oldPosition, oldRotation))
		{
			m_object->SetCoords(position);
			m_object->SetRotations(rotation);
		}
	};

	m_positionChangeConnection = object->DoOnCoordsChange([fixPosition, this](const CVector3f& oldPosition, const CVector3f& newPosition) {
		fixPosition(newPosition, m_object->GetRotations(), oldPosition, m_object->GetRotations());
	});
	m_rotationChangeConnection = object->DoOnRotationChange([fixPosition, this](const CVector3f& oldRotations, const CVector3f& newRotations) {
		fixPosition(m_object->GetCoords(), newRotations, m_object->GetCoords(), oldRotations);
	});
}

ObjectDecorator::~ObjectDecorator() = default;

void ObjectDecorator::GoTo(CVector3f const& coords, float speed, std::string const& animation, float animationSpeed)
{
	m_goTarget = coords;
	m_goSpeed = speed;
	m_object->PlayAnimation(animation, model::AnimationLoop::Looping, animationSpeed);
}

void ObjectDecorator::MovePath(const std::vector<MovePathNode>& path)
{
	m_movePath.assign(path.begin(), path.end());
	m_movePathDuration = std::chrono::duration<float>();
}

void ObjectDecorator::SetLimiter(std::unique_ptr<IMoveLimiter>&& limiter)
{
	m_limiter = std::move(limiter);
}

model::IObject* ObjectDecorator::GetObject()
{
	return m_object.get();
}

void ObjectDecorator::Update(std::chrono::duration<float> timeSinceLastUpdate)
{
	if (fabs(m_goSpeed) >= DBL_EPSILON)
	{
		CVector3f dir = m_goTarget - m_object->GetCoords();
		dir.Normalize();
		m_object->SetRotation(static_cast<float>(atan2(dir.y, dir.x) * 180.0f / (float)M_PI));
		dir = dir * timeSinceLastUpdate.count() * m_goSpeed;
		if (dir.GetLength() > (m_goTarget - m_object->GetCoords()).GetLength())
			dir = (m_goTarget - m_object->GetCoords());
		m_object->Move(dir.x, dir.y, dir.z);
		if ((m_object->GetCoords() - m_goTarget).GetLength() < 0.0001)
		{
			m_goSpeed = 0.0;
			m_object->PlayAnimation("", model::AnimationLoop::NonLooping, 0.0f);
		}
	}
	if (!m_movePath.empty())
	{
		m_movePathDuration += timeSinceLastUpdate;
		if (m_movePathDuration.count() < m_movePath.front().timePoint)
		{
			return;
		}
		while (m_movePath.size() > 1 && m_movePathDuration.count() > m_movePath[1].timePoint)
		{
			m_movePath.pop_front();
		}
		if (m_movePath.size() < 2)
		{
			m_movePath.clear();
			return;
		}
		const auto& begin = m_movePath.front();
		const auto& end = m_movePath[1];
		const float interpolationCoeff = (m_movePathDuration.count() - begin.timePoint) / (end.timePoint - begin.timePoint);
		auto position = end.position * interpolationCoeff + begin.position * (1.0f - interpolationCoeff);
		auto rotation = end.rotation * interpolationCoeff + begin.rotation * (1.0f - interpolationCoeff);
		m_object->SetCoords(position);
		m_object->SetRotations(rotation);
	}
}
}
}