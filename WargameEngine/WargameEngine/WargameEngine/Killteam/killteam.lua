Player1 = {}
Player2 = {}
hunter = nil
hunterGroup = nil
hunterGroupIndex = nil

function Init()
	--math.randomseed(os.time())
	Viewport:CameraStrategy(30, 12, 5, 0.3)
	AddLight()
	SetLightPosition(1, 0, 0, 50)
	SetLightDiffuse(1, 1, 1, 1, 1)
	SetLightAmbient(1, 0.5, 0.5, 0.5, 1)
	SetLightSpecular(1, 1, 1, 1, 1)
	Viewport:SetShadowMapViewport(Viewport:CreateShadowMapViewport(4096, 65, 0, 0, 50))
	if(GetRendererName() == "DirectX11") then
		SetShaders("directX11/per_pixel.hlsl", "directX11/per_pixel.hlsl")
	elseif(GetRendererName() == "OpenGLES") then
		SetShaders("GLES/per_pixel_shadow.vsh", "GLES/per_pixel_shadow.fsh")
		UI:SetScale(2)
		Viewport:EnableTouchMode()
	else
		SetShaders("openGL/per_pixel_shadow.vsh", "openGL/per_pixel_shadow.fsh")
	end
	SetSelectionCallback("OnSelection")
	UI:NewButton("ButtonEndPhase", 10, 10, 30, 90, "End phase", "EndPhase")
	UI:NewButton("ButtonRuler", 110, 10, 30, 80, "Ruler", "SetRuler")
	UI:NewButton("ButtonUndo", 200, 10, 30, 80, "Undo", "UndoAction")
	UI:NewButton("ButtonRedo", 290, 10, 30, 80, "Redo", "RedoAction")
	UI:NewButton("ButtonShoot", 380, 10, 30, 80, "Shoot", "Fire"):SetVisible(false)
	UI:NewButton("ButtonRun", 470, 10, 30, 80, "Run", "Run"):SetVisible(false)
	UI:NewButton("ButtonStrike", 380, 10, 30, 80, "Strike", "Strike"):SetVisible(false)
	UI:NewButton("ButtonSave", 380, 50, 30, 80, "Save", "Save")
	UI:NewButton("ButtonLoad", 470, 50, 30, 80, "Load", "Load")
	UI:NewStaticText("LabelPhase", 10, 40, 30, 180, "Deployment Phase")
	UI:NewStaticText("LabelUnit", 10, 570, 30, 200, "")
	SetGlobalProperty("Turn", 0)
	SetGlobalProperty("Player", "2")
	SetGlobalProperty("Phase", "NULL")
	SetOnStateRecievedCallback("OnStateRecieved")
end

function IsLockedInCombat(unit)
	local owner = unit:GetProperty("Owner")
	local x = unit:GetX()
	local y = unit:GetY()
	if(owner == "1") then
		for i=1, #Player2 do
			if(math.sqrt((x - Player2[i]:GetX()) * (x - Player2[i]:GetX()) + (y - Player2[i]:GetY()) * (y - Player2[i]:GetY())) < 3) then
				return true
			end
		end
	else
		for i=1, #Player1 do
			if(math.sqrt((x - Player1[i]:GetX()) * (x - Player1[i]:GetX()) + (y - Player1[i]:GetY()) * (y - Player1[i]:GetY())) < 3) then
				return true
			end
		end
	end
	return false
end

function MovePhase(player)
	if(player == "1") then
		for i=1, #Player1 do
			Player1[i]:SetSelectable(true)
			Player1[i]:SetProperty("MovementX", Player1[i]:GetX())
			Player1[i]:SetProperty("MovementY", Player1[i]:GetY())
			if(not IsLockedInCombat(Player1[i])) then
				local race = races[Player1[i]:GetProperty("Race")]
				local unit = race.units[Player1[i]:GetProperty("Name")]
				Player1[i]:SetMoveLimit("circle", Player1[i]:GetX(), Player1[i]:GetY(), unit.MovementSpeed)
			end
		end
	else
		for i=1, #Player2 do
			Player2[i]:SetSelectable(true)
			Player2[i]:SetProperty("MovementX", Player2[i]:GetX())
			Player2[i]:SetProperty("MovementY", Player2[i]:GetY())
			if(not IsLockedInCombat(Player2[i])) then
				local race = races[Player2[i]:GetProperty("Race")]
				local unit = race.units[Player2[i]:GetProperty("Name")]
				Player2[i]:SetMoveLimit("circle", Player2[i]:GetX(), Player2[i]:GetY(), unit.MovementSpeed)
			end
		end
	end
