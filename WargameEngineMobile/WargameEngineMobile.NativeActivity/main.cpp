#include <unistd.h>
#include <android/log.h>
#include <dlfcn.h>
#include "android_native_app_glue.h"
#include "../../WargameEngine/Application.h"
#ifdef SOUND_FMOD
#include "..\..\WargameEngine\impl\SoundPlayerFMod.h"
#define SOUND_PLAYER_CLASS CSoundPlayerFMod
#else
#include "..\..\WargameEngine\impl\SoundPlayerOpenSLES.h"
#define SOUND_PLAYER_CLASS CSoundPlayerOpenSLES
#endif
#include "..\..\WargameEngine\impl\TextWriter.h"
#include "..\..\WargameEngine\impl\ScriptHandlerLua.h"
#include "..\..\WargameEngine\impl\NetSocket.h"
#include "..\..\WargameEngine\view\BuiltInImageReaders.h"
#include "..\..\WargameEngine\view\OBJModelFactory.h"
#include "..\..\WargameEngine\view\ColladaModelFactory.h"
#include "..\..\WargameEngine\view\WBMModelFactory.h"
#include "..\..\WargameEngine\impl\PhysicsEngineBullet.h"
#include "..\..\WargameEngine\impl\PathfindingMicroPather.h"
#ifdef HAS_ASSIMP
#include "../../WargameEngine/impl/AssimpModelLoader.h"
#endif
#ifdef RENDERER_VULKAN
#include "..\..\WargameEngine\impl\GameWindowAndroidVulkan.h"
#define WINDOW_CLASS CGameWindowAndroidVulkan
#else
#include "..\..\WargameEngine\impl\GameWindowAndroid.h"
#define WINDOW_CLASS CGameWindowAndroid
#endif
/*
* Copyright (C) 2010 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "WargameEngineMobile", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "WargameEngineMobile", __VA_ARGS__))

using namespace wargameEngine;
using namespace view;

/**
* Our saved state data.
*/
struct saved_state {
	
};

/**
* Shared state for our app.
*/
struct engine {
	struct android_app* app;

	struct saved_state state;
	WINDOW_CLASS * window;
};

/**
* Process the next input event.
*/
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
	struct engine* engine = (struct engine*)app->userData;
	engine->window->HandleInput(event);
	return 0;
}

/**
* Process the next main command.
*/
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
	struct engine* engine = (struct engine*)app->userData;
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		// The system has asked us to save our current state.  Do so.
		engine->app->savedState = malloc(sizeof(struct saved_state));
		*((struct saved_state*)engine->app->savedState) = engine->state;
		engine->app->savedStateSize = sizeof(struct saved_state);
		break;
	case APP_CMD_RESUME:
		if (engine->app->window)
		{
			engine->window->SetActive(true);
		}
		break;
	case APP_CMD_INIT_WINDOW:
		// The window is being shown, get it ready.
		if (engine->app->window != NULL) {
			engine->window->Init(engine->app->window);
			engine->window->DrawFrame();
		}
		break;
	case APP_CMD_TERM_WINDOW:
		// The window is being hidden or closed, clean it up.
		engine->window->Shutdown();
		engine->app->window = nullptr;
		break;
	case APP_CMD_GAINED_FOCUS:
		engine->window->SetActive(true);
		break;
	case APP_CMD_LOST_FOCUS:
		// Also stop animating.
		engine->window->SetActive(false);
		break;
	}
}

/**
* This is the main entry point of a native application that is using
* android_native_app_glue.  It runs in its own thread, with its own
* event loop for receiving input events and doing other things.
*/
void android_main(struct android_app* state) {
	struct engine engine;
	memset(&engine, 0, sizeof(engine));
	
	const std::string storage = getenv("EXTERNAL_STORAGE");
	wargameEngine::Context context;
	wargameEngine::Module module;
	if (module.name.empty())
	{
		module.script = "main.lua";
		module.textures = "texture/";
		module.models = "models/";
		module.folder = storage + "/WargameEngine/";
	}
	try
	{
#ifdef RENDERER_VULKAN
		dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
#endif
		context.window = std::make_unique<WINDOW_CLASS>(state);
	}
	catch (std::exception const& e)
	{
		std::string ex = e.what();
		LOGW(("Cannot create vulkan renderer: " + ex).c_str());
	}
#ifdef SOUND_FMOD
	void* lib = dlopen("libfmod.so", RTLD_NOW | RTLD_LOCAL);
	lib = dlopen("libfmodL.so", RTLD_NOW | RTLD_LOCAL);
#endif
	context.soundPlayer = std::make_unique<SOUND_PLAYER_CLASS>();
	context.textWriter = std::make_unique<CTextWriter>();
	context.physicsEngine = std::make_unique<CPhysicsEngineBullet>();
	static_cast<CTextWriter*>(context.textWriter.get())->AddFontLocation(storage + "/WargameEngine/");
	context.scriptHandler = std::make_unique<CScriptHandlerLua>();
	context.pathFinder = std::make_unique<CPathfindingMicroPather>();
	context.socketFactory = []() {
		return std::make_unique<CNetSocket>();
	};
	context.imageReaders.push_back(std::make_unique<CBmpImageReader>());
	context.imageReaders.push_back(std::make_unique<CTgaImageReader>());
	context.imageReaders.push_back(std::make_unique<CDdsImageReader>());
	context.imageReaders.push_back(std::make_unique<CStbImageReader>());
	context.modelReaders.push_back(std::make_unique<CObjModelFactory>());
	context.modelReaders.push_back(std::make_unique<CColladaModelFactory>());
	context.modelReaders.push_back(std::make_unique<CWBMModelFactory>());
#ifdef HAS_ASSIMP
	context.modelReaders.push_back(std::make_unique<CAssimpModelLoader>());
#endif

	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.window = reinterpret_cast<WINDOW_CLASS*>(context.window.get());
	engine.app = state;

	if (state->savedState != NULL) {
		// We are starting with a previous saved state; restore from it.
		engine.state = *(struct saved_state*)state->savedState;
	}
	//context.window->LaunchMainLoop();
	// loop waiting for stuff to do.
	Application app(std::move(context));
	app.Run(std::move(module));
}
