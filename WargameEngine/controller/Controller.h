#pragma once
#include "../IPhysicsEngine.h"
#include "../IScriptHandler.h"
#include "../Signal.h"
#include "../model/IBoundingBoxManager.h"
#include "../model/Model.h"
#include "../view/Vector3.h"
#include "CommandHandler.h"
#include "IStateManager.h"
#include "Network.h"
#include <atomic>
#include <deque>
#include <functional>
#include <unordered_map>
#include <memory>
#include <thread>
#include <vector>
#include <map>

namespace wargameEngine
{
class IPathfinding;
class AsyncFileProvider;

namespace view
{
class View;
}

namespace controller
{
class IMoveLimiter;

struct MovePathNode
{
	CVector3f position;
	CVector3f rotation;
	float timePoint = 0.0f;
};

class ObjectDecorator
{
public:
	ObjectDecorator(std::shared_ptr<model::IObject> const& object);
	~ObjectDecorator();
	void GoTo(CVector3f const& coords, float speed, std::string const& animation, float animationSpeed);
	void MovePath(const std::vector<MovePathNode>& path);
	void SetLimiter(std::unique_ptr<IMoveLimiter>&& limiter);
	model::IObject* GetObject();
	void Update(std::chrono::duration<float> timeSinceLastUpdate);

private:
	std::shared_ptr<model::IObject> m_object;
	CVector3f m_goTarget;
	float m_goSpeed;
	std::unique_ptr<IMoveLimiter> m_limiter;
	signals::ScopedConnection m_positionChangeConnection;
	signals::ScopedConnection m_rotationChangeConnection;
	std::deque<MovePathNode> m_movePath;
	std::chrono::duration<float> m_movePathDuration;
};

class Controller : public IStateManager
{
public:
	typedef std::function<bool(std::shared_ptr<model::IObject> const& obj, std::string const& type, double x, double y, double z)> MouseButtonCallback;

	Controller(model::Model& model, IScriptHandler& scriptHandler, IPhysicsEngine& physicsEngine, IPathfinding& pathFinder, model::IBoundingBoxManager& boundingManager);
	~Controller();
	void Init(view::View& view, std::function<std::unique_ptr<INetSocket>()> const& socketFactory, const Path& scriptPath, AsyncFileProvider& asyncFileProvider);
	void InitAsync(view::View& view, std::function<std::unique_ptr<INetSocket>()> const& socketFactory, const Path& scriptPath, AsyncFileProvider& asyncFileProvider);
	void Update();

	virtual void SerializeState(IWriteMemoryStream& stream, bool hasAdresses = false) const override;
	virtual void LoadState(IReadMemoryStream& stream, bool hasAdresses = false) override;

