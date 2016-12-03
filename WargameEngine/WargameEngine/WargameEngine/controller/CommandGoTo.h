#pragma once
#include <memory>
#include <string>
#include "ICommand.h"
#include "../view/Vector3.h"

class CObjectDecorator;
class CGameController;
class IGameModel;

class CCommandGoTo : public ICommand
{
public:
	CCommandGoTo(std::shared_ptr<CObjectDecorator> object, CVector3f const& target, float speed, std::string const& animation, float animationSpeed);
	CCommandGoTo(IReadMemoryStream & stream, IGameModel & model, CGameController& controller);
	virtual void Execute() override;
	virtual void Rollback() override;
	virtual void Serialize(IWriteMemoryStream & stream) const override;
private:
	std::shared_ptr<CObjectDecorator> m_object;
	CVector3f m_target;
	float m_speed;
	std::string m_animation;
	float m_animationSpeed;
	CVector3f m_oldCoords;
};
