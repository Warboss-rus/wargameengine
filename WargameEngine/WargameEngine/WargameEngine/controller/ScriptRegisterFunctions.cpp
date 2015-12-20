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

FunctionArgument CreateTable(IArguments const& args)
{
	if (args.GetCount() != 3)
        throw std::runtime_error("3 argument expected (width, height, texture)");
	float width = args.GetFloat(1);
	float height = args.GetFloat(2);
	std::string texture = args.GetStr(3);
	auto view = CGameView::GetInstance().lock();
	view->GetModel().ResetLandscape(width, height, texture, 2, 2);
	view->ResetTable();
	return nullptr;
}

FunctionArgument CreateSkybox(IArguments const& args)
{
	if (args.GetCount() != 2)
        throw std::runtime_error("2 argument expected (size, texture folder)");
	double size = args.GetDbl(1);
	std::string texture = args.GetStr(2);
	CGameView::GetInstance().lock()->CreateSkybox(size, texture);
	return nullptr;
}

FunctionArgument CameraStrategy(IArguments const& args)
{
	if (args.GetCount() != 4)
        throw std::runtime_error("4 argument expected (max trans x, max trans y, max scale, min scale)");
	double maxTransX = args.GetDbl(1);
	double maxTransY = args.GetDbl(2);
	double maxScale = args.GetDbl(3);
	double minScale = args.GetDbl(4);
	CGameView::GetInstance().lock()->SetCamera(new CCameraStrategy(maxTransX, maxTransY, maxScale, minScale));
	return nullptr;
}

FunctionArgument CameraFirstPerson(IArguments const& args)
{
	if (args.GetCount() != 0)
		throw std::runtime_error("no arguments expected ()");
	CGameView::GetInstance().lock()->SetCamera(new CCameraFirstPerson());
	return nullptr;
}



FunctionArgument Ruler(IArguments const& args)
{
	if (args.GetCount() != 0)
        throw std::runtime_error("no arguments expected");
	CGameView::GetInstance().lock()->GetRuler().Enable();
	return nullptr;
}

FunctionArgument ShMessageBox(IArguments const& args)
{
	if (args.GetCount() < 1 || args.GetCount() > 2)
        throw std::runtime_error("1 or 2 argument expected (text, caption)");
	std::string text =  args.GetStr(1);
	std::string caption = "";
	if(args.GetCount() == 2)
		caption = args.GetStr(2);
	ShowMessageBox(text, caption);
	return nullptr;
}

FunctionArgument GetGlobalProperty(IArguments const& args)
{
	if(args.GetCount() != 1)
		throw std::runtime_error("1 argument expected (key)");
	std::string key = args.GetStr(1);
	return CGameView::GetInstance().lock()->GetModel().GetProperty(key);
}

FunctionArgument SetGlobalProperty(IArguments const& args)
{
	if(args.GetCount() != 2)
		throw std::runtime_error("2 arguments expected (key, value)");
	std::string key = args.GetStr(1);
	std::string value = args.GetStr(2);
	CGameView::GetInstance().lock()->GetModel().SetProperty(key, value);
	return nullptr;
}

FunctionArgument DeleteTimedCallback(IArguments const& args)
{
	if(args.GetCount() != 1)
		throw std::runtime_error("1 argument expected (ID)");
	unsigned int id = args.GetLong(1);
	ThreadPool::RemoveTimedCallback(id);
	return nullptr;
}

FunctionArgument ResizeWindow(IArguments const& args)
{
	if(args.GetCount() != 2)
		throw std::runtime_error("2 argument expected (height, width)");
	int height = args.GetInt(1);
	int width = args.GetInt(2);
	CGameView::GetInstance().lock()->ResizeWindow(height, width);
	return nullptr;
}

