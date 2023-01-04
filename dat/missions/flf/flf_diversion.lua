--[[
<?xml version='1.0' encoding='utf8'?>
<mission name="Divert the Dvaered Forces">
 <avail>
  <priority>30</priority>
  <chance>550</chance>
  <done>Diversion from Doranthex</done>
  <location>Computer</location>
  <faction>FLF</faction>
  <faction>Frontier</faction>
  <cond>faction.playerStanding("FLF") &gt;= 0 and diff.isApplied("FLF_base") and not diff.isApplied("flf_dead")</cond>
 </avail>
</mission>
--]]
--[[

   FLF diversion mission.

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

--]]

local fmt = require "fmt"
require "numstring"
require "missions/flf/flf_common"

-- localization stuff
misn_title  = _("FLF: Diversion in %s")

success_text = {}
success_text[1] = _("You receive a transmission from an FLF officer saying that the operation has completed, and you can now return to the base.")

pay_text = {}
pay_text[1] = _("The FLF commander in charge of the primary operation thanks you for your contribution and hands you your pay.")
pay_text[2] = _("You greet the FLF commander in charge of the primary operation, who seems happy that the mission was a success. You congratulate each other, and the commander hands you your pay.")

misn_desc = _("An FLF special task force needs an opening to complete a mission without too much Dvaered interference. Create this opening by wreaking havoc in the nearby {system} system.")

msg = _("%s has warped in!")

osd_title   = _("FLF Diversion")
osd_desc    = {}
osd_desc[1] = _("Fly to the %s system")
osd_desc[2] = _("Attack Dvaered ships to get as many pilots' attention as possible")
osd_desc[3] = _("Return to FLF base")
osd_desc["__save"] = true


function create ()
   missys = flf_getTargetSystem()
   if not misn.claim(missys) then misn.finish(false) end

   local num_dvaereds = missys:presences()["Dvaered"]
   local num_empire = missys:presences()["Empire"]
   local num_flf = missys:presences()["FLF"]
   if num_dvaereds == nil then num_dvaereds = 0 end
   if num_empire == nil then num_empire = 0 end
   if num_flf == nil then num_flf = 0 end
   dv_attention_target = num_dvaereds / 50
   credits = 200 * (num_dvaereds + num_empire - num_flf) * system.cur():jumpDist(missys, true) / 3
   credits = credits + rnd.sigma() * 10000
   reputation = math.max((num_dvaereds + num_empire - num_flf) / 25, 1)
   if credits < 10000 then misn.finish(false) end

   -- Set mission details
   misn.setTitle(misn_title:format(missys:name()))
   misn.setDesc(fmt.f(misn_desc, {system=missys:name()}))
   misn.setReward(creditstring(credits))
   marker = misn.markerAdd(missys, "computer")
end


function accept ()
   misn.accept()

   osd_desc[1] = osd_desc[1]:format(missys:name())
   misn.osdCreate(osd_title, osd_desc)

   dv_attention = 0
   dv_coming = false
   job_done = false

   hook.enter("enter")
   hook.jumpout("leave")
   hook.land("leave")
end


function enter ()
   if not job_done then
      if system.cur() == missys then
         misn.osdActive(2)
         update_dv()
      else
         misn.osdActive(1)
      end
   end
end


function leave ()
   dv_attention = 0
   hook.rm(update_dv_hook)
end


function update_dv ()
   for i, j in ipairs(pilot.get({faction.get("Dvaered")})) do
      hook.pilot(j, "attacked", "pilot_attacked_dv")
      hook.pilot(j, "death", "pilot_death_dv")
   end
   update_dv_hook = hook.timer(3.0, "update_dv")
end


function add_attention(p)
   p:setHilight(true)

   if not job_done then
      dv_attention = dv_attention + 1
      if dv_attention >= dv_attention_target and dv_attention - 1 < dv_attention_target then
         hook.rm(success_hook)
         success_hook = hook.timer(30.0, "timer_mission_success")
      end

      hook.pilot(p, "jump", "rm_attention")
      hook.pilot(p, "land", "rm_attention")
   end
end


function rm_attention ()
   dv_attention = math.max(dv_attention - 1, 0)
   if dv_attention < dv_attention_target then
      hook.rm(success_hook)
   end
end


function pilot_attacked_dv(p, attacker)
   if (attacker == player.pilot() or attacker:leader(true) == player.pilot())
         and not dv_coming and rnd.rnd() < 0.1 then
      dv_coming = true
      hook.timer(10.0, "timer_spawn_dv")
   end
end


function pilot_death_dv(p, attacker)
   if not dv_coming
         and (attacker == player.pilot()
            or attacker:leader(true) == player.pilot()
            or attacker:faction() == faction.get("FLF")) then
      dv_coming = true
      hook.timer(10.0, "timer_spawn_dv")
   end
end


function timer_spawn_dv ()
   dv_coming = false
   if not job_done then
      local shipnames = {
         "Dvaered Vendetta", "Dvaered Ancestor", "Dvaered Phalanx",
         "Dvaered Vigilance", "Dvaered Goddard",
      }
      local shipname = shipnames[rnd.rnd(1, #shipnames)]
      player.msg(msg:format(shipname))
      local p = pilot.add(shipname, "Dvaered")
      add_attention(p)
   end
end


function timer_mission_success ()
   if dv_attention >= dv_attention_target then
      job_done = true
      misn.osdActive(3)
      misn.markerRm(marker)
      hook.rm(update_dv_hook)
      hook.land("land")
      tk.msg("", success_text[rnd.rnd(1, #success_text)])

      for i, p in ipairs(pilot.get(nil, true)) do
         p:setHilight(false)
      end
   end
end


function land ()
   if planet.cur():faction() == faction.get("FLF") then
      tk.msg("", pay_text[rnd.rnd(1, #pay_text)])
      player.pay(credits)
      faction.get("FLF"):modPlayer(reputation)
      misn.finish(true)
   end
end
