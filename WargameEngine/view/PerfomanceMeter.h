#pragma once
#include "IRenderer.h"
#include <chrono>

namespace wargameEngine
{
namespace view
{
class PerfomanceMeter
{
public:
	static void ReportFrameEnd();
	static void ReportDraw(size_t verticesCount, IRenderer::RenderMode mode);
	static void Reset();
	static long long GetVerticesDrawn();
	static long long GetPolygonsDrawn();
	static long long GetDrawCalls();
	static size_t GetFps();
	static void StartBenchmark();
	static void EndBenchmark(const Path& resultPath);

private:
	static PerfomanceMeter* GetInstance();

	static std::unique_ptr<PerfomanceMeter> m_instance;
	std::chrono::high_resolution_clock::time_point m_lastFrameTime;
	long long m_verticesDrawn = 0;
	long long m_polygonsDrawn = 0;
	long long m_drawCalls = 0;
	float m_fps = 0;
	bool m_benchmark = false;
	std::vector<float> m_fpsHistory;
};
}
}