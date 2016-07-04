#include "ScriptRegisterFunctions.h"
#include "ScriptFunctionsProtocol.h"
#include "GameController.h"
#include "Network.h"
#include "../LogWriter.h"
#include "../OSSpecific.h"
#include "../ThreadPool.h"
#include "../view/CameraStrategy.h"
#include "../view/CameraFirstPerson.h"
#include "../view/GameView.h"

void RegisterModelFunctions(IScriptHandler & handler, CGameModel & model)
{
	handler.RegisterFunction(CREATE_TABLE, [&](IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 argument expected (width, height, texture)");
		float width = args.GetFloat(1);
		float height = args.GetFloat(2);
		std::string texture = args.GetStr(3);
		model.ResetLandscape(width, height, texture, 2, 2);
		return nullptr;
	});

	handler.RegisterFunction(GET_GLOBAL_PROPERTY, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (key)");
		std::string key = args.GetStr(1);
		return model.GetProperty(key);
	});

	handler.RegisterFunction(SET_GLOBAL_PROPERTY, [&](IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 arguments expected (key, value)");
		std::string key = args.GetStr(1);
		std::string value = args.GetStr(2);
		model.SetProperty(key, value);
		return nullptr;
	});

	handler.RegisterFunction(NEW_DECAL, [&](IArguments const& args) {
		if (args.GetCount() != 6)
			throw std::runtime_error("6 argument expected (decal, x, y, rotation, width, height)");
		sDecal decal;
		decal.texture = args.GetStr(1);
		decal.x = args.GetDbl(2);
		decal.y = args.GetDbl(3);
		decal.rotation = args.GetDbl(4);
		decal.width = args.GetDbl(5);
		decal.depth = args.GetDbl(6);
		model.GetLandscape().AddNewDecal(decal);
		return nullptr;
	});

	handler.RegisterFunction(NEW_STATIC_OBJECT, [&](IArguments const& args) {
		if (args.GetCount() != 4)
			throw std::runtime_error("4 argument expected (model, x, y, rotation)");
		std::string objectModel = args.GetStr(1);
		double x = args.GetDbl(2);
		double y = args.GetDbl(3);
		double rotation = args.GetDbl(4);
		model.GetLandscape().AddStaticObject(CStaticObject(objectModel, x, y, rotation));
		return nullptr;
	});
}

