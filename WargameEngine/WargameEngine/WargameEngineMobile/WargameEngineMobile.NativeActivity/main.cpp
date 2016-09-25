#include <unistd.h>
#include <android/log.h>
#include "android_native_app_glue.h"
#include "..\..\WargameEngine\view\GameView.h"
#include "..\..\WargameEngine\SoundPlayerOpenSLES.h"
#include "..\..\WargameEngine\view\TextWriter.h"
#include "..\..\WargameEngine\ScriptHandlerLua.h"
#include "..\..\WargameEngine\NetSocket.h"
#include "..\..\WargameEngine\view\BuiltInImageReaders.h"
#include "..\..\WargameEngine\view\OBJModelFactory.h"
#include "..\..\WargameEngine\view\ColladaModelFactory.h"
#include "..\..\WargameEngine\view\WBMModelFactory.h"
#include "..\..\WargameEngine\view\GameWindowAndroid.h"
#include "..\..\WargameEngine\PhysicsEngineBullet.h"
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
	CGameWindowAndroid * window;
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
	
	sGameViewContext context;
	if (context.module.name.empty())
	{
		context.module.script = L"main.lua";
		context.module.textures = L"texture\\";
		context.module.models = L"models\\";
	}
	context.window = std::make_unique<CGameWindowAndroid>(state);
	context.soundPlayer = std::make_unique<CSoundPlayerOpenSLES>();
	context.textWriter = std::make_unique<CTextWriter>(context.window->GetRenderer());
	context.physicsEngine = std::make_unique<CPhysicsEngineBullet>();
	static_cast<CTextWriter*>(context.textWriter.get())->AddFontLocation("/sdcard/WargameEngine/");
	context.scriptHandlerFactory = []() {
		return std::make_unique<CScriptHandlerLua>();
	};
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
	context.workingDir = L"/sdcard/WargameEngine/";

	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.window = reinterpret_cast<CGameWindowAndroid*>(context.window.get());
	engine.app = state;

	if (state->savedState != NULL) {
		// We are starting with a previous saved state; restore from it.
		engine.state = *(struct saved_state*)state->savedState;
	}
	//context.window->LaunchMainLoop();
	// loop waiting for stuff to do.
	CGameView view(&context);
}
