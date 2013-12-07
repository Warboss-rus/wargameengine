function CreateUnit(name, x, y, rot, owner)
	local object = nil
	if(name == "Tactical Marine(bolter)") then
		object = Object:New("Angel_of_Death.wbm", x, y, rot)
	elseif(name == "Tactical Marine(meltagun)") then
		object = Object:New("SM_Melta.wbm", x, y, rot)
	elseif(name == "Tactical Marine(heavy bolter)") then
		object = Object:New("SM_HB.wbm", x, y, rot)
	elseif(name == "Assault Marine") then
		object = Object:New("assault_marine.wbm", x, y, rot)
	elseif(name == "Terminator(storm bolter)") then
		object = Object:New("Terminator_SB+PF.wbm", x, y, rot)
	elseif(name == "Terminator(lightning claws)") then
		object = Object:New("Terminator_LC.wbm", x, y, rot)
	elseif(name == "Chaos Marine(bolter)" or name == "Chaos Terminator(combi bolter)" or name == "Chaos Terminator(lightning claws)") then
		object = Object:New("CSM.wbm", x, y, rot)
	elseif(name == "Chaos Marine(meltagun)") then
		object = Object:New("CSM_melta.wbm", x, y, rot)
	elseif(name == "Chaos Marine(heavy bolter)") then
		object = Object:New("CSM_HB.wbm", x, y, rot)
	elseif(name == "Raptor") then
		object = Object:New("raptor.wbm", x, y, rot)
	end
	object:SetProperty("Name", name)
	object:SetProperty("Owner", owner)
	if(name == "Tactical Marine(bolter)" or name == "Chaos Marine(bolter)") then
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
	elseif(name == "Tactical Marine(meltagun)" or name == "Chaos Marine(meltagun)") then
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
	elseif(name == "Tactical Marine(heavy bolter)" or name == "Chaos Marine(heavy bolter)") then
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
	elseif(name == "Assault Marine" or name == "Raptor") then
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
	elseif(name == "Terminator(storm bolter)" or name == "Chaos Terminator(combi bolter)") then
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
	elseif(name == "Terminator(lightning claws)" or name == "Chaos Terminator(lightning claws)") then
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
	if(GetCost(ui:GetChild("List2")) > 200 or GetCost(ui:GetChild("List4")) > 200) then
		MessageBox("Total unit cost must not exceed 200")
		return
	end
	game = loadfile("killteam.lua")
	game()
	local ui = UI:Get():GetChild("Panel1")
	local list = ui:GetChild("List2")
	for i=1, list:GetItemsCount() do
		local name = list:GetItem(i)
		local object = CreateUnit(name, 20 + (i - 1) / 2.5, (i - 1) % 5 * 2 - 5, -90, "1")
		Player1[i] = object
	end
	list = ui:GetChild("List4")
	for i=1, list:GetItemsCount() do
		local name = list:GetItem(i)
		local object = CreateUnit(name, -20 - (i - 1) / 2.5, (i - 1) % 5 * 2 - 5, 90, "2")
		Player2[i] = object
	end
	ui:SetVisible(false)
end

function GetCost(list)
	local sum = 0
	for i=1, list:GetItemsCount() do
		local name = list:GetItem(i)
		if(name == "Tactical Marine(bolter)" or name == "Chaos Marine(bolter)") then
			sum = sum + 15
		elseif(name == "Tactical Marine(meltagun)" or name == "Chaos Marine(meltagun)") then
			sum = sum + 30
		elseif(name == "Tactical Marine(heavy bolter)" or name == "Chaos Marine(heavy bolter)") then
			sum = sum + 30
		elseif(name == "Assault Marine" or name == "Raptor") then
			sum = sum + 20
		elseif(name == "Terminator(storm bolter)" or name == "Chaos Terminator(combi bolter)") then
			sum = sum + 40
		elseif(name == "Terminator(lightning claws)" or name == "Chaos Terminator(lightning claws)") then
			sum = sum + 40
		end
	end
	return sum
end

function AddItem1()
	local ui = UI:Get():GetChild("Panel1")
	local list1 = ui:GetChild("List1")
	local list2 = ui:GetChild("List2")
	list2:AddItem(list1:GetText())
	ui:GetChild("Label4"):SetText("Points: " .. GetCost(list2) .. "/200")
end

function DeleteItem1()
	local ui = UI:Get():GetChild("Panel1")
	local list = ui:GetChild("List2")
	list:DeleteItem(list:GetSelectedIndex())
	ui:GetChild("Label4"):SetText("Points: " .. GetCost(list) .. "/200")
end

function AddItem2()
	local ui = UI:Get():GetChild("Panel1")
	local list1 = ui:GetChild("List3")
	local list2 = ui:GetChild("List4")
	list2:AddItem(list1:GetText())
	ui:GetChild("Label8"):SetText("Points: " .. GetCost(list2) .. "/200")
end

function DeleteItem2()
	local ui = UI:Get():GetChild("Panel1")
	local list = ui:GetChild("List4")
	list:DeleteItem(list:GetSelectedIndex())
	ui:GetChild("Label8"):SetText("Points: " .. GetCost(list) .. "/200")
end

IncludeLibrary("base")
CameraSetLimits(15, 6, 5, 0.4)
local ui = UI:Get():NewPanel("Panel1", 0, 0, 640, 640)
ui:NewStaticText("Label1", 32, 0, 20, 200, "Availible units")
ui:NewStaticText("Label2", 242, 0, 20, 200, "Space Marines")
ui:NewStaticText("Label3", 452, 0, 20, 200, "Your roster")
ui:NewStaticText("Label4", 242, 50, 20, 200, "Points: 0/200")
local list = ui:NewList("List1", 2, 32, 230, 200)
list:AddItem("Tactical Marine(bolter)")
list:AddItem("Tactical Marine(meltagun)")
list:AddItem("Tactical Marine(heavy bolter)")
list:AddItem("Assault Marine")
list:AddItem("Terminator(storm bolter)")
list:AddItem("Terminator(lightning claws)")
ui:NewList("List2", 398, 32, 230, 200)
ui:NewButton("Button1", 210, 100, 30, 180, "Add>", "AddItem1")
ui:NewButton("Button2", 210, 140, 30, 180, "<Delete", "DeleteItem1")
ui:NewStaticText("Label5", 32, 335, 20, 200, "Availible units")
ui:NewStaticText("Label6", 277, 335, 20, 200, "Chaos")
ui:NewStaticText("Label7", 452, 335, 20, 200, "Your roster")
ui:NewStaticText("Label8", 242, 385, 20, 200, "Points: 0/200")
list = ui:NewList("List3", 2, 367, 230, 200)
list:AddItem("Chaos Marine(bolter)")
list:AddItem("Chaos Marine(meltagun)")
list:AddItem("Chaos Marine(heavy bolter)")
list:AddItem("Raptor")
list:AddItem("Chaos Terminator(combi bolter)")
list:AddItem("Chaos Terminator(lightning claws)")
ui:NewList("List4", 398, 367, 230, 200)
ui:NewButton("Button3", 210, 435, 30, 180, "Add>", "AddItem2")
ui:NewButton("Button4", 210, 475, 30, 180, "<Delete", "DeleteItem2")
ui:NewButton("Button5", 210, 280, 40, 200, "Battle", "Battle")