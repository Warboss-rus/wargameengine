#include "PerfomanceMeter.h"
#include <fstream>

namespace wargameEngine
{
namespace view
{
std::unique_ptr<PerfomanceMeter> PerfomanceMeter::m_instance;

void PerfomanceMeter::ReportFrameEnd()
{
	auto instance = GetInstance();
	auto currentTime = std::chrono::high_resolution_clock::now();
	instance->m_fps = 1.0f / std::chrono::duration<float>(currentTime - instance->m_lastFrameTime).count();
	instance->m_lastFrameTime = currentTime;
	if (instance->m_benchmark)
	{
		instance->m_fpsHistory.push_back(instance->m_fps);
	}
}

size_t GetPolygons(size_t verticesCount, IRenderer::RenderMode mode)
{
	switch (mode)
	{
	case IRenderer::RenderMode::LineLoop:
		return verticesCount - 1;
	case IRenderer::RenderMode::Lines:
		return verticesCount / 2;
	case IRenderer::RenderMode::Triangles:
		return verticesCount / 3;
	case IRenderer::RenderMode::TriangleStrip:
		return verticesCount - 2;
	}
	return 0;
}

void PerfomanceMeter::ReportDraw(size_t verticesCount, IRenderer::RenderMode mode)
{
	auto instance = GetInstance();
	instance->m_verticesDrawn += verticesCount;
	instance->m_polygonsDrawn += GetPolygons(verticesCount, mode);
	++instance->m_drawCalls;
}

void PerfomanceMeter::Reset()
{
	auto instance = GetInstance();
	instance->m_drawCalls = 0;
	instance->m_verticesDrawn = 0;
	instance->m_polygonsDrawn = 0;
}

long long PerfomanceMeter::GetVerticesDrawn()
{
	return GetInstance()->m_verticesDrawn;
}

long long PerfomanceMeter::GetPolygonsDrawn()
{
	return GetInstance()->m_polygonsDrawn;
}

long long PerfomanceMeter::GetDrawCalls()
{
	return GetInstance()->m_drawCalls;
}

size_t PerfomanceMeter::GetFps()
{
	return static_cast<size_t>(abs(GetInstance()->m_fps));
}

void PerfomanceMeter::StartBenchmark()
{
	GetInstance()->m_benchmark = true;
	GetInstance()->m_fpsHistory.clear();
}

void PerfomanceMeter::EndBenchmark(const Path& resultPath)
{
	auto instance = GetInstance();
	if (instance->m_benchmark)
	{
		instance->m_benchmark = false;
		auto& fpsHistory = instance->m_fpsHistory;
		std::ofstream file(resultPath);
		for (auto fps : fpsHistory)
		{
			file << fps << std::endl;
		}
		file.close();
		fpsHistory.clear();
	}
}

wargameEngine::view::PerfomanceMeter* PerfomanceMeter::GetInstance()
{
	if (!m_instance)
	{
		m_instance = std::make_unique<PerfomanceMeter>();
	}
	return m_instance.get();
}

}
}