--[[
<?xml version='1.0' encoding='utf8'?>
<mission name="Racing Skills 1">
 <flags>
   <unique />
 </flags>
 <avail>
  <priority>50</priority>
  <cond>
   planet.cur():class() ~= "1"
   and planet.cur():class() ~= "2"
   and planet.cur():class() ~= "3"
   and system.cur():presence("Civilian") &gt; 0
   and system.cur():presence("Pirate") &lt;= 0
   and (var.peek("tut_complete") == true
      or planet.cur():faction() ~= faction.get("Empire"))
  </cond>
  <chance>100</chance>
  <location>Bar</location>
  <faction>Dvaered</faction>
  <faction>Empire</faction>
  <faction>Frontier</faction>
  <faction>Goddard</faction>
  <faction>Independent</faction>
  <faction>Sirius</faction>
  <faction>Soromid</faction>
  <faction>Za'lek</faction>
 </avail>
</mission>
--]]
--[[
--
-- MISSION: Racing Skills 1
-- DESCRIPTION: A person asks you to join a race, where you fly to various checkpoints and board them before landing back at the starting planet
--
--]]

local fmt = require "fmt"
local mh = require "misnhelper"
local pilotname = require "pilotname"
local portrait = require "portrait"


ask_text = _([["Hiya there! We're having a race around this system system soon and need a 4th person to participate. There's a prize of {credits} if you win. Interested?"]])

yes_text = _([["That's great! I'll explain how it works. Once we take off from {planet}, there will be a countdown, and then we will proceed to the various checkpoints in order, finishing off by landing on {planet} at the end. Let's have some fun!"]])

checkpoint_text = _("Checkpoint {prev} reached. Proceed to Checkpoint {next}.")

checkpoint_final_text = _("Checkpoint {prev} reached. Land on {planet}.")

wintext = _([[The laid back person comes up to you and hands you a credit chip. 

"Nice racing! Here's your prize money. Let's race again sometime soon!"]])

fail_left_text = _([["Because you left the race, you have been disqualified."]])

lose_text = _([[As you congratulate the winner on a great race, the laid back person comes up to you.

"That was a lot of fun! If you ever have time, let's race again. Maybe you'll win next time!"]])

NPCname = _("A laid back person")
NPCdesc = _("You see a laid back person, who appears to be one of the locals, looking around the bar.")

misndesc = _("You're participating in a race!")

OSDtitle = _("Racing Skills")

chatter = {}
chatter[1] = _("Let's do this!")
chatter[2] = _("Wooo!")
chatter[3] = _("Time to Shake 'n Bake")
chatter[4] = _("Checkpoint %s baby!")
chatter[5] = _("Hooyah")
chatter[6] = _("Next!")
timermsg = "%s"
target = {1,1,1,1}

positionmsg = _("%s just reached checkpoint %s")
landmsg = _("%s just landed at %s and finished the race")


