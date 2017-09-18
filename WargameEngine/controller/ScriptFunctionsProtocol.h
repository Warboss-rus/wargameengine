#pragma once

/*MODEL*/

//void CreateLandscape(float width, float height, string texture)
//Creates a new landscape with given size and texture
constexpr char CREATE_LANDSCAPE[] = "CreateLandscape";

//string GetGlobalProperty(string key)
//Returns a global property value by given key. If no such property is present returns an empty string.
constexpr char GET_GLOBAL_PROPERTY[] = "GetGlobalProperty";

//void SetGlobalProperty(string key, string value)
//Sets a global property value.
constexpr char SET_GLOBAL_PROPERTY[] = "SetGlobalProperty";

//void NewDecal (string decal, double x, double y, double rotation, double width, double height)
//Creates a new decal with given texture, place and size. Decals do not have bounding boxes, but they are part of game model, so they are synchronized across network.
constexpr char NEW_DECAL[] = "NewDecal";

//void NewStaticObject(string model, double x, double y, double rotation)
//Creates a new static object with given model and positions. Static objects have bounding boxes, but they cannot be selected, moved or deleted. They are part of game model.
constexpr char NEW_STATIC_OBJECT[] = "NewStaticObject";

//void AddLight()
//Adds a new light to the scene
constexpr char ADD_LIGHT[] = "AddLight";

//void RemoveLight(int index)
//Removes light by its index
constexpr char REMOVE_LIGHT[] = "RemoveLight";

//void SetLightPosition(int index, double x, double y, double z)
//Changes the position of light source
constexpr char SET_LIGHT_SOURCE_POSITION[] = "SetLightPosition";

//void SetLightPosition(int index, double r, double g, double a, double a)
//Changes the ambient color of light source
constexpr char SET_LIGHT_SOURCE_AMBIENT[] = "SetLightAmbient";

//void SetLightPosition(int index, double r, double g, double a, double a)
//Changes the diffuse color of light source
constexpr char SET_LIGHT_SOURCE_DIFFUSE[] = "SetLightDiffuse";

//void SetLightPosition(int index, double r, double g, double a, double a)
//Changes the specular color of light source
constexpr char SET_LIGHT_SOURCE_SPECULAR[] = "SetLightSpecular";

/*VIEW*/

//void CreateSkybox(double size, string textureFolder)
//Create a sky box with a given size and textures
constexpr char CREATE_SKYBOX[] = "CreateSkybox";

constexpr char ENABLE_RULER[] = "Ruler";

constexpr char RESIZE_WINDOW[] = "ResizeWindow";

constexpr char ENABLE_MSAA[] = "EnableMSAA";

constexpr char DISABLE_MSAA[] = "DisableMSAA";

constexpr char SET_ANISOTROPY_LEVEL[] = "SetAnisotropy";

constexpr char GET_MAX_ANISOTROPY[] = "GetMaxAnisotropy";

constexpr char ENABLE_GPU_SKINNING[] = "EnableGPUSkinning";

constexpr char DISABLE_GPU_SKINNING[] = "DisableGPUSkinning";

constexpr char CLEAR_RESOURCES[] = "ClearResources";

constexpr char SET_WINDOW_TITLE[] = "SetWindowTitle";

constexpr char PRELOAD[] = "Preload";

constexpr char PRELOAD_MODEL[] = "PreloadMode";

constexpr char SET_PARTICLE_SYSTEM_SHADERS[] = "SetParticleSystemShaders";

constexpr char SET_SHADERS[] = "SetShaders";

constexpr char SET_SKYBOX_SHADERS[] = "SetSkyboxShaders";

constexpr char UNIFORM_1I[] = "Uniform1i";

constexpr char UNIFORM_1F[] = "Uniform1f";

constexpr char UNIFORM_1FV[] = "Uniform1fv";

constexpr char UNIFORM_2FV[] = "Uniform2fv";

constexpr char UNIFORM_3FV[] = "Uniform3fv";

constexpr char UNIFORM_4FV[] = "Uniform4fv";

constexpr char UNIFORM_MATRIX4V[] = "UniformMatrix4fv";

constexpr char NEW_PARTICLE_EFFECT[] = "NewParticleEffect";

constexpr char NEW_PARTICLE_TRACER[] = "NewParticleTracer";

constexpr char PLAY_SOUND[] = "PlaySound";

constexpr char PLAY_SOUND_POSITION[] = "PlaySoundPosition";

constexpr char PLAY_SOUND_PLAYLIST[] = "PlaySoundPlaylist";

constexpr char GET_RENDERER_NAME[] = "GetRendererName";

constexpr char ENABLE_VR[] = "EnableVR";

constexpr char START_BENCHMARK[] = "StartBenchmark";

constexpr char STOP_BENCHMARK[] = "StopBenchmark";

/*CONTROLLER*/

constexpr char LOAD_MODULE[] = "LoadModule";

constexpr char GET_FILES_LIST[] = "GetFilesList";

constexpr char PRINT[] = "print";

constexpr char RUN_SCRIPT[] = "RunScript";

constexpr char SET_SELECTION_CALLBACK[] = "SetSelectionCallback";

constexpr char SET_UPDATE_CALLBACK[] = "SetUpdateCallback";

constexpr char SET_ON_STATE_RECEIVED_CALLBACK[] = "SetOnStateRecievedCallback";

constexpr char SET_ON_STRING_RECEIVED_CALLBACK[] = "SetOnStringRecievedCallback";

constexpr char SET_TIMED_CALLBACK[] = "SetTimedCallback";

constexpr char DELETE_TIMED_CALLBACK[] = "DeleteTimedCallback";

constexpr char SET_LMB_CALLBACK[] = "SetLMBCallback";

constexpr char SET_RMB_CALLBACK[] = "SetRMBCallback";

constexpr char BIND_KEY[] = "BindKey";

constexpr char UNDO[] = "Undo";

constexpr char REDO[] = "Redo";

constexpr char LINE_OF_SIGHT[] = "LoS";

constexpr char BEGIN_ACTION_COMPOUND[] = "BeginActionCompound";

constexpr char END_ACTION_COMPOUND[] = "EndActionCompound";

constexpr char NET_HOST[] = "NetHost";

constexpr char NET_CLIENT[] = "NetClient";

constexpr char NET_SEND_MESSAGE[] = "NetSendMessage";

constexpr char SAVE_GAME[] = "SaveGame";

constexpr char LOAD_GAME[] = "LoadGame";

constexpr char GET_ABSOLUTE_PATH[] = "GetAbsolutePath";

constexpr char SET_GAMEPAD_BUTTONS_CALLBACK[] = "SetGamepadButtonsCallback";

constexpr char SET_GAMEPAD_AXIS_CALLBACK[] = "SetGamepadAxisCallback";