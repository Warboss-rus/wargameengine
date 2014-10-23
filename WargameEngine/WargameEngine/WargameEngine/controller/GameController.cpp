#include "GameController.h"
#include "../view/GameView.h"
#include "../model/GameModel.h"
#include "../model/ObjectGroup.h"
#include "LUARegisterFunctions.h"
#include "../Module.h"

std::shared_ptr<CGameController> CGameController::m_instanse = NULL;

std::weak_ptr<CGameController> CGameController::GetInstance()
{
	if (!m_instanse)
	{
		m_instanse.reset(new CGameController());
	}
	return std::weak_ptr<CGameController>(m_instanse);
}

CGameController::CGameController()
{
	CCommandHandler::GetInstance();
	m_lua.reset(new CLUAScript());
	RegisterFunctions(*m_lua.get());
	RegisterUI(*m_lua.get());
	RegisterObject(*m_lua.get());
	m_lua->RunScript(sModule::script);
}

void CGameController::FreeInstance()
{
	m_instanse.reset();
}

CGameController::~CGameController(void)
{
	CCommandHandler::FreeInstance();
}

void CGameController::SelectObjectGroup(int beginX, int beginY, int endX, int endY)
{
	double minX = (beginX < endX) ? beginX : endX;
	double maxX = (beginX > endX) ? beginX : endX;
	double minY = (beginY < endY) ? beginY : endY;
	double maxY = (beginY > endY) ? beginY : endY;
	CObjectGroup* group = new CObjectGroup();
	CGameModel * model = CGameModel::GetInstance().lock().get();
	for (unsigned long i = 0; i < model->GetObjectCount(); ++i)
	{
		std::shared_ptr<IObject> object = model->Get3DObject(i);
		if (object->GetX() > minX && object->GetX() < maxX && object->GetY() > minY && object->GetY() < maxY && object->IsSelectable())
		{
			group->AddChildren(object);
		}
	}
	switch (group->GetCount())
	{
	case 0:
	{
		model->SelectObject(NULL);
		delete group;
	}break;
	case 1:
	{
		model->SelectObject(group->GetChild(0));
		delete group;
	}break;
	default:
	{
		model->SelectObject(std::shared_ptr<IObject>(group));
	}break;
	}
}

std::shared_ptr<IObject> CGameController::GetNearestObject(double * start, double * end)
{
	std::shared_ptr<IObject> selectedObject = NULL;
	double minDistance = 10000000.0;
	CGameModel * model = CGameModel::GetInstance().lock().get();
	for (unsigned long i = 0; i < model->GetObjectCount(); ++i)
	{
		std::shared_ptr<IObject> object = model->Get3DObject(i);
		if (!object) continue;
		std::shared_ptr<IBounding> bounding = CGameView::GetInstance().lock()->GetModelManager()->GetBoundingBox(object->GetPathToModel());
		if (!bounding) continue;
		if (bounding->IsIntersectsRay(start, end, object->GetX(), object->GetY(), object->GetZ(), object->GetRotation(), m_selectedObjectCapturePoint))
		{
			double distance = sqrt(object->GetX() * object->GetX() + object->GetY() * object->GetY() + object->GetZ() * object->GetZ());
			if (distance < minDistance)
			{
				selectedObject = object;
				minDistance = distance;
				m_selectedObjectCapturePoint.x -= selectedObject->GetX();
				m_selectedObjectCapturePoint.y -= selectedObject->GetY();
				m_selectedObjectCapturePoint.z -= selectedObject->GetZ();
			}
		}
	}
	return selectedObject;
}

void CGameController::SelectObject(double * begin, double * end, bool add)
{
	std::shared_ptr<IObject> selectedObject = GetNearestObject(begin, end);
	if (selectedObject && !selectedObject->IsSelectable())
	{
		return;
	}
	std::shared_ptr<IObject> object = CGameModel::GetInstance().lock()->GetSelectedObject();
	if (CGameModel::IsGroup(object.get()))
	{
		CObjectGroup * group = (CObjectGroup *)object.get();
		if (add)
		{
			if (group->ContainsChildren(selectedObject))
			{
				group->RemoveChildren(selectedObject);
				if (group->GetCount() == 1)//Destroy group
				{
					CGameModel::GetInstance().lock()->SelectObject(group->GetChild(0));
				}
			}
			else
			{
				group->AddChildren(selectedObject);
			}
		}
		else
		{
			if (!group->ContainsChildren(selectedObject))
			{
				CGameModel::GetInstance().lock()->SelectObject(selectedObject);
			}
			else
			{
				group->SetCurrent(selectedObject);
			}
		}
	}
	else
	{
		if (add && object != NULL)
		{
			CObjectGroup * group = new CObjectGroup();
			group->AddChildren(object);
			group->AddChildren(selectedObject);
			CGameModel::GetInstance().lock()->SelectObject(std::shared_ptr<IObject>(group));
		}
		else
		{
			CGameModel::GetInstance().lock()->SelectObject(selectedObject);
		}
	}
}

