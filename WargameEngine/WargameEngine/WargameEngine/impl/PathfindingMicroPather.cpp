#include "PathfindingMicroPather.h"
#include "micropather.h"
#include "..\model\GameModel.h"
#include "..\model\IBoundingBoxManager.h"
#include "..\LogWriter.h"

struct CPathfindingMicroPather::Impl : public micropather::Graph
{
public:
	Impl()
		: m_solver(this, 10000, 8, true)
	{}

	void Init(CGameModel& model, const IBoundingBoxManager& boundingBoxManager, size_t horizontalResolution, size_t verticalResolution)
	{
		m_solver.Reset();
		m_field.clear();
		m_field.resize(horizontalResolution * verticalResolution, 0);
		auto& landscape = model.GetLandscape();
		m_width = landscape.GetWidth();
		m_height = landscape.GetDepth();
		m_horizontalResolution = horizontalResolution;
		m_verticalResolution = verticalResolution;
		m_boundingBoxManager = &boundingBoxManager;
		m_landscape = &landscape;

		//Add all existing objects and subscribe to their events
		for (size_t i = 0; i < model.GetObjectCount(); ++i)
		{
			auto object = model.Get3DObject(i);
			AddObjectAndSubscribe(*object);
		}
		for (size_t i = 0; i < landscape.GetStaticObjectCount(); ++i)
		{
			auto& object = landscape.GetStaticObject(i);
			AddObjectAndSubscribe(object);
		}
		model.DoOnObjectCreation([this](IObject* object) {
			AddObjectAndSubscribe(*object);
		});
		model.DoOnObjectRemove([this](IObject* object) {
			RemoveObject(*object);
		});
	}

	std::vector<CVector3f> GetPath(const CVector3f& from, const CVector3f& to)
	{
		std::vector<CVector3f> result;
		micropather::MPVector<void*> path;
		float cost = 0.0f;
		m_solver.Solve(PositionToIndex(from), PositionToIndex(to), &path, &cost);
		result.reserve(path.size());
		for (size_t i = 0; i < path.size(); ++i)
		{
			result.push_back(IndexToPosition(path[i]));
		}
		return result;
	}

private:
	float LeastCostEstimate(void* stateStart, void* stateEnd) override
	{
		auto start = IndexToPosition(stateStart);
		auto end = IndexToPosition(stateEnd);
		return (start - end).GetLength();
	}

	void AdjacentCost(void* state, MP_VECTOR<micropather::StateCost> *adjacent) override
	{
		const size_t idx = reinterpret_cast<size_t>(state);
		const size_t ix = idx % m_horizontalResolution;
		const size_t iy = idx / m_horizontalResolution;
	}

	void PrintStateInfo(void* state) override
	{
		(void)state;
#ifdef _DEBUG
		auto pos = IndexToPosition(state);
		(void)pos;
#endif
	}

	void AddObjectAndSubscribe(IBaseObject& object)
	{
		AddObject(object);
		object.DoOnCoordsChange([this, &object](const CVector3f& oldPos, const CVector3f& /*newPos*/) {
			RemoveObject(object, &oldPos);
			AddObject(object);
		});
		object.DoOnRotationChange([this, &object](const CVector3f& oldRotations, const CVector3f& /*newRotations*/) {
			RemoveObject(object, nullptr, &oldRotations);
			AddObject(object);
		});
	}

	void AddObject(IBaseObject& object)
	{
		//Increment object count for the cells occupied by this object
		auto cellIndices = GetAffectedCells(m_boundingBoxManager->GetBounding(object.GetPathToModel()), object.GetCoords(), object.GetRotations());
		for (size_t idx : cellIndices)
		{
			++m_field[idx];
		}
	}

	void RemoveObject(IBaseObject& object, const CVector3f* overridePosition = nullptr, const CVector3f* overrideRotation = nullptr)
	{
		CVector3f position = overridePosition ? *overridePosition : object.GetCoords();
		CVector3f rotations = overrideRotation ? *overrideRotation : object.GetRotations();
		auto bounding = m_boundingBoxManager->GetBounding(object.GetPathToModel());
		//Decrement object count for the cells occupied by this object
		auto cellIndices = GetAffectedCells(bounding, position, rotations);
		for (size_t idx : cellIndices)
		{
			--m_field[idx];
#ifdef _DEBUG
			if (m_field[idx] < 0)
			{
				LogWriter::WriteLine("Pathfinding error: cell has negative object count");
			}
#endif
		}
	}

	std::vector<size_t> GetAffectedCells(const Bounding& bounding, const CVector3f& position, const CVector3f& rotation) const
	{
		std::vector<size_t> result;
		return result;
	}

	void* PositionToIndex(const CVector3f& position) const
	{
		const size_t x = static_cast<size_t>(round((position.x + m_width / 2) * m_horizontalResolution / m_width));
		const size_t y = static_cast<size_t>(round((position.y + m_height / 2) * m_verticalResolution / m_height));
		return reinterpret_cast<void*>(y * m_horizontalResolution + x);
	}

	CVector3f IndexToPosition(size_t ix, size_t iy) const
	{
		const float x = 0.0f;
		const float y = 0.0f;
		return CVector3f(x, y, m_landscape->GetHeight(x, y));
	}
	
	CVector3f IndexToPosition(void* index)
	{
		const size_t idx = reinterpret_cast<size_t>(index);
		const size_t ix = idx % m_horizontalResolution;
		const size_t iy = idx / m_horizontalResolution;
		return IndexToPosition(ix, iy);
	}

	micropather::MicroPather m_solver;
	const IBoundingBoxManager* m_boundingBoxManager = nullptr;
	const Landscape* m_landscape = nullptr;
	std::vector<int> m_field;
	float m_width = 0.0f;
	float m_height = 0.0f;
	size_t m_horizontalResolution = 0;
	size_t m_verticalResolution = 0;
};

CPathfindingMicroPather::CPathfindingMicroPather()
	: m_pImpl(std::make_unique<Impl>())
{
}

CPathfindingMicroPather::~CPathfindingMicroPather() = default;

void CPathfindingMicroPather::Init(CGameModel& model, const IBoundingBoxManager& boundingBoxManager, size_t horizontalResolution, size_t verticalResolution)
{
	m_pImpl->Init(model, boundingBoxManager, horizontalResolution, verticalResolution);
}

std::vector<CVector3f> CPathfindingMicroPather::GetPath(const CVector3f& from, const CVector3f& to) const
{
	return m_pImpl->GetPath(from, to);
}
