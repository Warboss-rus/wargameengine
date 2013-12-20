Player1Roster = {}
Player2Roster = {}
Player1Army = nil
Player2Army = nil

function CreateUnit(army, rosterItem, x, y, rot, owner)
	local object = Object:New(army:GetUnitModel(rosterItem), x, y, rot)
	object:SetProperty("Name", rosterItem.Name .. "(" .. rosterItem.Weapon ..")")
	object:SetProperty("Owner", owner)
	if(owner == "1") then
		object:SetMoveLimit("rectangle", 15, 15, 30, -15)
	elseif(owner == "2") then
		object:SetMoveLimit("rectangle", -15, 15, -30, -15)
	end
	army:SetUnitStats(object, rosterItem)
	return object
end

function Battle()
	local ui = UI:Get():GetChild("Panel1")
	if(GetCost(Player1Roster) > 200 or GetCost(Player2Roster) > 200) then
		MessageBox("Total unit cost must not exceed 200")
		return
	end
	game = loadfile("killteam.lua")
	game()
	local ui = UI:Get():GetChild("Panel1")
	local list = ui:GetChild("List2")
	for i=1, #Player1Roster do
		Player1[i] = CreateUnit(Player1Army, Player1Roster[i], math.floor(20 + (i - 1) / 2.5), (i - 1) % 5 * 2 - 5, -90, "1")
	end
	list = ui:GetChild("List4")
	for i=1, #Player2Roster do
		Player2[i] = CreateUnit(Player2Army, Player2Roster[i], math.floor(-20 - (i - 1) / 2.5), (i - 1) % 5 * 2 - 5, 90, "2")
	end
	LoadMap(ui:GetChild("CBox3"):GetText())
	ui:SetVisible(false)
end

function GetCost(list)
	local sum = 0
	for i=1, #list do
		sum = sum + GetObjectCost(list[i])
	end
	return sum
end

function GetObjectCost(object)
	local sum = 0
	if(object.Name == "Tactical Marine" or object.Name == "Chaos Marine") then
		sum = 15
	elseif(object.Name == "Assault Marine" or object.Name == "Raptor") then
		sum = sum + 20
	elseif(object.Name == "Terminator" or object.Name == "Chaos Terminator") then
		sum = sum + 40
	end
	if(object.Weapon == "Melta gun(15)") then
		sum = sum + 15
	elseif(object.Weapon == "Heavy Bolter(10)") then
		sum = sum + 10
	end
	if(object["Feel no Pain(15)"] == "1") then
		sum = sum + 15
	end
	if(object["Prefered Enemy(20)"] == "1") then
		sum = sum + 20
	end
	if(object["Stealth(15)"] == "1") then
		sum = sum + 15
	end
	return sum
end

function Recalculate()
	local ui = UI:Get():GetChild("Panel1")
	--SMs
	local list = ui:GetChild("List2")
	local selected = list:GetSelectedIndex()
	list:ClearItems()
	local sum = 0
	for i=1, #Player1Roster do
		local cost = GetObjectCost(Player1Roster[i])
		list:AddItem(Player1Roster[i].Name .. "(" .. cost .. ")")
		sum = sum + cost
	end
	list:SetSelectedIndex(selected)
	ui:GetChild("Label4"):SetText("Points: " .. sum .. "/200")
	--Chaos
	list = ui:GetChild("List4")
	selected = list:GetSelectedIndex()
	list:ClearItems()
	sum = 0
	for i=1, #Player2Roster do
		local cost = GetObjectCost(Player2Roster[i])
		list:AddItem(Player2Roster[i].Name .. "(" .. cost .. ")")
		sum = sum + cost
	end
	list:SetSelectedIndex(selected)
	ui:GetChild("Label8"):SetText("Points: " .. sum .. "/200")
end

function AddItem1()
	local ui = UI:Get():GetChild("Panel1")
	local list1 = ui:GetChild("List1")
	local list2 = ui:GetChild("List2")
	local i = #Player1Roster + 1
	Player1Roster[i] = {}
	Player1Roster[i].Name = list1:GetText():sub(0, list1:GetText():len() - 4)
	Player1Roster[i].Weapon = Player1Army:GetDefaultWeapon(Player1Roster[i].Name)
	Player1Roster[i]["Feel no Pain(15)"] = "0"
	Player1Roster[i]["Prefered Enemy(20)"] = "0"
	Player1Roster[i]["Stealth(15)"] = "0"
	Recalculate()