const CVector3d * CGameController::GetCapturePoint() const
{
	return &m_selectedObjectCapturePoint;
}

bool CGameController::IsObjectInteresectSomeObjects(std::shared_ptr<IObject> current)
{
	CGameModel * model = CGameModel::GetInstance().lock().get();
	std::shared_ptr<IBounding> curBox = CGameView::GetInstance().lock()->GetModelManager()->GetBoundingBox(current->GetPathToModel());
	if (!curBox) return false;
	CVector3d curPos(current->GetCoords());
	double curAngle = current->GetRotation();
	for (unsigned long i = 0; i < model->GetObjectCount(); ++i)
	{
		std::shared_ptr<IObject> object = model->Get3DObject(i);
		if (!object) continue;
		std::shared_ptr<IBounding> bounding = CGameView::GetInstance().lock()->GetModelManager()->GetBoundingBox(object->GetPathToModel());
		if (!bounding) continue;
		CVector3d pos(object->GetCoords());
		double angle = object->GetRotation();
		if (current != object && IsInteresect(curBox.get(), curPos, curAngle, bounding.get(), pos, angle))
		{
			return true;
		}
	}
	return false;
}

bool TestRay(double *origin, double *dir, IObject * shooter, IObject* target)
{
	CGameModel* model = CGameModel::GetInstance().lock().get();
	CModelManager* modelManager = CGameView::GetInstance().lock()->GetModelManager();
	CVector3d coords;
	for (unsigned int i = 0; i < model->GetObjectCount(); ++i)
	{
		IObject * current = model->Get3DObject(i).get();
		if (current == shooter || current == target) continue;
		IBounding * box = modelManager->GetBoundingBox(current->GetPathToModel()).get();
		if (!box) continue;
		if (box->IsIntersectsRay(origin, dir, current->GetX(), current->GetY(), current->GetZ(), current->GetRotation(), coords))
		{
			return false;
		}
	}
	return true;
}

int BBoxlos(double origin[3], IBounding * target, IObject * shooter, IObject * targetObject)
{
	int result = 0;
	int total = 0;
	CBoundingBox * tarBox = dynamic_cast<CBoundingBox *>(target);
	if (!tarBox)
	{
		CBoundingCompound * compound = (CBoundingCompound*)target;
		for (unsigned int i = 0; i < compound->GetChildCount(); ++i)
		{
			result += BBoxlos(origin, compound->GetChild(i), shooter, targetObject);
		}
		result /= compound->GetChildCount();
		total = 100;
	}
	else
	{
		double dir[3];
		for (dir[0] = tarBox->GetMin()[0] + targetObject->GetX(); dir[0] < tarBox->GetMax()[0] + targetObject->GetX(); dir[0] += (tarBox->GetMax()[0] - tarBox->GetMin()[0]) / 10.0 + 0.0001)
		{
			for (dir[1] = tarBox->GetMin()[1] + targetObject->GetY(); dir[1] < tarBox->GetMax()[1] + targetObject->GetY(); dir[1] += (tarBox->GetMax()[1] - tarBox->GetMin()[1]) / 10.0 + 0.0001)
			{
				for (dir[2] = tarBox->GetMin()[2] + targetObject->GetZ(); dir[2] < tarBox->GetMax()[2] + targetObject->GetZ(); dir[2] += (tarBox->GetMax()[2] - tarBox->GetMin()[2]) / 10.0 + 0.0001)
				{
					total++;
					if (TestRay(origin, dir, shooter, targetObject))
						result++;
				}
			}
		}
	}
	return result * 100 / total;
}

int CGameController::GetLineOfSight(IObject * shooter, IObject * target)
{
	if (!shooter || !target) return -1;
	IBounding * targetBound = CGameView::GetInstance().lock()->GetModelManager()->GetBoundingBox(target->GetPathToModel()).get();
	double center[3] = { shooter->GetX(), shooter->GetY(), shooter->GetZ() + 2.0 };
	return BBoxlos(center, targetBound, shooter, target);
}