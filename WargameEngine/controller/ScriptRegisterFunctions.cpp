#include "ScriptRegisterFunctions.h"
#include "../AsyncFileProvider.h"
#include "../LogWriter.h"
#include "../ThreadPool.h"
#include "../Utils.h"
#include "../view/ISoundPlayer.h"
#include "../view/View.h"
#include "../view/PerfomanceMeter.h"
#include "Controller.h"
#include "Network.h"
#include "ScriptFunctionsProtocol.h"

namespace wargameEngine
{
namespace controller
{

void RegisterModelFunctions(IScriptHandler& handler, model::Model& model)
{
	handler.RegisterFunction(CREATE_LANDSCAPE, [&](IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 argument expected (width, height, texture)");
		float width = args.GetFloat(1);
		float height = args.GetFloat(2);
		Path texture = args.GetPath(3);
		model.ResetLandscape(width, height, texture, 2, 2);
		return nullptr;
	});

	handler.RegisterFunction(GET_GLOBAL_PROPERTY, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (key)");
		std::wstring key = args.GetWStr(1);
		return model.GetProperty(key);
	});

	handler.RegisterFunction(SET_GLOBAL_PROPERTY, [&](IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 arguments expected (key, value)");
		std::wstring key = args.GetWStr(1);
		std::wstring value = args.GetWStr(2);
		model.SetProperty(key, value);
		return nullptr;
	});

	handler.RegisterFunction(NEW_DECAL, [&](IArguments const& args) {
		if (args.GetCount() != 6)
			throw std::runtime_error("6 argument expected (decal, x, y, rotation, width, height)");
		model::Decal decal;
		decal.texture = args.GetPath(1);
		decal.x = args.GetFloat(2);
		decal.y = args.GetFloat(3);
		decal.rotation = args.GetFloat(4);
		decal.width = args.GetFloat(5);
		decal.depth = args.GetFloat(6);
		model.GetLandscape().AddNewDecal(decal);
		return nullptr;
	});

	handler.RegisterFunction(NEW_STATIC_OBJECT, [&](IArguments const& args) {
		if (args.GetCount() != 4)
			throw std::runtime_error("4 argument expected (model, x, y, rotation)");
		Path objectModel = args.GetPath(1);
		float x = args.GetFloat(2);
		float y = args.GetFloat(3);
		float rotation = args.GetFloat(4);
		model.AddStaticObject(model::StaticObject(objectModel, { x, y, 0.0f }, rotation));
		return nullptr;
	});

	handler.RegisterFunction(ADD_LIGHT, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected ()");
		model.AddLight();
		return nullptr;
	});

	handler.RegisterFunction(REMOVE_LIGHT, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (index)");
		size_t i = args.GetSizeT(1) - 1;
		model.RemoveLight(i);
		return nullptr;
	});

	handler.RegisterFunction(SET_LIGHT_SOURCE_POSITION, [&](IArguments const& args) {
		if (args.GetCount() != 4)
			throw std::runtime_error("4 argument expected (index, x, y, z)");
		size_t i = args.GetSizeT(1) - 1;
		float pos[3] = { 0.0f, 0.0f, 0.0f };
		pos[0] = args.GetFloat(2);
		pos[1] = args.GetFloat(3);
		pos[2] = args.GetFloat(4);
		model.GetLight(i).SetPosition(CVector3f(pos));
		return nullptr;
	});

	handler.RegisterFunction(SET_LIGHT_SOURCE_AMBIENT, [&](IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 argument expected (index, r, g, b, a)");
		size_t i = args.GetSizeT(1) - 1;
		float color[4];
		color[0] = args.GetFloat(2);
		color[1] = args.GetFloat(3);
		color[2] = args.GetFloat(4);
		color[3] = args.GetFloat(5);
		model.GetLight(i).SetAmbientColor(color);
		return nullptr;
	});

	handler.RegisterFunction(SET_LIGHT_SOURCE_DIFFUSE, [&](IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 argument expected (index, r, g, b, a)");
		size_t i = args.GetSizeT(1) - 1;
		float color[4];
		color[0] = args.GetFloat(2);
		color[1] = args.GetFloat(3);
		color[2] = args.GetFloat(4);
		color[3] = args.GetFloat(5);
		model.GetLight(i).SetDiffuseColor(color);
		return nullptr;
	});

	handler.RegisterFunction(SET_LIGHT_SOURCE_SPECULAR, [&](IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 argument expected (index, r, g, b, a)");
		size_t i = args.GetSizeT(1) - 1;
		float color[4];
		color[0] = args.GetFloat(2);
		color[1] = args.GetFloat(3);
		color[2] = args.GetFloat(4);
		color[3] = args.GetFloat(5);
		model.GetLight(i).SetSpecularColor(color);
		return nullptr;
	});
}

