Player1 = {}
Player2 = {}
Player1Count = 10
Player2Count = 10
hunter = nil

function SetInitParamters(unit)
	unit:SetProperty("WS", "4")
	unit:SetProperty("BS", "4")
	unit:SetProperty("S", "4")
	unit:SetProperty("T", "4")
	unit:SetProperty("Attacks", "1")
	unit:SetProperty("Sv", "3")
	unit:SetProperty("InvSv", "3")
	unit:SetProperty("MeleeAP", "7")
	unit:SetProperty("WeaponRange", "24")
	unit:SetProperty("WeaponS", "4")
	unit:SetProperty("WeaponAP", "5")
	unit:SetProperty("WeaponType", "RapidFire")
	unit:SetProperty("WeaponShots", "2")
	unit:SetSelectable(true)
end

function Init()
	for i = 1, 10 do
		Player1[i] = Object:New("SpaceMarine.obj", 20 + (i - 1) / 2.5, (i - 1) % 5 * 2 - 5, -90)
		Player1[i]:SetProperty("Name", "Tactical Space Marine with bolter")
		Player1[i]:SetProperty("Owner", "1")
		SetInitParamters(Player1[i])
		Player1[i]:SetMoveLimit("rectangle", 15, 15, 30, -15)
		Player2[i] = Object:New("CSM.obj", -20 - (i - 1) / 2.5, (i - 1) % 5 * 2 - 5, 90)
		Player2[i]:SetProperty("Name", "Chaos Space Marine with bolter")
		Player2[i]:SetProperty("Owner", "2")
		SetInitParamters(Player2[i])
		Player2[i]:SetMoveLimit("rectangle", -15, 15, -30, -15)
	end
	SetGlobalProperty("Turn", 0)
	SetGlobalProperty("Player", "2")
	SetGlobalProperty("Phase", "NULL")
end

function IsLockedInCombat(unit)
	local owner = unit:GetProperty("Owner")
	local x = unit:GetX()
	local y = unit:GetY()
	for i=1, 10 do
		if(owner == "1") then
			if(math.sqrt((x - Player2[i]:GetX()) * (x - Player2[i]:GetX()) + (y - Player2[i]:GetY()) * (y - Player2[i]:GetY())) < 3) then
				return true
			end
		else
			if(math.sqrt((x - Player1[i]:GetX()) * (x - Player1[i]:GetX()) + (y - Player1[i]:GetY()) * (y - Player1[i]:GetY())) < 3) then
				return true
			end
		end
	end
	return false
end

function MovePhase(player)
	if(player == "1") then
		for i = 1, 10 do
			Player1[i]:SetSelectable(true)
			if(not IsLockedInCombat(Player1[i])) then
				Player1[i]:SetMoveLimit("circle", Player1[i]:GetX(), Player1[i]:GetY(), 6)
			end
		end
	else
		for i = 1, 10 do
			Player2[i]:SetSelectable(true)
			if(not IsLockedInCombat(Player2[i])) then
				Player2[i]:SetMoveLimit("circle", Player2[i]:GetX(), Player2[i]:GetY(), 6)
			end
		end
	end
end

function ShootingPhase(player)
	UI:Get():GetChild("Button5"):SetVisible(true)
	UI:Get():GetChild("Button6"):SetVisible(true)
	if(player == "1") then
		for i = 1, 10 do
			Player1[i]:SetSelectable(true)
			Player1[i]:SetMoveLimit("static")
			Player1[i]:SetProperty("Shooted", "0")
		end
	else
		for i = 1, 10 do
			Player2[i]:SetSelectable(true)
			Player2[i]:SetMoveLimit("static")
			Player2[i]:SetProperty("Shooted", "0")
		end
	end
end

function MeleePhase(player)
	if(player == "1") then
		for i = 1, 10 do
			Player1[i]:SetProperty("StrikedAtMelee", "0")
			Player1[i]:SetMoveLimit("static")
		end
	else
		for i = 1, 10 do
			Player2[i]:SetProperty("StrikedAtMelee", "0")
			Player2[i]:SetMoveLimit("static")
		end
	end
