#include "view\GameView.h"

bool TestRay(double *origin, double *dir, IObject * shooter, IObject* target)
{
	CGameModel* model = CGameModel::GetIntanse().lock().get();
	CModelManager* modelManager = CGameView::GetIntanse().lock()->GetModelManager();
	CVector3d coords;
	for (int i = 0; i < model->GetObjectCount(); ++i)
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
		for (int i = 0; i < compound->GetChildCount(); ++i)
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

int Los(IObject * shooter, IObject * target)
{
	if (!shooter || !target) return -1;
	CModelManager* modelManager = CGameView::GetIntanse().lock()->GetModelManager();
	IBounding * targetBound = modelManager->GetBoundingBox(target->GetPathToModel()).get();
	double center[3] = { shooter->GetX(), shooter->GetY(), shooter->GetZ() + 2.0 };
	return BBoxlos(center, targetBound, shooter, target);
}