void RegisterViewFunctions(IScriptHandler& handler, view::View& view, AsyncFileProvider& fileProvider)
{
	handler.RegisterFunction(CREATE_SKYBOX, [&](IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected (size, texture folder)");
		float size = args.GetFloat(1);
		Path texture = args.GetPath(2);
		view.CreateSkybox(size, texture);
		return nullptr;
	});

	handler.RegisterFunction(ENABLE_RULER, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view.GetRuler().Enable();
		return nullptr;
	});

	handler.RegisterFunction(RESIZE_WINDOW, [&](IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected (height, width)");
		int height = args.GetInt(1);
		int width = args.GetInt(2);
		view.ResizeWindow(height, width);
		return nullptr;
	});

	handler.RegisterFunction(ENABLE_MSAA, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view.EnableMSAA(true);
		return nullptr;
	});

	handler.RegisterFunction(DISABLE_MSAA, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view.EnableMSAA(false);
		return nullptr;
	});

	handler.RegisterFunction(SET_ANISOTROPY_LEVEL, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected");
		float a = args.GetFloat(1);
		view.SetAnisotropyLevel(a);
		return nullptr;
	});

	handler.RegisterFunction(GET_MAX_ANISOTROPY, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		return view.GetMaxAnisotropy();
	});

	handler.RegisterFunction(ENABLE_GPU_SKINNING, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view.EnableGPUSkinning(true);
		return nullptr;
	});

	handler.RegisterFunction(DISABLE_GPU_SKINNING, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view.EnableGPUSkinning(false);
		return nullptr;
	});

	handler.RegisterFunction(CLEAR_RESOURCES, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view.ClearResources();
		return nullptr;
	});

	handler.RegisterFunction(SET_WINDOW_TITLE, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 arguments expected (title)");
		std::wstring title = args.GetWStr(1);
		view.SetWindowTitle(title);
		return nullptr;
	});

	handler.RegisterFunction(PRELOAD, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (image path)");
		Path image = args.GetPath(1);
		view.Preload(image);
		return nullptr;
	});

	handler.RegisterFunction(PRELOAD_MODEL, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (model name)");
		Path model = args.GetPath(1);
		view.PreloadModel(model);
		return nullptr;
	});

	handler.RegisterFunction(SET_PARTICLE_SYSTEM_SHADERS, [&](IArguments const& args) {
		int n = args.GetCount();
		Path vertex, fragment;
		if (n > 0)
			vertex = fileProvider.GetShaderAbsolutePath(args.GetPath(1));
		if (n > 1)
			fragment = fileProvider.GetShaderAbsolutePath(args.GetPath(2));
		if (n > 3)
			throw std::runtime_error("up to 2 argument expected (vertex shader, fragment shader)");
		view.GetParticleSystem().SetShaders(vertex, fragment, view.GetRenderer());
		return nullptr;
	});

	handler.RegisterFunction(SET_SHADERS, [&](IArguments const& args) {
		int n = args.GetCount();
		Path vertex, fragment, geometry;
		if (n > 0)
			vertex = fileProvider.GetShaderAbsolutePath(args.GetPath(1));
		if (n > 1)
			fragment = fileProvider.GetShaderAbsolutePath(args.GetPath(2));
		if (n > 2)
			geometry = fileProvider.GetShaderAbsolutePath(args.GetPath(3));
		if (n > 3)
			throw std::runtime_error("up to 3 argument expected (vertex shader, fragment shader, geometry shader)");
		view.GetViewport(0).SetShaders(vertex, fragment, geometry);
		return nullptr;
	});

	handler.RegisterFunction(SET_SKYBOX_SHADERS, [&](IArguments const& args) {
		int n = args.GetCount();
		Path vertex, fragment;
		if (n > 0)
			vertex = fileProvider.GetShaderAbsolutePath(args.GetPath(1));
		if (n > 1)
			fragment = fileProvider.GetShaderAbsolutePath(args.GetPath(2));
		if (n > 3)
			throw std::runtime_error("up to 2 argument expected (vertex shader, fragment shader)");
		view.SetSkyboxShaders(vertex, fragment);
		return nullptr;
	});

	handler.RegisterFunction(UNIFORM_1I, [&](IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 arguments expected (uniform name, value)");
		std::string name = args.GetStr(1);
		int value = args.GetInt(2);
		auto& shaderManager = view.GetRenderer().GetShaderManager();
		const size_t viewportsCount = view.GetViewportCount();
		for (size_t i = 0; i < viewportsCount; ++i)
		{
			auto shaderProgram = view.GetViewport(i).GetShaderProgram();
			if (shaderProgram)
			{
				shaderManager.PushProgram(*shaderProgram);
				shaderManager.SetUniformValue(name, 1, 1, &value);
				shaderManager.PopProgram();
			}
		}
		return nullptr;
	});

	handler.RegisterFunction(UNIFORM_1F, [&](IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 arguments expected (uniform name, value)");
		std::string name = args.GetStr(1);
		float value = args.GetFloat(2);
		auto& shaderManager = view.GetRenderer().GetShaderManager();
		const size_t viewportsCount = view.GetViewportCount();
		for (size_t i = 0; i < viewportsCount; ++i)
		{
			auto shaderProgram = view.GetViewport(i).GetShaderProgram();
			if (shaderProgram)
			{
				shaderManager.PushProgram(*shaderProgram);
				shaderManager.SetUniformValue(name, 1, 1, &value);
				shaderManager.PopProgram();
			}
		}
		return nullptr;
	});

	handler.RegisterFunction(UNIFORM_1FV, [&](IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
		std::string name = args.GetStr(1);
		size_t count = args.GetSizeT(2);
		std::vector<float> value = args.GetFloatArray(3);
		if (value.size() < count)
			throw std::runtime_error("Not enough elements in the array");
		auto& shaderManager = view.GetRenderer().GetShaderManager();
		const size_t viewportsCount = view.GetViewportCount();
		for (size_t i = 0; i < viewportsCount; ++i)
		{
			auto shaderProgram = view.GetViewport(i).GetShaderProgram();
			if (shaderProgram)
			{
				shaderManager.PushProgram(*shaderProgram);
				shaderManager.SetUniformValue(name, 1, count, &value[0]);
				shaderManager.PopProgram();
			}
		}
		return nullptr;
	});

	handler.RegisterFunction(UNIFORM_2FV, [&](IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
		std::string name = args.GetStr(1);
		size_t count = args.GetSizeT(2);
		std::vector<float> value = args.GetFloatArray(3);
		if (value.size() < count * 2)
			throw std::runtime_error("Not enough elements in the array");
		auto& shaderManager = view.GetRenderer().GetShaderManager();
		const size_t viewportsCount = view.GetViewportCount();
		for (size_t i = 0; i < viewportsCount; ++i)
		{
			auto shaderProgram = view.GetViewport(i).GetShaderProgram();
			if (shaderProgram)
			{
				shaderManager.PushProgram(*shaderProgram);
				shaderManager.SetUniformValue(name, 2, count, &value[0]);
				shaderManager.PopProgram();
			}
		}
		return nullptr;
	});

	handler.RegisterFunction(UNIFORM_3FV, [&](IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
		std::string name = args.GetStr(1);
		size_t count = args.GetSizeT(2);
		std::vector<float> value = args.GetFloatArray(3);
		if (value.size() < count * 3)
			throw std::runtime_error("Not enough elements in the array");
		auto& shaderManager = view.GetRenderer().GetShaderManager();
		const size_t viewportsCount = view.GetViewportCount();
		for (size_t i = 0; i < viewportsCount; ++i)
		{
			auto shaderProgram = view.GetViewport(i).GetShaderProgram();
			if (shaderProgram)
			{
				shaderManager.PushProgram(*shaderProgram);
				shaderManager.SetUniformValue(name, 3, count, &value[0]);
				shaderManager.PopProgram();
			}
		}
		return nullptr;
	});

	handler.RegisterFunction(UNIFORM_4FV, [&](IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
		std::string name = args.GetStr(1);
		size_t count = args.GetSizeT(2);
		std::vector<float> value = args.GetFloatArray(3);
		if (value.size() < count * 4)
			throw std::runtime_error("Not enough elements in the array");
		auto& shaderManager = view.GetRenderer().GetShaderManager();
		const size_t viewportsCount = view.GetViewportCount();
		for (size_t i = 0; i < viewportsCount; ++i)
		{
			auto shaderProgram = view.GetViewport(i).GetShaderProgram();
			if (shaderProgram)
			{
				shaderManager.PushProgram(*shaderProgram);
				shaderManager.SetUniformValue(name, 4, count, &value[0]);
				shaderManager.PopProgram();
			}
		}
		return nullptr;
	});

	handler.RegisterFunction(UNIFORM_MATRIX4V, [&](IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
		std::string name = args.GetStr(1);
		size_t count = args.GetSizeT(2);
		std::vector<float> value = args.GetFloatArray(3);
		if (value.size() < count * 16)
			throw std::runtime_error("Not enough elements in the array");
		auto& shaderManager = view.GetRenderer().GetShaderManager();
		const size_t viewportsCount = view.GetViewportCount();
		for (size_t i = 0; i < viewportsCount; ++i)
		{
			auto shaderProgram = view.GetViewport(i).GetShaderProgram();
			if (shaderProgram)
			{
				shaderManager.PushProgram(*shaderProgram);
				shaderManager.SetUniformValue(name, 16, count, &value[0]);
				shaderManager.PopProgram();
			}
		}
		return nullptr;
	});

	handler.RegisterFunction(NEW_PARTICLE_EFFECT, [&](IArguments const& args) {
		if (args.GetCount() != 5 && args.GetCount() != 6)
			throw std::runtime_error("5-6 arguments expected (effect file, x, y, z coordinates, scale[, max particles])");
		Path file = fileProvider.GetAbsolutePath(args.GetPath(1));
		float x = args.GetFloat(2);
		float y = args.GetFloat(3);
		float z = args.GetFloat(4);
		float scale = args.GetFloat(5);
		size_t maxParticles = args.GetCount() > 5 ? (size_t)args.GetLong(6) : 1000u;
		view.AddParticleEffect(file, { x, y, z }, scale, maxParticles);
		return nullptr;
	});

	/*handler.RegisterFunction(NEW_PARTICLE_TRACER, [&](IArguments const& args) {
		if (args.GetCount() != 10)
			throw std::runtime_error("10 arguments expected (effect file, begin coordinates, end coordinates, rotation, scale, speed)");
		Path file = view.GetAsyncFileProvider().GetAbsolutePath(args.GetPath(1));
		CVector3d begin, end;
		begin.x = args.GetDbl(2);
		begin.y = args.GetDbl(3);
		begin.z = args.GetDbl(4);
		end.x = args.GetDbl(5);
		end.y = args.GetDbl(6);
		end.z = args.GetDbl(7);
		double rot = args.GetDbl(8);
		double scale = args.GetDbl(9);
		float speed = args.GetFloat(10);
		view.GetParticleSystem().AddTracer(file, begin, end, rot, scale, speed);
		return nullptr;
	});*/

	handler.RegisterFunction(PLAY_SOUND, [&](IArguments const& args) {
		if (args.GetCount() < 2 || args.GetCount() > 3)
			throw std::runtime_error("2 or 3 arguments expected (channel, file, volume)");
		std::wstring channel = args.GetWStr(1);
		Path file = fileProvider.GetAbsolutePath(args.GetPath(2));
		float volume = args.GetCount() > 2 ? args.GetFloat(3) : 1.0f;
		view.GetSoundPlayer().Play(channel, file, volume);
		return nullptr;
	});

	handler.RegisterFunction(PLAY_SOUND_POSITION, [&](IArguments const& args) {
		if (args.GetCount() < 5 || args.GetCount() > 6)
			throw std::runtime_error("5 or 6 arguments expected (channel, file, x, y, z, volume)");
		std::wstring channel = args.GetWStr(1);
		Path file = fileProvider.GetAbsolutePath(args.GetPath(2));
		float x = args.GetFloat(3);
		float y = args.GetFloat(4);
		float z = args.GetFloat(5);
		float volume = args.GetFloat(6);
		view.GetSoundPlayer().PlaySoundPosition(channel, file, CVector3f(x, y, z), volume);
		return nullptr;
	});

	handler.RegisterFunction(PLAY_SOUND_PLAYLIST, [&](IArguments const& args) {
		int n = args.GetCount();
		if (n < 2 || n > 5)
			throw std::runtime_error("2 to 5 arguments expected (name, list of tracks, volume, shuffle, repeat)");
		std::wstring name = args.GetWStr(1);
		std::vector<std::wstring> files = args.GetStrArray(2);
		std::vector<Path> paths;
		for (auto& file : files)
		{
			paths.push_back(fileProvider.GetAbsolutePath(file));
		}
		float volume = n > 2 ? args.GetFloat(3) : 1.0f;
		bool shuffle = n > 3 ? args.GetBool(4) : false;
		bool repeat = n > 4 ? args.GetBool(5) : false;
		view.GetSoundPlayer().PlaySoundPlaylist(name, paths, volume, shuffle, repeat);
		return nullptr;
	});

	handler.RegisterFunction(GET_RENDERER_NAME, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		return view.GetRenderer().GetName();
	});

	handler.RegisterFunction(ENABLE_VR, [&](IArguments const& args) {
		if (args.GetCount() != 1 && args.GetCount() != 2)
			throw std::runtime_error("1 or 2 arguments expected(enable, mirror to screen");
		bool mirrorToScreen = args.GetCount() == 2 ? args.GetBool(2) : true;
		return view.EnableVRMode(args.GetBool(1), mirrorToScreen);
	});

	handler.RegisterFunction(START_BENCHMARK, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view::PerfomanceMeter::StartBenchmark();
		return 0;
	});

	handler.RegisterFunction(STOP_BENCHMARK, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected - filepath");
		const auto path = args.GetPath(1);
		view::PerfomanceMeter::EndBenchmark(fileProvider.GetAbsolutePath(path));
		return 0;
	});
}