FunctionArgument SetShaders(IArguments const& args)
{
	int n = args.GetCount();
	std::string vertex, fragment, geometry;
	if(n > 0) vertex = args.GetStr(1);
	if(n > 1) fragment = args.GetStr(2);
	if(n > 2) geometry = args.GetStr(3);
	if(n > 3) throw std::runtime_error("up to 3 argument expected (vertex shader, fragment shader, geometry shader)");
	CGameView::GetInstance().lock()->NewShaderProgram(vertex, fragment, geometry);
	return nullptr;
}

FunctionArgument EnableLight(IArguments const& args)
{
	if(args.GetCount() != 1)
		throw std::runtime_error("1 argument expected (index)");
	size_t i = args.GetLong(1) - 1;
	if (i < 0 || i > 7) throw std::runtime_error("only 8 light sources are supported");
	CGameView::GetInstance().lock()->EnableLight(i, true);
	return nullptr;
}

FunctionArgument DisableLight(IArguments const& args)
{
	if(args.GetCount() != 1)
		throw std::runtime_error("1 argument expected (index)");
	int i = args.GetInt(1) - 1;
	if (i < 0 || i > 7) throw std::runtime_error("only 8 light sources are supported");
	CGameView::GetInstance().lock()->EnableLight(i, false);
	return nullptr;
}

