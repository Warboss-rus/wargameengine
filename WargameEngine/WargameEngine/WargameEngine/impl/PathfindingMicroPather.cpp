#include "PathfindingMicroPather.h"
#include "micropather.h"
#include "..\model\Model.h"
#include "..\model\IBoundingBoxManager.h"
#include "..\LogWriter.h"

using namespace wargameEngine;

struct CPathfindingMicroPather::Impl : public micropather::Graph
{
public:
	Impl()
		: m_solver(this, 10000, 8, true)
	{}

	void Init(model::Model& model, model::IBoundingBoxManager& boundingBoxManager, size_t horizontalResolution, size_t verticalResolution)
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
		for (size_t i = 0; i < model.GetStaticObjectCount(); ++i)
		{
			auto& object = model.GetStaticObject(i);
			AddObjectAndSubscribe(object);
		}
		model.DoOnObjectCreation([this](model::IObject* object) {
			AddObjectAndSubscribe(*object);
		});
		model.DoOnObjectRemove([this](model::IObject* object) {
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
		if ((ix != 0) && (m_field[iy * m_horizontalResolution + ix - 1] == 0))//cell on the left
		{
			adjacent->push_back({ reinterpret_cast<void*>(iy * m_horizontalResolution + ix - 1), 1.0f });
		}
		if ((ix != m_horizontalResolution - 1) && (m_field[iy * m_horizontalResolution + ix + 1] == 0))//cell on the right
		{
			adjacent->push_back({ reinterpret_cast<void*>(iy * m_horizontalResolution + ix + 1), 1.0f });
		}
		if ((iy != 0) && (m_field[(iy - 1) * m_horizontalResolution + ix] == 0))//cell at the bottom
		{
			adjacent->push_back({ reinterpret_cast<void*>((iy - 1) * m_horizontalResolution + ix), 1.0f });
		}
		if ((iy != m_verticalResolution - 1) && (m_field[(iy + 1) * m_horizontalResolution + ix] == 0))//cell on the top
		{
			adjacent->push_back({ reinterpret_cast<void*>((iy + 1) * m_horizontalResolution + ix), 1.0f });
		}
	}

	void PrintStateInfo(void* state) override
	{
		(void)state;
#ifdef _DEBUG
		auto pos = IndexToPosition(state);
		(void)pos;
#endif
	}

	void AddObjectAndSubscribe(model::IBaseObject& object)
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

	void AddObject(model::IBaseObject& object)
	{
		//Increment object count for the cells occupied by this object
		auto cellIndices = GetAffectedCells(m_boundingBoxManager->GetBounding(object.GetPathToModel()), object.GetCoords(), object.GetRotations());
		for (size_t idx : cellIndices)
		{
			++m_field[idx];
		}
	}

	void RemoveObject(model::IBaseObject& object, const CVector3f* overridePosition = nullptr, const CVector3f* overrideRotation = nullptr)
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

	std::vector<size_t> GetAffectedCells(const model::Bounding& bounding, const CVector3f& position, const CVector3f& rotation) const
	{
		return std::visit([&](auto&& boundingItem)->std::vector<size_t> {
			std::vector<size_t> result;
			using T = std::decay_t<decltype(boundingItem)>;
			if constexpr (std::is_same_v<T, model::Bounding::Compound>)
			{
				auto& items = boundingItem.items;
				for (size_t i = 0; i < items.size(); ++i)
				{
					std::vector<size_t> subResult = GetAffectedCells(items[i], position, rotation);
					result.insert(result.end(), subResult.begin(), subResult.end());
				}
			}
			else if constexpr(std::is_same_v<T, model::Bounding::Box>)
			{
				//TODO: rotate bbox
				const float left = boundingItem.min[0] + position.x;
				const float right = boundingItem.max[0] + position.x;
				const float top = boundingItem.min[1] + position.y;
				const float bottom = boundingItem.max[1] + position.y;
				const size_t ileft = static_cast<size_t>(round((left + m_width / 2) * m_horizontalResolution / m_width));
				const size_t iright = static_cast<size_t>(round((right + m_width / 2) * m_horizontalResolution / m_width));
				const size_t itop = static_cast<size_t>(round((top + m_height / 2) * m_verticalResolution / m_height));
				const size_t ibottom = static_cast<size_t>(round((bottom + m_height / 2) * m_verticalResolution / m_height));
				for (size_t i = ileft; i <= iright; ++i)
				{
					for (size_t j = itop; j <= ibottom; ++j)
					{
						result.push_back(j * m_horizontalResolution + i);
					}
				}
			}
			else
			{
				static_assert(std::false_type::value, "unknown bounding type");
			}
			return result;
		}, bounding.data);
	}

	void* PositionToIndex(const CVector3f& position) const
	{
		const size_t x = static_cast<size_t>(round((position.x + m_width / 2) * m_horizontalResolution / m_width));
		const size_t y = static_cast<size_t>(round((position.y + m_height / 2) * m_verticalResolution / m_height));
		return reinterpret_cast<void*>(y * m_horizontalResolution + x);
	}

	CVector3f IndexToPosition(size_t ix, size_t iy) const
	{
		const float x = ix * m_width / m_horizontalResolution - m_width / 2;
		const float y = iy * m_height / m_verticalResolution - m_height / 2;
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
	model::IBoundingBoxManager* m_boundingBoxManager = nullptr;
	const model::Landscape* m_landscape = nullptr;
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

void CPathfindingMicroPather::Init(model::Model& model, model::IBoundingBoxManager& boundingBoxManager, size_t horizontalResolution, size_t verticalResolution)
{
	m_pImpl->Init(model, boundingBoxManager, horizontalResolution, verticalResolution);
}

std::vector<CVector3f> CPathfindingMicroPather::GetPath(const CVector3f& from, const CVector3f& to) const
{
	return m_pImpl->GetPath(from, to);
}
