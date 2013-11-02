function NewObject()
	local ui = UI:Get()--Retrives current assigned UI
	local model = ui:GetChild("ListBox1"):GetText() --Gets a text from Edit or Listbox (selected item)
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
	local panel = UI:Get():GetChild("Panel1")
	local sides = panel:GetChild("ListBox2"):GetText()
	local count = panel:GetChild("Edit1"):GetText()
	local group = panel:GetChild("CheckBox1"):GetState()
	RollDice(count, sides, group)--Rolls "count" dices with "sides" sides, group them by result if "group" and returns result as a messagebox
end

CreateSkybox(50, "skybox")--Creates a skybox (size in OpenGLUnits, path to texture folder (names are fixed))
CreateTable(30, 15, "sand.bmp")--Creates a table (width, height, texture)
CameraSetLimits(15, 6, 2.8, 0.4)--Changes camera limitations (max translation in X axis, max translation in Y axis, max scale, min scale)
local ui = UI:New()--Creates a new UI (doesn't displays anything)
ui:Set()--Assigns this UI to window. Old assigned UI will be destroyed (Even if you have it in some variable)
local list = ui:NewListbox("ListBox1", 10, 10, 30, 200)--Adds a new empty listbox tp UI (name, x, y, width, height)
list:AddItem("SpaceMarine.obj")
list:AddItem("CSM.obj")
list:AddItem("rhino.obj")
list:AddItem("Ruine.obj")
ui:NewButton("Button1", 220, 10, 30, 80, "Create", "NewObject")--Adds new button to UI (name, x, y, width, height, caption, callback function name)
ui:NewButton("Button2", 310, 10, 30, 80, "Delete", "DeleteSelectedObject")
ui:NewButton("Button3", 400, 10, 30, 100, "Roll Dices", "SetDicePanelVisibility")
ui:NewButton("Button4", 510, 10, 30, 80, "Ruler", "SetRuler")
ui:NewButton("Button5", 10, 50, 30, 80, "Undo", "UndoAction")
ui:NewButton("Button6", 100, 50, 30, 80, "Redo", "RedoAction")

local panel = ui:NewPanel("Panel1", 390, 40, 150, 120)
panel:SetVisible(false)--Sets visibility of objects and all its children
panel:NewStaticText("Label1", 5, 10, 30, 50, "Count")--Adds a new static text to UI (name, x, y, width, height, text)
panel:NewStaticText("Label2", 5, 50, 30, 50, "Faces")
panel:NewButton("Button5", 30, 110, 30, 60, "Roll", "RollDices")
panel:NewCheckbox("CheckBox1", 5, 85, 20, 100, "Group", false)--Adds a new checkbox text to UI (name, x, y, width, height, text, initial state)
panel:NewEdit("Edit1", 65, 10, 30, 50, "1")
local list2 = panel:NewListbox("ListBox2", 65, 50, 30, 50)
list2:AddItem(6)
list2:AddItem(3)
list2:AddItem(12)
list2:AddItem(20)
--[[Other functions
Object:Null()--(bool)Checks whenether this instance of object is NULL
Object:GetModel()--(string)Retrieves model path of this instance of object
Object:GetX()--(number)Retrieves x coordinate of this instance of object.
Object:GetY()--(number)Retrieves y coordinate of this instance of object.
Object:GetZ()--(number)Retrieves z coordinate of this instance of object.
Object:GetRotation()--(number)Retrieves rotation of this instance of object.
Object:Move(x, y, z)--()Translates current object by specified coordinates.
Object:Rotate(rotation)--()Rotates current object by specifed angle
Object:HideMesh(meshName)--()Hides specified mesh of current object
Object:ShowMesh(meshName)--()Unhides specified mesh of current object
MessageBox("It works this way", "Working")--()Show Message box with caption
MessageBox("Or this")--()Messagebox without caption
RunScript("script.lua")--()Runs another script file
ui:Free()--()Frees an UI. UI automaticly frees when deassigned from the window or at program exit]]