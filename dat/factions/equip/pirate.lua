require "factions/equip/generic"


-- Probability of cargo by class.
equip_classCargo["Yacht"] = .5
equip_classCargo["Luxury Yacht"] = .5
equip_classCargo["Scout"] = .5
equip_classCargo["Courier"] = .5
equip_classCargo["Freighter"] = .5
equip_classCargo["Armored Transport"] = .5
equip_classCargo["Fighter"] = .5
equip_classCargo["Bomber"] = .5
equip_classCargo["Corvette"] = .5
equip_classCargo["Destroyer"] = .5
equip_classCargo["Cruiser"] = .5
equip_classCargo["Carrier"] = .5
equip_classCargo["Drone"] = .1
equip_classCargo["Heavy Drone"] = .1

equip_typeOutfits_coreSystems["Hyena"] = equip_shipOutfits_coreSystems["Pirate Shark"]
equip_typeOutfits_coreSystems["Shark"] = equip_shipOutfits_coreSystems["Pirate Shark"]
equip_typeOutfits_coreSystems["Vendetta"] = {
   "Unicorp PT-80 Core System", "Milspec Prometheus 3603 Core System",
}
equip_typeOutfits_coreSystems["Lancelot"] = equip_typeOutfits_coreSystems["Vendetta"]
equip_typeOutfits_coreSystems["Ancestor"] = equip_typeOutfits_coreSystems["Vendetta"]
equip_typeOutfits_coreSystems["Phalanx"] = {
   "Unicorp PT-280 Core System", "Milspec Prometheus 4703 Core System",
}
equip_typeOutfits_coreSystems["Admonisher"] = equip_typeOutfits_coreSystems["Phalanx"]
equip_typeOutfits_coreSystems["Pacifier"] = {
   "Unicorp PT-400 Core System", "Milspec Prometheus 5403 Core System",
}
equip_typeOutfits_coreSystems["Kestrel"] = {
   "Unicorp PT-3400 Core System", "Milspec Prometheus 8503 Core System",
}

equip_typeOutfits_engines["Rhino"] = {
   "Unicorp Falcon 1200 Engine", "Tricon Cyclone II Engine",
}

equip_typeOutfits_weapons["Hyena"] = equip_shipOutfits_weapons["Pirate Shark"]
equip_typeOutfits_weapons["Shark"] = equip_shipOutfits_weapons["Pirate Shark"]
equip_typeOutfits_weapons["Vendetta"] = {
   {
      varied = true,
      probability = {
         ["Ion Cannon"] = 16,
      };
      "Laser Cannon MK2", "Vulcan Gun", "Plasma Blaster MK2",
      "Laser Cannon MK1", "Gauss Gun", "Plasma Blaster MK1",
      "Unicorp Mace Launcher", "TeraCom Mace Launcher",
      "Ion Cannon",
   },
}
equip_typeOutfits_weapons["Ancestor"] = {
   {
      varied = true;
      "Unicorp Medusa Launcher",
   },
   {
      varied = true,
      probability = {
         ["Ion Cannon"] = 16,
      };
      "Laser Cannon MK2", "Vulcan Gun", "Plasma Blaster MK2",
      "Laser Cannon MK1", "Gauss Gun", "Plasma Blaster MK1",
      "Unicorp Mace Launcher", "TeraCom Mace Launcher",
      "Ion Cannon",
   },
}
equip_typeOutfits_weapons["Phalanx"] = {
   {
      varied = true;
      "Unicorp Medusa Launcher", "TeraCom Medusa Launcher", 
      "Enygma Systems Huntsman Launcher",
   },
   {
      varied = true;
      "Ripper Cannon", "Shredder", "Plasma Cannon",
      "Laser Cannon MK2", "Vulcan Gun", "Plasma Blaster MK2",
   },
}
equip_typeOutfits_weapons["Rhino"] = {
   {
      varied = true,
      probability = {
         ["Heavy Ion Turret"] = 6,
      };
      "Heavy Ripper Turret", "Plasma Cluster Turret", "Turreted Mass Driver",
      "Heavy Ion Turret",
   },
   {
      varied = true, num = 1;
      "Mini Hyena Fighter Bay", "Mini Pirate Shark Fighter Bay",
   },
   {
      varied = true,
      probability = {
         ["EMP Grenade Launcher"] = 6,
      };
      "Laser Turret MK2", "Turreted Vulcan Gun", "Plasma Turret MK2",
      "EMP Grenade Launcher",
      "Mini Hyena Fighter Bay", "Mini Pirate Shark Fighter Bay",
   },
}
equip_typeOutfits_weapons["Kestrel"] = {
   {
      varied = true,
      probability = {
         ["Heavy Ion Turret"] = 6,
      };
      "Heavy Ripper Turret", "Railgun Turret", "Plasma Cluster Turret",
      "Heavy Ion Turret",
   },
   {
      varied = true, num = 2;
      "Mini Hyena Fighter Bay", "Mini Pirate Shark Fighter Bay",
   },
   {
      varied = true,
      probability = {
         ["EMP Grenade Launcher"] = 6,
      };
      "Laser Turret MK2", "Turreted Vulcan Gun", "Plasma Turret MK2",
      "EMP Grenade Launcher",
   },
}


--[[
-- @brief Does pirate pilot equipping
--
--    @param p Pilot to equip
--]]
function equip( p )
   equip_generic( p )
end
