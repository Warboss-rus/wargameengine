AddLight()
SetLightPosition(1, 0, 0, 30)
CreateSkybox(10, "skybox")
if(GetRendererName() == "DirectX11") then
elseif(GetRendererName() == "OpenGLES") then
	SetSkyboxShaders("GLES/skybox.vsh", "GLES/skybox.fsh")
	UI:SetScale(2)
	Viewport:EnableTouchMode()
elseif(GetRendererName() == "Vulkan") then
	--do nothing
else
	SetSkyboxShaders("openGL/skybox.vsh", "openGL/skybox.fsh")
end
EnableGPUSkinning()
CreateLandscape(30, 60, "Green_Grass.bmp")
Object:New("beast124.obj", 0, 0, 0)
--Object:New("Cart_dims.dae", 0, 0, 0)

PlaySoundPlaylist("cart_sounds", {
	"sound/costerupstart.aiff", 
	"sound/coasterupmiddle.aiff", 
	"sound/coasterupend.aiff", 
	"sound/rollercoastermiddle.aiff",
	"sound/rollercoasterfadeout.aiff",
	}, 1.0, false, true)