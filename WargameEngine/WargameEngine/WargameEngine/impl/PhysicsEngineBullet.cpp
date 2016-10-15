#include "PhysicsEngineBullet.h"
#include <vector>
#include <map>
#include <algorithm>
#pragma warning (push)
#pragma warning (disable: 4127)
#include <btBulletDynamicsCommon.h>
#pragma warning (pop)
#include "../model/ObjectInterface.h"
#include "../model/ObjectStatic.h"
#include "../model/Landscape.h"
#include "../view/IRenderer.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "../LogWriter.h"

CVector3d ToVector3d(btVector3 const& vec)
{
	return CVector3d(vec.x(), vec.z(), vec.y());
}

btVector3 ToBtVector3(CVector3d const& vec)
{
	return btVector3(static_cast<btScalar>(vec.x), static_cast<btScalar>(vec.z), static_cast<btScalar>(vec.y));
}

btQuaternion RotationToQuaternion(double rotation)
{
	return btQuaternion(btVector3(0, -1, 0), static_cast<btScalar>(rotation * M_PI / 180));
}

class CDebugDrawer : public btIDebugDraw
{
public:
	CDebugDrawer(IRenderer & renderer)
		:m_debugMode(0), m_renderer(renderer)
	{
	}
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
	{
		m_renderer.SetColor(color.x(), color.y(), color.z());
		m_renderer.RenderArrays(RenderMode::LINES, { ToVector3d(from), ToVector3d(to) }, {}, {});
	}

	virtual void drawContactPoint(const btVector3& /*PointOnB*/, const btVector3& /*normalOnB*/, btScalar /*distance*/, int /*lifeTime*/, const btVector3& /*color*/) override
	{
		
	}

	virtual void reportErrorWarning(const char* warningString) override
	{
		LogWriter::WriteLine(warningString);
	}

	virtual void draw3dText(const btVector3& /*location*/, const char* /*textString*/) override
	{
		
	}

	virtual void setDebugMode(int debugMode) override
	{
		m_debugMode = debugMode;
	}

	virtual int getDebugMode() const override
	{
		return m_debugMode;
	}
private:
	int m_debugMode;
	IRenderer & m_renderer;
};

