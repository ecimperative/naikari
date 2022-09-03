--[[
<?xml version='1.0' encoding='utf8'?>
<mission name="Dead Or Alive Bounty">
 <avail>
  <priority>41</priority>
  <cond>player.numOutfit("Mercenary License") &gt; 0</cond>
  <chance>360</chance>
  <location>Computer</location>
  <faction>Dvaered</faction>
  <faction>Empire</faction>
  <faction>Goddard</faction>
  <faction>Independent</faction>
  <faction>Sirius</faction>
  <faction>Soromid</faction>
  <faction>Za'lek</faction>
 </avail>
</mission>
--]]
--[[

   Dead or Alive Pirate Bounty

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

   Generalized replacement for bobbens' Empire Pirate Bounty mission.
   Can work with any faction.

--]]

local fmt = require "fmt"
require "jumpdist"
require "pilot/pirate"


subdue_text = {}
subdue_text[1] = _("You and your crew infiltrate the ship's pathetic security and subdue %s. You transport them to your ship.")
subdue_text[2] = _("Your crew has a difficult time getting past the ship's security, but eventually succeeds and subdues %s.")
subdue_text[3] = _("The ship's security system turns out to be no match for your crew. You infiltrate the ship and capture %s.")
subdue_text[4] = _("Your crew infiltrates the ship and captures %s.")
subdue_text[5] = _("Getting past this ship's security was surprisingly easy. Didn't they know that %s was wanted?")

pay_kill_text = {}
pay_kill_text[1] = _("After verifying that you killed %s, an officer hands you your pay.")
pay_kill_text[2] = _("After verifying that %s is indeed dead, the tired-looking officer smiles and hands you your pay.")
pay_kill_text[3] = _("The officer seems pleased that %s is finally dead. They thank you and promptly hand you your pay.")
pay_kill_text[4] = _("A frightful officer takes you into a locked room, where the death of %s is quietly verified. The officer then pays you and sends you off.")
pay_kill_text[5] = _("When you ask the officer for your bounty on %s, they sigh, lead you into an office, go through some paperwork, and hand you your pay, mumbling something about how useless the bounty system is.")
pay_kill_text[6] = _("The officer verifies the death of %s, goes through the necessary paperwork, and hands you your pay, looking bored the entire time.")

pay_capture_text = {}
pay_capture_text[1] = _("An officer takes %s into custody and hands you your pay.")
pay_capture_text[2] = _("The officer seems to think your decision to capture %s alive was foolish. They carefully take your target off your hands, taking precautions you think are completely unnecessary, and then hand you your pay")
pay_capture_text[3] = _("The officer you deal with seems to especially dislike %s. Your target is taken off your hands and you are handed your pay without a word.")
pay_capture_text[4] = _("A fearful-looking officer rushes %s into a secure hold, pays you the appropriate bounty, and then hurries off.")
pay_capture_text[5] = _("The officer you greet gives you a puzzled look when you say that you captured %s alive. Nonetheless, they politely take your target off of your hands and hand you your pay.")

share_text = {}
share_text[1] = _([["Greetings. I can see that you were trying to collect a bounty on %s. Well, as you can see, I earned the bounty, but I don't think I would have succeeded without your help, so I've transferred a portion of the bounty into your account."]])
share_text[2] = _([["Sorry about getting in the way of your bounty. I don't really care too much about the money, but I just wanted to make sure the galaxy would be rid of that scum; I've seen the villainy of %s first-hand, you see. So as an apology, I would like to offer you the portion of the bounty you clearly earned. The money will be in your account shortly."]])
share_text[3] = _([["Hey, thanks for the help back there. I don't know if I would have been able to handle %s alone! Anyway, since you were such a big help, I have transferred what I think is your fair share of the bounty to your bank account."]])
share_text[4] = _([["Heh, thanks! I think I would have been able to take out %s by myself, but still, I appreciate your assistance. Here, I'll transfer some of the bounty to you, as a token of my appreciation."]])
share_text[5] = _([["Ha ha ha, looks like I beat you to it this time, eh? Well, I don't do this often, but here, have some of the bounty. I think you deserve it."]])

-- Mission details
misn_title = {}
misn_title[1] = _("Tiny Dead or Alive Bounty in %s")
misn_title[2] = _("Small Dead or Alive Bounty in %s")
misn_title[3] = _("Moderate Dead or Alive Bounty in %s")
misn_title[4] = _("High Dead or Alive Bounty in %s")
misn_title[5] = _("Dangerous Dead or Alive Bounty in %s")
misn_desc   = _("The pirate known as %s was recently seen in the %s system. %s authorities want this pirate dead or alive.")