void RegisterViewFunctions(IScriptHandler & handler, CGameView & view)
{
	handler.RegisterFunction(CREATE_SKYBOX, [&](IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected (size, texture folder)");
		double size = args.GetDbl(1);
		std::string texture = args.GetStr(2);
		view.CreateSkybox(size, texture);
		return nullptr;
	});

	handler.RegisterFunction(CAMERA_STRATEGY, [&](IArguments const& args) {
		if (args.GetCount() != 4)
			throw std::runtime_error("4 argument expected (max trans x, max trans y, max scale, min scale)");
		double maxTransX = args.GetDbl(1);
		double maxTransY = args.GetDbl(2);
		double maxScale = args.GetDbl(3);
		double minScale = args.GetDbl(4);
		view.SetCamera(new CCameraStrategy(maxTransX, maxTransY, maxScale, minScale));
		return nullptr;
	});

	handler.RegisterFunction(CAMERA_FIRST_PERSON, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected ()");
		view.SetCamera(new CCameraFirstPerson());
		return nullptr;
	});

	handler.RegisterFunction(ENABLE_RULER, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view.GetRuler().Enable();
		return nullptr;
	});

	handler.RegisterFunction(MESSAGE_BOX, [&](IArguments const& args) {
		//TODO: rewrite within UI class
		if (args.GetCount() < 1 || args.GetCount() > 2)
			throw std::runtime_error("1 or 2 argument expected (text, caption)");
		std::string text = args.GetStr(1);
		std::string caption = "";
		if (args.GetCount() == 2)
			caption = args.GetStr(2);
		ShowMessageBox(text, caption);
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

	handler.RegisterFunction(ENABLE_LIGHT_SOURCE, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (index)");
		long i = args.GetLong(1) - 1;
		if (i < 0 || i > 7) throw std::runtime_error("only 8 light sources are supported");
		view.EnableLight(i, true);
		return nullptr;
	});

	handler.RegisterFunction(DISABLE_LIGHT_SOURCE, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (index)");
		int i = args.GetInt(1) - 1;
		if (i < 0 || i > 7) throw std::runtime_error("only 8 light sources are supported");
		view.EnableLight(i, false);
		return nullptr;
	});

	handler.RegisterFunction(SET_LIGHT_SOURCE_POSITION, [&](IArguments const& args)	{
		if (args.GetCount() != 4)
			throw std::runtime_error("4 argument expected (index, x, y, z)");
		int i = args.GetInt(1) - 1;
		if (i < 0 || i > 7) throw std::runtime_error("only 8 light sources are supported");
		float pos[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		pos[0] = args.GetFloat(2);
		pos[1] = args.GetFloat(3);
		pos[2] = args.GetFloat(4);
		view.SetLightPosition(i, pos);
		return nullptr;
	});

	handler.RegisterFunction(SET_LIGHT_SOURCE_AMBIENT, [&](IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 argument expected (index, r, g, b, a)");
		long i = args.GetLong(1) - 1;
		if (i < 0 || i > 7) throw std::runtime_error("only 8 light sources are supported");
		float color[4];
		color[0] = args.GetFloat(2);
		color[1] = args.GetFloat(3);
		color[2] = args.GetFloat(4);
		color[3] = args.GetFloat(5);
		view.SetLightColor(i, LightningType::AMBIENT, color);
		return nullptr;
	});

	handler.RegisterFunction(SET_LIGHT_SOURCE_DIFFUSE, [&](IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 argument expected (index, r, g, b, a)");
		long i = args.GetLong(1) - 1;
		if (i < 0 || i > 7) throw std::runtime_error("only 8 light sources are supported");
		float color[4];
		color[0] = args.GetFloat(2);
		color[1] = args.GetFloat(3);
		color[2] = args.GetFloat(4);
		color[3] = args.GetFloat(5);
		view.SetLightColor(i, LightningType::DIFFUSE, color);
		return nullptr;
	});

	handler.RegisterFunction(SET_LIGHT_SOURCE_SPECULAR, [&](IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 argument expected (index, r, g, b, a)");
		long i = args.GetLong(1) - 1;
		if (i < 0 || i > 7) throw std::runtime_error("only 8 light sources are supported");
		float color[4];
		color[0] = args.GetFloat(2);
		color[1] = args.GetFloat(3);
		color[2] = args.GetFloat(4);
		color[3] = args.GetFloat(5);
		view.SetLightColor(i, LightningType::SPECULAR, color);
		return nullptr;
	});

	handler.RegisterFunction(ENABLE_VERTEX_LIGHTING, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view.EnableVertexLightning(true);
		return nullptr;
	});

	handler.RegisterFunction(DISABLE_VERTEX_LIGHTING, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view.EnableVertexLightning(false);
		return nullptr;
	});

	handler.RegisterFunction(ENABLE_SHADOW_MAP, [&](IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 arguments (shadowMap size, shadow max Angle) expected");
		int size = args.GetInt(1);
		float angle = args.GetFloat(2);
		view.EnableShadowMap(size, angle);
		return nullptr;
	});

	handler.RegisterFunction(DISABLE_SHADOW_MAP, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view.DisableShadowMap();
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
		std::string title = args.GetStr(1);
		view.SetWindowTitle(title);
		return nullptr;
	});

	handler.RegisterFunction(PRELOAD, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (image path)");
		std::string image = args.GetStr(1);
		view.Preload(image);
		return nullptr;
	});

	handler.RegisterFunction(PRELOAD_MODEL, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (model name)");
		std::string model = args.GetStr(1);
		view.GetModelManager().LoadIfNotExist(model);
		return nullptr;
	});

	handler.RegisterFunction(LOAD_MODULE, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (module file)");
		std::string module = args.GetStr(1);
		view.LoadModule(module);
		return nullptr;
	});

	handler.RegisterFunction(SET_SHADERS, [&](IArguments const& args) {
		int n = args.GetCount();
		auto pathSource = view.GetAsyncFileProvider();
		std::string vertex, fragment, geometry;
		if (n > 0) vertex = pathSource.GetShaderAbsolutePath(args.GetStr(1));
		if (n > 1) fragment = pathSource.GetShaderAbsolutePath(args.GetStr(2));
		if (n > 2) geometry = pathSource.GetShaderAbsolutePath(args.GetStr(3));
		if (n > 3) throw std::runtime_error("up to 3 argument expected (vertex shader, fragment shader, geometry shader)");
		view.GetShaderManager().NewProgram(vertex, fragment, geometry);
		return nullptr;
	});

	handler.RegisterFunction(UNIFORM_1I, [&](IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 arguments expected (uniform name, value)");
		std::string name = args.GetStr(1);
		int value = args.GetInt(2);
		view.GetShaderManager().SetUniformValue(name, 1, &value);
		return nullptr;
	});

	handler.RegisterFunction(UNIFORM_1F, [&](IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 arguments expected (uniform name, value)");
		std::string name = args.GetStr(1);
		float value = args.GetFloat(2);
		view.GetShaderManager().SetUniformValue(name, 1, &value);
		return nullptr;
	});

	handler.RegisterFunction(UNIFORM_1FV, [&](IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
		std::string name = args.GetStr(1);
		unsigned int count = args.GetLong(2);
		std::vector<float> value = args.GetFloatArray(3);
		if (value.size() < count) throw std::runtime_error("Not enough elements in the array");
		view.GetShaderManager().SetUniformValue(name, count, &value[0]);
		return nullptr;
	});

	handler.RegisterFunction(UNIFORM_2FV, [&](IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
		std::string name = args.GetStr(1);
		unsigned int count = args.GetLong(2);
		std::vector<float> value = args.GetFloatArray(3);
		if (value.size() < count * 2) throw std::runtime_error("Not enough elements in the array");
		view.GetShaderManager().SetUniformValue2(name, count, &value[0]);
		return nullptr;
	});

	handler.RegisterFunction(UNIFORM_3FV, [&](IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
		std::string name = args.GetStr(1);
		unsigned int count = args.GetLong(2);
		std::vector<float> value = args.GetFloatArray(3);
		if (value.size() < count * 3) throw std::runtime_error("Not enough elements in the array");
		view.GetShaderManager().SetUniformValue3(name, count, &value[0]);
		return nullptr;
	});

	handler.RegisterFunction(UNIFORM_4FV, [&](IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
		std::string name = args.GetStr(1);
		unsigned int count = args.GetLong(2);
		std::vector<float> value = args.GetFloatArray(3);
		if (value.size() < count * 4) throw std::runtime_error("Not enough elements in the array");
		view.GetShaderManager().SetUniformValue4(name, count, &value[0]);
		return nullptr;
	});

	handler.RegisterFunction(UNIFORM_MATRIX4V, [&](IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
		std::string name = args.GetStr(1);
		unsigned int count = args.GetLong(2);
		std::vector<float> value = args.GetFloatArray(3);
		if (value.size() < count * 16) throw std::runtime_error("Not enough elements in the array");
		view.GetShaderManager().SetUniformMatrix4(name, count, &value[0]);
		return nullptr;
	});

	handler.RegisterFunction(NEW_PARTICLE_EFFECT, [&](IArguments const& args) {
		if (args.GetCount() != 7)
			throw std::runtime_error("7 arguments expected (effect file, x, y, z coordinates, rotation, scale, lifetime)");
		std::string file = view.GetAsyncFileProvider().GetAbsolutePath(args.GetStr(1));
		double x = args.GetDbl(2);
		double y = args.GetDbl(3);
		double z = args.GetDbl(4);
		double rot = args.GetDbl(5);
		double scale = args.GetDbl(6);
		float lifetime = args.GetFloat(7);
		view.GetParticleSystem().AddEffect(file, x, y, z, rot, scale, lifetime);
		return nullptr;
	});

	handler.RegisterFunction(NEW_PARTICLE_TRACER, [&](IArguments const& args) {
		if (args.GetCount() != 10)
			throw std::runtime_error("10 arguments expected (effect file, begin coordinates, end coordinates, rotation, scale, speed)");
		std::string file = view.GetAsyncFileProvider().GetAbsolutePath(args.GetStr(1));
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
	});

	handler.RegisterFunction(PLAY_SOUND, [&](IArguments const& args) {
		if (args.GetCount() < 1 || args.GetCount() > 2)
			throw std::runtime_error("1 or 2 arguments expected (file, volume)");
		std::string file = view.GetAsyncFileProvider().GetAbsolutePath(args.GetStr(1));
		float volume = args.GetFloat(2);
		view.GetSoundPlayer().Play(file, volume);
		return nullptr;
	});

	handler.RegisterFunction(PLAY_SOUND_POSITION, [&](IArguments const& args) {
		if (args.GetCount() < 4 || args.GetCount() > 5)
			throw std::runtime_error("4 or 5 arguments expected (file, x, y, z, volume)");
		std::string file = view.GetAsyncFileProvider().GetAbsolutePath(args.GetStr(1));
		double x = args.GetDbl(2);
		double y = args.GetDbl(3);
		double z = args.GetDbl(4);
		float volume = args.GetFloat(5);
		view.GetSoundPlayer().PlaySoundPosition(file, CVector3d(x, y, z), volume);
		return nullptr;
	});

	handler.RegisterFunction(PLAY_SOUND_PLAYLIST, [&](IArguments const& args) {
		int n = args.GetCount();
		if (n < 2 || n > 5)
			throw std::runtime_error("2 to 5 arguments expected (name, list or tracks, volume, shuffle, repeat)");
		std::string name = args.GetStr(1);
		std::vector<std::string> files = args.GetStrArray(2);
		for (auto& file : files)
		{
			file = view.GetAsyncFileProvider().GetAbsolutePath(file);
		}
		float volume = n > 2 ? args.GetFloat(3) : 1.0f;
		bool shuffle = n > 3 ? args.GetBool(4) : false;
		bool repeat = n > 4 ? args.GetBool(5) : false;
		view.GetSoundPlayer().PlaySoundPlaylist(name, files, volume, shuffle, repeat);
		return nullptr;
	});

	handler.RegisterFunction(GET_RENDERER_NAME, [&](IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		return view.GetRenderer().GetName();
	});
}

