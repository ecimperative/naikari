--[[
<?xml version='1.0' encoding='utf8'?>
<mission name="Tutorial Part 3">
 <flags>
  <unique />
 </flags>
 <avail>
  <priority>1</priority>
  <chance>100</chance>
  <location>Bar</location>
  <planet>Em 1</planet>
  <done>Tutorial Part 2</done>
 </avail>
</mission>
--]]
--[[

   Tutorial Part 3

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

   MISSION: Tutorial Part 3
   DESCRIPTION: Player mines ore from asteroids, then takes it to Em 5.

--]]

local fmt = require "fmt"
require "proximity"
require "events/tutorial/tutorial_common"
require "missions/neutral/common"


ask_text = _([["Hello again, {player}! Are you ready for that other job I had for you? It's a little more difficult than the last job, but I promise the reward is worth it."]])

accept_text = _([["Perfect! Alright, this time we will be going into space; I will be joining you. I will make my way to your ship now and wait for you there. When you're ready to begin, press the Take Off button."]])

overlay_text = _([["It's been a long time since I've been in space!" Albert notes with amusement. "Alright, the reason we're here is I need you to mine some Ore for me. Of course, I could buy it from the Commodity tab, but for my purposes I require specifically asteroid-mined Ore.

"To that end, I've marked an asteroid field in this system on your ship's overlay map. Could you press {overlaykey} to open your overlay map so I can show you, please?"]])

autonav_text = _([["Thank you! As you can see, I've marked an area with the label, 'Asteroid Field'. I need you to #bright-click#0 that area so we can be taken there as fast as possible with Autonav. I mean, of course, the trip will take the same duration in real-time, but giving the work to Autonav will allow you to leave the captain's chair and do other things; it makes the time passage feel almost instantaneous.

"Here, I brought a nice game we can play while Autonav takes us to the asteroid field! It's a sandbox game where you control a space trader coming up in a time of turmoil, taking odd jobs, acquiring ships, and hiring pilots as you build your fortune. As the game progresses, you have a chance to aid various factions to win their favor and decide the ultimate fate of the galaxy amid chaos as the Galactic Federation loses its grip on power! Sounds fun, right?"]])

mining_text = _([[Alright, let's start mining! Of course, mining is done with the same weaponry you would use to defend yourself against pirates should you be attacked. All you have to do is click on a suitable asteroid to target it, then use {primarykey} and {secondarykey} to fire your weapons and destroy the targeted asteroid. Once it's destroyed, it will drop some commodity, usually ore, and you can pick the commodity up simply by flying over its location.

"I just need you to mine enough ore to fill your cargo hold to capacity. I'll let you know what to do with it when your cargo hold is full."]])

cooldown_text = _([["That should do it! Great job! Whew, firing those weapons sure makes it warm in here, doesn't it? I'm sorry to trouble you, but could you please engage active cooldown by pressing {autobrake_key} twice?"]])

dest_text = _([["Ahhh, that's much better. I'm sure your weapons will work a lot better if they're not overheated, too, though I suppose the chance of running into pirates in this system is quite low.

"Alright, I need you to take this Ore to {planet}. You should be able to see it on your overlay map, right? You can interact with its icon the same way you would interact with the planet itself, so you should be able to tell Autonav to land us there no problem."]])

pay_text = _([[Albert steps out of your ship and stretches his arms, seemingly happy to be back in atmosphere. "Thank you for your service once again," he says as he hands you a credit chip with your payment. "If you like, I have one more job for you. Meet me at the bar in a bit."]])

misn_desc = _("Albert needs you to mine ore from some asteroids.")
misn_log = _([[You accepted another job from Albert, this time mining some ore from asteroids for him. He asked you to speak with him again on {planet} ({system} system) for another job.]])


function create()
   -- Note: This mission makes no system claims.
   misplanet, missys = planet.get("Em 5")
   credits = 15000

   misn.setNPC(_("Albert"),
         "neutral/unique/youngbusinessman.png",
         _("Albert is idling at the bar. He said he has another job for you."))
