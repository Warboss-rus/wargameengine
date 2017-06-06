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
CreateLandscape(60, 30, "Green_Grass.jpg")
Object:New("Roller Coaster.fbx", 0, 0, 0)