hunter = nil

function NewObject()
	local model = UI:GetChild("ComboBoxModels"):GetText() --Gets a text from Edit or Listbox (selected item)
	local object = Object:New(model, 0, 0, 0)--Creates a new object(model path, x, y, rotation) and returns it
end

function DeleteSelectedObject()
	local selected = Object:GetSelected()
	if(selected ~= nil) then
		selected:Delete()--Gets currently selected object and deletes it
	end
end

function SetDicewindowVisibility()
	local window = UI:NewWindow("Window1", 150, 120, "Dice Rolls", false)
	window:NewStaticText("Label1", 5, 10, 30, 50, "Count")--Adds a new static text to UI (name, x, y, width, height, text)
	window:NewStaticText("Label2", 5, 50, 30, 50, "Faces")
	window:NewButton("ButtonUndo", 30, 110, 30, 60, "Roll", RollDices)
	window:NewCheckbox("CheckBox1", 5, 85, 20, 100, "Group", false)--Adds a new checkbox text to UI (name, x, y, width, height, text, initial state)
	window:NewEdit("Edit1", 65, 10, 30, 50, "1")
	local list2 = window:NewCombobox("ComboBox2", 65, 50, 30, 50)
	list2:AddItem(6)
	list2:AddItem(3)
	list2:AddItem(12)
	list2:AddItem(20)
end

function SetRuler()
	Ruler()--Enables ruler for 1 action
end

function UndoAction()
	Undo()--Unexecutes last performed action
end

function RedoAction()
	Redo()--Reexecutes last unexecuted action
end

function RollDices()
	math.randomseed(os.time())
	local window = UI:GetChild("Window1")
	local sides = window:GetChild("ComboBox2"):GetText()
	local count = window:GetChild("Edit1"):GetText()
	local group = window:GetChild("CheckBox1"):GetState()
	local result = ""
	local grouped = {}
	for i = 1, sides do
		grouped[i] = 0
	end
	for i = 1, count do
		local rand = math.random(1, sides)
		if(group) then
			grouped[rand] = grouped[rand] + 1
		else
			result = result .. rand .. " "
		end
	end
	if(group) then
		for i = 1, sides do
			result = result .. i .. ": " .. grouped[i] .. "\n"
		end
	end
	MessageBox(result)
end

function LineOfSight()
	hunter = Object:GetSelected()
end

function OnSelection()
	if(hunter ~= nil) then
		MessageBox(LoS(hunter, Object:GetSelected()))
	end
	hunter = nil
end

function Host()
	NetHost(UI:GetChild("WindowNetwork"):GetChild("EditNetowkPort"):GetText())
	SetTimedCallback("CloseNetwork", 10, false)--Can't delete element from its callback. Delete it after 10 ms
end

function Client()
	local window = UI:GetChild("WindowNetwork")
	NetClient(window:GetChild("EditNetowkIp"):GetText(), window:GetChild("EditNetowkPort"):GetText())
	SetTimedCallback("CloseNetwork", 10, false)--Can't delete element from its callback. Delete it after 10 ms
end

function CloseNetwork()
	UI:DeleteChild("WindowNetwork")
end

function ShowNetwork()
	local window = UI:NewWindow("WindowNetwork", 200, 100, "Network", false)
	window:NewStaticText("StaticIP", 10, 5, 30, 80, "IP")
	window:NewStaticText("StaticPort", 100, 5, 30, 80, "Port")
	window:NewEdit("EditNetowkIp", 10, 40, 30, 80, "127.0.0.1")
	window:NewEdit("EditNetowkPort", 100, 40, 30, 50, "50000")
	window:NewButton("ButtonNetworkHost", 10, 90, 30, 80, "Host", Host)
	window:NewButton("ButtonNetworkClient", 100, 90, 30, 80, "Client", Client)
end

function Send()
	NetSendMessage(UI:GetChild("EditChatText"):GetText())
end

function StringRecieved(str)
	MessageBox(str)
end

function PlayAnim()
	local selected = Object:GetSelected()
	if(selected ~= nil) then
		selected:PlayAnimation("AnimationClip")
	end
end

function RMB(object, x, y, z)
	local selected = Object:GetSelected()
	if(selected ~= nil) then
		selected:GoTo(x, y, 2.0, "AnimationClip", 1.0)
	end
end

function TC1()
	local selected = Object:GetSelected()
	selected:ApplyTeamColor("_base", 255, 0, 0)
end

function TC2()
	local selected = Object:GetSelected()
	selected:ApplyTeamColor("_base", 0, 255, 0)
end

function TC3()
	local selected = Object:GetSelected()
	selected:ApplyTeamColor("_base", 0, 0, 255)
end

function Music()
	PlaySoundPlaylist("music", {
	"music/music_warhammer40ktheme.mp3", 
	"music/music_theme_spacemarines_01.mp3", 
	"music/music_force_commander_theme.mp3", 
	"music/music_dawnofwartheme.mp3",
	"music/battle_ingame_01.mp3",
	"music/music_urban_wasteland.mp3",
	"music/music_tank_march.mp3",
	"music/music_main_title.mp3",
	"music/music_invasion_theme.mp3",
	"music/music_evil_isador_theme.mp3",
	"music/music_acid.mp3",
	"music/music_ork_theme.mp3"
	}, 1.0, true, true)
end