FunctionArgument SetLightPosition(IArguments const& args)
{
	if(args.GetCount() != 4)
		throw std::runtime_error("4 argument expected (index, x, y, z)");
	int i = args.GetInt(1) - 1;
	if (i < 0 || i > 7) throw std::runtime_error("only 8 light sources are supported");
	float pos[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	pos[0] = args.GetFloat(2);
	pos[1] = args.GetFloat(3);
	pos[2] = args.GetFloat(4);
	CGameView::GetInstance().lock()->SetLightPosition(i, pos);
	return nullptr;
}

FunctionArgument SetLightAmbient(IArguments const& args)
{
	if(args.GetCount() != 5)
		throw std::runtime_error("5 argument expected (index, r, g, b, a)");
	size_t i = args.GetLong(1) - 1;
	if (i < 0 || i > 7) throw std::runtime_error("only 8 light sources are supported");
	float color[4];
	color[0] = args.GetFloat(2);
	color[1] = args.GetFloat(3);
	color[2] = args.GetFloat(4);
	color[3] = args.GetFloat(5);
	CGameView::GetInstance().lock()->SetLightColor(i, LightningType::AMBIENT, color);
	return nullptr;
}

FunctionArgument SetLightDiffuse(IArguments const& args)
{
	if(args.GetCount() != 5)
		throw std::runtime_error("5 argument expected (index, r, g, b, a)");
	size_t i = args.GetLong(1) - 1;
	if (i < 0 || i > 7) throw std::runtime_error("only 8 light sources are supported");
	float color[4];
	color[0] = args.GetFloat(2);
	color[1] = args.GetFloat(3);
	color[2] = args.GetFloat(4);
	color[3] = args.GetFloat(5);
	CGameView::GetInstance().lock()->SetLightColor(i, LightningType::DIFFUSE, color);
	return nullptr;
}

FunctionArgument SetLightSpecular(IArguments const& args)
{
	if(args.GetCount() != 5)
		throw std::runtime_error("5 argument expected (index, r, g, b, a)");
	size_t i = args.GetLong(1) - 1;
	if (i < 0 || i > 7) throw std::runtime_error("only 8 light sources are supported");
	float color[4];
	color[0] = args.GetFloat(2);
	color[1] = args.GetFloat(3);
	color[2] = args.GetFloat(4);
	color[3] = args.GetFloat(5);
	CGameView::GetInstance().lock()->SetLightColor(i, LightningType::SPECULAR, color);
	return nullptr;
}

FunctionArgument EnableVertexLighting(IArguments const& args)
{
	if(args.GetCount() != 0)
		throw std::runtime_error("no arguments expected");
	CGameView::GetInstance().lock()->EnableVertexLightning(true);
	return nullptr;
}

FunctionArgument DisableVertexLighting(IArguments const& args)
{
	if(args.GetCount() != 0)
		throw std::runtime_error("no arguments expected");
	CGameView::GetInstance().lock()->EnableVertexLightning(false);
	return nullptr;
}

FunctionArgument EnableShadowMap(IArguments const& args)
{
	if (args.GetCount() != 2)
		throw std::runtime_error("2 arguments (shadowMap size, shadow max Angle) expected");
	int size = args.GetInt(1);
	float angle = args.GetFloat(2);
	CGameView::GetInstance().lock()->EnableShadowMap(size, angle);
	return nullptr;
}

FunctionArgument DisableShadowMap(IArguments const& args)
{
	if (args.GetCount() != 0)
		throw std::runtime_error("no arguments expected");
	CGameView::GetInstance().lock()->DisableShadowMap();
	return nullptr;
}

FunctionArgument EnableMSAA(IArguments const& args)
{
	if (args.GetCount() != 0)
		throw std::runtime_error("no arguments expected");
	CGameView::GetInstance().lock()->EnableMSAA(true);
	return nullptr;
}

FunctionArgument DisableMSAA(IArguments const& args)
{
	if (args.GetCount() != 0)
		throw std::runtime_error("no arguments expected");
	CGameView::GetInstance().lock()->EnableMSAA(false);
	return nullptr;
}

FunctionArgument SetAnisotropy(IArguments const& args)
{
	if (args.GetCount() != 1)
		throw std::runtime_error("1 argument expected");
	float a = args.GetFloat(1);
	CGameView::GetInstance().lock()->SetAnisotropyLevel(a);
	return nullptr;
}

FunctionArgument GetMaxAnisotropy(IArguments const& args)
{
	if (args.GetCount() != 0)
		throw std::runtime_error("no arguments expected");
	return CGameView::GetInstance().lock()->GetMaxAnisotropy();
}

FunctionArgument ClearResources(IArguments const& args)
{
	if (args.GetCount() != 0)
		throw std::runtime_error("no arguments expected");
	CGameView::GetInstance().lock()->ClearResources();
	return nullptr;
}

FunctionArgument SetWindowTitle(IArguments const& args)
{
	if (args.GetCount() != 1)
		throw std::runtime_error("1 arguments expected (title)");
	std::string title = args.GetStr(1);
	CGameView::GetInstance().lock()->SetWindowTitle(title);
	return nullptr;
}

FunctionArgument Preload(IArguments const& args)
{
	if (args.GetCount() != 1)
		throw std::runtime_error("1 argument expected (image path)");
	std::string image = args.GetStr(1);
	CGameView::GetInstance().lock()->Preload(image);
	return nullptr;
}

FunctionArgument PreloadModel(IArguments const& args)
{
	if (args.GetCount() != 1)
		throw std::runtime_error("1 argument expected (model name)");
	std::string model = args.GetStr(1);
	auto view = CGameView::GetInstance().lock();
	view->GetModelManager().LoadIfNotExist(model);
	return nullptr;
}

FunctionArgument LoadModuleFile(IArguments const& args)
{
	if (args.GetCount() != 1)
		throw std::runtime_error("1 argument expected (module file)");
	std::string module = args.GetStr(1);
	CGameView::GetInstance().lock()->LoadModule(module);
	return nullptr;
}

FunctionArgument GetFilesList(IArguments const& args)
{
	if (args.GetCount() != 3)
		throw std::runtime_error("3 arguments expected (path, mask, recursive)");
	std::string path = args.GetStr(1);
	std::string mask = args.GetStr(2);
	bool recursive = args.GetBool(3);
	std::vector<std::string> files = GetFiles(path, mask, recursive);
	return files;
}

FunctionArgument Uniform1i(IArguments const& args)
{
	if (args.GetCount() != 2)
		throw std::runtime_error("2 arguments expected (uniform name, value)");
	std::string name = args.GetStr(1);
	int value = args.GetInt(2);
	CGameView::GetInstance().lock()->GetShaderManager().SetUniformValue(name, 1, &value);
	return nullptr;
}

FunctionArgument Uniform1f(IArguments const& args)
{
	if (args.GetCount() != 2)
		throw std::runtime_error("2 arguments expected (uniform name, value)");
	std::string name = args.GetStr(1);
	float value = args.GetFloat(2);
	CGameView::GetInstance().lock()->GetShaderManager().SetUniformValue(name, 1, &value);
	return nullptr;
}

FunctionArgument Uniform1fv(IArguments const& args)
{
	if (args.GetCount() != 3)
		throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
	std::string name = args.GetStr(1);
	unsigned int count = args.GetLong(2);
	std::vector<float> value = args.GetFloatArray(3);
	if (value.size() < count) throw std::runtime_error("Not enough elements in the array");
	CGameView::GetInstance().lock()->GetShaderManager().SetUniformValue(name, count, &value[0]);
	return nullptr;
}

FunctionArgument Uniform2fv(IArguments const& args)
{
	if (args.GetCount() != 3)
		throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
	std::string name = args.GetStr(1);
	unsigned int count = args.GetLong(2);
	std::vector<float> value = args.GetFloatArray(3);
	if (value.size() < count * 2) throw std::runtime_error("Not enough elements in the array");
	CGameView::GetInstance().lock()->GetShaderManager().SetUniformValue2(name, count, &value[0]);
	return nullptr;
}

FunctionArgument Uniform3fv(IArguments const& args)
{
	if (args.GetCount() != 3)
		throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
	std::string name = args.GetStr(1);
	unsigned int count = args.GetLong(2);
	std::vector<float> value = args.GetFloatArray(3);
	if (value.size() < count * 3) throw std::runtime_error("Not enough elements in the array");
	CGameView::GetInstance().lock()->GetShaderManager().SetUniformValue3(name, count, &value[0]);
	return nullptr;
}

FunctionArgument Uniform4fv(IArguments const& args)
{
	if (args.GetCount() != 3)
		throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
	std::string name = args.GetStr(1);
	unsigned int count = args.GetLong(2);
	std::vector<float> value = args.GetFloatArray(3);
	if (value.size() < count * 4) throw std::runtime_error("Not enough elements in the array");
	CGameView::GetInstance().lock()->GetShaderManager().SetUniformValue4(name, count, &value[0]);
	return nullptr;
}

FunctionArgument UniformMatrix4fv(IArguments const& args)
{
	if (args.GetCount() != 3)
		throw std::runtime_error("3 arguments expected (uniform name, values count, values array)");
	std::string name = args.GetStr(1);
	unsigned int count = args.GetLong(2);
	std::vector<float> value = args.GetFloatArray(3);
	if (value.size() < count * 16) throw std::runtime_error("Not enough elements in the array");
	CGameView::GetInstance().lock()->GetShaderManager().SetUniformMatrix4(name, count, &value[0]);
	return nullptr;
}

FunctionArgument EnableGPUSkinning(IArguments const& args)
{
	if (args.GetCount() != 0)
		throw std::runtime_error("no arguments expected");
	CGameView::GetInstance().lock()->EnableGPUSkinning(true);
	return nullptr;
}

FunctionArgument DisableGPUSkinning(IArguments const& args)
{
	if (args.GetCount() != 0)
		throw std::runtime_error("no arguments expected");
	CGameView::GetInstance().lock()->EnableGPUSkinning(false);
	return nullptr;
}

FunctionArgument NewParticleEffect(IArguments const& args)
{
	if (args.GetCount() != 7)
		throw std::runtime_error("7 arguments expected (effect file, x, y, z coordinates, rotation, scale, lifetime)");
	std::string file = args.GetStr(1);
	double x = args.GetDbl(2);
	double y = args.GetDbl(3);
	double z = args.GetDbl(4);
	double rot = args.GetDbl(5);
	double scale = args.GetDbl(6);
	float lifetime = args.GetFloat(7);
	CGameView::GetInstance().lock()->GetParticleSystem().AddEffect(file, x, y, z, rot, scale, lifetime);
	return nullptr;
}

FunctionArgument NewParticleTracer(IArguments const& args)
{
	if (args.GetCount() != 10)
		throw std::runtime_error("10 arguments expected (effect file, begin coordinates, end coordinates, rotation, scale, speed)");
	std::string file = args.GetStr(1);
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
	CGameView::GetInstance().lock()->GetParticleSystem().AddTracer(file, begin, end, rot, scale, speed);
	return nullptr;
}

FunctionArgument PlaySound(IArguments const& args)
{
	if (args.GetCount() < 1 || args.GetCount() > 2)
		throw std::runtime_error("1 or 2 arguments expected (file, volume)");
	std::string file = args.GetStr(1);
	float volume = args.GetFloat(2);
	CGameView::GetInstance().lock()->GetSoundPlayer().Play(file, volume);
	return nullptr;
}

FunctionArgument PlaySoundPosition(IArguments const& args)
{
	if (args.GetCount() < 4 || args.GetCount() > 5)
		throw std::runtime_error("4 or 5 arguments expected (file, x, y, z, volume)");
	std::string file = args.GetStr(1);
	double x = args.GetDbl(2);
	double y = args.GetDbl(3);
	double z = args.GetDbl(4);
	float volume = args.GetFloat(5);
	CGameView::GetInstance().lock()->GetSoundPlayer().PlaySoundPosition(file, CVector3d(x, y, z), volume);
	return nullptr;
}

FunctionArgument PlaySoundPlaylist(IArguments const& args)
{
	int n = args.GetCount();
	if (n < 2 || n > 5)
		throw std::runtime_error("2 or 5 arguments expected (name, list or tracks, volume, shuffle, repeat)");
	std::string name = args.GetStr(1);
	std::vector<std::string> files = args.GetStrArray(2);
	float volume = n > 2 ? args.GetFloat(3): 1.0f;
	bool shuffle = n > 3 ? args.GetBool(4) : false;
	bool repeat = n > 4 ? args.GetBool(5) : false;
	CGameView::GetInstance().lock()->GetSoundPlayer().PlaySoundPlaylist(name, files, volume, shuffle, repeat);
	return nullptr;
}

FunctionArgument NewDecal(IArguments const& args)
{
	if (args.GetCount() != 6)
		throw std::runtime_error("6 argument expected (decal, x, y, rotation, width, height)");
	sDecal decal;
	decal.texture = args.GetStr(1);
	decal.x = args.GetDbl(2);
	decal.y = args.GetDbl(3);
	decal.rotation = args.GetDbl(4);
	decal.width = args.GetDbl(5);
	decal.depth = args.GetDbl(6);
	auto view = CGameView::GetInstance().lock();
	view->GetModel().GetLandscape().AddNewDecal(decal);
	return nullptr;
}

FunctionArgument NewStaticObject(IArguments const& args)
{
	if (args.GetCount() != 4)
		throw std::runtime_error("4 argument expected (model, x, y, rotation)");
	std::string model = args.GetStr(1);
	double x = args.GetDbl(2);
	double y = args.GetDbl(3);
	double rotation = args.GetDbl(4);
	auto view = CGameView::GetInstance().lock();
	view->GetModel().GetLandscape().AddStaticObject(CStaticObject(model, x, y, rotation));
	return nullptr;
}

FunctionArgument Print(IArguments const& args)
{
	if (args.GetCount() != 1)
		throw std::runtime_error("1 argument expected (string)");
	std::string text = args.GetStr(1);
	LogWriter::WriteLine("LUA: " + text);
	return nullptr;
}

FunctionArgument GetRendererName(IArguments const& args)
{
	if (args.GetCount() != 0)
		throw std::runtime_error("no argument expected");
	return CGameView::GetInstance().lock()->GetRenderer().GetName();
}

void RegisterFunctions(IScriptHandler & handler, CGameController & controller)
{
	handler.RegisterFunction("RunScript", [&](IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (filename)");
		std::string filename = args.GetStr(1);
		handler.RunScript(filename);
		return nullptr;
	});

	auto GetCallbackFunction = [&](IArguments const& args, int index = 1)
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

	handler.RegisterFunction("SetSelectionCallback", [&, GetCallbackFunction](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		std::function<void()> function = GetCallbackFunction(args, 1);
		controller.SetSelectionCallback(function);
		return nullptr;
	});

	handler.RegisterFunction("SetUpdateCallback", [&, GetCallbackFunction](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		std::function<void()> function = GetCallbackFunction(args, 1);
		controller.SetUpdateCallback(function);
		return nullptr;
	});

	handler.RegisterFunction("SetOnStateRecievedCallback", [&, GetCallbackFunction](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		std::function<void()> function = GetCallbackFunction(args, 1);
		controller.GetNetwork().SetStateRecievedCallback(function);
		return nullptr;
	});

	handler.RegisterFunction("SetOnStringRecievedCallback", [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		std::string func = args.GetStr(1);
		std::function<void(const char*)> function;
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

	handler.RegisterFunction("SetTimedCallback", [&handler](IArguments const& args)
	{
		if (args.GetCount() != 3)
			throw std::runtime_error("3 argument expected (funcName, time, repeat)");
		std::string func = args.GetStr(1);
		unsigned int time = args.GetLong(2);
		bool repeat = args.GetBool(3);
		unsigned int index = ThreadPool::AddTimedCallback([=, &handler]() {handler.CallFunction(func);}, time, repeat);
		return (int)index;
	});

	handler.RegisterFunction("SetLMBCallback", [&](IArguments const& args)
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

	handler.RegisterFunction("SetRMBCallback", [&](IArguments const& args)
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

	handler.RegisterFunction("BindKey", [&, GetCallbackFunction](IArguments const& args)
	{
		if (args.GetCount() != 5)
			throw std::runtime_error("5 argument expected (key, shift, ctrl, alt, funcName)");
		unsigned char key = static_cast<unsigned char>(args.GetLong(1));
		bool shift = args.GetBool(2);
		bool ctrl = args.GetBool(3);
		bool alt = args.GetBool(4);
		std::function<void()> function = GetCallbackFunction(args, 1);
		controller.BindKey(key, shift, ctrl, alt, function);
		return nullptr;
	});

	handler.RegisterFunction("Undo", [&](IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		controller.GetCommandHandler().Undo();
		return nullptr;
	});

	handler.RegisterFunction("Redo", [&](IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		controller.GetCommandHandler().Redo();
		return nullptr;
	});

	handler.RegisterFunction("LoS", [&](IArguments const& args)
	{
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected (source, target)");
		IObject* shootingModel = (IObject*)args.GetClassInstance(1);
		IObject* target = (IObject*)args.GetClassInstance(2);
		return controller.GetLineOfSight(shootingModel, target);
	});

	handler.RegisterFunction("BeginActionCompound", [&](IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		controller.GetCommandHandler().BeginCompound();
		return nullptr;
	});

	handler.RegisterFunction("EndActionCompound", [&](IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		controller.GetCommandHandler().EndCompound();
		return nullptr;
	});

	handler.RegisterFunction("NetHost", [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (port)");
		unsigned int port = args.GetLong(1);
		controller.GetNetwork().Host(static_cast<unsigned short>(port));
		return nullptr;
	});

	handler.RegisterFunction("NetClient", [&](IArguments const& args)
	{
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected (ip, port)");
		std::string ip = args.GetStr(1);
		unsigned short port = static_cast<unsigned short>(args.GetLong(2));
		controller.GetNetwork().Client(ip.c_str(), port);
		return nullptr;
	});

	handler.RegisterFunction("NetSendMessage", [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 arguments expected (message)");
		std::string message = args.GetStr(1);
		controller.GetNetwork().SendMessage(message);
		return nullptr;
	});

	handler.RegisterFunction("SaveGame", [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 arguments expected (filename)");
		std::string path = args.GetStr(1);
		controller.Save(path);
		return nullptr;
	});

	handler.RegisterFunction("LoadGame", [&](IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 arguments expected (filename)");
		std::string path = args.GetStr(1);
		controller.Load(path);
		return nullptr;
	});

	handler.RegisterFunction(CREATE_TABLE, CreateTable);
	handler.RegisterFunction(CREATE_SKYBOX, CreateSkybox);
	handler.RegisterFunction("CameraStrategy", CameraStrategy);
	handler.RegisterFunction("CameraFirstPerson", CameraFirstPerson);
	handler.RegisterFunction("Ruler", Ruler);
	handler.RegisterFunction("MessageBox", ShMessageBox);
	handler.RegisterFunction("GetGlobalProperty", GetGlobalProperty);
	handler.RegisterFunction("SetGlobalProperty", SetGlobalProperty);
	handler.RegisterFunction("DeleteTimedCallback", DeleteTimedCallback);
	handler.RegisterFunction("ResizeWindow", ResizeWindow);
	handler.RegisterFunction("SetShaders", SetShaders);
	handler.RegisterFunction("EnableLight", EnableLight);
	handler.RegisterFunction("DisableLight", DisableLight);
	handler.RegisterFunction("SetLightPosition", SetLightPosition);
	handler.RegisterFunction("SetLightAmbient", SetLightAmbient);
	handler.RegisterFunction("SetLightDiffuse", SetLightDiffuse);
	handler.RegisterFunction("SetLightSpecular", SetLightSpecular);
	handler.RegisterFunction("EnableVertexLighting", EnableVertexLighting);
	handler.RegisterFunction("DisableVertexLighting", DisableVertexLighting);
	handler.RegisterFunction("EnableShadowMap", EnableShadowMap);
	handler.RegisterFunction("DisableShadowMap", DisableShadowMap);
	handler.RegisterFunction("EnableMSAA", EnableMSAA);
	handler.RegisterFunction("DisableMSAA", DisableMSAA);
	handler.RegisterFunction("SetAnisotropy", SetAnisotropy);
	handler.RegisterFunction("GetMaxAnisotropy", GetMaxAnisotropy);
	handler.RegisterFunction("ClearResources", ClearResources);
	handler.RegisterFunction("SetWindowTitle", SetWindowTitle);
	handler.RegisterFunction("Preload", Preload);
	handler.RegisterFunction("PreloadModel", PreloadModel);
	handler.RegisterFunction("LoadModule", LoadModuleFile);
	handler.RegisterFunction("GetFilesList", GetFilesList);
	handler.RegisterFunction("Uniform1i", Uniform1i);
	handler.RegisterFunction("Uniform1f", Uniform1f);
	handler.RegisterFunction("Uniform1fv", Uniform1fv);
	handler.RegisterFunction("Uniform2fv", Uniform2fv);
	handler.RegisterFunction("Uniform3fv", Uniform3fv);
	handler.RegisterFunction("Uniform4fv", Uniform4fv);
	handler.RegisterFunction("UniformMatrix4fv", UniformMatrix4fv);
	handler.RegisterFunction("EnableGPUSkinning", EnableGPUSkinning);
	handler.RegisterFunction("DisableGPUSkinning", DisableGPUSkinning);
	handler.RegisterFunction("NewParticleEffect", NewParticleEffect);
	handler.RegisterFunction("NewParticleTracer", NewParticleTracer);
	handler.RegisterFunction("PlaySound", PlaySound);
	handler.RegisterFunction("PlaySoundPosition", PlaySoundPosition);
	handler.RegisterFunction("PlaySoundPlaylist", PlaySoundPlaylist);
	handler.RegisterFunction("NewDecal", NewDecal);
	handler.RegisterFunction("NewStaticObject", NewStaticObject);
	handler.RegisterFunction("print", Print);
	handler.RegisterFunction("GetRendererName", GetRendererName);
}