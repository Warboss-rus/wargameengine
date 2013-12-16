Player1Roster = {}
Player2Roster = {}

function CreateUnit(rosterItem, x, y, rot, owner)
	local object = nil
	if(rosterItem.Name == "Tactical Marine" and (rosterItem.Weapon == "Bolter(0)")) then
		object = Object:New("Angel_of_Death.wbm", x, y, rot)
	elseif(rosterItem.Name == "Tactical Marine" and rosterItem.Weapon == "Melta gun(15)") then
		object = Object:New("SM_Melta.wbm", x, y, rot)
	elseif(rosterItem.Name == "Tactical Marine" and rosterItem.Weapon == "Heavy Bolter(10)") then
		object = Object:New("SM_HB.wbm", x, y, rot)
	elseif(rosterItem.Name == "Assault Marine") then
		object = Object:New("assault_marine.wbm", x, y, rot)
	elseif(rosterItem.Name == "Terminator" and rosterItem.Weapon == "Storm Bolter(0)") then
		object = Object:New("Terminator_SB+PF.wbm", x, y, rot)
	elseif(rosterItem.Name == "Terminator" and rosterItem.Weapon == "Lightning claws(0)") then
		object = Object:New("Terminator_LC.wbm", x, y, rot)
	elseif(rosterItem.Name == "Chaos Marine" and rosterItem.Weapon == "Bolter(0)") then
		object = Object:New("CSM.wbm", x, y, rot)
	elseif(rosterItem.Name == "Chaos Marine" and rosterItem.Weapon == "Melta gun(15)") then
		object = Object:New("CSM_melta.wbm", x, y, rot)
	elseif(rosterItem.Name == "Chaos Marine" and rosterItem.Weapon == "Heavy Bolter(10)") then
		object = Object:New("CSM_HB.wbm", x, y, rot)
	elseif(rosterItem.Name == "Raptor") then
		object = Object:New("raptor.wbm", x, y, rot)
	elseif(rosterItem.Name == "Chaos Terminator" and rosterItem.Weapon == "Combi Bolter(0)") then
		object = Object:New("Chaos_terminator_SB+PF.wbm", x, y, rot)
	elseif(rosterItem.Name == "Chaos Terminator" and rosterItem.Weapon == "Lightning claws(0)") then
		object = Object:New("Chaos_terminator_LC.wbm", x, y, rot)
	end
	object:SetProperty("Name", rosterItem.Name)
	object:SetProperty("Owner", owner)
	if(owner == "1") then
		object:SetMoveLimit("rectangle", 15, 15, 30, -15)
	elseif(owner == "2") then
		object:SetMoveLimit("rectangle", -15, 15, -30, -15)
	end
	if((rosterItem.Name == "Tactical Marine" or rosterItem.Name == "Chaos Marine") and rosterItem.Weapon == "Bolter(0)") then
		object:SetProperty("WS", "4")
		object:SetProperty("BS", "4")
		object:SetProperty("S", "4")
		object:SetProperty("T", "4")
		object:SetProperty("Attacks", "1")
		object:SetProperty("Sv", "3")
		object:SetProperty("InvSv", "7")
		object:SetProperty("MeleeAP", "7")
		object:SetProperty("RerollFailed2Wound", "0")
		object:SetProperty("MovementSpeed", "6")
		object:SetProperty("WeaponRange", "24")
		object:SetProperty("WeaponS", "4")
		object:SetProperty("WeaponAP", "5")
		object:SetProperty("WeaponType", "RapidFire")
		object:SetProperty("WeaponShots", "2")
	elseif((rosterItem.Name == "Tactical Marine" or rosterItem.Name == "Chaos Marine") and rosterItem.Weapon == "Melta gun(15)") then
		object:SetProperty("WS", "4")
		object:SetProperty("BS", "4")
		object:SetProperty("S", "4")
		object:SetProperty("T", "4")
		object:SetProperty("Attacks", "1")
		object:SetProperty("Sv", "3")
		object:SetProperty("InvSv", "7")
		object:SetProperty("MeleeAP", "7")
		object:SetProperty("RerollFailed2Wound", "0")
		object:SetProperty("MovementSpeed", "6")
		object:SetProperty("WeaponRange", "12")
		object:SetProperty("WeaponS", "8")
		object:SetProperty("WeaponAP", "1")
		object:SetProperty("WeaponType", "Assault")
		object:SetProperty("WeaponShots", "1")
	elseif((rosterItem.Name == "Tactical Marine" or rosterItem.Name == "Chaos Marine") and rosterItem.Weapon == "Heavy Bolter(10)") then
		object:SetProperty("WS", "4")
		object:SetProperty("BS", "4")
		object:SetProperty("S", "4")
		object:SetProperty("T", "4")
		object:SetProperty("Attacks", "1")
		object:SetProperty("Sv", "3")
		object:SetProperty("InvSv", "7")
		object:SetProperty("MeleeAP", "7")
		object:SetProperty("RerollFailed2Wound", "0")
		object:SetProperty("MovementSpeed", "6")
		object:SetProperty("WeaponRange", "36")
		object:SetProperty("WeaponS", "5")
		object:SetProperty("WeaponAP", "4")
		object:SetProperty("WeaponType", "Heavy")
		object:SetProperty("WeaponShots", "3")
	elseif(rosterItem.Name == "Assault Marine" or rosterItem.Name == "Raptor") then
		object:SetProperty("WS", "4")
		object:SetProperty("BS", "4")
		object:SetProperty("S", "4")
		object:SetProperty("T", "4")
		object:SetProperty("Attacks", "3")
		object:SetProperty("Sv", "3")
		object:SetProperty("InvSv", "7")
		object:SetProperty("MeleeAP", "7")
		object:SetProperty("RerollFailed2Wound", "0")
		object:SetProperty("MovementSpeed", "12")
		object:SetProperty("WeaponRange", "12")
		object:SetProperty("WeaponS", "4")
		object:SetProperty("WeaponAP", "5")
		object:SetProperty("WeaponType", "Pistol")
		object:SetProperty("WeaponShots", "1")
	elseif((rosterItem.Name == "Terminator" and rosterItem.Weapon == "Storm Bolter(0)") or (rosterItem.Name == "Chaos Terminator" and rosterItem.Weapon == "Combi Bolter(0)")) then
		object:SetProperty("WS", "4")
		object:SetProperty("BS", "4")
		object:SetProperty("S", "8")
		object:SetProperty("T", "4")
		object:SetProperty("Attacks", "2")
		object:SetProperty("Sv", "2")
		object:SetProperty("InvSv", "5")
		object:SetProperty("MeleeAP", "2")
		object:SetProperty("RerollFailed2Wound", "0")
		object:SetProperty("MovementSpeed", "6")
		object:SetProperty("WeaponRange", "24")
		object:SetProperty("WeaponS", "4")
		object:SetProperty("WeaponAP", "5")
		object:SetProperty("WeaponType", "Assault")
		object:SetProperty("WeaponShots", "2")
	elseif((rosterItem.Name == "Terminator" or rosterItem.Name == "Chaos Terminator") and rosterItem.Weapon == "Lightning claws(0)") then
		object:SetProperty("WS", "4")
		object:SetProperty("BS", "4")
		object:SetProperty("S", "4")
		object:SetProperty("T", "4")
		object:SetProperty("Attacks", "4")
		object:SetProperty("Sv", "2")
		object:SetProperty("InvSv", "5")
		object:SetProperty("MeleeAP", "3")
		object:SetProperty("RerollFailed2Wound", "1")
		object:SetProperty("MovementSpeed", "6")
		object:SetProperty("WeaponRange", "0")
		object:SetProperty("WeaponS", "0")
		object:SetProperty("WeaponAP", "0")
		object:SetProperty("WeaponType", "")
		object:SetProperty("WeaponShots", "0")
	end
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
		Player1[i] = CreateUnit(Player1Roster[i], math.floor(20 + (i - 1) / 2.5), (i - 1) % 5 * 2 - 5, -90, "1")
	end
	list = ui:GetChild("List4")
	for i=1, #Player2Roster do
		Player2[i] = CreateUnit(Player2Roster[i], math.floor(-20 - (i - 1) / 2.5), (i - 1) % 5 * 2 - 5, 90, "2")
	end
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
	ui:GetChild("Label5"):SetText("Points: " .. sum .. "/200")
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
	ui:GetChild("Label10"):SetText("Points: " .. sum .. "/200")
