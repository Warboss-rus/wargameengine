#include "pch.h"
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

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "WargameEngineMobile.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "WargameEngineMobile.NativeActivity", __VA_ARGS__))

/**
* Our saved state data.
*/
struct saved_state {
	float angle;
	int32_t x;
	int32_t y;
};

/**
* Shared state for our app.
*/
struct engine {
	struct android_app* app;

	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;

	struct saved_state state;
	sGameViewContext viewContext;
	std::unique_ptr<CGameView> view;
	CGameWindowAndroid * window;
};

/**
* Process the next input event.
*/
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
	struct engine* engine = (struct engine*)app->userData;
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		engine->state.x = AMotionEvent_getX(event, 0);
		engine->state.y = AMotionEvent_getY(event, 0);
		return 1;
	}
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
			engine->window->DrawFrame();;
		}
		break;
	case APP_CMD_TERM_WINDOW:
		// The window is being hidden or closed, clean it up.
		engine->window->Shutdown();
		break;
	case APP_CMD_GAINED_FOCUS:
		// When our app gains focus, we start monitoring the accelerometer.
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_enableSensor(engine->sensorEventQueue,
				engine->accelerometerSensor);
			// We'd like to get 60 events per second (in us).
			ASensorEventQueue_setEventRate(engine->sensorEventQueue,
				engine->accelerometerSensor, (1000L / 60) * 1000);
		}
		break;
	case APP_CMD_LOST_FOCUS:
		// When our app loses focus, we stop monitoring the accelerometer.
		// This is to avoid consuming battery while not being used.
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_disableSensor(engine->sensorEventQueue,
				engine->accelerometerSensor);
		}
		// Also stop animating.
		engine->window->SetActive(false);
		engine->window->DrawFrame();
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
	
	if (engine.viewContext.module.name.empty())
	{
		engine.viewContext.module.script = L"main.lua";
		engine.viewContext.module.textures = L"texture\\";
		engine.viewContext.module.models = L"models\\";
	}
	engine.viewContext.window = std::make_unique<CGameWindowAndroid>(state);
	engine.window = reinterpret_cast<CGameWindowAndroid*>(engine.viewContext.window.get());
	engine.viewContext.soundPlayer = std::make_unique<CSoundPlayerOpenSLES>();
	engine.viewContext.textWriter = std::make_unique<CTextWriter>(engine.viewContext.window->GetRenderer());
	engine.viewContext.scriptHandlerFactory = []() {
		return std::make_unique<CScriptHandlerLua>();
	};
	engine.viewContext.socketFactory = []() {
		return std::make_unique<CNetSocket>();
	};
	engine.viewContext.imageReaders.push_back(std::make_unique<CBmpImageReader>());
	engine.viewContext.imageReaders.push_back(std::make_unique<CTgaImageReader>());
	engine.viewContext.imageReaders.push_back(std::make_unique<CDdsImageReader>());
	engine.viewContext.imageReaders.push_back(std::make_unique<CStbImageReader>());
	engine.viewContext.modelReaders.push_back(std::make_unique<CObjModelFactory>());
	engine.viewContext.modelReaders.push_back(std::make_unique<CColladaModelFactory>());
	engine.viewContext.modelReaders.push_back(std::make_unique<CWBMModelFactory>());

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.app = state;

	engine.sensorManager = ASensorManager_getInstance();
	engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
		ASENSOR_TYPE_ACCELEROMETER);
	engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
		state->looper, LOOPER_ID_USER, NULL, NULL);

	if (state->savedState != NULL) {
		// We are starting with a previous saved state; restore from it.
		engine.state = *(struct saved_state*)state->savedState;
	}

	// loop waiting for stuff to do.

	engine.view = std::make_unique<CGameView>(&engine.viewContext);
}
