--[[
<?xml version='1.0' encoding='utf8'?>
<mission name="Assassination Job">
 <avail>
  <priority>39</priority>
  <cond>player.numOutfit("Mercenary License") &gt; 0 or planet.cur():blackmarket()</cond>
  <chance>660</chance>
  <location>Computer</location>
  <faction>Pirate</faction>
  <faction>Independent</faction>
 </avail>
</mission>
--]]
--[[

   Assassination Job

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

--

   Based on the pirate bounty mission (hence the weird terminology in
   many variable names). Replacement for the old "Pirate Empire bounty"
   mission.

   Unlike Pirate bounties, this mission is kill-only.

--]]

local fmt = require "fmt"
local mh = require "misnhelper"
require "jumpdist"
require "missions/pirate/common"
require "pilot/generic"


-- Mission details
misn_title = {}
misn_title[1] = _("PIRACY: Quick Assassination Job in %s")
misn_title[2] = _("PIRACY: Small Assassination Job in %s")
misn_title[3] = _("PIRACY: Moderate Assassination Job in %s")
misn_title[4] = _("PIRACY: Big Assassination Job in %s")
misn_title[5] = _("PIRACY: Dangerous Assassination Job in %s")
misn_title[6] = _("PIRACY: Highly Dangerous Assassination Job in %s")
misn_desc   = _("A meddlesome %s pilot known as %s was recently seen in the %s system. Local crime lords want this pilot dead.")
desc_illegal_warning = _("WARNING: This mission is illegal and will get you in trouble with the authorities!")

-- Messages
ran_msg = _("MISSION FAILURE! Target got away.")
stolen_msg = _("Another pilot eliminated {pilot}.")
abandoned_msg = _("MISSION FAILURE! You have left the %s system.")
pay_msg = _("MISSION SUCCESS! Pay has been transferred into your account.")

osd_title = _("Assassination")
osd_msg = {}
osd_msg[1] = _("Fly to the %s system")
osd_msg[2] = _("Kill %s")
osd_msg["__save"] = true


hunters = {}
hunter_hits = {}


