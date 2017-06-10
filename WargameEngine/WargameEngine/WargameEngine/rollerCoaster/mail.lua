AddLight()
SetLightPosition(1, 0, 0, 30)
CreateSkybox(10, "skybox")
if(GetRendererName() == "DirectX11") then
elseif(GetRendererName() == "OpenGLES") then
	SetSkyboxShaders("GLES/skybox_multiview.vsh", "GLES/skybox.fsh")
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
cart = Object:New("Cart_dims.obj", 0, -24, 90)

function Start(repeatAll)
	Viewport:CameraFirstPerson()
	PlaySoundPlaylist("cart_sounds", {
		"sound/costerupstart.aiff", 
		"sound/coasterupmiddle.aiff", 
		"sound/coasterupend.aiff", 
		"sound/rollercoastermiddle.aiff",
		"sound/rollercoasterfadeout.aiff",
		}, 1.0, false, true)
		
	local positions = {
		0, -24, 1,--10
		0, -23, 1.3,--11
		0, -10.5, 15,--25
		0, -10, 16,--26
		0, -9.5, 15,--27
		0, -5, 1.3,--32
		0, -4.5, 1,--33
		0, -4, 1.3--34
	}
	local rotations = {
		0, 0, 90,--10
		0, 45, 90,--11
		0, 45, 90,--25
		0, 0, 90,--26
		0, -60, 90,--27
		0, -60, 90,--32
		0, 0, 90,--33
		0, 60, 90,--34
	}
	local timestamps = {0, 1, 15, 16, 17, 22, 23, 24 }
	
	cart:MovePath(positions, rotations, timestamps, "spline3", repeatAll)
end

function StartLoop()
	Start(true)
end

function Benchmark()
	Start(false)
end

UI:NewButton("ButtonLoop", 10, 10, 50, 200, "Start loop", StartLoop)
UI:NewButton("ButtonBenchmark", 230, 10, 50, 200, "Benchmark", Benchmark)