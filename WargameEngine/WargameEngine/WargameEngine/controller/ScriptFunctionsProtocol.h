#pragma once

/*MODEL*/

//void CreateTable(float width, float height, string texture)
//Creates a new landscape with given size and texture
#define CREATE_TABLE "CreateTable"

//string GetGlobalProperty(string key)
//Returns a global property value by given key. If no such property is present returns an empty string.
#define GET_GLOBAL_PROPERTY "GetGlobalProperty"

//void SetGlobalProperty(string key, string value)
//Sets a global property value.
#define SET_GLOBAL_PROPERTY "SetGlobalProperty"

//void NewDeval (string decal, double x, double y, double rotation, double width, double height)
//Creates a new decal with given texture, place and size. Decals do not have bounding boxes, but they are part of game model, so they are synchronized across network.
#define NEW_DECAL "NewDecal"

//void NewStaticObject(string model, double x, double y, double rotation)
//Creates a new static object with given model and positions. Static objects have bounding boxes, but they cannot be selected, moved or deleted. They are part of game model.
#define NEW_STATIC_OBJECT "NewStaticObject"

/*VIEW*/

//void CreateSkybox(double size, string textureFolder)
//Create a sky box with a given size and textures
#define CREATE_SKYBOX "CreateSkybox"

//void CameraStrategy(double maxTransX, double maxTransY, double maxScale, double minScale)
//Creates a third person camera typically seen in strategy games. It can be zoomed with a mouse wheel and moved around via arrow keys. Holding Alt + moving the mouse
//will allow you to turn the camera around. The function arguments determine the translation limits for the camera.
#define CAMERA_STRATEGY "CameraStrategy"

//void CameraFirstPerson()
//Creates a first person camera. It can be moved around with arrow keys and zoomed in and out with mouse wheel. Mouse movement is rotating the camera
//Note that the mouse is locked so you won't be able to click buttons, select objects, etc.
#define CAMERA_FIRST_PERSON "CameraFirstPerson"

#define ENABLE_RULER "Ruler"

#define MESSAGE_BOX "MessageBox"

#define RESIZE_WINDOW "ResizeWindow"

#define ENABLE_LIGHT_SOURCE "EnableLight"

#define DISABLE_LIGHT_SOURCE "DisableLight"

#define SET_LIGHT_SOURCE_POSITION "SetLightPosition"

#define SET_LIGHT_SOURCE_AMBIENT "SetLightAmbient"

#define SET_LIGHT_SOURCE_DIFFUSE "SetLightDiffuse"

#define SET_LIGHT_SOURCE_SPECULAR "SetLightSpecular"

#define ENABLE_VERTEX_LIGHTING "EnableVertexLighting"

#define DISABLE_VERTEX_LIGHTING "DisableVertexLighting"

#define ENABLE_SHADOW_MAP "EnableShadowMap"

#define DISABLE_SHADOW_MAP "DisableShadowMap"

#define ENABLE_MSAA "EnableMSAA"

#define DISABLE_MSAA "DisableMSAA"

#define SET_ANISOTROPY_LEVEL "SetAnisotropy"

#define GET_MAX_ANISOTROPY "GetMaxAnisotropy"

#define ENABLE_GPU_SKINNING "EnableGPUSkinning"

#define DISABLE_GPU_SKINNING "DisableGPUSkinning"

#define CLEAR_RESOURCES "ClearResources"

#define SET_WINDOW_TITLE "SetWindowTitle"

#define PRELOAD "Preload"

#define PRELOAD_MODEL "PreloadModel"

#define SET_SHADERS "SetShaders"

#define UNIFORM_1I "Uniform1i"

#define UNIFORM_1F "Uniform1f"

#define UNIFORM_1FV "Uniform1fv"

#define UNIFORM_2FV "Uniform2fv"

#define UNIFORM_3FV "Uniform3fv"

#define UNIFORM_4FV "Uniform4fv"

#define UNIFORM_MATRIX4V "UniformMatrix4fv"

#define NEW_PARTICLE_EFFECT "NewParticleEffect"

#define NEW_PARTICLE_TRACER "NewParticleTracer"

#define PLAY_SOUND "PlaySound"

#define PLAY_SOUND_POSITION "PlaySoundPosition"

#define PLAY_SOUND_PLAYLIST "PlaySoundPlaylist"

#define GET_RENDERER_NAME "GetRendererName"

/*CONTROLLER*/

#define LOAD_MODULE "LoadModule"

#define GET_FILES_LIST "GetFilesList"

#define PRINT "print"

#define RUN_SCRIPT "RunScript"

#define SET_SELECTION_CALLBACK "SetSelectionCallback"

#define SET_UPDATE_CALLBACK "SetUpdateCallback"

#define SET_ON_STATE_RECEIVED_CALLBACK "SetOnStateRecievedCallback"

#define SET_ON_STRING_RECEIVED_CALLBACK "SetOnStringRecievedCallback"

#define SET_TIMED_CALLBACK "SetTimedCallback"

#define DELETE_TIMED_CALLBACK "DeleteTimedCallback"

#define SET_LMB_CALLBACK "SetLMBCallback"

#define SET_RMB_CALLBACK "SetRMBCallback"

#define BIND_KEY "BindKey"

#define UNDO "Undo"

#define REDO "Redo"

#define LINE_OF_SIGHT "LoS"

#define BEGIN_ACTION_COMPOUND "BeginActionCompound"

#define END_ACTION_COMPOUND "EndActionCompound"

#define NET_HOST "NetHost"

#define NET_CLIENT "NetClient"

#define NET_SEND_MESSAGE "NetSendMessage"

#define SAVE_GAME "SaveGame"

#define LOAD_GAME "LoadGame"

#define GET_ABSOLUTE_PATH "GetAbsolutePath"

#define SET_GAMEPAD_BUTTONS_CALLBACK "SetGamepadButtonsCallback"

#define SET_GAMEPAD_AXIS_CALLBACK "SetGamepadAxisCallback"