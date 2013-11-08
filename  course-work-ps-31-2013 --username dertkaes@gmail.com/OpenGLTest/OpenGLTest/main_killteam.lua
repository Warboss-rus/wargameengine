Player1 = {}
Player2 = {}
Player1Count = 10
Player2Count = 10

function Init()
	for i = 1, 10 do
		Player1[i] = Object:New("SpaceMarine.obj", 20 + (i - 1) / 2.5, (i - 1) % 5 * 2 - 5, -90)
		Player1[i]:SetProperty("Owner", "1")
		Player1[i]:SetSelectable(true)
		Player1[i]:SetMoveLimit("rectangle", 15, 15, 30, -15)
		Player2[i] = Object:New("CSM.obj", -20 - (i - 1) / 2.5, (i - 1) % 5 * 2 - 5, 90)
		Player2[i]:SetProperty("Owner", "2")
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
	if(player == "1") then
		for i = 1, 10 do
			Player1[i]:SetSelectable(true)
			Player1[i]:SetMoveLimit("static")
		end
	else
		for i = 1, 10 do
			Player2[i]:SetSelectable(true)
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
	if(GetGlobalProperty("Phase") ~= "Shooting") then
		MessageBox("You can only shoot in your shooting phase!")
		return
	end
	local hunter = Object:GetSelected()
	if(hunter:Null()) then
		MessageBox("Choose one of your models to shoot")
		return
	end
	local prey = hunter--TODO:Object picker
	if(prey:Null()) then
		MessageBox("Choose one of enemy models to shoot at")
		return
	end
	math.randomseed(os.time())
	local result = "Rolls to hit (2 dice, 3+): "
	local hits = 0
	local wounds = 0
	local unsaved = 0
	for i = 1, 2 do
		local rand = math.random(1, 6)
		result = result .. rand .. " "
		if(rand >= 3) then
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
	result = result .. "\nRolls saves (" .. wounds .. " dice, 3+): "
	for i = 1, wounds do
		local rand = math.random(1, 6)
		if(rand < 3) then
			unsaved = unsaved + 1
		end
		result = result .. rand .. " "
	end
	result = result .. "\nTotal " .. unsaved .. " unsaved wounds. "
	if(unsaved > 0) then
		result = result .. "Enemy dies"
	else
		result = result .. "Enemy still alive"
	end
	MessageBox(result)
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
ui:NewButton("Button2", 110, 10, 30, 80, "Shoot", "Fire")
ui:NewButton("Button4", 200, 10, 30, 80, "Ruler", "SetRuler")
ui:NewButton("Button5", 290, 10, 30, 80, "Undo", "UndoAction")
ui:NewButton("Button6", 380, 10, 30, 80, "Redo", "RedoAction")
ui:NewStaticText("Label1", 10, 40, 30, 180, "Deployment Phase")
Init()