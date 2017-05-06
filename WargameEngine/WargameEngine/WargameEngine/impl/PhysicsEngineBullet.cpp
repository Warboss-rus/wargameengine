#include "PhysicsEngineBullet.h"
#include <vector>
#include <map>
#include <algorithm>
#pragma warning (push)
#pragma warning (disable: 4127)
#include <btBulletDynamicsCommon.h>
#pragma warning (pop)
#include "../model/IBaseObject.h"
#include "../model/Landscape.h"
#include "../view/IRenderer.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "../LogWriter.h"

CVector3f ToVector3f(btVector3 const& vec)
{
	return CVector3f(vec.x(), vec.z(), vec.y());
}

btVector3 ToBtVector3(CVector3f const& vec)
{
	return btVector3(vec.x, vec.z, vec.y);
}

btQuaternion RotationToQuaternion(float rotation)
{
	return btQuaternion(btVector3(0, -1, 0), rotation * (float)M_PI / 180);
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
		const float fcolor[] = { color.x(), color.y(), color.z(), 1.0f };
		m_renderer.SetColor(fcolor);
		m_renderer.RenderArrays(RenderMode::LINES, { ToVector3f(from), ToVector3f(to) }, {}, {});
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

	void Update(std::chrono::duration<float> timeDelta)
	{
		m_dynamicsWorld->stepSimulation(static_cast<btScalar>(timeDelta.count()), 10);
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

		auto motionState = std::make_unique<CUpdateMotionState>(GetObjectTransform(object, m_shapeOffset[colShape]));
		btRigidBody::btRigidBodyConstructionInfo rbInfo(static_cast<btScalar>(mass), motionState.get(), colShape, localInertia);
		auto body = std::make_unique<btRigidBody>(rbInfo);
		m_dynamicsWorld->addRigidBody(body.get());
		body->setUserPointer(object);
		body->setAngularFactor(btVector3(0, 0, 0));
		auto coordConnection(object->DoOnCoordsChange(std::bind(&Impl::UpdateBodyFromObject, this, object, body.get())));
		auto rotationConnection(object->DoOnRotationChange(std::bind(&Impl::UpdateBodyFromObject, this, object, body.get())));
		motionState->DoOnUpdate(std::bind(&Impl::UpdateObjectFromBody, this, object, body.get()));
		m_objects.push_back({ std::move(body), std::move(motionState), object, coordConnection, rotationConnection });
	}

	void AddStaticObject(IBaseObject * staticObject)
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

	void SetGround(Landscape * landscape)
	{
		landscape;
	}

	bool CastRay(CVector3f const& origin, CVector3f const& dest, IObject ** obj, CVector3f & hitPoint, std::vector<IObject*> const& excludeObjects) const
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
				hitPoint = ToVector3f(btHitPoint);
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

	void AddBounding(const Path& modelName, Bounding const& bounding)
	{
		std::function<std::unique_ptr<btCollisionShape>(Bounding const&)> processShape = [&processShape, this](Bounding const& bounding)->std::unique_ptr<btCollisionShape> {
			if (bounding.type == Bounding::eType::Box)
			{
				auto& box = bounding.GetBox();
				CVector3f halfSize = (box.max - box.min) / 2;
				CVector3f boxCenter = (box.max + box.min) / 2 * bounding.scale;
				auto shape = std::make_unique<btBoxShape>(ToBtVector3(halfSize));
				m_shapeOffset[shape.get()] = boxCenter;
				btScalar btScale = static_cast<btScalar>(bounding.scale);
				shape->setLocalScaling(btVector3(btScale, btScale, btScale));
				return std::move(shape);
			}
			else if (bounding.type == Bounding::eType::Compound)
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

	Bounding GetAABB(const Path& path) const
	{
		auto it = m_collisionShapes.find(path);
		if (it == m_collisionShapes.end())
		{
			return Bounding();
		}
		btVector3 min, max;
		btTransform transform;
		transform.setIdentity();
		it->second->getAabb(transform, min, max);
		auto it2 = m_shapeOffset.find(it->second.get());
		if (it2 == m_shapeOffset.end())
		{
			return Bounding();
		}
		auto offset = it2->second;
		return Bounding::Box{ ToVector3f(min) + offset, ToVector3f(max) + offset };
	}
private:
	static btTransform GetObjectTransform(IBaseObject * object, CVector3f const& shapeOffset)
	{
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(ToBtVector3(object->GetCoords() + shapeOffset));
		transform.setRotation(RotationToQuaternion(object->GetRotation()));
		return transform;
	}

	void UpdateBodyFromObject(IBaseObject * object, btRigidBody * body)
	{
		btTransform transform = GetObjectTransform(object, m_shapeOffset[body->getCollisionShape()]);
		body->setWorldTransform(transform);
	}

	void UpdateObjectFromBody(IBaseObject * object, btRigidBody * body)
	{
		btTransform trans = body->getWorldTransform();
		object->SetCoords(ToVector3f(trans.getOrigin()) - m_shapeOffset[body->getCollisionShape()]);
	}

	class CUpdateMotionState : public btDefaultMotionState
	{
	public:
		CUpdateMotionState(const btTransform& startTrans)
			: btDefaultMotionState(startTrans)
		{
		}
		virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans) override
		{
			btDefaultMotionState::setWorldTransform(centerOfMassWorldTrans);
			if(m_onUpdate) m_onUpdate();
		}

		void DoOnUpdate(std::function<void()> const& onUpdate)
		{
			m_onUpdate = onUpdate;
		}
	private:
		std::function<void()> m_onUpdate;
	};

	struct Object
	{
		std::unique_ptr<btRigidBody> rigidBody;
		std::unique_ptr<btMotionState> motionState;
		IObject * object;
		CScopedConnection coordsConnection;
		CScopedConnection rotationConnection;
	};
	btDefaultCollisionConfiguration m_collisionConfiguration;
	btCollisionDispatcher m_dispatcher;
	std::unique_ptr<btBroadphaseInterface> m_overlappingPairCache;
	std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
	std::unique_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld;
	std::vector<Object> m_objects;
	std::map<Path, std::unique_ptr<btCollisionShape>> m_collisionShapes;
	std::vector<std::unique_ptr<btCollisionShape>> m_childCollisionShapes;
	std::map<const btCollisionShape*, CVector3f> m_shapeOffset;
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

void CPhysicsEngineBullet::Update(std::chrono::microseconds timeDelta)
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

void CPhysicsEngineBullet::AddStaticObject(IBaseObject * staticObject)
{
	m_pImpl->AddStaticObject(staticObject);
}

void CPhysicsEngineBullet::RemoveDynamicObject(IObject * object)
{
	m_pImpl->RemoveDynamicObject(object);
}

void CPhysicsEngineBullet::SetGround(Landscape * landscape)
{
	m_pImpl->SetGround(landscape);
}

bool CPhysicsEngineBullet::CastRay(CVector3f const& origin, CVector3f const& dest, IObject ** obj, CVector3f & hitPoint, std::vector<IObject*> const& excludeObjects) const
{
	return m_pImpl->CastRay(origin, dest, obj, hitPoint, excludeObjects);
}

bool CPhysicsEngineBullet::TestObject(IObject * object) const
{
	return m_pImpl->TestObject(object);
}

void CPhysicsEngineBullet::AddBounding(const Path& modelName, Bounding const& bounding)
{
	m_pImpl->AddBounding(modelName, bounding);
}

Bounding CPhysicsEngineBullet::GetBounding(const Path& path) const
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