directionX = 0
directionY = 0
function GamepadButtons(gamepadIndex, buttonIndex, state)
	if gamepadIndex == 1 and (buttonIndex >= 11 and buttonIndex <= 14) then
		local selected = Object:GetSelected()
		if selected then
			if buttonIndex == 11 then
				if state then directionY = 1 else directionY = 0 end
			elseif buttonIndex == 12 then
				if state then directionY = -1 else directionY = 0 end
			elseif buttonIndex == 13 then
				if state then directionX = 1 else directionX = 0 end
			elseif buttonIndex == 14 then
				if state then directionX = -1 else directionX = 0 end
			end
			selected:GoTo(selected:GetX() + directionX * 10, selected:GetY() + directionY * 10, 2.0, "AnimationClip", 1.0)
		end
	end
end

function VR()
	EnableVR(true, true)
end

AddLight()
SetLightPosition(1, 0, 0, 30)
CreateSkybox(50, "skybox")--Creates a skybox (size in OpenGLUnits, path to texture folder (names are fixed))
--EnableShadowMap(1024, 53)
EnableMSAA()
SetAnisotropy(GetMaxAnisotropy())
if(GetRendererName() == "DirectX11") then
	SetShaders("directX11/gpu_skinning.hlsl", "directX11/gpu_skinning.hlsl")
	SetParticleSystemShaders("directX11/particle.hlsl", "directX11/particle.hlsl")
	--SetSkyboxShaders("directX11/skybox.hlsl", "directX11/skybox.hlsl")
elseif(GetRendererName() == "OpenGLES") then
	--SetShaders("GLES/gpu_skinning.vsh", "GLES/gpu_skinning.fsh")
	--SetParticleSystemShaders("GLES/particle.vsh", "GLES/particle.fsh")
	SetSkyboxShaders("GLES/skybox.vsh", "GLES/skybox.fsh")
	UI:SetScale(2)
	Viewport:EnableTouchMode()
elseif(GetRendererName() == "Vulkan") then
	--do nothing
else
	SetShaders("openGL/gpu_skinning.vsh", "openGL/gpu_skinning.fsh")
	SetParticleSystemShaders("openGL/particle.vsh", "openGL/particle.fsh")
	SetSkyboxShaders("openGL/skybox.vsh", "openGL/skybox.fsh")
end
EnableGPUSkinning()
--EnableVertexLightning()
CreateLandscape(30, 15, "sand.bmp")--Creates a table (width, height, texture)
--Viewport:CameraStrategy(15, 6, 5, 0.5)--Changes camera limitations (max translation in X axis, max translation in Y axis, max scale, min scale)
--Viewport:CameraFirstPerson()
--UI:Get():ApplyTheme("uiTheme.xml")
local list = UI:NewCombobox("ComboBoxModels", 10, 10, 30, 200)--Adds a new empty listbox tp UI (name, x, y, width, height)
local files = GetFilesList("models", "*.wbm", false)--Find all models and add them into list
for i = 1, #files do
	list:AddItem(files[i])
end
files = GetFilesList("models", "*.dae", false)
for i = 1, #files do
	list:AddItem(files[i])
end
files = GetFilesList("models", "*.obj", false)
for i = 1, #files do
	list:AddItem(files[i])
end
UI:NewButton("ButtonCreate", 220, 10, 30, 80, "Create", NewObject)--Adds new button to UI (name, x, y, width, height, caption, callback function name)
UI:NewButton("ButtonDelete", 310, 10, 30, 80, "Delete", DeleteSelectedObject)
UI:NewButton("ButtonDice", 400, 10, 30, 100, "Roll Dices", SetDicewindowVisibility)
UI:NewButton("ButtonRuler", 510, 10, 30, 80, "Ruler", SetRuler)
UI:NewButton("ButtonUndo", 10, 50, 30, 80, "Undo", UndoAction)
UI:NewButton("ButtonRedo", 100, 50, 30, 80, "Redo", RedoAction)
UI:NewButton("ButtonLoS", 200, 50, 30, 80, "LoS", LineOfSight)
UI:NewButton("ButtonNetwork", 300, 50, 30, 80, "Network", ShowNetwork)
UI:NewEdit("EditChatText", 10, 550, 30, 200, "Text")
UI:NewButton("ButtonChatSend", 230, 550, 30, 80, "Send", Send)
UI:NewButton("ButtonPlayAnim", 10, 100, 30, 80, "Animation", PlayAnim)
UI:NewButton("ButtonMusic", 110, 100, 30, 80, "PlayMusic", Music)
UI:NewButton("ButtonTC1", 210, 100, 30, 80, "TeamColor1", TC1)
UI:NewButton("ButtonTC2", 310, 100, 30, 80, "TeamColor2", TC2)
UI:NewButton("ButtonTC3", 410, 100, 30, 80, "TeamColor3", TC3)
UI:NewButton("ButtonVR", 510, 100, 30, 80, "VR mode", VR)
BindKey(127, false, false, false, DeleteSelectedObject)--Bind Delete key

SetSelectionCallback(OnSelection)
NewDecal("chaos_crater_02.tga", 2.5, 2.5, 0, 5, 5)
SetOnStringRecievedCallback(StringRecieved)
SetRMBCallback("RMB", false)
NewParticleEffect("effect.xml", 0, 0, 0, 1, 1000)
SetGamepadButtonsCallback(GamepadButtons)