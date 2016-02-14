#pragma once
#include <memory>
#include <string>
#include "ICommand.h"
#include "../view/Vector3.h"

class IObject;
class IGameModel;

class CCommandGoTo : public ICommand
{
public:
	CCommandGoTo(std::shared_ptr<IObject> object, double x, double y, double speed, std::string const& animation, float animationSpeed);
	CCommandGoTo(IReadMemoryStream & stream, IGameModel& model);
	virtual void Execute() override;
	virtual void Rollback() override;
	virtual void Serialize(IWriteMemoryStream & stream) const override;
private:
	std::shared_ptr<IObject> m_object;
	double m_x;
	double m_y;
	double m_speed;
	std::string m_animation;
	float m_animationSpeed;
	CVector3d m_oldCoords;
};