fail_escape_text = _("MISSION FAILURE! {pilot} got away.")
fail_beaten_text = _("MISSION FAILURE! Another pilot eliminated {pilot}.")
fail_abandon_text = _("MISSION FAILURE! You have left the {system} system.")

osd_title = _("Bounty Hunt")
osd_msg = {}
osd_msg[1] = _("Fly to the %s system")
osd_msg[2] = _("Kill or capture %s")
osd_msg[3] = _("Land in %s territory to collect your bounty")
osd_msg["__save"] = true


hunters = {}
hunter_hits = {}


function create ()
   paying_faction = planet.cur():faction()

   local systems = getsysatdistance(system.cur(), 1, 3,
      function(s)
         local p = s:presences()["Pirate"]
         return p ~= nil and p > 0
      end)

   if #systems == 0 then
      -- No pirates nearby
      misn.finish(false)
   end

   missys = systems[rnd.rnd(1, #systems)]
   if not misn.claim(missys) then misn.finish(false) end

   local dist = system.cur():jumpDist(missys)
   if dist == nil then
      misn.finish(false)
   end
   -- Add enough leniency for landings every two jumps
   jumps_permitted = math.floor(dist + dist/2) + rnd.rnd(0, 5)

   local num_pirates = missys:presences()["Pirate"]
   if num_pirates <= 25 then
      level = 1
   elseif num_pirates <= 50 then
      level = rnd.rnd(1, 2)
   elseif num_pirates <= 75 then
      level = rnd.rnd(2, 3)
   elseif num_pirates <= 100 then
      level = rnd.rnd(3, 4)
   else
      level = rnd.rnd(4, #misn_title)
   end

   name = pirate_name()
   ship = "Hyena"
   credits = 50000
   reputation = 0
   pirate_faction = faction.get("Pirate")
   bounty_setup()

   -- Set mission details
   misn.setTitle(misn_title[level]:format(missys:name()))
   misn.setDesc(misn_desc:format(name, missys:name(), paying_faction:name()))
   misn.setReward(fmt.credits(credits))
   marker = misn.markerAdd(missys, "computer")
end


function accept ()
   misn.accept()

   osd_msg[1] = osd_msg[1]:format(missys:name())
   osd_msg[2] = osd_msg[2]:format(name)
   osd_msg[3] = osd_msg[3]:format(paying_faction:name())
   misn.osdCreate(osd_title, osd_msg)

   last_sys = system.cur()
   job_done = false
   target_killed = false

   hook.jumpin("jumpin")
   hook.jumpout("jumpout")
   hook.takeoff("takeoff")
   hook.land("land")
end


function jumpin ()
   -- Nothing to do.
   if system.cur() ~= missys then
      return
   end

   local pos = jump.pos(system.cur(), last_sys)
   local offset_ranges = {{-2500, -1500}, {1500, 2500}}
   local xrange = offset_ranges[rnd.rnd(1, #offset_ranges)]
   local yrange = offset_ranges[rnd.rnd(1, #offset_ranges)]
   pos = pos + vec2.new(rnd.rnd(xrange[1], xrange[2]), rnd.rnd(yrange[1], yrange[2]))
   spawn_pirate(pos)
end


function jumpout ()
   jumps_permitted = jumps_permitted - 1
   last_sys = system.cur()
   if not job_done and last_sys == missys then
      fail(fmt.f(fail_abandon_text, {system=missys:name()}))
   end
end


function takeoff ()
   spawn_pirate()
end


function land ()
   jumps_permitted = jumps_permitted - 1
   if job_done and planet.cur():faction() == paying_faction then
      local pay_text
      if target_killed then
         pay_text = pay_kill_text[rnd.rnd(1, #pay_kill_text)]
      else
         pay_text = pay_capture_text[rnd.rnd(1, #pay_capture_text)]
      end
      tk.msg("", pay_text:format(name))
      player.pay(credits)
      paying_faction:modPlayer(reputation)
      misn.finish(true)
   end
end


function pilot_disable ()
   for i, j in ipairs(pilot.get()) do
      j:taskClear()
   end
end


function pilot_board(p, boarder)
   if boarder == player.pilot() then
      local t = subdue_text[rnd.rnd(1, #subdue_text)]:format(name)
      tk.msg("", t)
      succeed()
      target_killed = false
      target_ship:changeAI("dummy")
      target_ship:setHilight(false)
      target_ship:disable() -- Stop it from coming back
      hook.rm(death_hook)
   else
      target_ship:changeAI("dummy")
      target_ship:setHilight(false)
      target_ship:disable()
      fail(_("Another pilot captured your bounty."))
   end
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
      target_killed = true
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
         target_killed = true
      elseif player_hits >= top_hits / 2 and rnd.rnd() < 0.5 then
         hailer = hook.pilot(top_hunter, "hail", "hunter_hail", top_hunter)
         credits = credits * player_hits / total_hits
         reputation = reputation * player_hits / total_hits
         hook.pilot(top_hunter, "jump", "hunter_leave")
         hook.pilot(top_hunter, "land", "hunter_leave")
         hook.jumpout("hunter_leave")
         hook.land("hunter_leave")
         player.msg("#r" .. fmt.f(fail_beaten_text, {pilot=name}) .. "#0")
         hook.timer(3, "timer_hail", top_hunter)
         misn.osdDestroy()
      else
         fail(fmt.f(fail_beaten_text, {pilot=name}))
      end
   end
end


function pilot_jump()
   fail(fmt.f(fail_escape_text, {pilot=name}))
end


function timer_hail(arg)
   if arg ~= nil and arg:exists() then
      arg:hailPlayer()
   end
end


function hunter_hail(arg)
   hook.rm(hailer)
   hook.rm(rehailer)
   player.commClose()

   local text = share_text[rnd.rnd(1, #share_text)]
   tk.msg("", text:format(name))

   player.pay(credits)
   paying_faction:modPlayer(reputation)
   misn.finish(true)
end


function hunter_leave ()
   misn.finish(false)
end


-- Set up the ship, credits, and reputation based on the level.
function bounty_setup ()
   if level == 1 then
      ship = "Hyena"
      credits = 50000 + rnd.sigma() * 15000
      reputation = 0
   elseif level == 2 then
      ship = "Pirate Shark"
      credits = 150000 + rnd.sigma() * 50000
      reputation = 0
   elseif level == 3 then
      if rnd.rnd() < 0.5 then
         ship = "Pirate Vendetta"
      else
         ship = "Pirate Ancestor"
      end
      credits = 250000 + rnd.sigma() * 80000
      reputation = 1
   elseif level == 4 then
      if rnd.rnd() < 0.5 then
         ship = "Pirate Admonisher"
      else
         ship = "Pirate Phalanx"
      end
      credits = 700000 + rnd.sigma() * 120000
      reputation = 2
   elseif level == 5 then
      ship = "Pirate Kestrel"
      credits = 1200000 + rnd.sigma() * 200000
      reputation = 4
   end
end


-- Spawn the ship at the location param.
function spawn_pirate(param)
   if job_done then
      return
   end

   if system.cur() == missys then
      if jumps_permitted >= 0 then
         misn.osdActive(2)
         target_ship = pilot.add(ship, pirate_faction, param)
         target_ship:rename(name)
         target_ship:setVisplayer()
         target_ship:setHilight()
         target_ship:setHostile()
         hook.pilot(target_ship, "disable", "pilot_disable")
         hook.pilot(target_ship, "board", "pilot_board")
         hook.pilot(target_ship, "attacked", "pilot_attacked")
         death_hook = hook.pilot(target_ship, "death", "pilot_death")
         pir_jump_hook = hook.pilot(target_ship, "jump", "pilot_jump")
         pir_land_hook = hook.pilot(target_ship, "land", "pilot_jump")
      else
         fail(fmt.f(fail_escape_text, {pilot=name}))
      end
   else
      local needed = system.cur():jumpDist(missys, true)
      if needed ~= nil and needed > jumps_permitted then
         -- Mission is now impossible to beat and therefore in a sort of
         -- "zombie" state, so immediately fail the mission. We use the
         -- "beaten" fail text here because the player isn't inside the
         -- system, so they wouldn't know that the pirate is no longer
         -- there, but they would know if someone else did the job.
         fail(fmt.f(fail_beaten_text, {pilot=name}))
      end
   end
end


-- Succeed the mission, make the player head to a planet for pay
function succeed ()
   job_done = true
   misn.osdActive(3)
   misn.markerRm(marker)
   hook.rm(pir_jump_hook)
   hook.rm(pir_land_hook)
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
