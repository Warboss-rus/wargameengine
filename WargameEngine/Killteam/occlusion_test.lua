function VR()
	EnableVR(true, true)
end

AddLight()
SetLightPosition(1, 0, 0, 30)
--EnableShadowMap(1024, 53)
EnableMSAA()
SetAnisotropy(GetMaxAnisotropy())
CreateSkybox(50, "skybox")--Creates a skybox (size in OpenGLUnits, path to texture folder (names are fixed))
if(GetRendererName() == "DirectX11") then
	SetShaders("directX11/gpu_skinning.hlsl", "directX11/gpu_skinning.hlsl")
	SetParticleSystemShaders("directX11/particle.hlsl", "directX11/particle.hlsl")
elseif(GetRendererName() == "OpenGLES") then
	SetShaders("GLES/gpu_skinning.vsh", "GLES/gpu_skinning.fsh")
	SetParticleSystemShaders("GLES/particle.vsh", "GLES/particle.fsh")
	SetSkyboxShaders("GLES/skybox.vsh", "GLES/skybox.fsh")
	UI:SetScale(2)
	Viewport:EnableTouchMode()
elseif(GetRendererName() == "Vulkan") then
	--do nothing
else
	--SetShaders("openGL/gpu_skinning.vsh", "openGL/gpu_skinning.fsh")
	--SetParticleSystemShaders("openGL/particle.vsh", "openGL/particle.fsh")
	SetSkyboxShaders("openGL/skybox.vsh", "openGL/skybox.fsh")
end
EnableGPUSkinning()
CreateLandscape(60, 60, "sand.dds")--Creates a table (width, height, texture)
local random = true
local objectList = {
	"Angel_of_Death.wbm",
	"assault_marine.wbm",
	"Chaos_terminator_LC.wbm",
	"CSM.wbm",
	"CSM_HB.wbm",
	"CSM_melta.wbm",
	"raptor.wbm",
	"SM_HB.wbm",
	"SM_Melta.wbm",
	"Terminator_SB+PF.wbm",
}
local count = 50
for i = 1, count do
	local x = -count / 2 + i - 0.5
	for j = 1, count do
		local y = -count / 2 + j - 0.5
		if(random) then
			local object = Object:New(objectList[math.random(#objectList)], x, y, 0)
		else
			local object = Object:New(objectList[1], x, y, 0)
		end
	end
end