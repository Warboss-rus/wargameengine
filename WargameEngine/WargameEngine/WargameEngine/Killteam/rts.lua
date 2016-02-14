local playerUnits = {}
local enemyUnits = {}
local points = {}
local playerBuilding = nil
local enemyBuilding = nil

function Init()
	CameraStrategy(30, 12, 5, 0.3)
	EnableLight(1)
	SetLightPosition(1, 0, 0, 50)
	SetLightDiffuse(1, 1, 1, 1, 1)
	SetLightAmbient(1, 0.5, 0.5, 0.5, 1)
	SetLightSpecular(1, 1, 1, 1, 1)
	EnableShadowMap(4096, 65)
	SetAnisotropy(GetMaxAnisotropy())
	EnableMSAA()
	SetShaders("per_pixel_shadow.vsh", "per_pixel_shadow.fsh")
	CreateSkybox(80, "skybox_sand")
	CreateTable(60, 30, "sand.dds")
	SetSelectionCallback("OnSelection")
	SetRMBCallback("OnRMB", true)
end

function CreateUI()
	local panelBuilding = UI:NewPanel("PanelBuilding", 200, 550, 50, 440)
	panelBuilding:NewButton("ButtonUnit1", 0, 0, 50, 100, "Unit1", "NewUnit1")
	panelBuilding:SetVisible(false)
	local panelUnit = UI:NewPanel("PanelUnit", 200, 550, 50, 440)
	panelUnit:NewButton("ButtonMove", 0, 0, 50, 100, "Move", "Move")
	panelUnit:NewButton("ButtonStop", 100, 0, 50, 100, "Stop", "Stop")
	panelUnit:NewButton("ButtonAttackMove", 200, 0, 50, 100, "AttackMove", "AttackMove")
	panelUnit:NewButton("ButtonChangeStance", 300, 0, 50, 100, "Stance: Attack", "ChangeStance")
	panelUnit:SetVisible(false)
	local panelInfo = UI:NewPanel("PanelInfo", 0, 550, 50, 200)
	panelInfo:NewStaticText("TextUnitName", 10, 0, 25, 90, "Nothing")
	panelInfo:NewStaticText("TextHealth", 10, 25, 25, 90, "Health: N/A")
	panelInfo:NewStaticText("TextOwner", 110, 25, 25, 90, "Owner: World")
	panelInfo:SetVisible(false)
	local panelResources = UI:NewPanel("PanelResources", 0, 0, 30, 120)
	panelResources:NewStaticText("TextMoney", 10, 0, 25, 110, "Money: 1000")
end

function NewUnit1()
	local unitStat = units["Tactical Marine"]
	local money = 0 + GetGlobalProperty("Player1Money")
	if money < unitStat.Cost then
		MessageBox("Not enough money")
		return
	end
	SetGlobalProperty("Player1Money", money - unitStat.Cost)
	UI:GetChild("PanelResources"):GetChild("TextMoney"):SetText("Money: "..GetGlobalProperty("Player1Money"))
	local unit = Object:New(unitStat.Model, 22, 0, 0)
	unit:SetProperty("Name", "Tactical Marine")
	unit:SetProperty("Owner", "Player1")
	unit:SetProperty("Type", unitStat.Type)
	unit:SetProperty("Health", unitStat.MaxHealth)
	unit:SetProperty("Stance", unitStat.DefaultStance)
	unit:SetMoveLimit("static")
	playerUnits[#playerUnits + 1] = unit
end

function Move()
	--todo
end

function Stop()
	local object = Object:GetSelected()
	object:GoTo(object:GetX(), object:GetY(), 100000, "", 1)
end

function AttackMove()
	--todo
end

function ChangeStance()
	local object = Object:GetSelected()
	local stance = object:GetProperty("Stance")
	if(stance == "Attack") then
		object:SetProperty("Stance", "Stand")
	elseif(stance == "Stand") then
		object:SetProperty("Stance", "HoldFire")
	elseif(stance == "HoldFire") then
		object:SetProperty("Stance", "Attack")
	end
	UI:GetChild("PanelUnit"):GetChild("ButtonChangeStance"):SetText("Stance: "..object:GetProperty("Stance"))
end

function OnSelection()
	UI:GetChild("PanelBuilding"):SetVisible(false)
	UI:GetChild("PanelUnit"):SetVisible(false)
	UI:GetChild("PanelInfo"):SetVisible(false)
	local object = Object:GetSelected()
	if(object ~= nil) then
		local info = UI:GetChild("PanelInfo")
		info:SetVisible(true)
		info:GetChild("TextUnitName"):SetText(object:GetProperty("Name"))
		info:GetChild("TextHealth"):SetText("Health: " .. object:GetProperty("Health"))
		info:GetChild("TextOwner"):SetText("Owner: " .. object:GetProperty("Owner"))
		if(object:GetProperty("Owner") == "Player1") then
			if(object:GetProperty("Type") == "Building") then
				UI:GetChild("PanelBuilding"):SetVisible(true)
			else
				UI:GetChild("PanelUnit"):SetVisible(true)
				UI:GetChild("PanelUnit"):GetChild("ButtonChangeStance"):SetText("Stance: "..object:GetProperty("Stance"))
			end
		end
	end
end

function OnRMB(obj, x, y, z)--obj has no methods for some reason
	local selected = Object:GetSelected()
	if(selected ~= nil and selected:GetProperty("Owner") == "Player1" and selected:GetProperty("Type") ~= "Building") then
		if(obj ~= nil--[[ and obj:GetProperty("Owner") ~= selected:GetProperty("Owner")]]) then
			local range = math.sqrt((selected:GetX() - x) * (selected:GetX() - x) + (selected:GetY() - y) * (selected:GetY() - y))
			local unitstat = units[selected:GetProperty("Name")]
			if(range > unitstat.Range) then
				MessageBox("Out of range")
				return
			end
			--attack
			NewParticleTracer("tracer.xml", selected:GetX(), selected:GetY(), 1, x, y, 1, 0, 1, 30)
			selected:PlayAnimation("shoot")
			--[[local rand = os.rand(unitstat.MinDamage, unitstat.MaxDamage)
			obj:SetProperty("Health", obj:GetProperty("Health") - rand)
			if(obj:GetProperty("Health") <= 0) then
				obj:Delete()
			end]]
		else
			--goto
			selected:GoTo(x, y, 2, "walk", 1)
		end
	end
end

RunScript("rtsunits.lua")
Init()
CreateUI()
SetGlobalProperty("Player1Money", 1000)
local buildingName = "Rhino"
playerBuilding = Object:New(units[buildingName].Model, 25, 0, 90)
playerBuilding:SetProperty("Name", "Rhino")
playerBuilding:SetProperty("Owner", "Player1")
playerBuilding:SetProperty("Type", units[buildingName].Type)
playerBuilding:SetProperty("Health", units[buildingName].MaxHealth)
playerBuilding:SetMoveLimit("static")
enemyBuilding = Object:New(units[buildingName].Model, -25, 0, -90)
enemyBuilding:SetProperty("Name", "Rhino")
enemyBuilding:SetProperty("Owner", "Player2")
enemyBuilding:SetProperty("Type", units[buildingName].Type)
enemyBuilding:SetProperty("Health", units[buildingName].MaxHealth)
enemyBuilding:SetMoveLimit("static")