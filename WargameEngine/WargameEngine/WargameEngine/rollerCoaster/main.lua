AddLight()
SetLightPosition(1, 0, 0, 30)
CreateSkybox(10, "skybox")
AddLight()
SetLightPosition(1, -20, -10, 50)
SetLightDiffuse(1, 1, 1, 1, 1)
SetLightAmbient(1, 0.5, 0.5, 0.5, 1)
SetLightSpecular(1, 1, 1, 1, 1)
Viewport:EnableFrustumCulling(false)
local shadowSize = 2048
local anisotropy = 16
if(GetRendererName() == "DirectX11") then
elseif(GetRendererName() == "OpenGLES") then
	SetSkyboxShaders("GLES/skybox_multiview.vsh", "GLES/skybox.fsh")
	SetShaders("GLES/shadow_multiview.vsh", "GLES/shadow.fsh")
	UI:SetScale(2)
	Viewport:EnableTouchMode()
	anisotropy = 4
	shadowSize = 1024
elseif(GetRendererName() == "Vulkan") then
	--do nothing
else
	SetSkyboxShaders("openGL/skybox.vsh", "openGL/skybox.fsh")
	SetShaders("openGL/shadow.vsh", "openGL/shadow.fsh")
end
SetAnisotropy(anisotropy)
Viewport:SetShadowMapViewport(Viewport:CreateShadowMapViewport(shadowSize, 65, -30, 30, 50))
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
	
	local path = {
		--position			--rotation		--duration
		{ {0, -24, 0.5}, 	{0, 0, 90}, 	0 },--0
		{ {0, -22.5, 0.8}, 	{0, 45, 90}, 	1},--1
		{ {0, -10.5, 15}, 	{0, 45, 90}, 	14},--15
		{ {0, -8, 15.5}, 	{0, 0, 90}, 	1},--16
		{ {0, -7.5, 15}, 	{0, -65, 90}, 	1},--17
		{ {0, -3, 2.3}, 	{0, -65, 90}, 	5},--22
		{ {0, -1.2, 2}, 	{0, 0, 90}, 	0.75},--22.75
		{ {0, 0.5, 2.3}, 	{0, 60, 90}, 	0.75},--23.5
		{ {0, 3.3, 9.5}, 	{0, 60, 90}, 	4.5},--28
		{ {0, 5.1, 10.5}, 	{0, 0, 90}, 	0.75},--28.75
		{ {0, 6.5, 9.5}, 	{0, -60, 90}, 	0.75},--29.5
		{ {0, 8.5, 4}, 		{0, -60, 90}, 	2},--31.5
		{ {0, 10, 3}, 		{0, 0, 90}, 	0.5},--32
		{ {0, 12, 3.5}, 	{0, 55, 90}, 	0.5},--32.5
		{ {0, 14.5, 8.5}, 	{0, 55, 90}, 	2.5},--35
		{ {0, 16, 9}, 		{0, 0, 90}, 	0.75},--35.75
		{ {0, 17.5, 8.5}, 	{0, -45, 90}, 	0.75},--36.5
		{ {0, 18.5, 4.5}, 	{0, -45, 90}, 	1.5},--38
		{ {0, 19, 4}, 		{0, 0, 90}, 	0.5},--38.5
		{ {0, 21, 4}, 		{0, 0, 90}, 	1},--39.5
		{ {2, 23, 4}, 		{0, 0, 0}, 		1},--40.5
		{ {4.5, 21, 4}, 	{0, 0, -90}, 	1},--41.5
		{ {4.5, 20, 4}, 	{0, 0, -90}, 	0.5},--42
		{ {4.5, 19, 4.5}, 	{0, -45, -90}, 	0.5},--42.5
		{ {4.5, 17, 8}, 	{0, -45, -90}, 	1},--43.5
		{ {4.5, 15.5, 9}, 	{0, 0, -90}, 	0.5},--44
		{ {4.5, 14, 8}, 	{0, 60, -90}, 	0.5},--44.5
		{ {4.5, 11.5, 2}, 	{0, 60, -90}, 	1.5},--46
		{ {4.5, 11, 1.3}, 	{0, 0, -90}, 	0.5},--46.5
		--first loop
		{ {4.5, 8, 1.3}, 	{0, 0, -90}, 	1},--47.5
		{ {4.25, 7, 2.5}, 	{0, -45, -90}, 	0.5},--48
		{ {4, 6, 3.7}, 		{0, -90, -90}, 	0.5},--48.5
		{ {4, 7, 4.9}, 		{0, -135, -90}, 0.5},--49
		{ {4, 8, 6}, 		{0, -180, -90}, 0.5},--49.5
		{ {4.25, 9, 4.9}, 	{0, -225, -90}, 0.5},--50
		{ {4.5, 10, 3.7}, 	{0, -270, -90}, 0.5},--50.5
		{ {4.75, 9, 2.5}, 	{0, -315, -90}, 0.5},--51
		{ {5.25, 8, 1.3}, 	{0, -360, -90}, 0.5},--51.5
		{ {5.25, 8, 1.3}, 	{0, 0, -90}, 	0},--51.5
		--second loop
		{ {5.25, 2, 1.3}, 	{0, 0, -90}, 	1},--52.5
		{ {5, 1, 2.5}, 		{0, -45, -90}, 	0.5},--53
		{ {4.75, 0, 3.7}, 	{0, -90, -90}, 	0.5},--53.5
		{ {4.75, 1, 4.9}, 	{0, -135, -90}, 0.5},--54
		{ {4.75, 2, 6}, 	{0, -180, -90}, 0.5},--54.5
		{ {5, 3, 4.9}, 		{0, -225, -90}, 0.5},--55
		{ {5.25, 4, 3.7}, 	{0, -270, -90}, 0.5},--55.5
		{ {5.5, 3, 2.5}, 	{0, -315, -90}, 0.5},--56
		{ {6, 2, 1.3}, 		{0, -360, -90}, 0.5},--56.5
		{ {6, 2, 1.3}, 		{0, 0, -90}, 	0},--56.5
		--end of loops
		{ {6, 0, 1.3}, 		{0, 0, -90}, 	1},--57.5
		{ {4, -2.3, 1.3}, 	{0, 0, -180}, 	1},--58.5
		{ {0, -2.3, 0.5}, 	{0, 0, -180}, 	1.5},--60
		{ {-1.2, -2.3, 0.5}, {0, 0, -180}, 	0.5},--60.5
		{ {-3.3, -3.5, 0.5}, {0, 0, -90}, 	1},--61.5
		{ {-3.3, -21, 0.5}, {0, 0, -90}, 	8.5},--70
		{ {-3.3, -22, 0}, 	{0, 0, -90}, 	0.5},--70.5
		{ {-3.3, -24, 0}, 	{0, 0, -90}, 	1.5},--72
		{ {-1.5, -25, 0}, 	{0, 0, 0}, 		1},--73
		{ {0, -24, 0.5}, 	{0, 0, 90}, 	1},--74
	}
	
	local positions = {}
	local rotations = {}
	local timestamps = {}
	local timestamp = 0
	
	for i = 1, #path do
		--copy positions
		table.insert(positions, path[i][1][1])
		table.insert(positions, path[i][1][2])
		table.insert(positions, path[i][1][3])
		--copy rotations
		table.insert(rotations, path[i][2][1])
		table.insert(rotations, path[i][2][2])
		table.insert(rotations, path[i][2][3])
		--copy timestamp
		timestamp = timestamp + path[i][3]
		table.insert(timestamps, timestamp)
	end
	
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

UI:NewButton("ButtonLoop", 10, 10, 50, 190, "Start loop", StartLoop)
UI:NewButton("ButtonBenchmark", 220, 10, 50, 190, "Benchmark", Benchmark)
UI:NewButton("ButtonCamera", 430, 10, 50, 190, "Camera mode", Camera)