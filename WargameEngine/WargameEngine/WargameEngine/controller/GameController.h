#pragma once
#include <memory>
#include <vector>
#include <map>
#include "../model/ObjectInterface.h"
#include "../view/Vector3.h"
#include <functional>
#include "LUAScriptHandler.h"
#include "CommandHandler.h"
#include "../Network.h"

class CGameController
{
public:
	static std::weak_ptr<CGameController> GetInstance();
	static void FreeInstance();
	~CGameController();

	void Update();
	void SelectObjectGroup(double beginX, double beginY, double endX, double endY);
	void SelectObject(double * begin, double * end, bool add, bool noCallback = false);
	bool IsObjectInteresectSomeObjects(std::shared_ptr<IObject> current);
	const CVector3d * GetCapturePoint() const;
	int GetLineOfSight(IObject * shooter, IObject * target);
	void SetSelectionCallback(std::function<void()> const& onSelect);
	void SetUpdateCallback(std::function<void()> const& onUpdate);
	void SetSingleCallback(std::function<void()> const& onSingleUpdate);
	std::vector<char> GetState(bool hasAdresses = false) const;
	void SetState(char* data, bool hasAdresses = false);
	void Save(std::string const& filename);
	void Load(std::string const& filename);
	void TryMoveSelectedObject(std::shared_ptr<IObject> object, double x, double y, double z);
	void BindKey(unsigned char key, bool shift, bool ctrl, bool alt, std::function<void()> const& func);
	bool OnKeyPress(unsigned char key, bool shift, bool ctrl, bool alt);
	void MoveObject(std::shared_ptr<IObject> obj, double deltaX, double deltaY);
	void RotateObject(std::shared_ptr<IObject> obj, double deltaRot);
	std::shared_ptr<IObject> CreateObject(std::string const& model, double x, double y, double rotation);
	void DeleteObject(std::shared_ptr<IObject> obj);
	void SetObjectProperty(std::shared_ptr<IObject> obj, std::string const& key, std::string const& value);
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
	
	CGameController(void);
	CGameController(CGameController const&){};
	std::shared_ptr<IObject> GetNearestObject(double * start, double * end);
	void Init();

	static std::shared_ptr<CGameController> m_instanse;
	CVector3d m_selectedObjectCapturePoint;
	std::map<sKeyBind, std::function<void()>> m_keyBindings;

	std::unique_ptr<CLUAScript> m_lua;
	std::unique_ptr<CCommandHandler> m_commandHandler;
	std::unique_ptr<CNetwork> m_network;

	std::function<void()> m_selectionCallback;
	std::function<void()> m_updateCallback;
	std::function<void()>m_singleCallback;

	friend bool operator< (sKeyBind const& one, sKeyBind const& two);
};

bool operator< (CGameController::sKeyBind const& one, CGameController::sKeyBind const& two);