end

function AddItem1()
	local ui = UI:Get():GetChild("Panel1")
	local list1 = ui:GetChild("List1")
	local list2 = ui:GetChild("List2")
	local i = #Player1Roster + 1
	Player1Roster[i] = {}
	Player1Roster[i].Name = list1:GetText():sub(0, list1:GetText():len() - 4)
	if(Player1Roster[i].Name == "Terminator") then
		Player1Roster[i].Weapon = "Storm Bolter(0)"
	else
		Player1Roster[i].Weapon = "Bolter(0)"
	end
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
	if(Player2Roster[i].Name == "Chaos Terminator") then
		Player2Roster[i].Weapon = "Combi Bolter(0)"
	else
		Player2Roster[i].Weapon = "Bolter(0)"
	end
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
	if(index > 0) then
		weapons:SetVisible(true)
		weapons:ClearItems()
		if(Player1Roster[index].Name == "Tactical Marine") then
			weapons:AddItem("Bolter(0)")
			weapons:AddItem("Melta gun(15)")
			weapons:AddItem("Heavy Bolter(10)")
			if(Player1Roster[index].Weapon == "Melta gun(15)") then
				weapons:SetSelectedIndex(2)
			elseif(Player1Roster[index].Weapon == "Heavy Bolter(10)") then
				weapons:SetSelectedIndex(3)
			else
				weapons:SetSelectedIndex(1)
			end
		elseif(Player1Roster[index].Name == "Assault Marine") then
			weapons:AddItem("Bolt-pistol(0)")
			weapons:SetSelectedIndex(1)
		elseif(Player1Roster[index].Name == "Terminator") then
			weapons:AddItem("Storm Bolter(0)")
			weapons:AddItem("Lightning claws(0)")
			if(Player1Roster[index].Weapon == "Lightning claws(0)") then
				weapons:SetSelectedIndex(2)
			else
				weapons:SetSelectedIndex(1)
			end
		end
	else
		weapons:SetVisible(false)
	end