auto GetCallbackFunction (IScriptHandler & handler, IArguments const& args, int index = 1)
{
	std::function<void()> function;
	if (args.IsStr(index))
	{
		std::string func = args.GetStr(1);
		if (!func.empty())
		{
			function = [func, &handler]()
			{
				handler.CallFunction(func);
			};
		}
	}
	return function;
};

void RegisterControllerFunctions(IScriptHandler & handler, CGameController & controller, CAsyncFileProvider & fileProvider, ThreadPool & threadPool)
{
	handler.RegisterFunction(DELETE_TIMED_CALLBACK, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (ID)");
		unsigned int id = args.GetLong(1);
		threadPool.RemoveTimedCallback(id);
		return nullptr;
	});

	handler.RegisterFunction(GET_FILES_LIST, [&](IArguments const& args)
	{
		if (args.GetCount() != 3)
			throw std::runtime_error("3 arguments expected (path, mask, recursive)");
		std::string path = fileProvider.GetAbsolutePath(args.GetStr(1));
		std::string mask = args.GetStr(2);
		bool recursive = args.GetBool(3);
		std::vector<std::string> files = GetFiles(path, mask, recursive);
		return files;
	});

	handler.RegisterFunction(PRINT, [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (string)");
		std::string text = args.GetStr(1);
		LogWriter::WriteLine("LUA: " + text);
		return nullptr;
	});

	handler.RegisterFunction(RUN_SCRIPT, [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (filename)");
		std::string filename = fileProvider.GetAbsolutePath(args.GetStr(1));
		handler.RunScript(filename);
		return nullptr;
	});

	handler.RegisterFunction(SET_SELECTION_CALLBACK, [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		std::function<void()> function = GetCallbackFunction(handler, args, 1);
		controller.SetSelectionCallback(function);
		return nullptr;
	});

	handler.RegisterFunction(SET_UPDATE_CALLBACK, [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		std::function<void()> function = GetCallbackFunction(handler, args, 1);
		controller.SetUpdateCallback(function);
		return nullptr;
	});

	handler.RegisterFunction(SET_ON_STATE_RECEIVED_CALLBACK, [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		std::function<void()> function = GetCallbackFunction(handler, args, 1);
		controller.GetNetwork().SetStateRecievedCallback(function);
		return nullptr;
	});

	handler.RegisterFunction(SET_ON_STRING_RECEIVED_CALLBACK, [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		std::string func = args.GetStr(1);
		std::function<void(std::string const&)> function;
		if (!func.empty())
		{
			function = [func, &handler](const std::string param)
			{
				handler.CallFunction(func, { param });
			};
		}
		controller.GetNetwork().SetStringRecievedCallback(function);
		return nullptr;
	});

	handler.RegisterFunction(SET_TIMED_CALLBACK, [&](IArguments const& args)
	{
		if (args.GetCount() != 3)
			throw std::runtime_error("3 argument expected (funcName, time, repeat)");
		std::string func = args.GetStr(1);
		unsigned int time = args.GetLong(2);
		bool repeat = args.GetBool(3);
		size_t index = threadPool.AddTimedCallback([=, &handler]() {handler.CallFunction(func);}, time, repeat);
		return (int)index;
	});

	handler.RegisterFunction(SET_LMB_CALLBACK, [&](IArguments const& args)
	{
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected (function name, disable default behavior)");
		std::string func = args.GetStr(1);
		bool disable = args.GetBool(2);
		auto callback = [=, &handler](std::shared_ptr<IObject> obj, std::string const& type, double x, double y, double z) {
			FunctionArgument instance(obj.get(), "Object");
			handler.CallFunction(func, { instance, type, x, y, z });
			return disable;
		};
		controller.SetLMBCallback(callback);
		return nullptr;
	});

	handler.RegisterFunction(SET_RMB_CALLBACK, [&](IArguments const& args)
	{
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected (function name, disable default behavior)");
		std::string func = args.GetStr(1);
		bool disable = args.GetBool(2);
		auto callback = [=, &handler](std::shared_ptr<IObject> obj, std::string const& type, double x, double y, double z) {
			FunctionArgument instance(obj.get(), type);
			handler.CallFunction(func, { instance, x, y, z });
			return disable;
		};
		controller.SetRMBCallback(callback);
		return nullptr;
	});

	handler.RegisterFunction(BIND_KEY, [&](IArguments const& args)
	{
		if (args.GetCount() != 5)
			throw std::runtime_error("5 argument expected (key, shift, ctrl, alt, funcName)");
		unsigned char key = static_cast<unsigned char>(args.GetLong(1));
		bool shift = args.GetBool(2);
		bool ctrl = args.GetBool(3);
		bool alt = args.GetBool(4);
		std::function<void()> function = GetCallbackFunction(handler, args, 1);
		controller.BindKey(key, shift, ctrl, alt, function);
		return nullptr;
	});

	handler.RegisterFunction(UNDO, [&](IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		controller.GetCommandHandler().Undo();
		return nullptr;
	});

	handler.RegisterFunction(REDO, [&](IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		controller.GetCommandHandler().Redo();
		return nullptr;
	});

	handler.RegisterFunction(LINE_OF_SIGHT, [&](IArguments const& args)
	{
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected (source, target)");
		IObject* shootingModel = (IObject*)args.GetClassInstance(1);
		IObject* target = (IObject*)args.GetClassInstance(2);
		return FunctionArgument(static_cast<int>(controller.GetLineOfSight(shootingModel, target)));
	});

	handler.RegisterFunction(BEGIN_ACTION_COMPOUND, [&](IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		controller.GetCommandHandler().BeginCompound();
		return nullptr;
	});

	handler.RegisterFunction(END_ACTION_COMPOUND, [&](IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		controller.GetCommandHandler().EndCompound();
		return nullptr;
	});

	handler.RegisterFunction(NET_HOST, [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (port)");
		unsigned int port = args.GetLong(1);
		controller.GetNetwork().Host(static_cast<unsigned short>(port));
		return nullptr;
	});

	handler.RegisterFunction(NET_CLIENT, [&](IArguments const& args)
	{
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected (ip, port)");
		std::string ip = args.GetStr(1);
		unsigned short port = static_cast<unsigned short>(args.GetLong(2));
		controller.GetNetwork().Client(ip.c_str(), port);
		return nullptr;
	});

	handler.RegisterFunction(NET_SEND_MESSAGE, [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 arguments expected (message)");
		std::string message = args.GetStr(1);
		controller.GetNetwork().SendMessage(message);
		return nullptr;
	});

	handler.RegisterFunction(SAVE_GAME, [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 arguments expected (filename)");
		std::string path = args.GetStr(1);
		controller.Save(path);
		return nullptr;
	});

	handler.RegisterFunction(LOAD_GAME, [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 arguments expected (filename)");
		std::string path = args.GetStr(1);
		controller.Load(path);
		return nullptr;
	});

	handler.RegisterFunction(GET_ABSOLUTE_PATH, [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 arguments expected (relative path)");
		std::string path = args.GetStr(1);
		return fileProvider.GetAbsolutePath(path);
	});

	handler.RegisterFunction(SET_GAMEPAD_BUTTONS_CALLBACK, [&](IArguments const& args) {
		if (args.GetCount() < 1)
			throw std::runtime_error("at least 1 arguments expected (function name, gamepadIndex, buttonIndex)");
		std::string functionName = args.GetStr(1);
		int filterGamepadIndex = -1;
		int filterButtonIndex = -1;
		if (args.GetCount() > 1)
			filterGamepadIndex = args.GetInt(2);
		if (args.GetCount() > 2)
			filterButtonIndex = args.GetInt(3);
		controller.SetGamepadButtonCallback([=, &handler](int gamepadIndex, int buttonIndex, bool state) {
			if (filterGamepadIndex != -1 && gamepadIndex + 1 != filterGamepadIndex) return false;
			if (filterButtonIndex != -1 && buttonIndex + 1 != filterButtonIndex) return false;
			handler.CallFunction(functionName, {gamepadIndex + 1, buttonIndex + 1, state});
			return true;
		});
		return nullptr;
	});

	handler.RegisterFunction(SET_GAMEPAD_AXIS_CALLBACK, [&](IArguments const& args) {
		if (args.GetCount() < 1)
			throw std::runtime_error("at least 1 arguments expected (function name, gamepadIndex, axisIndex)");
		std::string functionName = args.GetStr(1);
		int filterGamepadIndex = -1;
		int filterAxisIndex = -1;
		if (args.GetCount() > 1)
			filterGamepadIndex = args.GetInt(2);
		if (args.GetCount() > 2)
			filterAxisIndex = args.GetInt(3);
		controller.SetGamepadAxisCallback([=, &handler](int gamepadIndex, int axisIndex, double horizontal, double vertical) {
			if (filterGamepadIndex != -1 && gamepadIndex + 1 != filterGamepadIndex) return false;
			if (filterAxisIndex != -1 && axisIndex + 1 != filterAxisIndex) return false;
			handler.CallFunction(functionName, { gamepadIndex + 1, axisIndex + 1, horizontal, vertical });
			return true;
		});
		return nullptr;
	});
}