end

function EndTurn()
	for i = 1, 10 do
		Player1[i]:SetSelectable(false)
		Player1[i]:SetMoveLimit("static")
		Player2[i]:SetSelectable(false)
		Player2[i]:SetMoveLimit("static")
	end
end

function EndPhase()
	hunter = nil
	if(GetGlobalProperty("Phase") == "Move") then
		SetGlobalProperty("Phase", "Shooting")
		ShootingPhase(GetGlobalProperty("Player"))
	elseif(GetGlobalProperty("Phase") == "Shooting") then
		SetGlobalProperty("Phase", "Melee")
		MeleePhase(GetGlobalProperty("Player"))
	else
		EndTurn()
		SetGlobalProperty("Phase", "Move")
		if(GetGlobalProperty("Player") == "1") then
			SetGlobalProperty("Player", "2")
		else
			SetGlobalProperty("Player", "1")
			SetGlobalProperty("Turn", GetGlobalProperty("Turn") + 1)
		end
		MovePhase(GetGlobalProperty("Player"))
	end
	UI:Get():GetChild("Label1"):SetText("Turn " .. GetGlobalProperty("Turn") .. " Player" .. GetGlobalProperty("Player") .. " " .. GetGlobalProperty("Phase") .. " Phase ")
	OnSelection()
end

function CancelEnemySelection()
	hunter = nil;
	local owner = hunter:GetProperty("Owner")
	for i=1, 10 do
		if(owner == "1") then
			Player2[i]:SetSelectable(false)
		else
			Player1[i]:SetSelectable(false)
		end
	end
end

function GetToWound(S, T)
	local result = 4 + T - S
	if(result < 2) then
		result = 2
	end
	if(result == 7) then
		result = 6
	end
	return result
end

function Fire()
	hunter = Object:GetSelected()
	if(hunter:Null()) then
		MessageBox("Choose one of your models to shoot")
		hunter = nil
		return
	end
	if(hunter:GetProperty("Shooted") ~= "0") then
		MessageBox("This unit already shooted or runned")
		hunter = nil
		return
	end
	if(IsLockedInCombat(hunter)) then
		MessageBox("This unit is locked in close combat and cannot shoot")
		hunter = nil
		return
	end
	MessageBox("Choose an enemy to shoot at or choose an ally model to cancel shooting")
	local owner = hunter:GetProperty("Owner")
	for i=1, 10 do
		if(owner == "1") then
			Player2[i]:SetSelectable(true)
		else
			Player1[i]:SetSelectable(true)
		end
	end
end

