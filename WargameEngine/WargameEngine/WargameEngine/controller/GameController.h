#pragma once
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include "IStateManager.h"
#include "IScriptHandler.h"
#include "CommandHandler.h"
#include "../model/GameModel.h"
#include "../view/Vector3.h"
#include "Network.h"
#include "../Signal.h"
#include "../IPhysicsEngine.h"
#include <thread>
#include <atomic>
#include <deque>

class CGameView;

class CObjectDecorator
{
public:
	CObjectDecorator(std::shared_ptr<IObject> const& object);
	void GoTo(CVector3f const& coords, float speed, std::string const& animation, float animationSpeed);
	IObject* GetObject();
	void Update(std::chrono::duration<float> timeSinceLastUpdate);
private:
	std::shared_ptr<IObject> m_object;
	CVector3f m_goTarget;
	float m_goSpeed;
};

class CGameController : public IStateManager
{
public:
	typedef std::function<bool(std::shared_ptr<IObject> const& obj, std::wstring const& type, double x, double y, double z)> MouseButtonCallback;

	CGameController(CGameModel& model, std::unique_ptr<IScriptHandler> && scriptHandler, IPhysicsEngine & physicsEngine);
	~CGameController();
	void Init(CGameView & view, std::function<std::unique_ptr<INetSocket>()> const& socketFactory, const Path& scriptPath);
	void InitAsync(CGameView & view, std::function<std::unique_ptr<INetSocket>()> const& socketFactory, const Path& scriptPath);
	void Update();

	virtual void SerializeState(IWriteMemoryStream & stream, bool hasAdresses = false) const override;
	virtual void LoadState(IReadMemoryStream & stream, bool hasAdresses = false) override;

	bool OnLeftMouseDown(CVector3f const& begin, CVector3f const& end, int modifiers);
	bool OnLeftMouseUp(CVector3f const& begin, CVector3f const& end, int modifiers);
	bool OnRightMouseDown(CVector3f const& begin, CVector3f const& end, int modifiers);
	bool OnRightMouseUp(CVector3f const& begin, CVector3f const& end, int modifiers);
	bool OnMouseMove(CVector3f const& begin, CVector3f const& end, int modifiers);
	bool OnGamepadButtonStateChange(int gamepadIndex, int buttonIndex, bool newState);
	bool OnGamepadAxisChange(int gamepadIndex, int axisIndex, double horizontal, double vertical);
	size_t GetLineOfSight(IObject * shooter, IObject * target);
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
	std::shared_ptr<IObject> CreateObject(const Path& model, float x, float y, float rotation);
	void DeleteObject(std::shared_ptr<IObject> const& obj);
	void SetObjectProperty(std::shared_ptr<IObject> const& obj, std::wstring const& key, std::wstring const& value);
	void PlayObjectAnimation(std::shared_ptr<IObject> const& object, std::string const& animation, eAnimationLoopMode loopMode, float speed);
	void ObjectGoTo(std::shared_ptr<IObject> const& object, float x, float y, float speed, std::string const& animation, float animationSpeed);
	CCommandHandler & GetCommandHandler();
	CNetwork& GetNetwork();
	std::shared_ptr<CObjectDecorator> GetDecorator(std::shared_ptr<IObject> const& object);
	void QueueTask(std::function<void()> const& handler);
private:
	struct sKeyBind
	{
		sKeyBind(unsigned char k, bool s, bool c, bool a) :key(k), shift(s), ctrl(c), alt(a) {}
		unsigned char key;
		bool shift;
		bool ctrl;
		bool alt;
	};
	CGameController(CGameController const&) = delete;
	std::shared_ptr<IObject> GetNearestObject(const float * start, const float * end);
	void SelectObjectGroup(double beginX, double beginY, double endX, double endY);
	void SelectObject(const float * begin, const float * end, bool add, bool noCallback = false);
	void TryMoveSelectedObject(std::shared_ptr<IObject> const& object, CVector3f const& pos);
	void MoveObject(std::shared_ptr<IObject> const& obj, float deltaX, float deltaY);
	void RotateObject(std::shared_ptr<IObject> const& obj, float deltaRot);
	size_t BBoxlos(CVector3f const& origin, sBounding * target, IObject * shooter, IObject * targetObject);
	CVector3f RayToPoint(CVector3f const& begin, CVector3f const& end, float z = 0);
	static void PackProperties(std::map<std::wstring, std::wstring> const&properties, IWriteMemoryStream & stream);

	CGameModel& m_model;
	CGameView* m_view;
	IPhysicsEngine & m_physicsEngine;
	CVector3f m_selectedObjectCapturePoint;
	std::unique_ptr<CVector3f> m_selectedObjectBeginCoords;
	std::unique_ptr<CVector2d> m_selectionRectangleBegin;
	float m_selectedObjectPrevRotation = 0;
	std::unique_ptr<CVector3f> m_rotationPosBegin;
	std::chrono::high_resolution_clock::time_point m_lastUpdateTime;

	std::unique_ptr<IScriptHandler> m_scriptHandler;
	std::unique_ptr<CCommandHandler> m_commandHandler;
	std::unique_ptr<CNetwork> m_network;

	std::map<sKeyBind, std::function<void()>> m_keyBindings;
	std::function<void()> m_selectionCallback;
	std::function<void()> m_updateCallback;
	std::function<void()>m_singleCallback;
	CExclusiveSignal<int, int, bool> m_onGamepadButton;
	CExclusiveSignal<int, int, double, double> m_onGamepadAxis;
	MouseButtonCallback m_lmbCallback;
	MouseButtonCallback m_rmbCallback;
	std::map<IObject*, std::shared_ptr<CObjectDecorator>> m_objectDecorators;

	std::thread m_controllerThread;
	std::atomic_bool m_destroyThread;
	std::chrono::milliseconds m_updatePeriod;
	std::deque<std::function<void()>> m_tasks;
	std::mutex m_taskMutex;

	friend bool operator< (sKeyBind const& one, sKeyBind const& two);
};

bool operator< (CGameController::sKeyBind const& one, CGameController::sKeyBind const& two);