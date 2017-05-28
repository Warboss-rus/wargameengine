#include <android/log.h>
#include <jni.h>
#include <dlfcn.h>
#include "..\..\WargameEngine\Application.h"
#include "../../WargameEngine/view/View.h"
#include "..\..\WargameEngine\impl\TextWriter.h"
#include "..\..\WargameEngine\impl\ScriptHandlerLua.h"
#include "..\..\WargameEngine\impl\NetSocket.h"
#include "..\..\WargameEngine\view\BuiltInImageReaders.h"
#include "..\..\WargameEngine\view\OBJModelFactory.h"
#include "..\..\WargameEngine\view\ColladaModelFactory.h"
#include "..\..\WargameEngine\view\WBMModelFactory.h"
#include "..\..\WargameEngine\impl\PhysicsEngineBullet.h"
#include "..\..\WargameEngine\impl\GvrGameWindow.h"
#include "..\..\WargameEngine\impl\SoundPlayerFMod.h"
#include "..\..\WargameEngine\impl\PathfindingMicroPather.h"

using namespace wargameEngine;
using namespace view;

#define JNI_METHOD(return_type, method_name) \
  JNIEXPORT return_type JNICALL              \
      Java_com_WargameEngineMobileGoogleVR_WargameEngineMobileGoogleVR_##method_name

namespace {
struct NativeAppJni
{
	Context context;
	Module module;
	std::unique_ptr<Application> app;
	CGvrGameWindow * window;
	NativeAppJni(gvr_context* gvr_context)
	{
		const std::string storage = getenv("EXTERNAL_STORAGE");
		module = Module(storage + "/WargameEngine/test.module");
		if (module.name.empty())
		{
			module.script = "main.lua";
			module.textures = "texture/";
			module.models = "models/";
			module.folder = storage + "/WargameEngine/";
		}
		window = new CGvrGameWindow(gvr_context);
		context.window.reset(window);
		context.soundPlayer = std::make_unique<CSoundPlayerFMod>();
		context.textWriter = std::make_unique<CTextWriter>();
		context.physicsEngine = std::make_unique<CPhysicsEngineBullet>();
		context.pathFinder = std::make_unique<CPathfindingMicroPather>();
		static_cast<CTextWriter*>(context.textWriter.get())->AddFontLocation(storage + "/WargameEngine/");
		context.scriptHandler = std::make_unique<CScriptHandlerLua>();
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
	}
};

inline jlong jptr(NativeAppJni *nativeRenderer) 
{
  return reinterpret_cast<intptr_t>(nativeRenderer);
}

inline NativeAppJni *native(jlong ptr) 
{
  return reinterpret_cast<NativeAppJni *>(ptr);
}
}  // anonymous namespace

extern "C" {

JNI_METHOD(jlong, nativeCreateRenderer)(JNIEnv *env, jclass clazz, jobject class_loader, jobject android_context, jlong native_gvr_api)
{
  return jptr(new NativeAppJni(reinterpret_cast<gvr_context *>(native_gvr_api)));
}

JNI_METHOD(void, nativeDestroyRenderer)(JNIEnv *env, jclass clazz, jlong nativeRenderer) 
{
  delete native(nativeRenderer);
}

JNI_METHOD(void, nativeInitializeGl)(JNIEnv *env, jobject obj, jlong nativeRenderer) 
{
	auto renderer = native(nativeRenderer);
	renderer->window->Init();
	renderer->app = std::make_unique<Application>(std::move(renderer->context));
	renderer->app->Run(std::move(renderer->module));
	renderer->app->GetView().GetViewport(0).GetCamera().AttachToVR(0);
}

JNI_METHOD(void, nativeDrawFrame)(JNIEnv *env, jobject obj, jlong nativeRenderer) 
{
  native(nativeRenderer)->window->Draw();
}

JNI_METHOD(void, nativeOnTouchEvent)(JNIEnv *env, jobject obj, jlong nativeRenderer, jint action, jfloat x, jfloat y)
{
  native(nativeRenderer)->window->GetAndroidInput().HandleMotionEvent(action, x, y);
}

JNI_METHOD(void, nativeOnScaleEvent)(JNIEnv *env, jobject obj, jlong nativeRenderer, jfloat delta)
{
	native(nativeRenderer)->window->GetAndroidInput().HandleZoom(delta);
}

JNI_METHOD(void, nativeOnPause)(JNIEnv *env, jobject obj, jlong nativeRenderer) 
{
  native(nativeRenderer)->window->Pause();
}

JNI_METHOD(void, nativeOnResume)(JNIEnv *env, jobject obj, jlong nativeRenderer) 
{
  native(nativeRenderer)->window->Resume();
}

}  // extern "C"