struct CPhysicsEngineBullet::Impl
{
public:
	Impl()
		: m_dispatcher(&m_collisionConfiguration)
		, m_overlappingPairCache(std::make_unique<btDbvtBroadphase>())
		, m_solver(std::make_unique<btSequentialImpulseConstraintSolver>())
		, m_dynamicsWorld(std::make_unique<btDiscreteDynamicsWorld>(&m_dispatcher, m_overlappingPairCache.get(), m_solver.get(), &m_collisionConfiguration))
	{
		m_dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));
		CreateGround();
	}

	void CreateGround()
	{
		auto groundShape = std::make_unique<btStaticPlaneShape>(btVector3(0, 1.0f, 0), 1.0f);
		auto groundMotionState = std::make_unique<btDefaultMotionState>(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState.get(), groundShape.get(), btVector3(0, 0, 0));
		auto groundRigidBody = std::make_unique<btRigidBody>(groundRigidBodyCI);
		m_dynamicsWorld->addRigidBody(groundRigidBody.get());
		m_ground = groundRigidBody.get();
		m_objects.push_back(Object{ std::move(groundRigidBody), std::move(groundMotionState), nullptr });
		m_childCollisionShapes.push_back(std::move(groundShape));
	}

	~Impl()
	{
		Reset();
		for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0;i--)
		{
			btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
			m_dynamicsWorld->removeCollisionObject(obj);
		}
	}

	static btTransform GetObjectTransform(IObject * object, CVector3d const& shapeOffset)
	{
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(ToBtVector3(object->GetCoords() + shapeOffset));
		transform.setRotation(RotationToQuaternion(object->GetRotation()));
		return transform;
	}

	void Update(long long timeDelta)
	{
		for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body)
			{
				IObject * object = reinterpret_cast<IObject*>(body->getUserPointer());
				if (object)
				{
					btTransform transform = GetObjectTransform(object, m_shapeOffset[body->getCollisionShape()]);
					body->setWorldTransform(transform);
				}
			}
		}
		double timeStep = static_cast<double>(timeDelta) / 1000;
		m_dynamicsWorld->stepSimulation(static_cast<btScalar>(timeStep), 10);
		for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				btTransform trans = body->getWorldTransform();
				IObject * object = reinterpret_cast<IObject*>(body->getUserPointer());
				if (object)
				{
					object->SetCoords(ToVector3d(trans.getOrigin()) - m_shapeOffset[body->getCollisionShape()]);
				}
			}
		}
	}

	void Reset()
	{
		for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0;i--)
		{
			btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
			m_dynamicsWorld->removeCollisionObject(obj);
		}
		m_objects.clear();
		m_collisionShapes.clear();
		m_childCollisionShapes.clear();
		CreateGround();
	}

	void AddDynamicObject(IObject * object, double mass)
	{
		btCollisionShape* colShape = m_collisionShapes.at(object->GetPathToModel()).get();
		
		bool isDynamic = fabs(mass) > DBL_EPSILON;
		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(static_cast<btScalar>(mass), localInertia);

		auto motionState = std::make_unique<btDefaultMotionState>(GetObjectTransform(object, m_shapeOffset[colShape]));
		btRigidBody::btRigidBodyConstructionInfo rbInfo(static_cast<btScalar>(mass), motionState.get(), colShape, localInertia);
		auto body = std::make_unique<btRigidBody>(rbInfo);
		m_dynamicsWorld->addRigidBody(body.get());
		body->setUserPointer(object);
		body->setAngularFactor(btVector3(0, 0, 0));
		m_objects.push_back(Object{ std::move(body), std::move(motionState), object });
	}

	void AddStaticObject(CStaticObject * staticObject)
	{
		btScalar mass(0.);
		btVector3 localInertia(0, 0, 0);
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(ToBtVector3(staticObject->GetCoords()));
		transform.setRotation(RotationToQuaternion(staticObject->GetRotation()));

		auto motionState = std::make_unique<btDefaultMotionState>(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState.get(), m_collisionShapes.at(staticObject->GetPathToModel()).get(), localInertia);
		auto body = std::make_unique<btRigidBody>(rbInfo);
		body->setUserPointer(nullptr);
		m_dynamicsWorld->addRigidBody(body.get());
		m_objects.push_back(Object{std::move(body), std::move(motionState), nullptr});
	}

	void SetGround(CLandscape * landscape)
	{
		landscape;
	}

	bool CastRay(CVector3d const& origin, CVector3d const& dest, IObject ** obj, CVector3d & hitPoint, std::vector<IObject*> const& excludeObjects) const
	{
		*obj = nullptr;
		btCollisionWorld::AllHitsRayResultCallback RayCallback(ToBtVector3(origin), ToBtVector3(dest));
		m_dynamicsWorld->rayTest(ToBtVector3(origin), ToBtVector3(dest), RayCallback);
		if (RayCallback.hasHit())
		{
			auto& objects = RayCallback.m_collisionObjects;
			for (int i = 0; i < objects.size(); ++i)
			{
				const btCollisionObject* collisionObject = objects[i];
				const btRigidBody* body = btRigidBody::upcast(collisionObject);
				btTransform trans;
				if (body)
				{
					trans = body->getWorldTransform();
					*obj = reinterpret_cast<IObject*>(body->getUserPointer());
					if (std::find(excludeObjects.begin(), excludeObjects.end(), *obj) != excludeObjects.end())
					{
						*obj = 0;
					}
				}
				btVector3 btHitPoint = trans.invXform(RayCallback.m_hitPointWorld[i]);
				hitPoint = ToVector3d(btHitPoint);
				auto offsetIt = body ? m_shapeOffset.find(body->getCollisionShape()) : m_shapeOffset.end();
				if (offsetIt != m_shapeOffset.end())
				{
					hitPoint += offsetIt->second;
				}
				if (*obj)
				{
					return true;
				}
			}
		}
		return *obj != nullptr;
	}

	void AddBounding(std::wstring const& modelName, sBounding const& bounding)
	{
		std::function<std::unique_ptr<btCollisionShape>(sBounding const&)> processShape = [&processShape, this](sBounding const& bounding)->std::unique_ptr<btCollisionShape> {
			if (bounding.type == sBounding::eType::BOX)
			{
				auto& box = bounding.GetBox();
				CVector3d halfSize = (box.max - box.min) / 2;
				CVector3d boxCenter = (box.max + box.min) / 2 * bounding.scale;
				auto shape = std::make_unique<btBoxShape>(ToBtVector3(halfSize));
				m_shapeOffset[shape.get()] = boxCenter;
				btScalar btScale = static_cast<btScalar>(bounding.scale);
				shape->setLocalScaling(btVector3(btScale, btScale, btScale));
				return std::move(shape);
			}
			else if (bounding.type == sBounding::eType::COMPOUND)
			{
				auto& compound = bounding.GetCompound();
				auto shape = std::make_unique<btCompoundShape>();
				for (auto& child : compound.items)
				{
					auto childShape = processShape(child);
					btTransform transform;
					transform.setIdentity();
					transform.setOrigin(ToBtVector3(m_shapeOffset[childShape.get()]));
					shape->addChildShape(transform, childShape.get());
					m_childCollisionShapes.push_back(std::move(childShape));
				}
				btScalar btScale = static_cast<btScalar>(bounding.scale);
				shape->setLocalScaling(btVector3(btScale, btScale, btScale));
				return std::move(shape);
			}
			return nullptr;
		};
		
		m_collisionShapes[modelName] = processShape(bounding);
	}

	void RemoveDynamicObject(IObject * objectToRemove)
	{
		for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0;i--)
		{
			btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
			IObject *object = reinterpret_cast<IObject*>(obj->getUserPointer());
			if (object == objectToRemove)
			{
				m_dynamicsWorld->removeCollisionObject(obj);
			}
		}
		m_objects.erase(std::remove_if(m_objects.begin(), m_objects.end(), [objectToRemove] (Object const& obj){
			return obj.object == objectToRemove;
		}), m_objects.end());
	}

	bool TestObject(IObject * object)
	{
		btCollisionShape* colShape = m_collisionShapes.at(object->GetPathToModel()).get();

		btScalar mass = 0.0f;
		btVector3 localInertia(0, 0, 0);

		std::unique_ptr<btDefaultMotionState> motionState = std::make_unique<btDefaultMotionState>(GetObjectTransform(object, m_shapeOffset[colShape]));
		btRigidBody::btRigidBodyConstructionInfo rbInfo(static_cast<btScalar>(mass), motionState.get(), colShape, localInertia);
		std::unique_ptr<btRigidBody> body = std::make_unique<btRigidBody>(rbInfo);
		body->setUserPointer(object);

		bool result = false;
		struct ContactSensorCallback : public btCollisionWorld::ContactResultCallback 
		{
			btCollisionObject* ground;
			bool* result;
			IObject * self;
			virtual btScalar addSingleResult(btManifoldPoint& /*cp*/, const btCollisionObjectWrapper* colObj0, int /*partId0*/, int /*index0*/,
				const btCollisionObjectWrapper* colObj1, int /*partId1*/, int /*index1*/)
			{
				auto object1 = colObj0->getCollisionObject()->getUserPointer();
				auto object2 = colObj1->getCollisionObject()->getUserPointer();
				if (object1 != object2 && (object1 == self || object2 == self) && (colObj0->getCollisionObject() != ground) && (colObj1->getCollisionObject() != ground))//if 2 objects collide and neither of them i
				{
					*result = true;
				}
				return 0;
			}
		} callback;
		callback.ground = m_ground;
		callback.result = &result;
		callback.self = object;
		
		m_dynamicsWorld->contactTest(body.get(), callback);
		return result;
	}

	void Draw()
	{
		m_dynamicsWorld->debugDrawWorld();
	}

	void EnableDebugDraw(IRenderer & renderer)
	{
		m_debugDrawer = std::make_unique<CDebugDrawer>(renderer);
		m_debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		m_dynamicsWorld->setDebugDrawer(m_debugDrawer.get());
	}

	sBounding::sBox GetAABB(std::wstring const& path) const
	{
		auto it = m_collisionShapes.find(path);
		btVector3 min, max;
		btTransform transform;
		transform.setIdentity();
		it->second->getAabb(transform, min, max);
		auto offset = m_shapeOffset.find(it->second.get())->second;
		return{ ToVector3d(min) + offset, ToVector3d(max) + offset };
	}