end

function DeleteItem1()
	local ui = UI:Get():GetChild("Panel1")
	local list = ui:GetChild("List2")
	table.remove(Player1Roster, list:GetSelectedIndex())
	Recalculate()
end

function AddItem2()
	local ui = UI:Get():GetChild("Panel1")
	local list1 = ui:GetChild("List3")
	local list2 = ui:GetChild("List4")
	local i = #Player2Roster + 1
	Player2Roster[i] = {}
	Player2Roster[i].Name = list1:GetText():sub(0, list1:GetText():len() - 4)
	Player2Roster[i].Weapon = Player2Army:GetDefaultWeapon(Player2Roster[i].Name)
	Player2Roster[i]["Feel no Pain(15)"] = "0"
	Player2Roster[i]["Prefered Enemy(20)"] = "0"
	Player2Roster[i]["Stealth(15)"] = "0"
	Recalculate()
end

function DeleteItem2()
	local ui = UI:Get():GetChild("Panel1")
	local list = ui:GetChild("List4")
	table.remove(Player2Roster, list:GetSelectedIndex())
	Recalculate()
end

function OnWeaponChange1()
	local ui = UI:Get():GetChild("Panel1")
	local index = ui:GetChild("List2"):GetSelectedIndex()
	Player1Roster[index].Weapon = ui:GetChild("Panel2"):GetChild("RadioGroup1"):GetText()
	Recalculate()
end

function OnWeaponChange2()
	local ui = UI:Get():GetChild("Panel1")
	local index = ui:GetChild("List4"):GetSelectedIndex()
	Player2Roster[index].Weapon = ui:GetChild("Panel3"):GetChild("RadioGroup2"):GetText()
	Recalculate()
end

function OnUnitChange1()
	local ui = UI:Get():GetChild("Panel1")
	local index = ui:GetChild("List2"):GetSelectedIndex()
	local weapons = ui:GetChild("Panel2"):GetChild("RadioGroup1")
	weapons:ClearItems()
	if(index > 0) then
		local weaponList = Player1Army:GetUnitWeapons(Player1Roster[index].Name)
		weapons:SetSelectedIndex(1)
		for i=1, #weaponList do
			weapons:AddItem(weaponList[i])
			if(weaponList[i] == Player1Roster[index].Weapon) then
				weapons:SetSelectedIndex(i)
			end
		end
	end
end

function OnUnitChange2()
	local ui = UI:Get():GetChild("Panel1")
	local index = ui:GetChild("List4"):GetSelectedIndex()
	local weapons = ui:GetChild("Panel3"):GetChild("RadioGroup2")
	weapons:ClearItems()
	if(index > 0) then
		local weaponList = Player2Army:GetUnitWeapons(Player2Roster[index].Name)
		weapons:SetSelectedIndex(1)
		for i=1, #weaponList do
			weapons:AddItem(weaponList[i])
			if(weaponList[i] == Player2Roster[index].Weapon) then
				weapons:SetSelectedIndex(i)
			end
		end
	end
end

function OnArmyChange1()
	local ui = UI:Get():GetChild("Panel1")
	local armyName = ui:GetChild("CBox1"):GetText()
	if(armyName == "SpaceMarines") then
		Player1Army = SpaceMarines:New()
	elseif(armyName == "Chaos") then
		Player1Army = Chaos:New()
	end
	for i=1, #Player1Roster do
		Player1Roster[i] = nil
	end
	local list = ui:GetChild("List1")
	list:ClearItems()
	local armyList = Player1Army:GetUnitList()
	for i=1, #armyList do
		list:AddItem(armyList[i])
	end
	Recalculate()
end

function OnArmyChange2()
	local ui = UI:Get():GetChild("Panel1")
	local armyName = ui:GetChild("CBox2"):GetText()
	if(armyName == "SpaceMarines") then
		Player2Army = SpaceMarines:New()
	elseif(armyName == "Chaos") then
		Player2Army = Chaos:New()
	end
	for i=1, #Player2Roster do
		Player2Roster[i] = nil
	end
	local list = ui:GetChild("List3")
	list:ClearItems()
	local armyList = Player2Army:GetUnitList()
	for i=1, #armyList do
		list:AddItem(armyList[i])
	end
	Recalculate()
