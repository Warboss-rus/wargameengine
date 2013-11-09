Player1 = {}
Player2 = {}
Player1Count = 10
Player2Count = 10
hunter = nil

function Init()
	for i = 1, 10 do
		Player1[i] = Object:New("SpaceMarine.obj", 20 + (i - 1) / 2.5, (i - 1) % 5 * 2 - 5, -90)
		Player1[i]:SetProperty("Owner", "1")
		Player1[i]:SetProperty("BS", "4")
		Player1[i]:SetProperty("Sv", "3")
		Player1[i]:SetProperty("WeaponType", "RapidFire")
		Player1[i]:SetProperty("WeaponRange", "24")
		Player1[i]:SetProperty("WeaponShots", "2")
		Player1[i]:SetSelectable(true)
		Player1[i]:SetMoveLimit("rectangle", 15, 15, 30, -15)
		Player2[i] = Object:New("CSM.obj", -20 - (i - 1) / 2.5, (i - 1) % 5 * 2 - 5, 90)
		Player2[i]:SetProperty("Owner", "2")
		Player2[i]:SetProperty("BS", "4")
		Player2[i]:SetProperty("Sv", "3")
		Player2[i]:SetProperty("WeaponType", "RapidFire")
		Player2[i]:SetProperty("WeaponRange", "24")
		Player2[i]:SetProperty("WeaponShots", "2")
		Player2[i]:SetSelectable(true)
		Player2[i]:SetMoveLimit("rectangle", -15, 15, -30, -15)
	end
	SetGlobalProperty("Turn", 0)
	SetGlobalProperty("Player", "2")
	SetGlobalProperty("Phase", "NULL")
end

function MovePhase(player)
	if(player == "1") then
		for i = 1, 10 do
			Player1[i]:SetSelectable(true)
			Player1[i]:SetMoveLimit("circle", Player1[i]:GetX(), Player1[i]:GetY(), 6)
		end
	else
		for i = 1, 10 do
			Player2[i]:SetSelectable(true)
			Player2[i]:SetMoveLimit("circle", Player2[i]:GetX(), Player2[i]:GetY(), 6)
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

function EndTurn()
	UI:Get():GetChild("Button5"):SetVisible(false)
	UI:Get():GetChild("Button6"):SetVisible(false)
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
end

function Fire()
	hunter = Object:GetSelected()
	if(hunter:Null()) then
		MessageBox("Choose one of your models to shoot")
		hunter = nil;
		return
	end
	if(hunter:GetProperty("Shooted") ~= "0") then
		MessageBox("This unit already shooted or runned")
		hunter = nil;
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
	SetSelectionCallback("Fire2")
end

function Fire2()
	if(hunter == nil) then
		SetSelectionCallback("")
		return;
	end
	local prey = Object:GetSelected()
	if(prey:Null()) then
		return;
	end
	if(prey:GetProperty("Owner") == hunter:GetProperty("Owner")) then
		MessageBox("Cannot shoot to ally models. Cancelling shooting")
		hunter = nil;
		SetSelectionCallback("")
		return
	end
	local range = math.sqrt((hunter:GetX() - prey:GetX()) * (hunter:GetX() - prey:GetX()) + (hunter:GetY() - prey:GetY()) * (hunter:GetY() - prey:GetY()))
	local weaponRange = 0 + hunter:GetProperty("WeaponRange")
	if(range > weaponRange) then
		MessageBox("Target is out of range of this weapon(range=" .. range .. "; weaponRange=" .. weaponRange .. ")")
		hunter = nil;
		SetSelectionCallback("")
		return
	end
	local numShots = hunter:GetProperty("WeaponShots")
	if((hunter:GetProperty("WeaponType") == "RapidFire") and (range > (weaponRange / 2))) then
		numShots = numShots / 2
	end
	math.randomseed(os.time())
	local toHit = 7 - hunter:GetProperty("BS")
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
	result = result .. "\nRolls to wound (" .. hits .. " dice, 4+): "
	for i = 1, hits do
		local rand = math.random(1, 6)
		if(rand >= 4) then
			wounds = wounds + 1
		end
		result = result .. rand .. " "
	end
	local save = 0 + prey:GetProperty("Sv")
	result = result .. "\nRolls saves (" .. wounds .. " dice, " .. save .. "+): "
	for i = 1, wounds do
		local rand = math.random(1, 6)
		if(rand < save) then
			unsaved = unsaved + 1
		end
		result = result .. rand .. " "
	end
	result = result .. "\nTotal " .. unsaved .. " unsaved wounds. "
	if(unsaved > 0) then
		result = result .. "Enemy dies"
		prey:Delete()
	else
		result = result .. "Enemy still alive"
	end
	hunter:SetProperty("Shooted", "1")
	SetSelectionCallback("")
	Object:SelectNull()
	MessageBox(result)
	local owner = hunter:GetProperty("Owner")
	for i=1, 10 do
		if(owner == "1") then
			Player2[i]:SetSelectable(false)
		else
			Player1[i]:SetSelectable(false)
		end
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
ui:NewStaticText("Label1", 10, 40, 30, 180, "Deployment Phase")
Init()