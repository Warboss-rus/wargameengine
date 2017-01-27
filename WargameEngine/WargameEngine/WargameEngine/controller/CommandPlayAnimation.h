#pragma once
#include <memory>
#include <string>
#include "ICommand.h"
#include "../model/Animation.h"

class IObject;
class IGameModel;

class CCommandPlayAnimation : public ICommand
{
public:
	CCommandPlayAnimation(std::shared_ptr<IObject> object, std::string const& animation, eAnimationLoopMode loopMode, float speed);
	CCommandPlayAnimation(IReadMemoryStream & stream, IGameModel& model);
	virtual void Execute() override;
	virtual void Rollback() override;
	virtual void Serialize(IWriteMemoryStream & stream) const override;
private:
	std::shared_ptr<IObject> m_object;
	std::string m_animation;
	eAnimationLoopMode m_loopMode;
	float m_speed;
};