#pragma once
#include <memory>
#include "..\IPathfinding.h"

class CPathfindingMicroPather : public wargameEngine::IPathfinding
{
public:
	CPathfindingMicroPather();
	~CPathfindingMicroPather();

	void Init(wargameEngine::model::Model& model, wargameEngine::model::IBoundingBoxManager& boundingBoxManager, size_t horizontalResolution, size_t verticalResolution) override;
	std::vector<CVector3f> GetPath(const CVector3f& from, const CVector3f& to) const override;

private:
	struct Impl;
	std::unique_ptr<Impl> m_pImpl;
};