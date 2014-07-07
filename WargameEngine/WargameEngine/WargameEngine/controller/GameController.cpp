#include "GameController.h"

std::shared_ptr<CGameController> CGameController::m_instanse = NULL;

std::weak_ptr<CGameController> CGameController::GetIntanse()
{
	if (!m_instanse)
	{
		m_instanse.reset(new CGameController());
	}
	return std::weak_ptr<CGameController>(m_instanse);
}

CGameController::~CGameController(void)
{
}