end


function accept()
   if tk.yesno("", fmt.f(ask_text, {player=player.name()})) then
      tk.msg("", fmt.f(accept_text, {player=player.name()}))

      misn.accept()

      misn.setTitle(_("Albert's Supplies"))
      misn.setReward(fmt.credits(credits))
      misn.setDesc(misn_desc)

      local osd_desc = {
         _("Press the Take Off button to go into space"),
         fmt.f(_("Press {overlaykey} to open your overlay map"),
            {overlaykey=naev.keyGet("overlay")}),
         _("Fly to Asteroid Field indicated on overlay map by right-clicking the area"),
         fmt.f(_("Use {primarykey} and {secondarykey} to fire your weapons and destroy asteroids, then collect Ore from the destroyed asteroids until your cargo hold is full"),
            {primarykey=naev.keyGet("primary"),
               secondarykey=naev.keyGet("secondary")}),
         fmt.f(_("Engage Active Cooldown by pressing {autobrake_key} twice, then wait for your ship to fully cool down"),
            {autobrake_key=naev.keyGet("autobrake")}),
         fmt.f(_("Land on {planet} ({system} system)"),
            {planet=misplanet, system=missys}),
      }
      misn.osdCreate(_("Albert's Supplies"), osd_desc)

      hook.enter("enter")
   else
      misn.finish()
   end
end


function enter()
   hook.timer(1, "timer_enter")
end


function timer_enter()
   player.allowLand(false)
   player.pilot():setNoJump(true)

   tk.msg("", fmt.f(overlay_text,
            {player=player.name(), overlaykey=tutGetKey("overlay")}))

   misn.osdActive(2)

   local pos = vec2.new(18000, -1200)
   mark = system.mrkAdd(_("Asteroid Field"), pos)

   input_hook = hook.input("input", "overlay")
   hook.timer(0.5, "proximity",
         {location=pos, radius=2500, funcname="asteroid_proximity"})
end


function input(inputname, inputpress, arg)
   if not inputpress then
      return
   end

   if inputname ~= arg then
      return
   end

   hook.safe(string.format("safe_%s", arg))
   hook.rm(input_hook)
end


function safe_overlay()
   tk.msg("", autonav_text)
   misn.osdActive(3)
end


function asteroid_proximity()
   hook.rm(input_hook)
   system.mrkRm(mark)

   tk.msg("", fmt.f(mining_text,
         {primarykey=tutGetKey("primary"),
            secondarykey=tutGetKey("secondary")}))
   misn.osdActive(4)

   hook.timer(0.5, "timer_mining")
end


function timer_mining()
   if player.pilot():cargoFree() > 0 then
      hook.timer(0.5, "timer_mining")
      return
   end

   hook.rm(input_hook)
   system.mrkRm(mark)

   tk.msg("", fmt.f(cooldown_text, {autobrake_key=tutGetKey("autobrake")}))
   misn.osdActive(5)

   hook.timer(1, "timer_cooldown")
end


function timer_cooldown()
   if player.pilot():temp() > 250 then
      hook.timer(1, "timer_cooldown")
      return
   end

   player.allowLand(true)
   player.pilot():setNoJump(false)

   tk.msg("", fmt.f(dest_text, {planet=misplanet:name()}))
   misn.osdActive(6)

   hook.land("land")
end


function land()
   if planet.cur() ~= misplanet then
      return
   end

   tk.msg("", pay_text)

   local ftonnes = player.pilot():cargoHas("Ore")
   player.pilot():cargoRm("Ore", ftonnes)
   player.pay(credits)

   addMiscLog(fmt.f(misn_log,
         {planet=misplanet:name(), system=missys:name()}))
   misn.finish(true)
end


function abort()
   if system.cur() == missys then
      player.allowLand(true)
      player.pilot():setNoJump(false)
      system.mrkRm(mark)
   end
   misn.finish(false)
end