private:
	struct Object
	{
		std::unique_ptr<btRigidBody> rigidBody;
		std::unique_ptr<btMotionState> motionState;
		IObject * object;
	};
	btDefaultCollisionConfiguration m_collisionConfiguration;
	btCollisionDispatcher m_dispatcher;
	std::unique_ptr<btBroadphaseInterface> m_overlappingPairCache;
	std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
	std::unique_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld;
	std::vector<Object> m_objects;
	std::map<std::wstring, std::unique_ptr<btCollisionShape>> m_collisionShapes;
	std::vector<std::unique_ptr<btCollisionShape>> m_childCollisionShapes;
	std::map<const btCollisionShape*, CVector3d> m_shapeOffset;
	std::unique_ptr<CDebugDrawer> m_debugDrawer;
	btCollisionObject * m_ground;
};

CPhysicsEngineBullet::CPhysicsEngineBullet()
	:m_pImpl(std::make_unique<Impl>())
{
}

CPhysicsEngineBullet::~CPhysicsEngineBullet()
{
}

void CPhysicsEngineBullet::Update(long long timeDelta)
{
	m_pImpl->Update(timeDelta);
}

void CPhysicsEngineBullet::Reset()
{
	m_pImpl->Reset();
}

void CPhysicsEngineBullet::AddDynamicObject(IObject * object, double mass)
{
	m_pImpl->AddDynamicObject(object, mass);
}

