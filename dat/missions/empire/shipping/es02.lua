--[[
<?xml version='1.0' encoding='utf8'?>
<mission name="Empire Shipping 3">
 <flags>
  <unique />
 </flags>
 <avail>
  <priority>10</priority>
  <cond>faction.playerStanding("Empire") &gt;= 10 and faction.playerStanding("Dvaered") &gt;= 0 and faction.playerStanding("FLF") &lt; 10</cond>
  <chance>50</chance>
  <done>Empire Shipping 2</done>
  <location>Bar</location>
  <planet>Halir</planet>
 </avail>
 <notes>
  <campaign>Empire Shipping</campaign>
 </notes>
</mission>
--]]
--[[

   Empire VIP Rescue

   Author: bobbens
      minor edits by Infiltrator
      - Mission fixed to suit big systems (Anatolis, 11/02/2011)

   Rescue a VIP stranded on a disabled ship in a system while FLF and Dvaered
    are fighting.

   Stages:

      0) Go to sector.
      1) Board ship and rescue VIP.
      2) Rescued VIP, returning to base.
      3) VIP died or jump out of system without VIP  --> mission failure.
]]--

local fleet = require "fleet"
require "numstring"
require "missions/empire/common"

-- Mission details
bar_desc = _("Commander Soldner is waiting for you.")
misn_title = _("Empire VIP Rescue")
misn_desc = {}
misn_desc[1] = _("Board the transport ship in the %s system to rescue the VIP")
misn_desc[2] = _("Land on %s (%s system) with the VIP")

