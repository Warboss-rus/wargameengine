#pragma once
#include <memory>

class CGameController
{
private:
	static std::shared_ptr<CGameController> m_instanse;

	CGameController(void){};
	CGameController(CGameController const&){};
public:
	static std::weak_ptr<CGameController> GetIntanse();
	~CGameController(void);
};