void CPhysicsEngineBullet::AddStaticObject(CStaticObject * staticObject)
{
	m_pImpl->AddStaticObject(staticObject);
}

void CPhysicsEngineBullet::RemoveDynamicObject(IObject * object)
{
	m_pImpl->RemoveDynamicObject(object);
}

void CPhysicsEngineBullet::SetGround(CLandscape * landscape)
{
	m_pImpl->SetGround(landscape);
}

bool CPhysicsEngineBullet::CastRay(CVector3d const& origin, CVector3d const& dest, IObject ** obj, CVector3d & hitPoint, std::vector<IObject*> const& excludeObjects) const
{
	return m_pImpl->CastRay(origin, dest, obj, hitPoint, excludeObjects);
}

bool CPhysicsEngineBullet::TestObject(IObject * object) const
{
	return m_pImpl->TestObject(object);
}

void CPhysicsEngineBullet::AddBounding(std::wstring const& modelName, sBounding const& bounding)
{
	m_pImpl->AddBounding(modelName, bounding);
}

sBounding CPhysicsEngineBullet::GetBounding(std::wstring const& path) const
{
	return m_pImpl->GetAABB(path);
}

void CPhysicsEngineBullet::Draw() const
{
	m_pImpl->Draw();
}

void CPhysicsEngineBullet::EnableDebugDraw(IRenderer & renderer)
{
	m_pImpl->EnableDebugDraw(renderer);
}