void RegisterControllerFunctions(IScriptHandler& handler, Controller& controller, AsyncFileProvider& fileProvider, ThreadPool& threadPool)
{
	handler.RegisterFunction(DELETE_TIMED_CALLBACK, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (ID)");
		size_t id = args.GetSizeT(1);
		threadPool.RemoveTimedCallback(id);
		return nullptr;
	});

	handler.RegisterFunction(GET_FILES_LIST, [&](IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 arguments expected (path, mask, recursive)");
		Path path = fileProvider.GetAbsolutePath(args.GetPath(1));
		Path mask = args.GetPath(2);
		bool recursive = args.GetBool(3);
		std::vector<Path> files = GetFiles(path, mask, recursive);
		return TransformVector(files);
	});

	handler.RegisterFunction(PRINT, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (string)");
		std::wstring text = args.GetWStr(1);
		LogWriter::WriteLine(L"LUA: " + text);
		return nullptr;
	});

	handler.RegisterFunction(RUN_SCRIPT, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (filename)");
		Path filename = fileProvider.GetAbsolutePath(args.GetPath(1));
		handler.RunScript(filename);
		return nullptr;
	});

	handler.RegisterFunction(SET_SELECTION_CALLBACK, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		std::function<void()> function = std::bind(args.GetFunction(1), FunctionArguments());
		controller.SetSelectionCallback(function);
		return nullptr;
	});

	handler.RegisterFunction(SET_UPDATE_CALLBACK, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		std::function<void()> function = std::bind(args.GetFunction(1), FunctionArguments());
		controller.SetUpdateCallback(function);
		return nullptr;
	});

	handler.RegisterFunction(SET_ON_STATE_RECEIVED_CALLBACK, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		std::function<void()> function = std::bind(args.GetFunction(1), FunctionArguments());
		controller.GetNetwork().SetStateRecievedCallback(function);
		return nullptr;
	});

	handler.RegisterFunction(SET_ON_STRING_RECEIVED_CALLBACK, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		auto func = args.GetFunction(1);
		controller.GetNetwork().SetStringRecievedCallback([func](std::wstring const& param) { func({ param }); });
		return nullptr;
	});

	handler.RegisterFunction(SET_TIMED_CALLBACK, [&](IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 argument expected (funcName, time, repeat)");
		auto func = std::bind(args.GetFunction(1), FunctionArguments());
		unsigned int time = args.GetLong(2);
		bool repeat = args.GetBool(3);
		size_t index = threadPool.AddTimedCallback(func, time, repeat);
		return (int)index;
	});

	handler.RegisterFunction(SET_LMB_CALLBACK, [&](IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected (function name, disable default behavior)");
		auto func = args.GetFunction(1);
		bool disable = args.GetBool(2);
		auto callback = [=](std::shared_ptr<model::IObject> obj, std::string const& type, double x, double y, double z) {
			FunctionArgument instance(obj.get(), "Object");
			func({ instance, type, x, y, z });
			return disable;
		};
		controller.SetLMBCallback(callback);
		return nullptr;
	});

	handler.RegisterFunction(SET_RMB_CALLBACK, [&](IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected (function name, disable default behavior)");
		auto func = args.GetFunction(1);
		bool disable = args.GetBool(2);
		auto callback = [=](std::shared_ptr<model::IObject> obj, std::string const& type, double x, double y, double z) {
			FunctionArgument instance(nullptr /*obj.get()*/, type);
			func({ instance, x, y, z });
			return disable;
		};
		controller.SetRMBCallback(callback);
		return nullptr;
	});

	handler.RegisterFunction(BIND_KEY, [&](IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 argument expected (key, shift, ctrl, alt, funcName)");
		unsigned char key = static_cast<unsigned char>(args.GetLong(1));
		bool shift = args.GetBool(2);
		bool ctrl = args.GetBool(3);
		bool alt = args.GetBool(4);
		std::function<void()> function = std::bind(args.GetFunction(5), FunctionArguments());
		controller.BindKey(key, shift, ctrl, alt, function);
		return nullptr;
	});

	handler.RegisterFunction(UNDO, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		controller.GetCommandHandler().Undo();
		return nullptr;
	});

	handler.RegisterFunction(REDO, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		controller.GetCommandHandler().Redo();
		return nullptr;
	});

	handler.RegisterFunction(LINE_OF_SIGHT, [&](IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected (source, target)");
		model::IObject* shootingModel = (model::IObject*)args.GetClassInstance(1);
		model::IObject* target = (model::IObject*)args.GetClassInstance(2);
		return FunctionArgument(static_cast<int>(controller.GetLineOfSight(shootingModel, target)));
	});

	handler.RegisterFunction(BEGIN_ACTION_COMPOUND, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		controller.GetCommandHandler().BeginCompound();
		return nullptr;
	});

	handler.RegisterFunction(END_ACTION_COMPOUND, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		controller.GetCommandHandler().EndCompound();
		return nullptr;
	});

	handler.RegisterFunction(NET_HOST, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (port)");
		unsigned int port = args.GetLong(1);
		controller.GetNetwork().Host(static_cast<unsigned short>(port));
		return nullptr;
	});

	handler.RegisterFunction(NET_CLIENT, [&](IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected (ip, port)");
		std::string ip = args.GetStr(1);
		unsigned short port = static_cast<unsigned short>(args.GetLong(2));
		controller.GetNetwork().Client(ip.c_str(), port);
		return nullptr;
	});

	handler.RegisterFunction(NET_SEND_MESSAGE, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 arguments expected (message)");
		std::wstring message = args.GetWStr(1);
		controller.GetNetwork().SendMessage(message);
		return nullptr;
	});

	handler.RegisterFunction(SAVE_GAME, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 arguments expected (filename)");
		Path path = args.GetPath(1);
		controller.Save(path);
		return nullptr;
	});

	handler.RegisterFunction(LOAD_GAME, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 arguments expected (filename)");
		Path path = args.GetPath(1);
		controller.Load(path);
		return nullptr;
	});

	handler.RegisterFunction(GET_ABSOLUTE_PATH, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 arguments expected (relative path)");
		Path path = args.GetPath(1);
		return fileProvider.GetAbsolutePath(path).native();
	});

	handler.RegisterFunction(SET_GAMEPAD_BUTTONS_CALLBACK, [&](IArguments const& args) {
		if (args.GetCount() < 1)
			throw std::runtime_error("at least 1 arguments expected (function name, gamepadIndex, buttonIndex)");
		auto func = args.GetFunction(1);
		int filterGamepadIndex = -1;
		int filterButtonIndex = -1;
		if (args.GetCount() > 1)
			filterGamepadIndex = args.GetInt(2);
		if (args.GetCount() > 2)
			filterButtonIndex = args.GetInt(3);
		controller.SetGamepadButtonCallback([=](int gamepadIndex, int buttonIndex, bool state) {
			if (filterGamepadIndex != -1 && gamepadIndex + 1 != filterGamepadIndex)
				return false;
			if (filterButtonIndex != -1 && buttonIndex + 1 != filterButtonIndex)
				return false;
			func({ gamepadIndex + 1, buttonIndex + 1, state });
			return true;
		});
		return nullptr;
	});

	handler.RegisterFunction(SET_GAMEPAD_AXIS_CALLBACK, [&](IArguments const& args) {
		if (args.GetCount() < 1)
			throw std::runtime_error("at least 1 arguments expected (function name, gamepadIndex, axisIndex)");
		auto func = args.GetFunction(1);
		int filterGamepadIndex = -1;
		int filterAxisIndex = -1;
		if (args.GetCount() > 1)
			filterGamepadIndex = args.GetInt(2);
		if (args.GetCount() > 2)
			filterAxisIndex = args.GetInt(3);
		controller.SetGamepadAxisCallback([=](int gamepadIndex, int axisIndex, double horizontal, double vertical) {
			if (filterGamepadIndex != -1 && gamepadIndex + 1 != filterGamepadIndex)
				return false;
			if (filterAxisIndex != -1 && axisIndex + 1 != filterAxisIndex)
				return false;
			func({ gamepadIndex + 1, axisIndex + 1, horizontal, vertical });
			return true;
		});
		return nullptr;
	});
}
}
}