	bool OnLeftMouseDown(CVector3f const& begin, CVector3f const& end, int modifiers);
	bool OnLeftMouseUp(CVector3f const& begin, CVector3f const& end, int modifiers);
	bool OnRightMouseDown(CVector3f const& begin, CVector3f const& end, int modifiers);
	bool OnRightMouseUp(CVector3f const& begin, CVector3f const& end, int modifiers);
	bool OnMouseMove(CVector3f const& begin, CVector3f const& end, int modifiers);
	bool OnGamepadButtonStateChange(int gamepadIndex, int buttonIndex, bool newState);
	bool OnGamepadAxisChange(int gamepadIndex, int axisIndex, double horizontal, double vertical);
	size_t GetLineOfSight(model::IObject* shooter, model::IObject* target);
	void SetSelectionCallback(std::function<void()> const& onSelect);
	void SetUpdateCallback(std::function<void()> const& onUpdate);
	void SetSingleCallback(std::function<void()> const& onSingleUpdate);
	void SetLMBCallback(MouseButtonCallback const& callback);
	void SetRMBCallback(MouseButtonCallback const& callback);
	void SetGamepadButtonCallback(std::function<bool(int gamepadIndex, int buttonIndex, bool newState)> const& handler);
	void SetGamepadAxisCallback(std::function<bool(int gamepadIndex, int axisIndex, double horizontal, double vertical)> const& handler);
	void Save(const Path& filename);
	void Load(const Path& filename);
	void BindKey(unsigned char key, bool shift, bool ctrl, bool alt, std::function<void()> const& func);
	bool OnKeyPress(unsigned char key, bool shift, bool ctrl, bool alt);
	std::shared_ptr<model::IObject> CreateObject(const Path& model, float x, float y, float rotation);
	void DeleteObject(std::shared_ptr<model::IObject> const& obj);
	void SetObjectProperty(std::shared_ptr<model::IObject> const& obj, std::wstring const& key, std::wstring const& value);
	void PlayObjectAnimation(std::shared_ptr<model::IObject> const& object, std::string const& animation, model::AnimationLoop loopMode, float speed);
	void ObjectGoTo(std::shared_ptr<model::IObject> const& object, float x, float y, float speed, std::string const& animation, float animationSpeed);
	void ObjectMovePath(std::shared_ptr<model::IObject> const& object, const std::vector<MovePathNode>& path);
	void SetMovementLimiter(std::shared_ptr<model::IObject> const& object, std::unique_ptr<IMoveLimiter>&& limiter);
	CommandHandler& GetCommandHandler();
	Network& GetNetwork();
	std::shared_ptr<ObjectDecorator> GetDecorator(std::shared_ptr<model::IObject> const& object);
	void QueueTask(std::function<void()> const& handler);

private:
	struct sKeyBind
	{
		sKeyBind(unsigned char k, bool s, bool c, bool a)
			: key(k)
			, shift(s)
			, ctrl(c)
			, alt(a)
		{
		}
		unsigned char key;
		bool shift;
		bool ctrl;
		bool alt;
	};
	Controller(Controller const&) = delete;
	std::shared_ptr<model::IObject> GetNearestObject(const float* start, const float* end);
	void SelectObjectGroup(double beginX, double beginY, double endX, double endY);
	void SelectObject(const float* begin, const float* end, bool add, bool noCallback = false);
	void TryMoveSelectedObject(std::shared_ptr<model::IObject> const& object, CVector3f const& pos);
	void MoveObject(std::shared_ptr<model::IObject> const& obj, float deltaX, float deltaY);
	void RotateObject(std::shared_ptr<model::IObject> const& obj, float deltaRot);
	size_t BBoxlos(CVector3f const& origin, model::Bounding* target, model::IObject* shooter, model::IObject* targetObject);
	CVector3f RayToPoint(CVector3f const& begin, CVector3f const& end, float z = 0);
	static void PackProperties(std::unordered_map<std::wstring, std::wstring> const& properties, IWriteMemoryStream& stream);

	model::Model& m_model;
	IPhysicsEngine& m_physicsEngine;
	model::IBoundingBoxManager& m_boundingManager;
	IScriptHandler& m_scriptHandler;
	IPathfinding& m_pathFinder;

	CommandHandler m_commandHandler;
	std::unique_ptr<Network> m_network;

	CVector3f m_selectedObjectCapturePoint;
	std::unique_ptr<CVector3f> m_selectedObjectBeginCoords;
	std::unique_ptr<CVector2d> m_selectionRectangleBegin;
	float m_selectedObjectPrevRotation = 0;
	std::unique_ptr<CVector3f> m_rotationPosBegin;
	std::chrono::high_resolution_clock::time_point m_lastUpdateTime;
	std::map<sKeyBind, std::function<void()>> m_keyBindings;
	std::function<void()> m_selectionCallback;
	std::function<void()> m_updateCallback;
	std::function<void()> m_singleCallback;
	signals::ExclusiveSignal<int, int, bool> m_onGamepadButton;
	signals::ExclusiveSignal<int, int, double, double> m_onGamepadAxis;
	MouseButtonCallback m_lmbCallback;
	MouseButtonCallback m_rmbCallback;
	std::unordered_map<model::IObject*, std::shared_ptr<ObjectDecorator>> m_objectDecorators;

	std::thread m_controllerThread;
	std::atomic_bool m_destroyThread;
	std::chrono::milliseconds m_updatePeriod;
	std::deque<std::function<void()>> m_tasks;
	std::mutex m_taskMutex;

	friend bool operator<(sKeyBind const& one, sKeyBind const& two);
};

bool operator<(Controller::sKeyBind const& one, Controller::sKeyBind const& two);
}
}
