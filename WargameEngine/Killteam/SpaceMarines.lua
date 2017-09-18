races["SpaceMarines"] = {
	racename = "SpaceMarines",
	units = {
		["Tactical Marine"] = {
			SupportedWeapons = {
				"Bolter",
				"Melta gun",
				"Heavy Bolter"
			},
			Cost = 15,
			RangedWeapon = "Bolter",
			MeleeWeapon = "Knife",
			Model = "Angel_of_Death.wbm",
			WS = 4,
			BS = 4,
			T = 4,
			Attacks = 1,
			Sv = 3,
			MovementSpeed = 6
		},
		["Assault Marine"] = {
			SupportedWeapons = {
				"Bolt-pistol"
			},
			Cost = 20,
			RangedWeapon = "Bolt-pistol",
			MeleeWeapon = "Knife",
			Model = "assault_marine.wbm",
			WS = 4,
			BS = 4,
			T = 4,
			Attacks = 3,
			Sv = 3,
			MovementSpeed = 12
		},
		["Terminator"] = {
			SupportedWeapons = {
				"Power Fist",
				"Lightning claws"
			},
			Cost = 40,
			RangedWeapon = "Storm Bolter",
			MeleeWeapon = "Power Fist",
			Model = "Terminator_SB+PF.wbm",
			WS = 4,
			BS = 4,
			T = 4,
			Attacks = 2,
			Sv = 2,
			MovementSpeed = 6
		},
	},
	weapons = {
		["Knife"] = {
			S = 4,
			Melee = true,
			Cost = 0,
			MeleeAP = 7,
			Model = "",
		},
		["Power Fist"] = {
			S = 8,
			Melee = true,
			Cost = 0,
			MeleeAP = 2,
			Model = "Terminator_SB+PF.wbm",
		},
		["Lightning claws"] = {
			S = 4,
			Melee = true,
			Cost = 0,
			MeleeAP = 3,
			Model = "Terminator_LC.wbm",
			RerollFailed2Wound = true
		},
		["none"] = {
			Range = 0,
			Melee = false,
			Cost = 0,
			Model = "",
			S = 0,
			AP = 0,
			Type = "",
			Shots = 0
		},
		["Bolter"] = {
			Range = 24,
			Melee = false,
			Cost = 0,
			Model = "Angel_of_Death.wbm",
			S = 4,
			AP = 5,
			Type = "RapidFire",
			Shots = 2
		},
		["Heavy Bolter"] = {
			Range = 36,
			Melee = false,
			Cost = 10,
			Model = "SM_HB.wbm",
			S = 5,
			AP = 4,
			Type = "Heavy",
			Shots = 3
		},
		["Melta gun"] = {
			Range = 12,
			Melee = false,
			Cost = 15,
			Model = "SM_Melta.wbm",
			S = 8,
			AP = 1,
			Type = "Assault",
			Shots = 1
		},
		["Bolt-pistol"] = {
			Range = 12,
			Melee = false,
			Cost = 0,
			Model = "",
			S = 4,
			AP = 5,
			Type = "Pistol",
			Shots = 1
		},
		["Storm Bolter"] = {
			Range = 24,
			Melee = false,
			Cost = 0,
			Model = "",
			S = 4,
			AP = 5,
			Type = "Assault",
			Shots = 2
		}
	}
}