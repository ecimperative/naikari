require "factions/equip/generic"


equip_typeOutfits_hulls["Vendetta"] = {
   "S&K Light Combat Plating",
}
equip_typeOutfits_hulls["Ancestor"] = {
   "S&K Light Combat Plating",
}
equip_typeOutfits_hulls["Phalanx"] = {
   "S&K Medium Combat Plating",
}
equip_typeOutfits_hulls["Vigilance"] = {
   "S&K Medium-Heavy Combat Plating",
}
equip_typeOutfits_hulls["Goddard"] = {
   "S&K Superheavy Combat Plating",
}

equip_typeOutfits_weapons["Vendetta"] = {
   {
      num = 2, probability = {
         ["Vulcan Gun"] = 4, ["Gauss Gun"] = 4,
      };
      "Vulcan Gun", "Gauss Gun",
      "Unicorp Mace Launcher", "TeraCom Mace Launcher",
   },
   {
      num = 2, probability = {
         ["Vulcan Gun"] = 4, ["Gauss Gun"] = 4,
      };
      "Vulcan Gun", "Gauss Gun",
      "Unicorp Mace Launcher", "TeraCom Mace Launcher",
   },
   {
      probability = {
         ["Vulcan Gun"] = 4, ["Gauss Gun"] = 4,
      };
      "Vulcan Gun", "Gauss Gun",
      "Unicorp Mace Launcher", "TeraCom Mace Launcher",
   },
}
equip_typeOutfits_weapons["Ancestor"] = {
   {
      num = 2, varied = true;
      "Unicorp Headhunter Launcher", "Unicorp Vengeance Launcher",
   },
   {
      varied = true;
      "Vulcan Gun", "TeraCom Mace Launcher",
   },
}
equip_typeOutfits_weapons["Phalanx"] = {
   {
      num = 3, varied = true;
      "Unicorp Headhunter Launcher", "Unicorp Vengeance Launcher",
      "TeraCom Headhunter Launcher", "TeraCom Vengeance Launcher",
      "Unicorp Caesar IV Launcher", "TeraCom Imperator Launcher",
   },
   {
      probability = {
         ["Shredder"] = 8,
      };
      "Shredder", "Vulcan Gun",
   },
}
equip_typeOutfits_weapons["Vigilance"] = {
   {
      num = 2;
      "Railgun",
   },
   {
      num = 1;
      "Enygma Systems Turreted Fury Launcher",
      "Enygma Systems Turreted Headhunter Launcher",
   },
   {
      num = 1;
      "Mass Driver", "Turreted Vulcan Gun",
      "Enygma Systems Turreted Fury Launcher",
      "Enygma Systems Turreted Headhunter Launcher",
   },
   {
      "Mass Driver", "Turreted Vulcan Gun",
   },
}
equip_typeOutfits_weapons["Goddard"] = {
   {
      num = 1;
      "Enygma Systems Turreted Fury Launcher",
      "Enygma Systems Turreted Headhunter Launcher",
   },
   {
      "Repeating Railgun",
   },
   {
      "Railgun", "Turreted Mass Driver",
   },
}

equip_typeOutfits_structurals["Phalanx"] = {
   {
      varied = true;
      "Medium Fuel Pod", "Battery II", "Shield Capacitor II",
      "Plasteel Plating", "Active Plating",
   },
}
equip_typeOutfits_structurals["Vigilance"] = {
   {
      varied = true;
      "Large Fuel Pod", "Battery III", "Shield Capacitor IV",
      "Shield Capacitor III", "Nanobond Plating",
   },
   {
      varied = true;
      "Medium Fuel Pod", "Battery II", "Shield Capacitor II",
      "Plasteel Plating", "Active Plating",
   },
}


--[[
-- @brief Does Dvaered pilot equipping
--
--    @param p Pilot to equip
--]]
function equip( p )
   equip_generic( p )
end