function Fire2()
	local prey = Object:GetSelected()
	if(prey:GetProperty("Owner") == hunter:GetProperty("Owner")) then
		MessageBox("Cannot shoot at ally models. Cancelling shooting")
		CancelEnemySelection()
		return
	end
	if(IsLockedInCombat(prey)) then
		MessageBox("Cannot shoot at enemy locked in melee combat")
		return
	end
	local range = math.sqrt((hunter:GetX() - prey:GetX()) * (hunter:GetX() - prey:GetX()) + (hunter:GetY() - prey:GetY()) * (hunter:GetY() - prey:GetY()))
	local weaponRange = 0 + hunter:GetProperty("WeaponRange")
	if(range > weaponRange) then
		MessageBox("Target is out of range of this weapon(range=" .. range .. "; weaponRange=" .. weaponRange .. ")")
		return
	end
	local numShots = 0 + hunter:GetProperty("WeaponShots")
	if((hunter:GetProperty("WeaponType") == "RapidFire") and (range > (weaponRange / 2))) then
		numShots = numShots / 2
	end
	math.randomseed(os.time())
	local toHit = 7 - hunter:GetProperty("BS")
	local toWound = GetToWound(hunter:GetProperty("WeaponS"), prey:GetProperty("T"))
	local save = 0 + prey:GetProperty("Sv")
	if(0 + hunter:GetProperty("WeaponAP") <= save) then
		save = 7
	end
	if(0 + prey:GetProperty("InvSv") < save) then
		save = 0 + prey:GetProperty("InvSv")
	end
	local result = "Rolls to hit (".. numShots .. " dice, " .. toHit .. "+): "
	local hits = 0
	local wounds = 0
	local unsaved = 0
	for i = 1, numShots do
		local rand = math.random(1, 6)
		result = result .. rand .. " "
		if(rand >= toHit) then
			hits = hits + 1
		end
	end
	result = result .. "\nRolls to wound (" .. hits .. " dice, " .. toWound .. "+): "
	for i = 1, hits do
		local rand = math.random(1, 6)
		if(rand >= toWound) then
			wounds = wounds + 1
		end
		result = result .. rand .. " "
	end
	if(save < 7) then
		result = result .. "\nRolls saves (" .. wounds .. " dice, " .. save .. "+): "
		for i = 1, wounds do
			local rand = math.random(1, 6)
			if(rand < save) then
				unsaved = unsaved + 1
			end
			result = result .. rand .. " "
		end
	else
		result = result .. "\n Saves are ignored by weapon AP"
	end
	result = result .. "\nTotal " .. unsaved .. " unsaved wounds. "
	if(unsaved > 0) then
		result = result .. "Enemy dies"
		prey:Delete()
	else
		result = result .. "Enemy still alive"
	end
	hunter:SetProperty("Shooted", "1")
	Object:SelectNull()
	MessageBox(result)
	CancelEnemySelection()
end

function Strike()
	hunter = Object:GetSelected()
	if(hunter:Null()) then
		MessageBox("Choose one of your models to strike")
		hunter = nil
		return
	end
	if(hunter:GetProperty("StrikedAtMelee") ~= "0") then
		MessageBox("This unit already striked")
		hunter = nil
		return
	end
	if(not IsLockedInCombat(hunter)) then
		MessageBox("Only models locked in close combat can strike")
		hunter = nil
		return
	end
	MessageBox("Choose an enemy to strike at or choose an ally model to cancel")
	local owner = hunter:GetProperty("Owner")
	for i=1, 10 do
		if(owner == "1") then
			Player2[i]:SetSelectable(true)
		else
			Player1[i]:SetSelectable(true)
		end
	end
end

function Strike2()
	local prey = Object:GetSelected()
	if(prey:GetProperty("Owner") == hunter:GetProperty("Owner")) then
		MessageBox("Cannot strike at ally models. Cancelling striking")
		CancelEnemySelection()
		return
	end
	local range = math.sqrt((hunter:GetX() - prey:GetX()) * (hunter:GetX() - prey:GetX()) + (hunter:GetY() - prey:GetY()) * (hunter:GetY() - prey:GetY()))
	if(range > 3) then
		MessageBox("Target is not in base contact with striker(range=" .. range .. "; baseContact=3)")
		return
	end
	local numStrikes = 0 + hunter:GetProperty("Attacks")
	math.randomseed(os.time())
	local toHit = 8 - hunter:GetProperty("WS")
	local toWound = GetToWound(hunter:GetProperty("S"), prey:GetProperty("T"))
	local save = 0 + prey:GetProperty("Sv")
	if(0 + hunter:GetProperty("MeleeAP") <= save) then
		save = 7
	end
	if(0 + prey:GetProperty("InvSv") < save) then
		save = 0 + prey:GetProperty("InvSv")
	end
	local result = "Rolls to hit (".. numStrikes .. " dice, " .. toHit .. "+): "
	local hits = 0
	local wounds = 0
	local unsaved = 0
	for i = 1, numStrikes do
		local rand = math.random(1, 6)
		result = result .. rand .. " "
		if(rand >= toHit) then
			hits = hits + 1
		end
	end
	result = result .. "\nRolls to wound (" .. hits .. " dice, " .. toWound .. "+): "
	for i = 1, hits do
		local rand = math.random(1, 6)
		if(rand >= toWound) then
			wounds = wounds + 1
		end
		result = result .. rand .. " "
	end
	if(save < 7) then
		result = result .. "\nRolls saves (" .. wounds .. " dice, " .. save .. "+): "
		for i = 1, wounds do
			local rand = math.random(1, 6)
			if(rand < save) then
				unsaved = unsaved + 1
			end
			result = result .. rand .. " "
		end
	else
		result = result .. "\n Saves are ignored by melee AP"
	end
	result = result .. "\nTotal " .. unsaved .. " unsaved wounds. "
	if(unsaved > 0) then
		result = result .. "Enemy dies"
		prey:Delete()
	else
		result = result .. "Enemy still alive"
	end
	hunter:SetProperty("StrikedAtMelee", "1")
	Object:SelectNull()
	MessageBox(result)
	CancelEnemySelection()