end

function OnUnitChange2()
	local ui = UI:Get():GetChild("Panel1")
	local index = ui:GetChild("List4"):GetSelectedIndex()
	local weapons = ui:GetChild("Panel3"):GetChild("RadioGroup2")
	if(index > 0) then
		weapons:SetVisible(true)
		weapons:ClearItems()
		if(Player2Roster[index].Name == "Chaos Marine") then
			weapons:AddItem("Bolter(0)")
			weapons:AddItem("Melta gun(15)")
			weapons:AddItem("Heavy Bolter(10)")
			if(Player2Roster[index].Weapon == "Melta gun(15)") then
				weapons:SetSelectedIndex(2)
			elseif(Player2Roster[index].Weapon == "Heavy Bolter(10)") then
				weapons:SetSelectedIndex(3)
			else
				weapons:SetSelectedIndex(1)
			end
		elseif(Player2Roster[index].Name == "Raptor") then
			weapons:AddItem("Bolt-pistol(0)")
			weapons:SetSelectedIndex(1)
		elseif(Player2Roster[index].Name == "Chaos Terminator") then
			weapons:AddItem("Combi Bolter(0)")
			weapons:AddItem("Lightning claws(0)")
			if(Player2Roster[index].Weapon == "Lightning claws(0)") then
				weapons:SetSelectedIndex(2)
			else
				weapons:SetSelectedIndex(1)
			end
		end
	else
		weapons:SetVisible(false)
	end
end

IncludeLibrary("base")
IncludeLibrary("math")
IncludeLibrary("table")
IncludeLibrary("string")
CameraSetLimits(15, 6, 5, 0.4)
local ui = UI:Get():NewPanel("Panel1", 0, 0, 640, 640)
ui:NewStaticText("Label1", 32, 0, 20, 200, "Space Marines")
ui:NewStaticText("Label2", 32, 30, 20, 200, "Availible units")
ui:NewStaticText("Label3", 262, 0, 20, 200, "Your roster")
ui:NewStaticText("Label4", 452, 0, 20, 200, "Options")
ui:NewStaticText("Label5", 50, 120, 20, 200, "Points: 0/200")
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
panel:NewCheckbox("Checkbox1", 5, 85, 20, 200, "Feel no Pain(15)", false)
panel:NewCheckbox("Checkbox2", 5, 110, 20, 200, "Prefered Enemy(20)", false)
panel:NewCheckbox("Checkbox3", 5, 135, 20, 200, "Stealth(15)", false)

ui:NewStaticText("Label6", 72, 335, 20, 200, "Chaos")
ui:NewStaticText("Label7", 32, 365, 20, 200, "Availible units")
ui:NewStaticText("Label8", 262, 335, 20, 200, "Your roster")
ui:NewStaticText("Label9", 452, 335, 20, 200, "Options")
ui:NewStaticText("Label10", 50, 455, 20, 200, "Points: 0/200")
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
panel:NewCheckbox("Checkbox4", 5, 85, 20, 200, "Feel no Pain(15)", false)
panel:NewCheckbox("Checkbox5", 5, 110, 20, 200, "Prefered Enemy(20)", false)
panel:NewCheckbox("Checkbox6", 5, 135, 20, 200, "Stealth(15)", false)
ui:NewButton("Button5", 210, 280, 40, 200, "Battle", "Battle")