--[[
<?xml version='1.0' encoding='utf8'?>
<mission name="Hot dogs from space">
 <flags>
  <unique />
 </flags>
 <avail>
  <priority>20</priority>
  <chance>20</chance>
  <location>Bar</location>
  <faction>Dvaered</faction>
  <faction>Empire</faction>
  <faction>Frontier</faction>
  <faction>Goddard</faction>
  <faction>Independent</faction>
  <faction>Sirius</faction>
  <faction>Soromid</faction>
  <faction>Za'lek</faction>
  <cond>
   (function ()
      local count = 0
      for i, p in pairs(system.cur():planets()) do
         if p:services()["inhabited"] then
            count=count+1
         end
      end
      return count &gt; 1
   end)()
   and (player.misnDone("Tutorial Part 4")
      or (system.cur() ~= system.get("Hakoi")
         and system.cur() ~= system.get("Eneguoz")))
  </cond>
 </avail>
</mission>
--]]
--[[

   MISSION: Hot dogs from space
   DESCRIPTION: An old man who owns a hot dog factory wants to go to space

   The old man has elevated pressure in his cochlea so he can't go to space.
   He's getting old and wants to go to space before he dies. He owns a hot dog
   factory and will pay you in hot dogs (food). Because of his illness, you
   can't land on any planet outside the system (the player doesn't know this).

   NOTE: This mission is best suited in systems with 2 or more planets, but can
   be used in any system with a planet.

--]]

require "numstring"
require "missions/neutral/common"


-- Bar information, describes how he appears in the bar
bar_desc = _("You see an old man with a cap on, on which the letters R-E-Y-N-I-R are imprinted.")

-- Mission details. We store some text for the mission with specific variables.
misn_title = _("Rich reward from space!")
misn_reward = _("Lots of cash")
misn_desc = _("Reynir wants to travel to space and will reward you richly.")

cargoname = "Food"

-- Stage one
text = {}      --We store mission text in tables.  As we need them, we create them.

text[1] = _([["Do you like money?"]])     --Use double brackets _([[]]) for block quotes over several lines.

text[2] = _([["Ever since I was a kid I've wanted to go to space. However, my doctor says I can't go to space because I have an elevated pressure in my cochlea, a common disease around here.

"I am getting old now, as you can see. Before I die I want to travel to space, and I want you to fly me there! I own a hot dog factory, so I can reward you richly! Will you do it?"]])
text[3] = _([["Thank you so much! Just fly me around in the system, preferably near %s."]])

text[4] = _([[Reynir walks out of the ship. You notice that he's bleeding out of both ears. "Where have you taken me?! Get me back to %s right now!!"]])
text[5] = _([["Thank you so much! Here's %s of hot dogs. They're worth more than their weight in gold, aren't they?"]])
text[6] = _([[Reynir walks out of the ship, amazed by the view. "So this is what %s looks like! I've always wondered. I want to go back to %s now, please."]])
text[7] = _([[Reynir doesn't look happy when you meet him outside the ship.

"I lost my hearing out there! Damn you!! I made a promise, though, so I'd better keep it. Here's your reward, %s of hot dogs..."]])

-- Other text for the mission -- ??
osd_msg = {}
osd_msg[1] = _("Fly around in the system, preferably near %s")
osd_msg[2] = _("Land on %s (%s system)")

log_text_good = _([[You took an old man named Reynir on a ride in outer space. He was happy and paid you in the form of %s of hot dogs.]])
log_text_bad = _([[You took an old man named Reynir on a ride in outer space, but he was made very angry because the distance you traveled led to him getting injured and losing his hearing. Still, he begrudgingly paid you in the form of %s of hot dogs.]])


function create ()
   -- Note: this mission does not make any system claims.
   misn.setNPC(_("Reynir"), "neutral/unique/reynir.png", bar_desc)

   -- Mission variables
   misn_base, misn_base_sys = planet.cur()
   misn_bleeding = false
end


function accept ()
   if tk.yesno("", text[1]) and tk.yesno("", text[2]) then
      misn.accept()

      misn.setTitle(misn_title)
      misn.setReward(misn_reward)
      misn.setDesc(misn_desc)
      hook.land("landed")

      tk.msg("", string.format(text[3], misn_base:name()))
      misn.osdCreate(misn_title, {osd_msg[1]:format(misn_base:name())})
      local commod = misn.cargoNew(N_("Reynir"), N_("An old man named Reynir who wanted to fly in space."))
      cargoID = misn.cargoAdd(commod, 0)
   end

end


function landed()
   -- If landed on misn_base then give reward
   if planet.cur() == misn_base then
      misn.cargoRm(cargoID)
      if misn_bleeding then
         reward = math.min(1, player.pilot():cargoFree())
         reward_text = text[7]
         log_text = log_text_bad
      else
         reward = player.pilot():cargoFree()
         reward_text = text[5]
         log_text = log_text_good
      end
      tk.msg("", string.format(reward_text, tonnestring(reward)))
      player.pilot():cargoAdd(cargoname, reward)
      addMiscLog(log_text:format(tonnestring(reward)))
      misn.finish(true)
   -- If we're in misn_base_sys but not on misn_base then...
   elseif system.cur() == misn_base_sys then
      tk.msg("", string.format(text[6], planet.cur():name(), misn_base:name()))
      misn.osdCreate(misn_title,
            {osd_msg[2]:format(misn_base:name(), misn_base_sys:name())})
   -- If we're in another system then make Reynir bleed out his ears ;)
   else
      tk.msg("", string.format(text[4], misn_base:name()))
      misn.osdCreate(misn_title,
            {osd_msg[2]:format(misn_base:name(), misn_base_sys:name())})
      misn_bleeding = true
   end
end
