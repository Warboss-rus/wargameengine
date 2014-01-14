#include "view\GameView.h"
#include <vector>

void GetCoordinateBount(std::vector<float*> & coordinate, IBounding* bount)
{
	if(dynamic_cast<CBoundingCompound *>(bount)!= NULL)
	{
		CBoundingCompound* bountCompound = (CBoundingCompound*)bount;
		for(size_t i = 0; i < bountCompound->GetChildCount(); ++i)
		{
			GetCoordinateBount(coordinate, bountCompound->GetChild(i));
		}
	}
	CBoundingBox* box = (CBoundingBox*)bount;
	coordinate.push_back(box->GetMin());
	coordinate.push_back(box->GetMax());
}

bool TestIntersetion(IObject* shootingModel,IObject* target, CVector3d & mass)
{
	CVector3d intersectionCoords;
	//CVector3d coordinateShootingModel = shootingModel->GetCoords();
	std::vector<float *> coordinate1;
	CModelManager* modelManager = CGameView::GetIntanse().lock()->GetModelManager();
	IBounding* bount1 = modelManager->GetBoundingBox(shootingModel->GetPathToModel()).get();
	GetCoordinateBount(coordinate1, bount1);
	float min1[3];
	memcpy(&min1[0], coordinate1[0], sizeof(float) * 3);
	float max1[3];
	memcpy(&max1[0], coordinate1[1], sizeof(float) * 3);
	CVector3d coordinateShootingModel((min1[0]+max1[0])/2, (min1[1]+max1[1])/2, (min1[2]+max1[2])/2);
	for(size_t i = 0; i < CGameModel::GetIntanse().lock()->GetObjectCount(); i++)
	{
		IObject * object = CGameModel::GetIntanse().lock()->Get3DObject(i).get();
		if(object == shootingModel || object == target) continue;
		if(CGameView::GetIntanse().lock()->GetModelManager()->GetBoundingBox(object->GetPathToModel())->
			IsIntersectsRay((float*)&coordinateShootingModel, (float*)&mass, object->GetX(), object->GetY(), object->GetZ(), object->GetRotation(), intersectionCoords))
		{
			return false;
		}	
	}
	return true;
}

int Los(IObject* shootingModel,IObject* target)
{
	int los = 0;
	std::vector<float *> coordinate;
	std::vector<float *> coordinate1;
	CModelManager* modelManager = CGameView::GetIntanse().lock()->GetModelManager();
	IBounding* bount = modelManager->GetBoundingBox(target->GetPathToModel()).get();
	GetCoordinateBount(coordinate, bount);
	IBounding* bount1 = modelManager->GetBoundingBox(shootingModel->GetPathToModel()).get();
	GetCoordinateBount(coordinate1, bount1);
	float min1[3];
	memcpy(&min1[0], coordinate1[0], sizeof(float) * 3);
	float max1[3];
	memcpy(&max1[0], coordinate1[1], sizeof(float) * 3);
	CVector3d coordinateShootingModel((min1[0]+max1[0])/2, (min1[1]+max1[1])/2, (min1[2]+max1[2])/2);
	float min[3];
	memcpy(&min[0], coordinate[0], sizeof(float) * 3);
	float max[3];
	memcpy(&max[0], coordinate[1], sizeof(float) * 3);
	CVector3d mass[100];
	CVector3d centre((min[0]+max[0])/2, (min[1]+max[1])/2, (min[2]+max[2])/2);
	if(((centre.x > coordinateShootingModel.x) && (centre.y < coordinateShootingModel.y))||
		((centre.x < coordinateShootingModel.x) && (centre.y > coordinateShootingModel.y)))
	{
		CVector3d w(min[0], max[1], min[2]);
		CVector3d w1(max[0], min[1], max[2]);
		float deltaX = max[0] - min[0];
		float deltaY = max[1] - min[1];
		float deltaZ = (max[2] - min[2]) / 100;
		for(int i = 0; i < 100; i++)
		{
			mass[i].x = w.x + i * (deltaX/100);
			mass[i].y = w.y + i * (deltaY/100);
			if((i% 10 == 0) && (i != 0))
			{
				deltaZ = deltaZ * i;
			}
			mass[i].z = deltaZ;
		}
	}
	else
	{
		float deltaX = max[0] - min[0];
		float deltaY = max[1] - min[1];
		float deltaZ = (max[2] - min[2]) / 100;
		for(int i = 0; i < 100; i++)
		{
			mass[i].x = min[0] + i * (deltaX/100);
			mass[i].y = min[1] +i * (deltaY/100);
			if((i% 10 == 0) && (i != 0))
			{
				deltaZ = deltaZ * i;
			}
			mass[i].z = deltaZ;
		}
	}
	
	for(int j = 0; j < 100; j++)
	{
		if(TestIntersetion(shootingModel, target, mass[j]))
		{
			los++;
		}
	}
	return los;
}