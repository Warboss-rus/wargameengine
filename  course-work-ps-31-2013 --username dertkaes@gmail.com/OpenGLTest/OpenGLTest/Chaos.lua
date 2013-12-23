Chaos = {}

function Chaos:New()
	newObj = {}
	self.__index = self
	return setmetatable(newObj, self)
end

function Chaos:GetUnitList()
	return {
		"Chaos Marine(15)",
		"Raptor(20)",
		"Chaos Terminator(40)"
	}
end

function Chaos:GetUnitWeapons(unit)
	if(unit == "Chaos Marine") then
		return {
			"Bolter(0)",
			"Melta gun(15)",
			"Heavy Bolter(10)"
		}
	elseif(unit == "Raptor") then
		return {
			"Bolt-pistol(0)"
		}
	elseif(unit == "Chaos Terminator") then
		return {
			"Combi Bolter(0)",
			"Lightning claws(0)"
		}
	end
end

function Chaos:GetDefaultWeapon(unit)
	if(unit == "Chaos Marine") then
		return "Bolter(0)"
	elseif(unit == "Raptor") then
		return "Bolt-pistol(0)"
	elseif(unit == "Chaos Terminator") then
		return "Combi Bolter(0)"
	end
end

function Chaos:GetUnitModel(rosterItem)
	if(rosterItem.Name == "Chaos Marine" and (rosterItem.Weapon == "Bolter(0)")) then
		return "CSM.wbm"
	elseif(rosterItem.Name == "Chaos Marine" and rosterItem.Weapon == "Melta gun(15)") then
		return "CSM_melta.wbm"
	elseif(rosterItem.Name == "Chaos Marine" and rosterItem.Weapon == "Heavy Bolter(10)") then
		return "CSM_HB.wbm"
	elseif(rosterItem.Name == "Raptor") then
		return "raptor.wbm"
	elseif(rosterItem.Name == "Chaos Terminator" and rosterItem.Weapon == "Combi Bolter(0)") then
		return "Chaos_terminator_SB+PF.wbm"
	elseif(rosterItem.Name == "Chaos Terminator" and rosterItem.Weapon == "Lightning claws(0)") then
		return "Chaos_terminator_LC.wbm"
	end
	return ""
end

function Chaos:SetUnitStats(object, rosterItem)
	if(rosterItem.Name == "Chaos Marine" and rosterItem.Weapon == "Bolter(0)") then
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
	elseif(rosterItem.Name == "Chaos Marine" and rosterItem.Weapon == "Melta gun(15)") then
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
	elseif(rosterItem.Name == "Chaos Marine" and rosterItem.Weapon == "Heavy Bolter(10)") then
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
	elseif(rosterItem.Name == "Raptor") then
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
	elseif(rosterItem.Name == "Chaos Terminator" and rosterItem.Weapon == "Combi Bolter(0)") then
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
	elseif(rosterItem.Name == "Chaos Terminator" and rosterItem.Weapon == "Lightning claws(0)") then
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
end