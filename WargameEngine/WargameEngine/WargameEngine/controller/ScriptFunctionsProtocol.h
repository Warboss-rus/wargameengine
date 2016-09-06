#pragma once

/*MODEL*/

//void CreateTable(float width, float height, string texture)
//Creates a new landscape with given size and texture
#define CREATE_TABLE L"CreateTable"

//string GetGlobalProperty(string key)
//Returns a global property value by given key. If no such property is present returns an empty string.
#define GET_GLOBAL_PROPERTY L"GetGlobalProperty"

//void SetGlobalProperty(string key, string value)
//Sets a global property value.
#define SET_GLOBAL_PROPERTY L"SetGlobalProperty"

//void NewDeval (string decal, double x, double y, double rotation, double width, double height)
//Creates a new decal with given texture, place and size. Decals do not have bounding boxes, but they are part of game model, so they are synchronized across network.
#define NEW_DECAL L"NewDecal"

//void NewStaticObject(string model, double x, double y, double rotation)
//Creates a new static object with given model and positions. Static objects have bounding boxes, but they cannot be selected, moved or deleted. They are part of game model.
#define NEW_STATIC_OBJECT L"NewStaticObject"

/*VIEW*/

//void CreateSkybox(double size, string textureFolder)
//Create a sky box with a given size and textures
#define CREATE_SKYBOX L"CreateSkybox"

#define ENABLE_RULER L"Ruler"

#define RESIZE_WINDOW L"ResizeWindow"

#define ENABLE_LIGHT_SOURCE L"EnableLight"

#define DISABLE_LIGHT_SOURCE L"DisableLight"

#define SET_LIGHT_SOURCE_POSITION L"SetLightPosition"

#define SET_LIGHT_SOURCE_AMBIENT L"SetLightAmbient"

#define SET_LIGHT_SOURCE_DIFFUSE L"SetLightDiffuse"

#define SET_LIGHT_SOURCE_SPECULAR L"SetLightSpecular"

#define ENABLE_VERTEX_LIGHTING L"EnableVertexLighting"

#define DISABLE_VERTEX_LIGHTING L"DisableVertexLighting"

#define ENABLE_SHADOW_MAP L"EnableShadowMap"

#define DISABLE_SHADOW_MAP L"DisableShadowMap"

#define ENABLE_MSAA L"EnableMSAA"

#define DISABLE_MSAA L"DisableMSAA"

#define SET_ANISOTROPY_LEVEL L"SetAnisotropy"

#define GET_MAX_ANISOTROPY L"GetMaxAnisotropy"

#define ENABLE_GPU_SKINNING L"EnableGPUSkinning"

#define DISABLE_GPU_SKINNING L"DisableGPUSkinning"

#define CLEAR_RESOURCES L"ClearResources"

#define SET_WINDOW_TITLE L"SetWindowTitle"

#define PRELOAD L"Preload"

#define PRELOAD_MODEL L"PreloadModel"

#define SET_SHADERS L"SetShaders"

#define UNIFORM_1I L"Uniform1i"

#define UNIFORM_1F L"Uniform1f"

#define UNIFORM_1FV L"Uniform1fv"

#define UNIFORM_2FV L"Uniform2fv"

#define UNIFORM_3FV L"Uniform3fv"

#define UNIFORM_4FV L"Uniform4fv"

#define UNIFORM_MATRIX4V L"UniformMatrix4fv"

#define NEW_PARTICLE_EFFECT L"NewParticleEffect"

#define NEW_PARTICLE_TRACER L"NewParticleTracer"

#define PLAY_SOUND L"PlaySound"

#define PLAY_SOUND_POSITION L"PlaySoundPosition"

#define PLAY_SOUND_PLAYLIST L"PlaySoundPlaylist"

#define GET_RENDERER_NAME L"GetRendererName"

/*CONTROLLER*/

#define LOAD_MODULE L"LoadModule"

#define GET_FILES_LIST L"GetFilesList"

#define PRINT L"print"

#define RUN_SCRIPT L"RunScript"

#define SET_SELECTION_CALLBACK L"SetSelectionCallback"

#define SET_UPDATE_CALLBACK L"SetUpdateCallback"

#define SET_ON_STATE_RECEIVED_CALLBACK L"SetOnStateRecievedCallback"

#define SET_ON_STRING_RECEIVED_CALLBACK L"SetOnStringRecievedCallback"

#define SET_TIMED_CALLBACK L"SetTimedCallback"

#define DELETE_TIMED_CALLBACK L"DeleteTimedCallback"

#define SET_LMB_CALLBACK L"SetLMBCallback"

#define SET_RMB_CALLBACK L"SetRMBCallback"

#define BIND_KEY L"BindKey"

#define UNDO L"Undo"

#define REDO L"Redo"

#define LINE_OF_SIGHT L"LoS"

#define BEGIN_ACTION_COMPOUND L"BeginActionCompound"

#define END_ACTION_COMPOUND L"EndActionCompound"

#define NET_HOST L"NetHost"

#define NET_CLIENT L"NetClient"

#define NET_SEND_MESSAGE L"NetSendMessage"

#define SAVE_GAME L"SaveGame"

#define LOAD_GAME L"LoadGame"

#define GET_ABSOLUTE_PATH L"GetAbsolutePath"

#define SET_GAMEPAD_BUTTONS_CALLBACK L"SetGamepadButtonsCallback"

#define SET_GAMEPAD_AXIS_CALLBACK L"SetGamepadAxisCallback"