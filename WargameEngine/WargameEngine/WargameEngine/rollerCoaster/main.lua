AddLight()
SetLightPosition(1, 0, 0, 30)
CreateSkybox(10, "skybox")
Viewport:EnableFrustumCulling(false)
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

repeatAll = false

function SoundMiddle()
	PlaySound("cart_sounds", "sound/coasterupmiddle.aiff", 1.0)
end

function SoundTop()
	PlaySound("cart_sounds", "sound/coasterupend.aiff", 1.0)
end

function SoundRoll()
	PlaySoundPlaylist("cart_sounds", {"sound/rollercoastermiddle.aiff"}, 1.0, false, true)
end

function End()
	if(repeatAll) then
		Start()
	else
		PlaySound("cart_sounds", "", 0.0)
		StopBenchmark("benchmark.csv")
	end
end

function Start()
	PlaySound("cart_sounds", "sound/costerupstart.aiff", 1.0)
	SetTimedCallback(SoundMiddle, 6000, false)
	SetTimedCallback(SoundTop, 13000, false)
	SetTimedCallback(SoundRoll, 17000, false)
	SetTimedCallback(End, 74000, false)
		
	local positions = {
		0, -24, 0.5,--0
		0, -22.5, 0.8,--1
		0, -10.5, 15,--15
		0, -8, 15.5,--16
		0, -7.5, 15,--17
		0, -3, 2.3,--22
		0, -1.2, 2,--22.75
		0, 0.5, 2.3,--23.5
		0, 3.3, 9.5,--28
		0, 5.1, 10.5,--28.75
		0, 6.5, 9.5,--29.5
		0, 8.5, 4,--31.5
		0, 10, 3,--32
		0, 12, 3.5,--32.5
		0, 14.5, 8.5,--35
		0, 16, 9,--35.75
		0, 17.5, 8.5,--36.5
		0, 18.5, 4.5,--38
		0, 19, 4, --38.5
		0, 21, 4, --39.5
		2, 23, 4, --40.5
		4.5, 21, 4, --41.5
		4.5, 20, 4, --42
		4.5, 19, 4.5,--42.5
		4.5, 17, 8,--43.5
		4.5, 15.5, 9,--44
		4.5, 14, 8,--44.5
		4.5, 11.5, 2,--46
		4.5, 11, 1.3,--46.5
		--first loop
		4.5, 8, 1.3,--47.5 0 degrees
		4.25, 7, 2.5,--48 45 degrees
		4, 6, 3.7,--48.5 90 degrees
		4, 7, 4.9,--49 135 degrees
		4, 8, 6,--49.5 180 degrees
		4.25, 9, 4.9,--50 225 degrees
		4.5, 10, 3.7,--50.5 270 degrees
		4.75, 9, 2.5, --51 315 degrees
		5.25, 8, 1.3, --51.5 360 degrees
		5.25, 8, 1.3,--51.5 additional frame to fix rotation interpolation
		--second loop
		5.25, 2, 1.3,--52.5
		5, 1, 2.5,--53 45 degrees
		4.75, 0, 3.7,--53.5 90 degrees
		4.75, 1, 4.9,--54 135 degrees
		4.75, 2, 6,--54.5 180 degrees
		5, 3, 4.9,--55 225 degrees
		5.25, 4, 3.7,--55.5 270 degrees
		5.5, 3, 2.5, --56 315 degrees
		6, 2, 1.3, --56.5 360 degrees
		6, 2, 1.3,--56.5 additional frame to fix rotation interpolation
		6, 0, 1.3,--57.5
		4, -2.3, 1.3,--58.5
		0, -2.3, 0.5,--60
		-1.2, -2.3, 0.5,--60.5
		-3.3, -3.5, 0.5,--61.5
		-3.3, -21, 0.5,--70
		-3.3, -22, 0,--70.5
		-3.3, -24, 0,--72
		-1.5, -25, 0,--73
		0, -24, 0.5,--74
	}
	local rotations = {
		0, 0, 90,--0
		0, 45, 90,--1
		0, 45, 90,--15
		0, 0, 90,--16
		0, -65, 90,--17
		0, -65, 90,--22
		0, 0, 90,--22.75
		0, 60, 90,--23.5
		0, 60, 90,--28
		0, 0, 90,--28.75
		0, -60, 90,--29.5
		0, -60, 90,--31.5
		0, 0, 90,--32
		0, 55, 90,--32.5
		0, 55, 90,--35
		0, 0, 90,--35.75
		0, -45, 90,--36.5
		0, -45, 90,--38
		0, 0, 90,--38.5
		0, 0, 90,--39.5
		0, 0, 0,--40.5
		0, 0, -90,--41.5
		0, 0, -90,--42
		0, -45, -90,--42.5
		0, -45, -90,--43.5
		0, 0, -90,--44
		0, 60, -90, --44.5
		0, 60, -90,--46
		0, 0, -90,--46.5
		--first loop
		0, 0, -90,--47.5
		0, -45, -90,--48
		0, -90, -90,--48.5
		0, -135, -90,--49
		0, -180, -90,--49.5
		0, -225, -90,--50
		0, -270, -90,--50.5
		0, -315, -90,--51
		0, -360, -90,--51.5
		0, 0, -90,--51.5 additional frame to fix rotation interpolation
		--first loop
		0, 0, -90,--52.5
		0, -45, -90,--53
		0, -90, -90,--53.5
		0, -135, -90,--54
		0, -180, -90,--54.5
		0, -225, -90,--55
		0, -270, -90,--55.5
		0, -315, -90,--56
		0, -360, -90,--56.5
		0, 0, -90,--56.5 additional frame to fix rotation interpolation
		0, 0, -90,--57.5
		0, 0, -180,--58.5
		0, 0, -180,--60
		0, 0, -180,--60.5
		0, 0, -90,--61.5
		0, 0, -90,--70
		0, 0, -90,--70.5
		0, 0, -90,--72
		0, 0, 0,--73
		0, 0, 90,--74
	}
	local timestamps = {0, 1, 15, 16, 17, 22, 22.75, 23.5, 28, 28.75, 29.5, 31, 32, 33, 35, 35.75, 36.5, 38, 38.5, 39.5, 40.5, 41.5, 42, 42.5, 43.5, 44, 44.5, 46, 46.5, 
		47.5, 48, 48.5, 49, 49.5, 50, 50.5, 51, 51.5, 51.5, 52.5, 53, 53.5, 54, 54.5, 55, 55.5, 56, 56.5, 56.5, 57.5, 58.5, 60, 60.5, 61.5, 70, 70.5, 72, 73, 74}
	
	cart:MovePath(positions, rotations, timestamps, "spline3")
	StartBenchmark()
end

function StartLoop()
	repeatAll = true
	Start()
end

function Benchmark()
	repeatAll = false
	Start()
end

firstPerson = false
function Camera()
	firstPerson = not firstPerson
	if(firstPerson) then
		Viewport:CameraFirstPerson()
		Viewport:CameraAttachToObject(cart, 0, 0, 2)
		Viewport:SetCameraTarget(1, 0, 2)
		Viewport:CameraResetInput()
	else
		Viewport:CameraAttachToObject(nil, 0, 0, 0)
		Viewport:CameraStrategy(30, 60, 5, 0.5)
		Viewport:SetCameraPosition(-10, 0, 10)
		Viewport:CameraAttachKeyboardMouse()
	end
end

UI:NewButton("ButtonLoop", 10, 10, 50, 200, "Start loop", StartLoop)
UI:NewButton("ButtonBenchmark", 230, 10, 50, 200, "Benchmark", Benchmark)
UI:NewButton("ButtonCamera", 450, 10, 50, 200, "Camera mode", Camera)