end

IncludeLibrary("base")
IncludeLibrary("math")
IncludeLibrary("table")
IncludeLibrary("string")
dofile("Chaos.lua")
dofile("SpaceMarines.lua")
CameraSetLimits(15, 6, 5, 0.4)
local ui = UI:Get():NewPanel("Panel1", 0, 0, 640, 640)
local cbox = ui:NewCombobox("CBox1", 10, 0, 30, 180)
cbox:AddItem("SpaceMarines")
cbox:AddItem("Chaos")
cbox:SetSelectedIndex(1)
cbox:SetOnChangeCallback("OnArmyChange1")
Player1Army = SpaceMarines:New()
ui:NewStaticText("Label1", 32, 30, 20, 200, "Availible units")
ui:NewStaticText("Label2", 262, 0, 20, 200, "Your roster")
ui:NewStaticText("Label3", 452, 0, 20, 200, "Options")
ui:NewStaticText("Label4", 50, 120, 20, 200, "Points: 0/200")
local list = ui:NewList("List1", 2, 57, 65, 200)
list:AddItem("Tactical Marine(15)")
list:AddItem("Assault Marine(20)")
list:AddItem("Terminator(40)")
ui:NewList("List2", 212, 27, 240, 200):SetOnChangeCallback("OnUnitChange1")
ui:NewButton("Button1", 10, 160, 30, 180, "Add>", "AddItem1")
ui:NewButton("Button2", 10, 200, 30, 180, "<Delete", "DeleteItem1")
local panel = ui:NewPanel("Panel2", 420, 30, 240, 200)--:SetVisible(false)
local radiogroup = panel:NewRadioGroup("RadioGroup1", 5, 5, 70, 200)
radiogroup:SetOnChangeCallback("OnWeaponChange1")
--panel:NewCheckbox("Checkbox1", 5, 85, 20, 200, "Feel no Pain(15)", false)
--panel:NewCheckbox("Checkbox2", 5, 110, 20, 200, "Prefered Enemy(20)", false)
--panel:NewCheckbox("Checkbox3", 5, 135, 20, 200, "Stealth(15)", false)

cbox = ui:NewCombobox("CBox2", 10, 335, 30, 180)
cbox:AddItem("SpaceMarines")
cbox:AddItem("Chaos")
cbox:SetSelectedIndex(2)
cbox:SetOnChangeCallback("OnArmyChange2")
Player2Army = Chaos:New()
ui:NewStaticText("Label5", 32, 365, 20, 200, "Availible units")
ui:NewStaticText("Label6", 262, 335, 20, 200, "Your roster")
ui:NewStaticText("Label7", 452, 335, 20, 200, "Options")
ui:NewStaticText("Label8", 50, 455, 20, 200, "Points: 0/200")
list = ui:NewList("List3", 2, 392, 65, 200)
list:AddItem("Chaos Marine(15)")
list:AddItem("Raptor(20)")
list:AddItem("Chaos Terminator(40)")
ui:NewList("List4", 212, 362, 240, 200):SetOnChangeCallback("OnUnitChange2")
ui:NewButton("Button3", 10, 495, 30, 180, "Add>", "AddItem2")
ui:NewButton("Button4", 10, 535, 30, 180, "<Delete", "DeleteItem2")
panel = ui:NewPanel("Panel3", 420, 365, 240, 200)--:SetVisible(false)
radiogroup = panel:NewRadioGroup("RadioGroup2", 5, 5, 70, 200)
radiogroup:SetOnChangeCallback("OnWeaponChange2")
--panel:NewCheckbox("Checkbox4", 5, 85, 20, 200, "Feel no Pain(15)", false)
--panel:NewCheckbox("Checkbox5", 5, 110, 20, 200, "Prefered Enemy(20)", false)
--panel:NewCheckbox("Checkbox6", 5, 135, 20, 200, "Stealth(15)", false)
ui:NewButton("Button5", 280, 280, 40, 200, "Battle", "Battle")
ui:NewStaticText("Label9", 10, 285, 30, 30, "Map:")
list = ui:NewCombobox("CBox3", 50, 285, 30, 200)
list:AddItem("sand.map")
list:AddItem("city.map")
list:AddItem("hell.map")