end

function OnSelection()
	local selected = Object:GetSelected()
	if(selected:Null()) then
		UI:Get():GetChild("Label2"):SetText("")
		return
	else
		UI:Get():GetChild("Label2"):SetText(selected:GetProperty("Name"))
	end
	UI:Get():GetChild("Button5"):SetVisible(false)
	UI:Get():GetChild("Button6"):SetVisible(false)
	UI:Get():GetChild("Button7"):SetVisible(false)
	if(selected:GetProperty("Owner") == GetGlobalProperty("Player")) then
		if(GetGlobalProperty("Phase") == "Shooting" and selected:GetProperty("Shooted") == "0") then
			UI:Get():GetChild("Button5"):SetVisible(true)
			UI:Get():GetChild("Button6"):SetVisible(true)
		elseif (GetGlobalProperty("Phase") == "Melee" and selected:GetProperty("StrikedAtMelee") == "0") then
			UI:Get():GetChild("Button7"):SetVisible(true)
		end
	end
	if(hunter == nil) then
		return
	end
	if(GetGlobalProperty("Phase") == "Shooting") then
		Fire2()
	elseif(GetGlobalProperty("Phase") == "Melee") then
		Strike2()
	end
end

function Run()
	local object = Object:GetSelected()
	if(object:Null()) then
		MessageBox("Choose one of your models to run")
		return
	end
	if(object:GetProperty("Shooted") ~= "0") then
		MessageBox("This unit already shooted or runned")
		return
	end
	if(IsLockedInCombat(object)) then
		MessageBox("This unit is locked in close combat and cannot run")
		return
	end
	math.randomseed(os.time())
	local rand = math.random(1, 6)
	MessageBox("Unit rolls " .. rand .. " to run")
	object:SetMoveLimit("circle", object:GetX(), object:GetY(), rand)
	object:SetProperty("Shooted", "1")
end

function SetRuler()
	Ruler()
end

function UndoAction()
	Undo()
end

function RedoAction()
	Redo()
end

IncludeLibrary("math")
IncludeLibrary("os")
CreateSkybox(80, "skybox")
CreateTable(60, 30, "sand.bmp")
CameraSetLimits(30, 12, 5, 0.2)
local ui = UI:New()
ui:Set()
ui:NewButton("Button1", 10, 10, 30, 90, "End phase", "EndPhase")
ui:NewButton("Button2", 110, 10, 30, 80, "Ruler", "SetRuler")
ui:NewButton("Button3", 200, 10, 30, 80, "Undo", "UndoAction")
ui:NewButton("Button4", 290, 10, 30, 80, "Redo", "RedoAction")
ui:NewButton("Button5", 380, 10, 30, 80, "Shoot", "Fire"):SetVisible(false)
ui:NewButton("Button6", 470, 10, 30, 80, "Run", "Run"):SetVisible(false)
ui:NewButton("Button7", 380, 10, 30, 80, "Strike", "Strike"):SetVisible(false)
ui:NewStaticText("Label1", 10, 40, 30, 180, "Deployment Phase")
ui:NewStaticText("Label2", 10, 570, 30, 200, "")
Init()
SetSelectionCallback("OnSelection")