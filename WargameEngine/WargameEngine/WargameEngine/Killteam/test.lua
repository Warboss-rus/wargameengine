hunter = nil

function NewObject()
	local ui = UI:Get()--Retrives current assigned UI
	local model = ui:GetChild("ComboBox1"):GetText() --Gets a text from Edit or Listbox (selected item)
	Object:New(model, 0, 0, 0)--Creates a new object(model path, x, y, rotation) and returns it
end

function DeleteSelectedObject()
	Object:GetSelected():Delete()--Gets currently selected object and deletes it
end

function SetDicePanelVisibility()
	local ui = UI:Get()
	local oldState = ui:GetChild("Panel1"):GetVisible()--Get visibility of object and all its children
	ui:GetChild("Panel1"):SetVisible(not oldState)
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
	local panel = UI:Get():GetChild("Panel1")
	local sides = panel:GetChild("ComboBox2"):GetText()
	local count = panel:GetChild("Edit1"):GetText()
	local group = panel:GetChild("CheckBox1"):GetState()
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
	local ui = UI:Get()
	NetHost(50000)
	ui:GetChild("Button8"):SetVisible(false)
	ui:DeleteChild("Button9")
	ui:DeleteChild("Edit2")
end

function Client()
	local ui = UI:Get()
	NetClient(ui:GetChild("Edit2"):GetText(), 50000)
	ui:GetChild("Button9"):SetVisible(false)
	ui:DeleteChild("Button8")
	ui:DeleteChild("Edit2")
end

function Send()
	NetSendMessage(UI:Get():GetChild("Edit3"):GetText())
end

function StringRecieved(str)
	MessageBox(str)
end

function PlayAnim()
	Object:GetSelected():PlayAnimation("AnimationClip")
end

IncludeLibrary("math")--Includes the following library
IncludeLibrary("os")
SetLightPosition(1, 0, 0, 30)
EnableShadowMap(1024, 53)
EnableMSAA()
SetAnisotropy(GetMaxAnisotropy())
SetShaders("shadow.vsh", "shadow.fsh")
--EnableVertexLightning()
CreateSkybox(50, "skybox")--Creates a skybox (size in OpenGLUnits, path to texture folder (names are fixed))
CreateTable(30, 15, "sand.bmp")--Creates a table (width, height, texture)
CameraSetLimits(15, 6, 5, 0.4)--Changes camera limitations (max translation in X axis, max translation in Y axis, max scale, min scale)
local ui = UI:Get()--Get current UI
ui:ApplyTheme("ui.uit")
local list = ui:NewCombobox("ComboBox1", 10, 10, 30, 200)--Adds a new empty listbox tp UI (name, x, y, width, height)
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
ui:NewButton("Button1", 220, 10, 30, 80, "Create", "NewObject")--Adds new button to UI (name, x, y, width, height, caption, callback function name)
ui:NewButton("Button2", 310, 10, 30, 80, "Delete", "DeleteSelectedObject")
ui:NewButton("Button3", 400, 10, 30, 100, "Roll Dices", "SetDicePanelVisibility")
ui:NewButton("Button4", 510, 10, 30, 80, "Ruler", "SetRuler")
ui:NewButton("Button5", 10, 50, 30, 80, "Undo", "UndoAction")
ui:NewButton("Button6", 100, 50, 30, 80, "Redo", "RedoAction")
ui:NewButton("Button7", 200, 50, 30, 80, "LoS", "LineOfSight")
ui:NewButton("Button8", 300, 50, 30, 80, "Host", "Host")
ui:NewEdit("Edit2", 400, 50, 30, 80, "127.0.0.1")
ui:NewButton("Button9", 500, 50, 30, 80, "Client", "Client")
ui:NewEdit("Edit3", 10, 550, 30, 200, "Text")
ui:NewButton("Button10", 230, 550, 30, 80, "Send", "Send")
ui:NewButton("Button11", 10, 100, 30, 80, "Animation", "PlayAnim")
BindKey(127, false, false, false, "DeleteSelectedObject")--Bind Delete key

local panel = ui:NewPanel("Panel1", 390, 40, 150, 120)
panel:SetVisible(false)--Sets visibility of objects and all its children
panel:NewStaticText("Label1", 5, 10, 30, 50, "Count")--Adds a new static text to UI (name, x, y, width, height, text)
panel:NewStaticText("Label2", 5, 50, 30, 50, "Faces")
panel:NewButton("Button5", 30, 110, 30, 60, "Roll", "RollDices")
panel:NewCheckbox("CheckBox1", 5, 85, 20, 100, "Group", false)--Adds a new checkbox text to UI (name, x, y, width, height, text, initial state)
panel:NewEdit("Edit1", 65, 10, 30, 50, "1")
local list2 = panel:NewCombobox("ComboBox2", 65, 50, 30, 50)
list2:AddItem(6)
list2:AddItem(3)
list2:AddItem(12)
list2:AddItem(20)
SetSelectionCallback("OnSelection")
Object:NewDecal("chaos_crater_02.tga", 0, 0, 0, 5, 5)
SetOnStringRecievedCallback("StringRecieved")
--Preload("asphalt.bmp")