#pragma once
#include <memory>
#include "../view/Vector3.h"
#include "CommandHandler.h"
#include "LUAScriptHandler.h"
#include <functional>
#define callback(x) std::function<void()>(x)

class IObject;

class CGameController
{
public:
	static std::weak_ptr<CGameController> GetInstance();
	static void FreeInstance();
	~CGameController();

	void SelectObjectGroup(int beginX, int beginY, int endX, int endY);
	void SelectObject(double * begin, double * end, bool add);
	bool IsObjectInteresectSomeObjects(std::shared_ptr<IObject> current);
	const CVector3d * GetCapturePoint() const;
	int GetLineOfSight(IObject * shooter, IObject * target);
	void SetSelectionCallback(callback(onSelect));
	std::vector<char> GetState(bool hasAdresses = false) const;
	void SetState(char* data, bool hasAdresses = false);
	void Save(std::string const& filename);
	void Load(std::string const& filename);
	void TryMoveSelectedObject(std::shared_ptr<IObject> object, double x, double y, double z);
private:
	CGameController(void);
	CGameController(CGameController const&){};
	std::shared_ptr<IObject> GetNearestObject(double * start, double * end);
	void Init();

	static std::shared_ptr<CGameController> m_instanse;
	CVector3d m_selectedObjectCapturePoint;
	std::shared_ptr<CLUAScript> m_lua;
	callback(m_selectionCallback);
};