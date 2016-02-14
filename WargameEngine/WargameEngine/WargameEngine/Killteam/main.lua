Player1Roster = {}
Player2Roster = {}
Player1Army = nil
Player2Army = nil
races = {}

function getKeysSorted(tbl, func)
  local keys = {}
  for key in pairs(tbl) do
    table.insert(keys, key)
  end

  table.sort(keys, func)

  return keys
end

function GetUnits(army, list)
	local unitList = getKeysSorted(army.units, function(a, b) return army.units[a].Cost < army.units[b].Cost end)
	for i=1, #unitList do
		list:AddItem(unitList[i] .. " (" .. army.units[unitList[i]].Cost .. ")")
	end
end

function CreateUnit(army, rosterItem, x, y, rot, owner)
	local model = army.weapons[rosterItem.Weapon].Model
	local object = Object:New(model, x, y, rot)
	object:SetProperty("Name", rosterItem.Name)
	if(army.weapons[rosterItem.Weapon].Melee == true) then
		object:SetProperty("MeleeWeapon", rosterItem.Weapon)
		if(army.weapons[rosterItem.Weapon].NoRanged == true) then
			object:SetProperty("RangedWeapon", "")
		else
			object:SetProperty("RangedWeapon", army.units[rosterItem.Name].RangedWeapon)
		end
	else
		object:SetProperty("RangedWeapon", rosterItem.Weapon)
		object:SetProperty("MeleeWeapon", army.units[rosterItem.Name].MeleeWeapon)
	end
	object:SetProperty("Race", army.racename)
	object:SetProperty("Owner", owner)
	if(owner == "1") then
		object:SetMoveLimit("rectangle", 15, 15, 30, -15)
	elseif(owner == "2") then
		object:SetMoveLimit("rectangle", -15, 15, -30, -15)
	end
	return object
end

function Battle()
	local ui = UI:GetChild("Panel1")
	if(GetCost(Player1Roster) > 200 or GetCost(Player2Roster) > 200) then
		MessageBox("Total unit cost must not exceed 200")
		return
	end
	Init()
	local ui = UI:GetChild("Panel1")
	local list = ui:GetChild("List2")
	for i=1, #Player1Roster do
		Player1[i] = CreateUnit(Player1Army, Player1Roster[i], math.floor(20 + (i - 1) / 2.5), (i - 1) % 5 * 2 - 5, -90, "1")
	end
	list = ui:GetChild("List4")
	for i=1, #Player2Roster do
		Player2[i] = CreateUnit(Player2Army, Player2Roster[i], math.floor(-20 - (i - 1) / 2.5), (i - 1) % 5 * 2 - 5, 90, "2")
	end
	LoadMap(GetAbsolutePath(ui:GetChild("CBox3"):GetText()))
	SetTimedCallback("ClearUI", 10, false)--Can't delete element from its callback. Delete it after 10 ms
end

function ClearUI()
	UI:DeleteChild("Panel1")
end

function GetCost(list)
	local sum = 0
	for i=1, #list do
		sum = sum + Player1Army.units[Player1Roster[i].Name].Cost + Player1Army.weapons[Player1Roster[i].Weapon].Cost
	end
	return sum
end

function Recalculate()
	local ui = UI:GetChild("Panel1")
	--Player1
	local list = ui:GetChild("List2")
	local selected = list:GetSelectedIndex()
	list:ClearItems()
	local sum = 0
	for i=1, #Player1Roster do
		local cost = Player1Army.units[Player1Roster[i].Name].Cost + Player1Army.weapons[Player1Roster[i].Weapon].Cost
		list:AddItem(Player1Roster[i].Name .. "(" .. cost .. ")")
		sum = sum + cost
	end
	if(list:GetItemsCount() > 0) then
		list:SetSelectedIndex(selected)
	end
	ui:GetChild("Label4"):SetText("Points: " .. sum .. "/200")
	--Player2
	list = ui:GetChild("List4")
	selected = list:GetSelectedIndex()
	list:ClearItems()
	sum = 0
	for i=1, #Player2Roster do
		local cost = Player2Army.units[Player2Roster[i].Name].Cost + Player2Army.weapons[Player2Roster[i].Weapon].Cost
		list:AddItem(Player2Roster[i].Name .. "(" .. cost .. ")")
		sum = sum + cost
	end
	if(list:GetItemsCount() > 0) then
		list:SetSelectedIndex(selected)
	end
	ui:GetChild("Label8"):SetText("Points: " .. sum .. "/200")
end

function AddItem1()
	local ui = UI:GetChild("Panel1")
	local list1 = ui:GetChild("List1")
	local list2 = ui:GetChild("List2")
	local i = #Player1Roster + 1
	Player1Roster[i] = {}
	Player1Roster[i].Name = string.sub(list1:GetText(), 1, string.find(list1:GetText(), "%(") - 2)
	Player1Roster[i].Weapon = Player1Army.units[Player1Roster[i].Name].SupportedWeapons[1]
	Recalculate()
end

function DeleteItem1()
	local ui = UI:GetChild("Panel1")
	local list = ui:GetChild("List2")
	table.remove(Player1Roster, list:GetSelectedIndex())
	Recalculate()
end

function AddItem2()
	local ui = UI:GetChild("Panel1")
	local list1 = ui:GetChild("List3")
	local list2 = ui:GetChild("List4")
	local i = #Player2Roster + 1
	Player2Roster[i] = {}
	Player2Roster[i].Name = string.sub(list1:GetText(), 1, string.find(list1:GetText(), "%(") - 2)
	Player2Roster[i].Weapon = Player2Army.units[Player2Roster[i].Name].SupportedWeapons[1]
	Recalculate()