end

function ShootingPhase(player)
	UI:GetChild("ButtonShoot"):SetVisible(true)
	UI:GetChild("ButtonRun"):SetVisible(true)
	if(player == "1") then
		for i=1, #Player1 do
			Player1[i]:SetSelectable(true)
			Player1[i]:SetMoveLimit("static")
			Player1[i]:SetProperty("Shooted", "0")
			
		end
	else
		for i=1, #Player2 do
			Player2[i]:SetSelectable(true)
			Player2[i]:SetMoveLimit("static")
			Player2[i]:SetProperty("Shooted", "0")
		end
	end
end

function MeleePhase(player)
	if(player == "1") then
		for i=1, #Player1 do
			Player1[i]:SetProperty("StrikedAtMelee", "0")
			Player1[i]:SetMoveLimit("static")
		end
	else
		for i=1, #Player2 do
			Player2[i]:SetProperty("StrikedAtMelee", "0")
			Player2[i]:SetMoveLimit("static")
		end
	end
end

function EndTurn()
	Object:Select(nil)
	for i=1, #Player1 do
		Player1[i]:SetSelectable(false)
		Player1[i]:SetMoveLimit("static")
	end
	for i=1, #Player2 do
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
	UI:GetChild("LabelPhase"):SetText("Turn " .. GetGlobalProperty("Turn") .. " Player" .. GetGlobalProperty("Player") .. " " .. GetGlobalProperty("Phase") .. " Phase ")
	OnSelection()
end

function CancelEnemySelection()
	if(hunter == nil) then
		return
	end
	if(hunter:GetProperty("Owner") == "1") then
		for i=1, #Player2 do
			Player2[i]:SetSelectable(false)
		end
	else
		for i=1, #Player1 do
			Player1[i]:SetSelectable(false)
		end
	end
	hunter = nil
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