text = {}
text[1] = _([[You meet up once more with Commander Soldner at the bar.

"Hello again, %s. Still interested in doing another mission? This one will be more dangerous."]])
text[2] = _([[Commander Soldner nods and continues, "We've had reports that a transport vessel came under attack while transporting a VIP. They managed to escape, but the engine ended up giving out in the %s system. The ship is now disabled and we need someone to board the ship and rescue the VIP. There have been many FLF ships detected near the sector, but we've managed to organise a Dvaered escort for you.

"You're going to have to fly to the %s system, find and board the transport ship to rescue the VIP, and then fly back. The sector is most likely going to be hot. That's where your Dvaered escorts will come in. Their mission will be to distract and neutralise all possible hostiles. You must not allow the transport ship to be destroyed before you rescue the VIP. His survival is vital."]])
text[3] = _([["Be careful with the Dvaered; they can be a bit blunt, and might accidentally destroy the transport ship. If all goes well, you'll be paid %s when you return with the VIP. Good luck, pilot."]])
text[4] = _([[The ship's hatch opens and immediately an unconscious VIP is brought aboard by his bodyguard. Looks like there is no one else aboard.]])
text[5] = _([[You land at the starport. It looks like the VIP has already recovered. He thanks you profusely before heading off. You proceed to pay Commander Soldner a visit. He seems to be happy, for once.

"It seems like you managed to pull it off. I had my doubts at first, but you've proven to be a very skilled pilot. We have nothing more for you now, but check in periodically in case something comes up for you."]])

msg = {}
msg[1] = _("MISSION FAILED: VIP is dead.")
msg[2] = _("MISSION FAILED: You abandoned the VIP.")

log_text_success = _([[You successfully rescued a VIP for the Empire.]])


function create ()
   -- Target destination
   destsys     = system.get("Slaccid")
   ret,retsys  = planet.getLandable("Halir")
   if ret== nil then
      misn.finish(false)
   end

   -- Must claim system
   if not misn.claim(destsys) then
      misn.finish(false)
   end

   -- Add NPC.
   misn.setNPC(_("Soldner"), "empire/unique/soldner.png", bar_desc)
end


function accept ()
   -- Intro text
   if not tk.yesno("", string.format(text[1], player.name())) then
      misn.finish()
   end

   -- Accept the mission
   misn.accept()

   -- Set marker
   misn_marker = misn.markerAdd(destsys, "low")

   -- Mission details
   misn_stage = 0
   reward = 750e3
   misn.setTitle(misn_title)
   misn.setReward(creditstring(reward))
   misn.setDesc(string.format(misn_desc[1], destsys:name()))

   -- Flavour text and mini-briefing
   tk.msg("", string.format(text[2], destsys:name(), destsys:name()))
   tk.msg("", string.format(text[3], creditstring(reward)))
   misn.osdCreate(misn_title, {misn_desc[1]:format(destsys:name())})
   -- Set hooks
   hook.land("land")
   hook.enter("enter")
   hook.jumpout("jumpout")

   -- Initiate mission variables (A.)
   prevsys = system.cur()
end


function land ()
   landed = planet.cur()

   if landed == ret then
      -- Successfully rescued the VIP
      if misn_stage == 2 then
         player.pay(reward)
         faction.modPlayer("Empire", 5)

         tk.msg("", text[5])

         emp_addShippingLog(log_text_success)
         misn.finish(true)
      end
   end
end


function enter ()
   sys = system.cur()

   if misn_stage == 0 and sys == destsys then
      -- Force FLF combat music (note: must clear this later on).
      var.push("music_combat_force", "FLF")

      -- Put the VIP a ways off of the player but near the jump.
      enter_vect = jump.pos(sys, prevsys)
      m,a = enter_vect:polar()
      enter_vect:setP(m-3000, a)
      v = pilot.add("Gawain", "Trader", enter_vect, _("Trader Gawain"), {ai="dummy"})

      v:setPos(enter_vect)
      v:setVel(vec2.new(0, 0)) -- Clear velocity
      v:disable()
      v:setHilight(true)
      v:setVisplayer(true)
      v:memory().noboard = true
      v:setFaction("Empire")
      v:rename(_("VIP"))
      hook.pilot(v, "board", "board")
      hook.pilot(v, "death", "death")

      -- FLF Spawn around the Gawain
      p = fleet.add({2, 1, 1, 1},
            {"Hyena", "Lancelot", "Vendetta", "Pacifier"}, "FLF", enter_vect,
            {_("FLF Hyena"), _("FLF Lancelot"), _("FLF Vendetta"),
               _("FLF Pacifier")})
      for k,v in ipairs(p) do
         v:setHostile()
      end
      
      -- Now Dvaered
      -- They will jump together with you in the system at the jump point. (A.)
      p = fleet.add({2, 2, 1, 1},
            {"Dvaered Vendetta", "Dvaered Ancestor", "Dvaered Phalanx",
               "Dvaered Vigilance"},
            "Dvaered", prevsys)
      for k,v in ipairs(p) do
         v:setFriendly()
      end

      -- Add more ships on a timer to make this messy
      hook.timer(rnd.rnd(3, 5) , "delay_flf")

      -- Pass to next stage
      misn_stage = 1

   -- Can't run away from combat
   elseif misn_stage == 1 then
      -- Notify of mission failure
      player.msg(msg[2])
      var.pop("music_combat_force")
      misn.finish(false)
   end
end


function jumpout ()
   -- Storing the system the player jumped from.
   prevsys = system.cur()

   if prevsys == destsys then
      var.pop("music_combat_force")
   end
end


function delay_flf ()
   if misn_stage ~= 0 then
      return
   end

   -- More ships to pressure player from behind
   p = fleet.add(1, {"Hyena", "Lancelot", "Vendetta"}, "FLF", prevsys,
         {_("FLF Hyena"), _("FLF Lancelot"), _("FLF Vendetta")})
   for k,v in ipairs(p) do
      v:setHostile()
   end
end


function board(p, boarder)
   if boarder ~= player.pilot() then
      return
   end
   player.unboard()
   tk.msg("", text[4])

   misn_stage = 2
   misn.markerMove(misn_marker, retsys)
   misn.setDesc(string.format(misn_desc[2], ret:name(), retsys:name()))
   misn.osdCreate(misn_title, {misn_desc[2]:format(ret:name(),retsys:name())})
end


function death ()
   if misn_stage == 1 then
      -- Notify of death
      player.msg(msg[1])
      var.pop("music_combat_force")
      misn.finish(false)
   end
end


function abort ()
   -- If aborted you'll also leave the VIP to fate. (A.)
   player.msg(msg[2])
   if system.cur() == destsys then
      var.pop("music_combat_force")
   end
   misn.finish(false)
end