end

function DeleteItem2()
	local ui = UI:GetChild("Panel1")
	local list = ui:GetChild("List4")
	table.remove(Player2Roster, list:GetSelectedIndex())
	Recalculate()
end

function OnWeaponChange1()
	local ui = UI:GetChild("Panel1")
	local index = ui:GetChild("List2"):GetSelectedIndex()
	local weapon = ui:GetChild("Panel2"):GetChild("RadioGroup1"):GetText()
	weapon = string.sub(weapon, 1, string.find(weapon, "%(") - 2)
	Player1Roster[index].Weapon = weapon
	Recalculate()
end

function OnWeaponChange2()
	local ui = UI:GetChild("Panel1")
	local index = ui:GetChild("List4"):GetSelectedIndex()
	local weapon = ui:GetChild("Panel3"):GetChild("RadioGroup2"):GetText()
	weapon = string.sub(weapon, 1, string.find(weapon, "%(") - 2)
	Player2Roster[index].Weapon = weapon
	Recalculate()
end

function OnUnitChange1()
	local ui = UI:GetChild("Panel1")
	local unitName = ui:GetChild("List2"):GetText()
	unitName = unitName:sub(1, string.find(unitName, "%(") - 1)
	local weapons = ui:GetChild("Panel2"):GetChild("RadioGroup1")
	weapons:ClearItems()
	if(unitName ~= "") then
		local weaponList = Player1Army.units[unitName].SupportedWeapons
		local selectedIndex = 1
		for i=1, #weaponList do
			local text = weaponList[i] .. " (" .. Player1Army.weapons[weaponList[i]].Cost .. ")"
			weapons:AddItem(text)
			if(weaponList[i] == Player1Roster[ui:GetChild("List2"):GetSelectedIndex()].Weapon) then
				selectedIndex = i
			end
		end
		weapons:SetSelectedIndex(selectedIndex)
	end
end

function OnUnitChange2()
	local ui = UI:GetChild("Panel1")
	local unitName = ui:GetChild("List4"):GetText()
	unitName = unitName:sub(1, string.find(unitName, "%(") - 1)
	local weapons = ui:GetChild("Panel3"):GetChild("RadioGroup2")
	weapons:ClearItems()
	if(unitName ~= "") then
		local weaponList = Player2Army.units[unitName].SupportedWeapons
		local selectedIndex = 1
		for i=1, #weaponList do
			local text = weaponList[i] .. " (" .. Player2Army.weapons[weaponList[i]].Cost .. ")"
			weapons:AddItem(text)
			if(weaponList[i] == Player2Roster[ui:GetChild("List4"):GetSelectedIndex()].Weapon) then
				selectedIndex = i
			end
		end
		weapons:SetSelectedIndex(selectedIndex)
	end
end

function OnArmyChange1()
	local ui = UI:GetChild("Panel1")
	local armyName = ui:GetChild("CBox1"):GetText()
	Player1Army = races[armyName]
	for i=1, #Player1Roster do
		Player1Roster[i] = nil
	end
	local list = ui:GetChild("List1")
	list:ClearItems()
	GetUnits(Player1Army, list)
	Recalculate()
end

function OnArmyChange2()
	local ui = UI:GetChild("Panel1")
	local armyName = ui:GetChild("CBox2"):GetText()
	Player2Army = races[armyName]
	for i=1, #Player2Roster do
		Player2Roster[i] = nil
	end
	local list = ui:GetChild("List3")
	list:ClearItems()
	GetUnits(Player2Army, list)
	Recalculate()
end

--ResizeWindow(600, 600)
RunScript("killteam.lua")
--RunScript all your races here
RunScript("Chaos.lua")
RunScript("SpaceMarines.lua")
local ui = UI:NewPanel("Panel1", 0, 0, 640, 640)
local cbox = ui:NewCombobox("CBox1", 10, 0, 30, 180)
local raceslist = getKeysSorted(races, function(a, b) return a < b end)
for i=1, #raceslist do
	cbox:AddItem(raceslist[i])
end
cbox:SetSelectedIndex(2)
cbox:SetOnChangeCallback("OnArmyChange1")
Player1Army = races[cbox:GetText()]
ui:NewStaticText("Label1", 32, 30, 20, 200, "Availible units")
ui:NewStaticText("Label2", 262, 0, 20, 200, "Your roster")
ui:NewStaticText("Label3", 452, 0, 20, 200, "Options")
ui:NewStaticText("Label4", 50, 120, 20, 200, "Points: 0/200")
local list = ui:NewList("List1", 2, 57, 65, 200)
GetUnits(Player1Army, list)
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
for i=1, #raceslist do
	cbox:AddItem(raceslist[i])
end
cbox:SetSelectedIndex(1)
cbox:SetOnChangeCallback("OnArmyChange2")
Player2Army = races[cbox:GetText()]
ui:NewStaticText("Label5", 32, 365, 20, 200, "Availible units")
ui:NewStaticText("Label6", 262, 335, 20, 200, "Your roster")
ui:NewStaticText("Label7", 452, 335, 20, 200, "Options")
ui:NewStaticText("Label8", 50, 455, 20, 200, "Points: 0/200")
list = ui:NewList("List3", 2, 392, 65, 200)
GetUnits(Player2Army, list)
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