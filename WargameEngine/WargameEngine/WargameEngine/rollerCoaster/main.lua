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
	
	local path = {
		--position			--rotation		--duration --timestamp
		{ {0, -24.2, 0.5}, 	{0, 0, 90}, 	0 },--0
		{ {0, -22.5, 0.8}, 	{0, 45, 90}, 	1 },--1
		{ {0, -16.5, 7.9}, 	{0, 45, 90}, 	7},--8
		{ {0, -10.5, 15}, 	{0, 45, 90}, 	7},--15
		{ {0, -9, 15.5},	{0, 0, 90}, 	1},--16
		{ {0, -7.5, 15}, 	{0, -65, 90}, 	1},--17
		{ {0, -5.25, 8.65}, {0, -65, 90}, 	1.75},--18.75
		{ {0, -3, 2.3}, 	{0, -65, 90}, 	1.75},--20.5
		{ {0, -1.2, 2}, 	{0, 0, 90}, 	0.75},--21.25
		--second hill
		{ {0, 0.5, 2.3}, 	{0, 65, 90}, 	0.75},--22
		{ {0, 3.3, 9.5}, 	{0, 65, 90}, 	3},--25
		{ {0, 5.1, 10.5}, 	{0, 0, 90}, 	0.75},--25.75
		{ {0, 6.5, 9.5}, 	{0, -65, 90}, 	0.75},--26.5
		{ {0, 8.5, 4}, 		{0, -65, 90}, 	1.5},--28
		{ {0, 10, 3}, 		{0, 0, 90}, 	0.5},--28.5
		--third hill
		{ {0, 12, 3.5}, 	{0, 65, 90}, 	0.5},--29
		{ {0, 14.5, 8.5}, 	{0, 65, 90}, 	1.5},--30.5
		{ {0, 16, 9}, 		{0, 0, 90}, 	0.6},--31.1
		{ {0, 17.5, 8.5}, 	{0, -65, 90}, 	0.6},--31.7
		{ {0, 18.5, 4.5}, 	{0, -65, 90}, 	1},--32.7
		{ {0, 19, 4}, 		{0, 0, 90}, 	0.5},--33.2
		--turn
		{ {0, 21, 4}, 		{0, 0, 90}, 	0.7},--33.9
		{ {2.25, 23, 4}, 	{0, 0, 0}, 		0.7},--34.6
		{ {4.5, 21, 4}, 	{0, 0, -90}, 	0.7},--35.3
		--forth hill
		{ {4.5, 20, 4}, 	{0, 0, -90}, 	0.5},--35.8
		{ {4.5, 19, 4.5}, 	{0, -55, -90}, 	0.5},--36.3
		{ {4.5, 17, 8}, 	{0, -55, -90}, 	1},--37.3
		{ {4.5, 15.5, 9}, 	{0, 0, -90}, 	0.5},--37.8
		{ {4.5, 14, 8}, 	{0, 65, -90}, 	0.5},--38.3
		{ {4.5, 11.5, 2}, 	{0, 65, -90}, 	1.5},--39.8
		{ {4.5, 11, 1.3}, 	{0, 0, -90}, 	0.5},--40.3
		--first loop
		{ {4.5, 8, 1.3}, 	{0, 0, -90}, 	0.8},--41.1
		{ {4, 6, 3.7}, 		{0, -90, -90}, 	0.8},--41.9
		{ {4.5, 8, 6}, 		{0, -180, -90}, 0.8},--42.7
		{ {5, 10, 3.7}, 	{0, -270, -90}, 0.8},--43.5
		{ {5.25, 8, 1.3}, 	{0, -360, -90}, 0.8},--44.3
		{ {5.25, 8, 1.3}, 	{0, 0, -90}, 	0.001},--44.3
		--second loop
		{ {5.25, 2, 1.3}, 	{0, 0, -90}, 	1.2},--45.1
		{ {4.75, 0, 3.7}, 	{0, -90, -90}, 	0.8},--45.9
		{ {5.25, 2, 6}, 	{0, -180, -90}, 0.8},--46.7
		{ {5.75, 4, 3.7}, 	{0, -270, -90}, 0.8},--47.5
		{ {6, 2, 1.3}, 		{0, -360, -90}, 0.8},--48.3
		{ {6, 2, 1.3}, 		{0, 0, -90}, 	0.001},--48.3
		--end of loops
		{ {6, 0, 1.3}, 		{0, 0, -90}, 	0.8},--49.1
		{ {4, -2.3, 1.3}, 	{0, 0, -180}, 	1.2},--50.3
		{ {0, -2.3, 0.5}, 	{0, 0, -180}, 	1.5},--51.8
		{ {-1.2, -2.3, 0.5}, {0, 0, -180}, 	0.7},--52.5
		{ {-3.3, -3.5, 0.5}, {0, 0, -90}, 	1},--53.5 
		{ {-3.3, -12.25, 0.5},{0, 0, -90}, 	4.25},--62
		{ {-3.3, -21, 0.5}, {0, 0, -90}, 	4.25},--62
		{ {-3.3, -22, 0}, 	{0, 0, -90}, 	0.5},--62.5
		{ {-3.3, -24, 0}, 	{0, 0, -90}, 	1.5},--64
		{ {-1.5, -25, 0}, 	{0, 0, 0}, 		1},--65
		{ {0, -24, 0.5}, 	{0, 0, 90}, 	1},--66
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
	
	SetTimedCallback(End, math.ceil(timestamp * 1000), false)
	
	cart:MovePath(positions, rotations, timestamps, "spline3")
	if not repeatAll then
		StartBenchmark()
	end
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