function create ()
   curplanet, missys = planet.cur()

   -- Get planets, excluding the current planet, and jumps.
   local all_planets = missys:planets()
   local planets = {}
   for i, pnt in ipairs(all_planets) do
      if pnt ~= curplanet then
         planets[#planets + 1] = pnt
      end
   end
   local jumps = missys:jumps()

   -- Define points.
   -- XXX: points contains a mixture of planets and jump points, since
   -- vec2s can't be saved directly. Both planets and jumps have a pos()
   -- member, so this is OK as long as we're extra-careful.
   local numpoints = math.min(3, #planets + #jumps)
   points = {}
   points.__save = true
   radii = {}
   radii.__save = true
   while #points < numpoints and #planets + #jumps > 0 do
      local i = rnd.rnd(1, #planets + #jumps)
      local point
      if i <= #planets then
         local pnt = table.remove(planets, i)
         points[#points + 1] = {pnt:pos():get()}
         radii[#radii + 1] = pnt:radius()
      else
         local jp = table.remove(jumps, i - #planets)
         points[#points + 1] = {jp:pos():get()}
         radii[#radii + 1] = 200
      end
   end

   -- Add the current planet as the last point.
   points[#points + 1] = {curplanet:pos():get()}
   radii[#radii + 1] = curplanet:radius()

   if #points < 3 then
      misn.finish(false)
   end

   -- Calculate total distance.
   local dist = 0
   local last_pos = vec2.new(table.unpack(points[#points]))
   for i, point in ipairs(points) do
      local pos = vec2.new(table.unpack(point))
      dist = dist + vec2.dist(last_pos, pos)
      last_pos = pos
   end

   -- Choose number of laps.
   laps = 1

   -- Calculate reward.
   credits = #points*5000 + dist*(1.75^(laps-1))
   credits = credits * (1 + 0.05*rnd.twosigma())

   misn.setNPC(NPCname, portrait.get(curplanet:faction()), NPCdesc)
end


function accept ()
   if tk.yesno("", fmt.f(ask_text, {credits=fmt.credits(credits)})) then
      tk.msg("", fmt.f(yes_text, {planet=curplanet:name()}))

      misn.accept()

      race_started = false
      next_point = 1
      current_lap = 1
      racers_landed = 0

      misn.setTitle(OSDtitle)
      misn.setDesc(misndesc)
      misn.setReward(fmt.credits(credits))

      gen_osd()

      hook.takeoff("takeoff")
   else
      misn.finish()
   end
end


function gen_osd()
   local osd_desc = {
      fmt.f(_("Wait for Race Announcer's signal near {planet} ({system} system)"),
         {planet=curplanet:name(), system=missys:name()}),
      string.format(n_("Go to Checkpoint %d, indicated on overlay map",
            "Go to Checkpoint %d, indicated on overlay map", next_point),
         next_point),
      fmt.f(n_("Lap {current_lap:d}/{total_laps:d}",
            "Lap {current_lap:d}/{total_laps:d}", current_lap),
         {current_lap=current_lap, total_laps=laps}),
      fmt.f(_("Land on {planet} ({system} system)"),
         {planet=curplanet:name(), system=missys:name()}),
   }
   misn.osdCreate(OSDtitle, osd_desc)

   if race_started then
      system.mrkRm(mark)
      if current_lap >= laps and next_point >= #points then
         misn.osdActive(4)
         misn.markerAdd(missys, "high", curplanet)
      else
         misn.osdActive(2)
         local pos = vec2.new(table.unpack(points[next_point]))
         mark = system.mrkAdd(
            string.format(n_("Checkpoint %d", "Checkpoint %d", next_point),
               next_point),
            pos)
      end
   elseif not player.isLanded() then
      system.mrkRm(mark)
      local ppos = player.pilot():pos()
      local pos = vec2.new(table.unpack(points[#points]))
      local radius = radii[#radii]
      if vec2.dist(ppos, pos) > radius then
         mark = system.mrkAdd(_("Starting Point"), pos)
      else
         local pos = vec2.new(table.unpack(points[next_point]))
         mark = system.mrkAdd(
            string.format(n_("Checkpoint %d", "Checkpoint %d", next_point),
               next_point),
            pos)
      end
   end
end


function takeoff()
   gen_osd()

   local f = faction.dynAdd(nil, N_("Referee"), nil, {ai="stationary"})
   local angle = rnd.rnd() * 2 * math.pi
   local radius = 1.5 * curplanet:radius()
   local pos = (curplanet:pos()
         + vec2.new(math.cos(angle) * radius, math.sin(angle) * radius))
   referee = pilot.add("Llama", f, pos, _("Race Announcer"))
   referee:setInvincible()
   referee:setVisible()
   referee:setNoClear()

   racers = {
      pilot.add("Llama", "Civilian", curplanet, pilotname.generic()),
      pilot.add("Llama", "Civilian", curplanet, pilotname.generic()),
      pilot.add("Llama", "Civilian", curplanet, pilotname.generic()),
   }
   local face_target = vec2.new(table.unpack(points))
   for i, p in ipairs(racers) do
      p:setInvincible()
      p:setVisible()
      p:setHilight()
      p:setNoClear()
      p:control()
      p:face(face_target, true)

      local mem = p:memory()
      mem.race_points = points
      mem.race_radii = radii
      mem.race_laps = laps
      mem.race_next_point = next_point
      mem.race_current_lap = current_lap
      mem.race_land_dest = curplanet

      hook.pilot(p, "land", "racer_land")
      hook.pilot(p, "jump", "racer_jump")

      if i == 1 then
         p:broadcast(_("Let's do this!"))
      end
   end

   countdown_hook = hook.timer(3, "timer_ready")
   hook.timer(1/20, "timer_check_status")
   hook.custom("race_racer_next_point", "racer_next_point")
   hook.custom("race_racer_next_lap", "racer_next_lap")
   hook.jumpout("jumpout")
   hook.land("land")
end


function timer_ready()
   referee:broadcast(_("Alright, take your places, everyone!"))
   countdown_hook = hook.timer(3, "timer_countdown", 5)
end


function timer_countdown(count)
   if count > 0 then
      referee:broadcast(string.format(p_("race_countdown", "%d…"), count))
      countdown_hook = hook.timer(1, "timer_countdown", count - 1)
   else
      referee:broadcast(p_("race_countdown", "Go!"))
      race_started = true
      gen_osd()
      for i, p in ipairs(racers) do
         local mem = p:memory()
         local pos = vec2.new(
            table.unpack(mem.race_points[mem.race_next_point]))
         p:moveto(pos, false)

         hook.pilot(p, "idle", "racer_idle")
      end
   end
end


function timer_check_status()
   if race_started then
      -- Record the player's progress thru the race.
      local npoints = #points
      local pos = vec2.new(table.unpack(points[next_point]))
      local radius = radii[next_point]
      if (current_lap < laps or next_point < npoints)
            and vec2.dist(player.pos(), pos) <= radius then
         next_point = next_point + 1
         if current_lap >= laps and next_point >= npoints then
            -- Final step.
            player.msg(fmt.f(_("Land on {planet}."),
                  {planet=curplanet:name()}))
         elseif next_point > npoints then
            -- Next lap.
            next_point = next_point - npoints
            current_lap = current_lap + 1

            local tt = {
               string.format(
                  n_("Beginning Lap %d.", "Beginning Lap %d.", current_lap),
                  current_lap),
               string.format(
                  n_(" Proceed to Checkpoint %d.",
                     " Proceed to Checkpoint %d.", next_point),
                  next_point),
            }
            local text = table.concat(tt)
            player.msg(text)
         else
            -- Next checkpoint.
            local tt = {
               string.format(
                  n_("Reached Checkpoint %d.", "Reached Checkpoint %d.",
                     next_point - 1),
                  next_point - 1),
               string.format(
                  n_(" Proceed to Checkpoint %d.",
                     " Proceed to Checkpoint %d.", next_point),
                  next_point),
            }
            local text = table.concat(tt)
            player.msg(text)
         end
         gen_osd()
      end
   else
      -- Check to see if the player has left or re-enterd the start
      -- point and stop or restart the countdown as needed.
      local dist = vec2.dist(player.pos(), curplanet:pos())
      local radius = curplanet:radius()
      if countdown_hook ~= nil and dist > radius then
         referee:comm(player.pilot(),
               fmt.f(_("Wait, {player}, I didn't give the signal yet! Come back to the starting point."),
                  {player=player.name()}),
               true)
         hook.rm(countdown_hook)
         countdown_hook = nil
         gen_osd()
      elseif countdown_hook == nil and dist <= radius then
         referee:broadcast(_("Alright, let's try this again. Take your places, everyone!"))
         countdown_hook = hook.timer(3, "timer_countdown", 5)
         gen_osd()
      end
   end

   hook.timer(1/20, "timer_check_status")
end


function racer_idle(p)
   local mem = p:memory()

   -- Record the racer's progress thru the race.
   local pos = vec2.new(table.unpack(mem.race_points[mem.race_next_point]))
   local radius = mem.race_radii[mem.race_next_point]
   if mem.race_current_lap >= mem.race_laps
         and mem.race_next_point >= #mem.race_points then
      p:land(mem.race_land_dest)
   elseif vec2.dist(p:pos(), pos) <= radius then
      mem.race_next_point = mem.race_next_point + 1
      if mem.race_current_lap >= mem.race_laps
            and mem.race_next_point >= #mem.race_points then
         -- Final step.
         p:land(mem.race_land_dest)
      elseif mem.race_next_point > #mem.race_points then
         -- Next lap.
         mem.race_next_point = mem.race_next_point - #mem.race_points
         mem.race_current_lap = mem.race_current_lap + 1

         local pos = vec2.new(
            table.unpack(mem.race_points[mem.race_next_point]))
         p:moveto(pos, false)

         hook.trigger("race_racer_next_lap", p, mem.race_current_lap)
      else
         local pos = vec2.new(
            table.unpack(mem.race_points[mem.race_next_point]))
         p:moveto(pos, false)

         hook.trigger("race_racer_next_point", p, mem.race_next_point - 1)
      end
   else
      p:moveto(pos, false)
   end
end


function racer_land(p, land_planet)
   local mem = p:memory()
   if mem.race_next_point < #mem.race_points
         or mem.race_current_lap < mem.race_laps then
      return
   end
   racers_landed = racers_landed + 1
end


function racer_jump(p, jp)
end


function racer_next_point(p, point)
end


function racer_next_lap(p, lap)
end


function jumpout()
   mh.showFailMsg(_("You left the race."))
   misn.finish(false)
end


function land()
   if current_lap >= laps and next_point >= #points then
      -- Consider the mission a "success" (meaning it generates the
      -- race2 mission) even if the player loses.
      if racers_landed <= 0 then
         tk.msg("", wintext)
         player.pay(credits)
      else
         tk.msg("", lose_text)
      end
      misn.finish(true)
   else
      tk.msg("", fail_left_text)
      misn.finish(false)
   end
end


function abort()
end
