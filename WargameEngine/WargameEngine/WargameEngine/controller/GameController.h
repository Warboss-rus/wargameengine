#pragma once
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include "IStateManager.h"
#include "LUAScriptHandler.h"
#include "CommandHandler.h"
#include "../model/GameModel.h"
#include "../view/Vector3.h"
#include "../Network.h"

class CGameController : public IStateManager
{
public:
	typedef std::function<bool(std::shared_ptr<IObject> obj, std::string const& type, double x, double y, double z)> MouseButtonCallback;

	CGameController(CGameModel& model);
	void Init();
	void Update();

	virtual std::vector<char> GetState(bool hasAdresses = false) const override;
	virtual void SetState(char* data, bool hasAdresses = false) override;

	bool OnLeftMouseDown(CVector3d const& begin, CVector3d const& end, int modifiers);
	bool OnLeftMouseUp(CVector3d const& begin, CVector3d const& end, int modifiers);
	bool OnRightMouseDown(CVector3d const& begin, CVector3d const& end, int modifiers);
	bool OnRightMouseUp(CVector3d const& begin, CVector3d const& end, int modifiers);
	bool OnMouseMove(CVector3d const& begin, CVector3d const& end, int modifiers);
	int GetLineOfSight(IObject * shooter, IObject * target);
	void SetSelectionCallback(std::function<void()> const& onSelect);
	void SetUpdateCallback(std::function<void()> const& onUpdate);
	void SetSingleCallback(std::function<void()> const& onSingleUpdate);
	void SetLMBCallback(MouseButtonCallback const& callback);
	void SetRMBCallback(MouseButtonCallback const& callback);
	void Save(std::string const& filename);
	void Load(std::string const& filename);
	void BindKey(unsigned char key, bool shift, bool ctrl, bool alt, std::function<void()> const& func);
	bool OnKeyPress(unsigned char key, bool shift, bool ctrl, bool alt);
	std::shared_ptr<IObject> CreateObject(std::string const& model, double x, double y, double rotation);
	void DeleteObject(std::shared_ptr<IObject> obj);
	void SetObjectProperty(std::shared_ptr<IObject> obj, std::string const& key, std::string const& value);
	void PlayObjectAnimation(std::shared_ptr<IObject> object, std::string const& animation, int loopMode, float speed);
	void ObjectGoTo(std::shared_ptr<IObject> object, double x, double y, double speed, std::string const& animation, float animationSpeed);
	CCommandHandler & GetCommandHandler();
	CNetwork& GetNetwork();
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
	std::shared_ptr<IObject> GetNearestObject(const double * start, const double * end);
	void SelectObjectGroup(double beginX, double beginY, double endX, double endY);
	void SelectObject(const double * begin, const double * end, bool add, bool noCallback = false);
	bool IsObjectInteresectSomeObjects(std::shared_ptr<IObject> current);
	void TryMoveSelectedObject(std::shared_ptr<IObject> object, CVector3d const& pos);
	void MoveObject(std::shared_ptr<IObject> obj, double deltaX, double deltaY);
	void RotateObject(std::shared_ptr<IObject> obj, double deltaRot);
	bool TestRay(double *origin, double *dir, IObject * shooter, IObject* target);
	int BBoxlos(double origin[3], IBounding * target, IObject * shooter, IObject * targetObject);

	CGameModel& m_model;
	CVector3d m_selectedObjectCapturePoint;
	std::unique_ptr<CVector3d> m_selectedObjectBeginCoords;
	std::unique_ptr<CVector2d> m_selectionRectangleBegin;
	double m_selectedObjectPrevRotation;
	std::unique_ptr<CVector3d> m_rotationPosBegin;

	std::unique_ptr<CLUAScript> m_lua;
	std::unique_ptr<CCommandHandler> m_commandHandler;
	std::unique_ptr<CNetwork> m_network;

	std::map<sKeyBind, std::function<void()>> m_keyBindings;
	std::function<void()> m_selectionCallback;
	std::function<void()> m_updateCallback;
	std::function<void()>m_singleCallback;
	MouseButtonCallback m_lmbCallback;
	MouseButtonCallback m_rmbCallback;

	friend bool operator< (sKeyBind const& one, sKeyBind const& two);
};

bool operator< (CGameController::sKeyBind const& one, CGameController::sKeyBind const& two);