function create ()
   paying_faction = faction.get("Pirate")

   local target_factions = {
      "Civilian",
      "Dvaered",
      "Empire",
      "Frontier",
      "Goddard",
      "Independent",
      "Sirius",
      "Soromid",
      "Trader",
      "Za'lek",
   }

   local systems = getsysatdistance(system.cur(), 1, 6,
      function(s)
         for i, j in ipairs(target_factions) do
            local p = s:presences()[j]
            if p ~= nil and p > 0 then
               return true
            end
         end
         return false
      end, nil, true)

   if #systems == 0 then
      -- No enemy presence nearby
      misn.finish(false)
   end

   missys = systems[rnd.rnd(1, #systems)]
   if not misn.claim(missys) then misn.finish(false) end

   target_faction = nil
   while target_faction == nil and #target_factions > 0 do
      local i = rnd.rnd(1, #target_factions)
      local p = missys:presences()[target_factions[i]]
      if p ~= nil and p > 0 then
         target_faction = target_factions[i]
      else
         for j = i, #target_factions do
            target_factions[j] = target_factions[j + 1]
         end
      end
   end

   if target_faction == nil then
      -- Should not happen, but putting this here just in case.
      misn.finish(false)
   end

   jumps_permitted = system.cur():jumpDist(missys, true) + rnd.rnd(3, 10)
   if rnd.rnd() < 0.05 then
      jumps_permitted = jumps_permitted - 1
   end

   name = pilot_name()
   level_setup()
   bounty_setup()

   -- Set mission details
   misn.setTitle(misn_title[level]:format(missys:name()))

   if planet.cur():faction() == faction.get("Pirate") then
      misn.setDesc(misn_desc:format(_(target_faction), name, missys:name()))
   else
      -- We're not on a pirate stronghold, so include a warning that the
      -- mission is in fact illegal (for new players).
      misn.setDesc(misn_desc:format(_(target_faction), name, missys:name())
            .. "\n\n" .. desc_illegal_warning)
   end

   misn.setReward(fmt.credits(credits))
   marker = misn.markerAdd(missys, "computer")
end


function accept ()
   misn.accept()

   osd_msg[1] = osd_msg[1]:format(missys:name())
   osd_msg[2] = osd_msg[2]:format(name)
   misn.osdCreate(osd_title, osd_msg)

   last_sys = system.cur()

   hook.jumpin("jumpin")
   hook.jumpout("jumpout")
   hook.takeoff("takeoff")
end


function jumpin ()
   -- Nothing to do.
   if system.cur() ~= missys then
      return
   end

   local pos = jump.pos(system.cur(), last_sys)
   local offset_ranges = { { -2500, -1500 }, { 1500, 2500 } }
   local xrange = offset_ranges[rnd.rnd(1, #offset_ranges)]
   local yrange = offset_ranges[rnd.rnd(1, #offset_ranges)]
   pos = pos + vec2.new(rnd.rnd(xrange[1], xrange[2]), rnd.rnd(yrange[1], yrange[2]))
   spawn_target(pos)
end


function jumpout ()
   jumps_permitted = jumps_permitted - 1
   last_sys = system.cur()
   if not job_done and last_sys == missys then
      fail(abandoned_msg:format(last_sys:name()))
   end
end


function takeoff ()
   spawn_target()
end


function pilot_attacked(p, attacker, dmg)
   if attacker ~= nil then
      local found = false

      for i, j in ipairs(hunters) do
         if j == attacker then
            hunter_hits[i] = hunter_hits[i] + dmg
            found = true
         end
      end

      if not found then
         local i = #hunters + 1
         hunters[i] = attacker
         hunter_hits[i] = dmg
      end
   end
end


function pilot_death(p, attacker)
   if attacker == player.pilot() or attacker:leader() == player.pilot() then
      succeed()
   else
      local top_hunter = nil
      local top_hits = 0
      local player_hits = 0
      local total_hits = 0
      for i, j in ipairs(hunters) do
         total_hits = total_hits + hunter_hits[i]
         if j ~= nil and j:exists() then
            if j == player.pilot() or j:leader() == player.pilot() then
               player_hits = player_hits + hunter_hits[i]
            elseif hunter_hits[i] > top_hits then
               top_hunter = j
               top_hits = hunter_hits[i]
            end
         end
      end

      if top_hunter == nil or player_hits >= top_hits then
         succeed()
      elseif player_hits >= top_hits / 2 and rnd.rnd() < 0.5 then
         succeed()
      else
         mh.showFailMsg(fmt.f(stolen_msg, {pilot=name}))
         misn.finish(false)
      end
   end
end


function pilot_jump ()
   fail(ran_msg)
end


-- Set up the level of the mission
function level_setup ()
   local num_tfaction = missys:presences()[target_faction]

   if target_faction == "Civilian" or target_faction == "Independent" then
      level = 1
   elseif target_faction == "Trader" then
      if num_tfaction <= 100 then
         level = rnd.rnd(1, 2)
      else
         level = rnd.rnd(2, 3)
      end
   elseif target_faction == "Dvaered" then
      if num_tfaction <= 200 then
         level = 2
      elseif num_tfaction <= 300 then
         level = rnd.rnd(2, 3)
      elseif num_tfaction <= 600 then
         level = rnd.rnd(2, 4)
      elseif num_tfaction <= 800 then
         level = rnd.rnd(3, 5)
      else
         level = rnd.rnd(4, 5)
      end
   elseif target_faction == "Frontier" then
      if num_tfaction <= 150 then
         level = rnd.rnd(1, 2)
      else
         level = rnd.rnd(2, 3)
      end
   elseif target_faction == "Sirius" then
      if num_tfaction <= 150 then
         level = 1
      elseif num_tfaction <= 200 then
         level = rnd.rnd(1, 2)
      elseif num_tfaction <= 500 then
         level = rnd.rnd(1, 3)
      elseif num_tfaction <= 800 then
         level = rnd.rnd(2, 5)
      else
         level = rnd.rnd(3, #misn_title)
      end
      -- House Sirius does not have a Destroyer class ship
      if level == 4 then level = 3 end
   elseif target_faction == "Za'lek" then
      if num_tfaction <= 300 then
         level = 3
      elseif num_tfaction <= 500 then
         level = rnd.rnd(3, 4)
      elseif num_tfaction <= 800 then
         level = rnd.rnd(3, 5)
      else
         level = rnd.rnd(4, #misn_title)
      end
   else
      if num_tfaction <= 150 then
         level = 1
      elseif num_tfaction <= 200 then
         level = rnd.rnd(1, 2)
      elseif num_tfaction <= 300 then
         level = rnd.rnd(1, 3)
      elseif num_tfaction <= 500 then
         level = rnd.rnd(1, 4)
      elseif num_tfaction <= 800 then
         level = rnd.rnd(2, 5)
      else
         level = rnd.rnd(3, #misn_title)
      end
   end
end


-- Set up the ship, credits, and reputation based on the level.
function bounty_setup ()
   ship = "Schroedinger"
   credits = 50000
   reputation = 0

   -- Random chance to get a mercenary target instead
   if rnd.rnd() < 0.05 then
      target_faction = "Mercenary"
   end

   if target_faction == "Empire" then
      if level == 1 then
         ship = "Empire Shark"
         credits = 200000 + rnd.sigma() * 15000
         reputation = 1
      elseif level == 2 then
         ship = "Empire Lancelot"
         credits = 450000 + rnd.sigma() * 50000
         reputation = 2
      elseif level == 3 then
         ship = "Empire Admonisher"
         credits = 800000 + rnd.sigma() * 80000
         reputation = 3
      elseif level == 4 then
         ship = "Empire Pacifier"
         credits = 1200000 + rnd.sigma() * 120000
         reputation = 6
      elseif level == 5 then
         ship = "Empire Hawking"
         credits = 1800000 + rnd.sigma() * 200000
         reputation = 10
      elseif level == 6 then
         ship = "Empire Peacemaker"
         credits = 3000000 + rnd.sigma() * 500000
         reputation = 20
      end
   elseif target_faction == "Dvaered" then
      if level <= 2 then
         local choices = {"Dvaered Vendetta", "Dvaered Ancestor"}
         ship = choices[rnd.rnd(1, #choices)]
         credits = 450000 + rnd.sigma() * 50000
         reputation = 2
      elseif level == 3 then
         ship = "Dvaered Phalanx"
         credits = 800000 + rnd.sigma() * 80000
         reputation = 3
      elseif level == 4 then
         ship = "Dvaered Vigilance"
         credits = 1200000 + rnd.sigma() * 120000
         reputation = 6
      elseif level >= 5 then
         ship = "Dvaered Goddard"
         credits = 1800000 + rnd.sigma() * 200000
         reputation = 10
      end
   elseif target_faction == "Soromid" then
      if level == 1 then
         ship = "Soromid Brigand"
         credits = 200000 + rnd.sigma() * 15000
         reputation = 1
      elseif level == 2 then
         local choices = {"Soromid Reaver", "Soromid Marauder"}
         ship = choices[rnd.rnd(1, #choices)]
         credits = 450000 + rnd.sigma() * 50000
         reputation = 2
      elseif level == 3 then
         ship = "Soromid Odium"
         credits = 800000 + rnd.sigma() * 80000
         reputation = 3
      elseif level == 4 then
         ship = "Soromid Nyx"
         credits = 1200000 + rnd.sigma() * 120000
         reputation = 6
      elseif level == 5 then
         ship = "Soromid Ira"
         credits = 1800000 + rnd.sigma() * 200000
         reputation = 10
      elseif level == 6 then
         if rnd.rnd() < 0.5 then
            ship = "Soromid Arx"
         else
            ship = "Soromid Vox"
         end
         credits = 3000000 + rnd.sigma() * 500000
         reputation = 20
      end
   elseif target_faction == "Frontier" then
      if level == 1 then
         ship = "Hyena"
         credits = 100000 + rnd.sigma() * 7500
         reputation = 0
      elseif level == 2 then
         local choices = {"Lancelot", "Ancestor"}
         ship = choices[rnd.rnd(1, #choices)]
         credits = 450000 + rnd.sigma() * 50000
         reputation = 2
      elseif level >= 3 then
         ship = "Phalanx"
         credits = 800000 + rnd.sigma() * 80000
         reputation = 3
      end
   elseif target_faction == "Sirius" then
      if level == 1 then
         ship = "Sirius Fidelity"
         credits = 200000 + rnd.sigma() * 15000
         reputation = 1
      elseif level == 2 then
         ship = "Sirius Shaman"
         credits = 450000 + rnd.sigma() * 50000
         reputation = 2
      elseif level == 3 or level == 4 then
         ship = "Sirius Preacher"
         credits = 800000 + rnd.sigma() * 80000
         reputation = 3
      elseif level == 5 then
         ship = "Sirius Dogma"
         credits = 1800000 + rnd.sigma() * 200000
         reputation = 10
      elseif level == 6 then
         ship = "Sirius Divinity"
         credits = 3000000 + rnd.sigma() * 500000
         reputation = 20
      end
   elseif target_faction == "Za'lek" then
      if level <= 3 then
         ship = "Za'lek Sting"
         credits = 800000 + rnd.sigma() * 80000
         reputation = 3
      elseif level == 4 then
         ship = "Za'lek Demon"
         credits = 1200000 + rnd.sigma() * 120000
         reputation = 6
      elseif level == 5 then
         local choices = {"Za'lek Diablo", "Za'lek Mephisto"}
         ship = choices[rnd.rnd(1, #choices)]
         credits = 1800000 + rnd.sigma() * 200000
         reputation = 10
      elseif level == 6 then
         ship = "Za'lek Hephaestus"
         credits = 3000000 + rnd.sigma() * 500000
         reputation = 20
      end
   elseif target_faction == "Trader" then
      if level == 1 then
         local choices = {"Gawain", "Koala"}
         ship = choices[rnd.rnd(1, #choices)]
         credits = 75000 + rnd.sigma() * 5000
         reputation = 0
      elseif level == 2 then
         local choices = {"Llama", "Quicksilver"}
         ship = choices[rnd.rnd(1, #choices)]
         credits = 350000 + rnd.sigma() * 50000
         reputation = 2
      elseif level >= 3 then
         local choices = {"Rhino", "Mule"}
         ship = choices[rnd.rnd(1, #choices)]
         credits = 700000 + rnd.sigma() * 80000
         reputation = 3
      end
   elseif target_faction == "Mercenary" then
      if level == 1 then
         local choices = {"Hyena", "Shark"}
         ship = choices[rnd.rnd(1, #choices)]
         credits = 200000 + rnd.sigma() * 15000
         reputation = 0
      elseif level == 2 then
         local choices = {"Lancelot", "Vendetta", "Ancestor"}
         ship = choices[rnd.rnd(1, #choices)]
         credits = 450000 + rnd.sigma() * 50000
         reputation = 1
      elseif level == 3 then
         local choices = {"Admonisher", "Phalanx"}
         ship = choices[rnd.rnd(1, #choices)]
         credits = 800000 + rnd.sigma() * 80000
         reputation = 2
      elseif level == 4 then
         local choices = {"Pacifier", "Vigilance"}
         ship = choices[rnd.rnd(1, #choices)]
         credits = 1200000 + rnd.sigma() * 120000
         reputation = 3
      elseif level == 5 then
         local choices = {"Kestrel", "Hawking"}
         ship = choices[rnd.rnd(1, #choices)]
         credits = 1800000 + rnd.sigma() * 200000
         reputation = 5
      elseif level >= 6 then
         ship = "Goddard"
         credits = 3000000 + rnd.sigma() * 500000
         reputation = 10
      end
   elseif target_faction == "Civilian" then
      local choices = {"Schroedinger", "Hyena", "Gawain", "Llama"}
      ship = choices[rnd.rnd(1, #choices)]
      credits = 50000 + rnd.sigma() * 5000
      reputation = 0
   elseif target_faction == "Independent" then
      local choices = {"Schroedinger", "Hyena", "Gawain"}
      ship = choices[rnd.rnd(1, #choices)]
      credits = 50000 + rnd.sigma() * 5000
      reputation = 0
   end
end


-- Spawn the ship at the location source.
function spawn_target(source)
   if not job_done and system.cur() == missys then
      if jumps_permitted >= 0 then
         misn.osdActive(2)
         target_ship = pilot.add(ship, target_faction, source, name)
         target_ship:setHilight(true)
         hook.pilot(target_ship, "attacked", "pilot_attacked")
         hook.pilot(target_ship, "death", "pilot_death")
         hook.pilot(target_ship, "jump", "pilot_jump")
         hook.pilot(target_ship, "land", "pilot_jump")
      else
         fail(ran_msg)
      end
   else
      local needed = system.cur():jumpDist(missys, true)
      if needed ~= nil and needed > jumps_permitted then
         -- Mission is now impossible to beat and therefore in a sort of
         -- "zombie" state, so immediately fail the mission. We use the
         -- "beaten" fail text here because the player isn't inside the
         -- system, so they wouldn't know that the pirate is no longer
         -- there, but they would know if someone else did the job.
         mh.showFailMsg(fmt.f(stolen_msg, {pilot=name}))
         misn.finish(false)
      end
   end
end


-- Succeed the mission
function succeed ()
   player.msg("#g" .. pay_msg .. "#0")
   player.pay(credits)

   paying_faction:modPlayer(reputation)
   misn.finish(true)
end


-- Fail the mission, showing message to the player.
function fail(message)
   if message ~= nil then
      -- Pre-colourized, do nothing.
      if message:find("#") then
         player.msg(message)
      -- Colourize in red.
      else
         player.msg("#r" .. message .. "#0")
      end
   end
   misn.finish(false)
end
