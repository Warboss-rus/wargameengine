#include "ObjectManager.h"

void CObjectManager::Draw()
{
	for(unsigned int i = 0; i < m_objects.size(); ++i)
	{
		m_objects[i]->Draw();
	}
}

void CObjectManager::AddObject(IObject * object)
{
	m_objects.push_back(object);
}

void CObjectManager::DeleteObject(IObject * object)
{
	for(unsigned int i = 0; i < m_objects.size(); ++i)
	{
		if(m_objects[i] == object)
		{
			delete m_objects[i];
			m_objects.erase(m_objects.begin() + i);
		}
	}
}