races["Chaos"] = {
	racename = "Chaos",
	units = {
		["Chaos Marine"] = {
			SupportedWeapons = {
				"Bolter",
				"Melta gun",
				"Heavy Bolter"
			},
			Cost = 15,
			RangedWeapon = "Bolter",
			MeleeWeapon = "Knife",
			Model = "CSM.wbm",
			WS = 4,
			BS = 4,
			T = 4,
			Attacks = 1,
			Sv = 3,
			InvSv = 7,
			MovementSpeed = 6
		},
		["Raptor"] = {
			SupportedWeapons = {
				"Bolt-pistol"
			},
			Cost = 20,
			RangedWeapon = "Bolt-pistol",
			MeleeWeapon = "Knife",
			Model = "raptor.wbm",
			WS = 4,
			BS = 4,
			T = 4,
			Attacks = 3,
			Sv = 3,
			InvSv = 7,
			MovementSpeed = 12
		},
		["Chaos Terminator"] = {
			SupportedWeapons = {
				"Power Fist",
				"Lightning claws"
			},
			Cost = 40,
			RangedWeapon = "Combi Bolter",
			MeleeWeapon = "Power Fist",
			Model = "Chaos_terminator_SB+PF.wbm",
			WS = 4,
			BS = 4,
			T = 4,
			Attacks = 2,
			Sv = 2,
			InvSv = 5,
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
			AdditionalAttacks = 0,
			NoRanged = false,
			RerollFailed2Wound = false
		},
		["Power Fist"] = {
			S = 8,
			Melee = true,
			Cost = 0,
			MeleeAP = 2,
			Model = "Chaos_terminator_SB+PF.wbm",
			AdditionalAttacks = 0,
			NoRanged = false,
			RerollFailed2Wound = false
		},
		["Lightning claws"] = {
			S = 4,
			Melee = true,
			Cost = 0,
			MeleeAP = 3,
			Model = "Chaos_terminator_LC.wbm",
			AdditionalAttacks = 2,
			NoRanged = true,
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
			Model = "CSM.wbm",
			S = 4,
			AP = 5,
			Type = "RapidFire",
			Shots = 2
		},
		["Heavy Bolter"] = {
			Range = 36,
			Melee = false,
			Cost = 10,
			Model = "CSM_HB.wbm",
			S = 5,
			AP = 4,
			Type = "Heavy",
			Shots = 3
		},
		["Melta gun"] = {
			Range = 12,
			Melee = false,
			Cost = 15,
			Model = "CSM_melta.wbm",
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
		["Combi Bolter"] = {
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