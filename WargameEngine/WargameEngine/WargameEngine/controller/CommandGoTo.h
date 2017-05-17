#pragma once
#include <memory>
#include <string>
#include "ICommand.h"
#include "../view/Vector3.h"

namespace wargameEngine
{
namespace model
{
class IModel;
}

namespace controller
{
class ObjectDecorator;
class Controller;

class CCommandGoTo : public ICommand
{
public:
	CCommandGoTo(std::shared_ptr<ObjectDecorator> object, CVector3f const& target, float speed, std::string const& animation, float animationSpeed);
	CCommandGoTo(IReadMemoryStream & stream, model::IModel & model, Controller& controller);
	virtual void Execute() override;
	virtual void Rollback() override;
	virtual void Serialize(IWriteMemoryStream & stream) const override;
private:
	std::shared_ptr<ObjectDecorator> m_object;
	CVector3f m_target;
	float m_speed;
	std::string m_animation;
	float m_animationSpeed;
	CVector3f m_oldCoords;
};
}
}