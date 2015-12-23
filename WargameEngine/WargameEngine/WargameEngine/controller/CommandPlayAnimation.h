#pragma once
#include <memory>
#include <string>
#include "ICommand.h"
#include "..\model\Animation.h"

class IObject;

class CCommandPlayAnimation : public ICommand
{
public:
	CCommandPlayAnimation(std::shared_ptr<IObject> object, std::string const& animation, eAnimationLoopMode loopMode, float speed);
	virtual void Execute() override;
	virtual void Rollback() override;
	virtual std::vector<char> Serialize() const override;
private:
	std::shared_ptr<IObject> m_object;
	std::string m_animation;
	eAnimationLoopMode m_loopMode;
	float m_speed;
};