function CheckEndGame()
	if(#Player2 == 0) then
		MessageBox("Player1 wins")
	elseif(#Player1 == 0) then
		MessageBox("Player2 wins")
	end
end

function RemoveObject(prey)
	if(prey:GetProperty("Owner") == "2") then
		for i=1, #Player2 do
			if(Player2[i]:Equals(prey)) then
				table.remove(Player2, i)
				prey:Delete()
				CheckEndGame()
				return
			end
		end
	else
		for i=1, #Player1 do
			if(Player1[i]:Equals(prey)) then
				table.remove(Player1, i)
				prey:Delete()
				CheckEndGame()
				return
			end
		end
	end
	prey:Delete()
end

function NextHunter()
	if(hunterGroup == nil) then
		CancelEnemySelection()
		return
	end
	hunterGroupIndex = hunterGroupIndex + 1
	if(hunterGroupIndex <= #hunterGroup) then--next hunter in group
		hunter = hunterGroup[hunterGroupIndex]
	else
		hunterGroup = nil
		hunterGroupIndex = nil
		CancelEnemySelection()
	end
end

function Fire()
	hunter = Object:GetSelected()
	if(hunter== nil) then
		MessageBox("Choose one of your models to shoot")
		hunter = nil
		return
	end
	if(hunter:IsGroup()) then
		hunterGroup = {}
		for i=1, hunter:GetGroupChildrenCount() do
			hunterGroup[i] = hunter:GetGroupChildrenAt(i)
		end
		hunterGroupIndex = 1
		hunter = hunterGroup[hunterGroupIndex]
	else
		if(IsLockedInCombat(hunter)) then
			MessageBox("This unit is locked in close combat and cannot shoot")
			hunter = nil
			return
		end
	end
	MessageBox("Choose an enemy to shoot at or choose an ally model to cancel shooting")
	local owner = hunter:GetProperty("Owner")
	if(owner == "1") then
		for i=1, #Player2 do
			Player2[i]:SetSelectable(true)
		end
	else
		for i=1, #Player1 do
			Player1[i]:SetSelectable(true)
		end
	end
end

function Fire2(prey)
	if(prey:GetProperty("Owner") == hunter:GetProperty("Owner")) then
		MessageBox("Cannot shoot at ally models. Cancelling shooting")
		CancelEnemySelection()
		return
	end
	if(hunter:GetProperty("Shooted") ~= "0") then
		MessageBox("This unit had already shooted or runned")
		NextHunter()
		return
	end
	if(IsLockedInCombat(hunter)) then
		MessageBox("This unit is locked in close combat and cannot shoot")
		NextHunter()
		return
	end
	if(IsLockedInCombat(prey)) then
		MessageBox("Cannot shoot at enemy locked in melee combat")
		return
	end
	local army = races[hunter:GetProperty("Race")]
	local weapon = army.weapons[hunter:GetProperty("RangedWeapon")]
	local range = math.sqrt((hunter:GetX() - prey:GetX()) * (hunter:GetX() - prey:GetX()) + (hunter:GetY() - prey:GetY()) * (hunter:GetY() - prey:GetY()))
	local weaponRange = weapon.Range
	if(range > weaponRange) then
		MessageBox("Target is out of range of this weapon(range=" .. range .. "; weaponRange=" .. weaponRange .. ")")
		return
	end
	local numShots = weapon.Shots
	if((weapon.Type == "RapidFire") and (range > (weaponRange / 2))) then
		numShots = numShots / 2
	end
	local toHit = 7 - army.units[hunter:GetProperty("Name")].BS
	if(toHit < 2) then
		toHit = 2
	end
	local los = LoS(hunter, prey)
	if(los < 75) then
		toHit = toHit + 1
	end
	if(los < 50) then
		toHit = toHit + 1
	end
	if(los < 25) then
		toHit = toHit + 1
	end
	if(los < 10) then
		MessageBox("Can't see the target(visibility=" .. los .."%)")
		return
	end
	local angle = math.deg(math.atan(prey:GetX() - hunter:GetX(), prey:GetY() - hunter:GetY()))
	angle = math.rad((hunter:GetRotation() - angle) / 2)
	toHit = toHit + math.floor(4 * (1 - math.abs(math.cos(angle))))
	if(toHit > 6) then
		toHit = 6
	end
	if(weapon.Type == "Heavy") then
		local deltaX = hunter:GetProperty("MovementX") - hunter:GetX()
		local deltaY = hunter:GetProperty("MovementY") - hunter:GetY()
		if((math.sqrt(deltaX * deltaX + deltaY * deltaY) > 0.25)) then
			toHit = 6
		end
	end
	local preyArmy = races[prey:GetProperty("Race")]
	local preyUnit = preyArmy.units[prey:GetProperty("Name")]
	local toWound = GetToWound(weapon.S, preyUnit.T)
	local save = preyUnit.Sv
	if(weapon.AP <= save) then
		save = 7
	end
	if(preyUnit.InvSv ~= nil and preyUnit.InvSv < save) then
		save = preyUnit.InvSv
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
		unsaved = wounds
	end
	
	result = result .. "\nTotal " .. unsaved .. " unsaved wounds. "
	if(unsaved > 0) then
		result = result .. "Enemy dies"
		RemoveObject(prey)
	else
		result = result .. "Enemy still alive"
	end
	hunter:SetProperty("Shooted", "1")
	MessageBox(result)
	if(hunterGroupIndex ~= nil) then
		NextHunter()
		if(hunter ~= nil) then
			if(unsaved == 0) then
				Object:Select(nil)
				Fire2(prey)
			else
				MessageBox("Select next target")
			end
		end
		return
	end
	CancelEnemySelection()
	Object:Select(nil)
	OnSelection()
end

function Strike()
	hunter = Object:GetSelected()
	if(hunter== nil) then
		MessageBox("Choose one of your models to strike")
		hunter = nil
		return
	end
	if(hunter:IsGroup()) then
		hunterGroup = {}
		for i=1, hunter:GetGroupChildrenCount() do
			hunterGroup[i] = hunter:GetGroupChildrenAt(i)
		end
		hunterGroupIndex = 1
		hunter = hunterGroup[hunterGroupIndex]
	else
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

function Strike2(prey)
	if(prey:GetProperty("Owner") == hunter:GetProperty("Owner")) then
		MessageBox("Cannot strike at ally models. Cancelling striking")
		CancelEnemySelection()
		return
	end
	if(hunter:GetProperty("StrikedAtMelee") ~= "0") then
		MessageBox("This unit already striked")
		NextHunter()
		return
	end
	if(not IsLockedInCombat(hunter)) then
		MessageBox("Only models locked in close combat can strike")
		NextHunter()
		return
	end
	local range = math.sqrt((hunter:GetX() - prey:GetX()) * (hunter:GetX() - prey:GetX()) + (hunter:GetY() - prey:GetY()) * (hunter:GetY() - prey:GetY()))
	if(range >= 3) then
		MessageBox("Target is not in base contact with striker(range=" .. range .. "; baseContact=3)")
		return
	end
	local army = races[hunter:GetProperty("Race")]
	local weapon = army.weapons[hunter:GetProperty("MeleeWeapon")]
	local unit = army.units[hunter:GetProperty("Name")]
	local numStrikes = unit.Attacks + weapon.AdditionalAttacks
	local toHit = 8 - unit.WS
	if(toHit < 2) then
		toHit = 2
	elseif(toHit > 6) then
		toHit = 6
	end
	local preyArmy = races[prey:GetProperty("Race")]
	local preyUnit = preyArmy.units[prey:GetProperty("Name")]
	local toWound = GetToWound(weapon.S, preyUnit.T)
	local save = preyUnit.Sv
	if(weapon.AP <= save) then
		save = 7
	end
	if(preyUnit.InvSv ~= nil and preyUnit.InvSv < save) then
		save = preyUnit.InvSv
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
	if(weapon.RerollFailed2Wound == true) then
		result = result .. "\nRerolls failed to wound(" .. hits - wounds .. " dice, " .. toWound .. "+): "
		for i = 1, hits - wounds do
			local rand = math.random(1, 6)
			if(rand >= toWound) then
				wounds = wounds + 1
			end
			result = result .. rand .. " "
		end
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
		unsaved = wounds
	end
	result = result .. "\nTotal " .. unsaved .. " unsaved wounds. "
	if(unsaved > 0) then
		result = result .. "Enemy dies"
		RemoveObject(prey)
	else
		result = result .. "Enemy still alive"
	end
	hunter:SetProperty("StrikedAtMelee", "1")
	MessageBox(result)
	if(hunterGroupIndex ~= nil) then
		NextHunter()
		if(hunter ~= nil) then
			if(unsaved == 0) then
				Object:Select(nil)
				Strike2(prey)
				return
			else
				MessageBox("Select next target")
			end
		end
		return
	end
	CancelEnemySelection()
	Object:Select(nil)
	OnSelection()
end

function OnSelection()
	UI:GetChild("ButtonShoot"):SetVisible(false)
	UI:GetChild("ButtonRun"):SetVisible(false)
	UI:GetChild("ButtonStrike"):SetVisible(false)
	local selected = Object:GetSelected()
	if(selected == nil) then
		UI:GetChild("LabelUnit"):SetText("")
		return
	else
		UI:GetChild("LabelUnit"):SetText(selected:GetProperty("Name"))
	end
	if(selected:GetProperty("Owner") == GetGlobalProperty("Player")) then
		if(GetGlobalProperty("Phase") == "Shooting" and selected:GetProperty("Shooted") == "0") then
			UI:GetChild("ButtonShoot"):SetVisible(true)
			UI:GetChild("ButtonRun"):SetVisible(true)
		elseif (GetGlobalProperty("Phase") == "Melee" and selected:GetProperty("StrikedAtMelee") == "0") then
			UI:GetChild("ButtonStrike"):SetVisible(true)
		end
	end
	if(hunter == nil or selected:IsGroup()) then
		return
	end
	if(GetGlobalProperty("Phase") == "Shooting") then
		Fire2(selected)
	elseif(GetGlobalProperty("Phase") == "Melee") then
		Strike2(selected)
	end
end

function Run()
	local object = Object:GetSelected()
	if(object== nil) then
		MessageBox("Choose one of your models to run")
		return
	end
	if(object:GetProperty("Shooted") ~= "0") then
		MessageBox("This unit already shooted or runned")
		return
	end
	CancelEnemySelection()
	if(object:IsGroup()) then
		for i=1, object:GetGroupChildrenCount() do
			RunObject(object:GetGroupChildrenAt(i))
		end
		return
	end
	RunObject(object)
end

function RunObject(object)
	if(IsLockedInCombat(object)) then
		MessageBox("This unit is locked in close combat and cannot run")
		return
	end
	object:SetProperty("Shooted", "1")
	local rand = math.random(1, 6)
	MessageBox(object:GetProperty("Name") .. " rolls " .. rand .. " to run")
	object:SetMoveLimit("circle", object:GetX(), object:GetY(), rand)
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

function Split(str)
	local result = {}
	for i in string.gmatch(str, "%S+") do
		result[#result+1] = i
	end
	return result
end

function LoadMap(filename)
	for line in io.lines(filename) do
		local splitted = Split(line)
		if(splitted[1] == "object") then
			NewStaticObject(splitted[2], splitted[3], splitted[4], splitted[5])
		elseif(splitted[1] == "decal") then
			NewDecal(splitted[2], splitted[3], splitted[4], splitted[5], splitted[6], splitted[7])
		elseif(splitted[1] == "skybox") then
			CreateSkybox(80, splitted[2])
		elseif(splitted[1] == "table") then
			CreateLandscape(60, 30, splitted[2])
		end
	end
end

function Save()
	SaveGame("killteam.sav")
end

function Load()
	LoadGame("killteam.sav")
end

function OnStateRecieved()
	Player1 = {}
	Player2 = {}
	Player1index = 1
	Player2index = 1
	for i = 1, Object:GetCount() do
		object = Object:GetAt(i)
		if object:GetProperty("Owner") == "1" then
			Player1[Player1index] = object
			Player1index = Player1index + 1
		elseif object:GetProperty("Owner") == "2" then
			Player2[Player2index] = object
			Player2index = Player2index + 1
		end
	end
	EndTurn()
	if(GetGlobalProperty("Phase") == "Move") then
		MovePhase(GetGlobalProperty("Player"))
	elseif(GetGlobalProperty("Phase") == "Shooting") then
		ShootingPhase(GetGlobalProperty("Player"))
	elseif(GetGlobalProperty("Phase") == "Melee") then
		MeleePhase(GetGlobalProperty("Player"))
	end
	if(GetGlobalProperty("Phase") == "NULL") then
		UI:GetChild("LabelPhase"):SetText("Deployment Phase")
		for i = 1, #Player1 do
			Player1[i]:SetSelectable(true)
			Player1[i]:SetMoveLimit("rectangle", 15, 15, 30, -15)
			
		end
		for i = 1, #Player2 do
			Player2[i]:SetSelectable(true)
			Player2[i]:SetMoveLimit("rectangle", -15, 15, -30, -15)
		end
	else
		UI:GetChild("LabelPhase"):SetText("Turn " .. GetGlobalProperty("Turn") .. " Player" .. GetGlobalProperty("Player") .. " " .. GetGlobalProperty("Phase") .. " Phase ")
	end
	OnSelection()
end