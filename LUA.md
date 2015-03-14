WargameEngine supports all features from LUA 5.2 and adds some more functions to interact with an engine.

# LUA #

LUA is a lightweight interpreted scripting language. The full specifications can be found at [the project homepage](http://www.lua.org/manual/5.2/), but I recommend this tutorial: [Learn LUA in 15 minutes](http://tylerneylon.com/a/learn-lua/).
If an error occurs during script execution it will be appended to a [Log File](Log.md).

# Exported functions #

WargameEngine adds some extra functions and classes to LUA.
  * **[UI Class](LuaUIClass.md)** allows scripter to manipulate user interface.
  * **[Object Class](LuaObjectClass.md)** allows to create object, change their properties and many more.
  * **[CreateTable](LuaCreateTable.md)** creates a new table.
  * **[CreateSkybox](LuaCreateSkybox.md)** creates a new skybox.
  * **[CameraSetLimits](LuaCameraSetLimits.md)** sets camera properties such as maximum camera transition and minimum and maximum scale.
  * **[Ruler](LuaRuler.md)** enables a [WargameEngine#Ruler](WargameEngine#Ruler.md) mode.
  * **[Undo](LuaUndo.md)** undoes last [WargameEngine#Action](WargameEngine#Action.md).
  * **[Redo](LuaRedo.md)** redoes last [WargameEngine#Action](WargameEngine#Action.md).
  * **[MessageBox](LuaMessageBox.md)** shows a simple messagebox.
  * **[RunScript](LuaRunScript.md)** runs an LUA script. Equivalent to `dofile` LUA function.
  * **[GetGlobalProperty](LuaGetGlobalProperty.md)** gets a global [Property](Property.md).
  * **[SetGlobalProperty](LuaSetGlobalProperty.md)** sets a global [Property](Property.md).
  * **[IncludeLibrary](LuaIncludeLibrary.md)** includes one of the standard LUA library.
  * **[SetSelectionCallback](LuaSetSelectionCallback.md)** sets a callback that is triggered on a selection of an object.
  * **[SetUpdateCallback](LuaSetUpdateCallback.md)** sets a callback that triggers at every frame drawn. Caution! This can slow your module down seriously!
  * **[SetOnStateRecievedCallback](LuaSetOnStateRecievedCallback.md)** sets a callback that triggers then a new full game state is recieved via [Network](Network.md) or after loading a game.
  * **[SetOnStringRecievedCallback](LuaSetOnStringRecievedCallback.md)** sets a callback that triggers the a string is recieved via [Network](Network.md).
  * **[SetTimedCallback](LuaSetTimedCallback.md)** sets a callback that triggers over specified time.
  * **[DeleteTimedCallback](LuaDeleteTimedCallback.md)** delete an already specified timed callback.
  * **[LoS](LuaLoS.md)** calculates a visibility percent of one object to another.
  * **[ResizeWindow](LuaResizeWindow.md)** set the window's size.
  * **[BindKey](LuaBindKey.md)** sets a callback that triggers when a specific key combination is pressed.
  * **[SetShaders](LuaSetShaders.md)** creates a new [Shader Program](Shaders.md).
  * **[EnableLight](LuaLight#EnableLight.md)** enables lighting.
  * **[DisableLight](LuaLight#DisableLight.md)** disables lighting.
  * **[SetLightPosition](LuaLight#SetLightPosition.md)** sets a position of the specified light.
  * **[SetLightAmbient](LuaLight#SetLightAmbient.md)** sets an ambient color of the specified light.
  * **[SetLightDiffuse](LuaLight#SetLightDiffuse.md)** sets a diffuse color of the specified light.
  * **[SetLightSpecular](LuaLight#SetLightSpecular.md)** sets a specular color of the specified light.
  * **[EnableVertexLightning](LuaVertexLighting#EnableVertexLighting.md)** enables per-vertex lighting (doesn't require any shaders, but not very nice-looking).
  * **[DisableVertexLightning](LuaVertexLighting#DisableVertexLighting.md)** disables per-vertex lighting.
  * **[EnableShadowMap](LuaEnableShadowMap.md)** enables a shadowmap. Dramaticly descreases FPS.
  * **[DisableShadowMap](LuaDisableShadowMap.md)** disables a shadowmap.
  * **[EnableMSAA](LuaEnableMSAA.md)** enables MSAA anti-aliasing. Slightly descreases FPS.
  * **[DisableMSAA](LuaDisableMSAA.md)** disables MSAA anti-aliasing.
  * **[SetAnisotropy](LuaSetAnisotropy.md)** sets an anisotropy level. The higher level of anisotropy results in better texture filtering but descreases perfomance.
  * **[GetMaxAnisotropy](LuaGetMaxAnisotropy.md)** returns a maximum supported level of anisotropy.
  * **[BeginActionCompound](LuaBeginActionCompound.md)** begins an [ActionCompound](ActionCompound.md).
  * **[EndActionCompound](LuaEndActionCompound.md)** ends an [ActionCompound](ActionCompound.md).
  * **[NetHost](LuaNetHost.md)** creates a [Network](Network.md) session and awaits a client to join in. Warning! The host will hang until a client is accepted.
  * **[NetClient](LuaNetClient.md)** join an exciting [Network](Network.md) session. Warning! The client will hang until the host accept it.
  * **[NetSendMessage](LuaNetSendMessage.md)** sends a string to other computer in a [Network](Network.md) session.
  * **[SaveGame](LuaSaveGame.md)** saves a full game state to a file.
  * **[LoadGame](LuaLoadGame.md)** loads a full game state from a file.
  * **[ClearResources](LuaClearResources.md)** removes all cached models and textures.
  * **[SetWindowTitle](LuaSetWindowTitle.md)** sets a window title.
  * **[Preload](LuaPreload.md)** waits until all resources are loaded and shows an image meanwhile.
  * **[PreloadModel](LuaPreloadModel.md)** preloads specified model and its textures, so it won't be necessary during gameplay.
  * **[LoadModuleFile](LuaLoadModuleFile.md)** closes this module and loads specified one.
  * **[GetFilesList](LuaGetFilesList.md)** returns a list of files in a given folder by specified file mask.
  * **[Uniform1i](LUAUniform#Uniform1i.md)** sets an integer uniform variable in a [Shader](Shaders.md).
  * **[Uniform1f](LUAUniform#Uniform1f.md)** sets a float uniform variable in a [Shader](Shaders.md).
  * **[Uniform1fv](LUAUniform#Uniform1fv.md)** sets a float uniform array in a [Shader](Shaders.md).
  * **[Uniform2fv](LUAUniform#Uniform2fv.md)** sets a float2 uniform array in a [Shader](Shaders.md).
  * **[Uniform3fv](LUAUniform#Uniform3fv.md)** sets a float3 uniform array in a [Shader](Shaders.md).
  * **[Uniform4fv](LUAUniform#Uniform4fv.md)** sets a float4 uniform array in a [Shader](Shaders.md).
  * **[UniformMatrix4fv](LUAUniform#UniformMatrix4fv.md)** sets a float matrix4 uniform in a [Shader](Shaders.md).