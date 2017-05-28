#include "PerfomanceMeter.h"

namespace wargameEngine
{
namespace view
{
std::unique_ptr<PerfomanceMeter> PerfomanceMeter::m_instance;

void PerfomanceMeter::ReportFrameEnd()
{
	auto instance = GetInstance();
	auto currentTime = std::chrono::high_resolution_clock::now();
	instance->m_fps = static_cast<size_t>(1.0f / std::chrono::duration<float>(currentTime - instance->m_lastFrameTime).count());
	instance->m_lastFrameTime = currentTime;
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
	return GetInstance()->m_fps;
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