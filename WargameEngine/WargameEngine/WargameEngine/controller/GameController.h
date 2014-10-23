#pragma once
#include <memory>
#include "../view/Vector3.h"
#include "CommandHandler.h"
#include "LUAScriptHandler.h"

class IObject;

class CGameController
{
private:
	static std::shared_ptr<CGameController> m_instanse;
	CVector3d m_selectedObjectCapturePoint;
	std::shared_ptr<CLUAScript> m_lua;

	CGameController(void);
	CGameController(CGameController const&){};
	std::shared_ptr<IObject> GetNearestObject(double * start, double * end);
public:
	static std::weak_ptr<CGameController> GetInstance();
	static void FreeInstance();
	~CGameController();

	void SelectObjectGroup(int beginX, int beginY, int endX, int endY);
	void SelectObject(double * begin, double * end, bool add);
	bool IsObjectInteresectSomeObjects(std::shared_ptr<IObject> current);
	const CVector3d * GetCapturePoint() const;
	int GetLineOfSight(IObject * shooter, IObject * target);
};