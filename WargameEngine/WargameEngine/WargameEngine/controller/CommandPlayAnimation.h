#pragma once
#include <memory>
#include <string>
#include "ICommand.h"
#include "../model/Animation.h"

namespace wargameEngine
{
namespace model
{
class IObject;
class IModel;
}

namespace controller
{
class CCommandPlayAnimation : public ICommand
{
public:
	CCommandPlayAnimation(std::shared_ptr<model::IObject> object, std::string const& animation, model::AnimationLoop loopMode, float speed);
	CCommandPlayAnimation(IReadMemoryStream & stream, model::IModel& model);
	virtual void Execute() override;
	virtual void Rollback() override;
	virtual void Serialize(IWriteMemoryStream & stream) const override;
private:
	std::shared_ptr<model::IObject> m_object;
	std::string m_animation;
	model::AnimationLoop m_loopMode;
	float m_speed;
};
}
}