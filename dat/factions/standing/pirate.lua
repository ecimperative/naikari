require "factions/standing/skel"


_fdelta_distress = {-0.5, 0.25} -- Maximum change constraints
_fdelta_kill = {-8, 10} -- Maximum change constraints

_fthis = faction.get("Pirate")

-- Secondary hit modifiers.
_fmod_distress_enemy = 1 -- Distress of the faction's enemies
_fmod_distress_friend = 0 -- Distress of the faction's allies
_fmod_kill_enemy = 1 -- Kills of the faction's enemies
_fmod_kill_friend = 0 -- Kills of the faction's allies
_fmod_misn_enemy = 0.3 -- Missions done for the faction's enemies
_fmod_misn_friend = 0.3 -- Missions done for the faction's allies

_fstanding_friendly = 20
_fstanding_neutral = 0


_ftext_standing = {}
_ftext_standing[95] = _("Legendary Pirate")
_ftext_standing[80] = _("Clan Lord")
_ftext_standing[60] = _("Clan Warrior")
_ftext_standing[40] = _("Clan Plunderer")
_ftext_standing[20] = _("Clan Gangster")
_ftext_standing[0] = _("Common Thief")
_ftext_standing[-1] = _("Normie")

_ftext_friendly = _("Friendly")
_ftext_neutral = _("Neutral")
_ftext_hostile = _("Hostile")
_ftext_bribed = _("Paid Off")


function faction_hit(current, amount, source, secondary)
   return math.max(-20, default_hit(current, amount, source, secondary))
end
