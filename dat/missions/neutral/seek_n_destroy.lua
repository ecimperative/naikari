--[[
<?xml version='1.0' encoding='utf8'?>
<mission name="Seek And Destroy">
 <avail>
  <priority>43</priority>
  <cond>player.numOutfit("Mercenary License") &gt; 0</cond>
  <chance>875</chance>
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

   The player searches for an outlaw across several systems

   Stages :
   0) Next system will give a clue
   2) Next system will contain the target
   4) Target was killed

--]]

require "numstring"
require "jumpdist"
local portrait = require "portrait"
require "pilot/generic"
require "pilot/pirate"


clue_text    = {}
clue_text[1] = _("The pilot tells you that %s is supposed to have business in %s soon.")
clue_text[2] = _([["I've heard that %s likes to hang around in %s."]])
clue_text[3] = _([["You can probably catch %s in %s."]])
clue_text[4] = _([["If you're looking for %s, I would suggest going to %s and taking a look there; that's where they were last time I heard."]])
clue_text[5] = _([["If I was looking for %s, I would look in the %s system. That's probably a good bet."]])

dono_text    = {}
dono_text[1] = _("This person has never heard of %s. It seems you will have to ask someone else.")
dono_text[2] = _("This person is also looking for %s, but doesn't seem to know anything you don't.")
dono_text[3] = _([["%s? Nope, I haven't seen that person in years at this point."]])
dono_text[4] = _([["Sorry, I have no idea where %s is."]])
dono_text[5] = _([["Oh, hell no, I stay as far away from %s as I possibly can."]])
dono_text[6] = _([["I haven't a clue where %s is."]])
dono_text[7] = _([["I don't give a damn about %s. Go away."]])
dono_text[8] = _([["%s? Don't know, don't care."]])
dono_text[9] = _("When you ask about %s, you are promptly told to get lost.")
dono_text[10] = _([["I'd love to get back at %s for last year, but I haven't seen them in quite some time now."]])
dono_text[11] = _([["I've not seen %s, but good luck in your search."]])
dono_text[12] = _([["Wouldn't revenge be nice? Unfortunately I haven't a clue where %s is, though."]])
dono_text[13] = _([["I used to work with %s. We haven't seen each other since they stole my favorite ship, though."]])
dono_text[14] = _([["%s has owed me 500 k¢ for over a decade and never paid me back! I have no clue where they are, though."]])

money_text    = {}
money_text[1] = _([["%s, you say? Well, I don't offer my services for free. Pay me %s and I'll tell you where to look; how does that sound?"]])
money_text[2] = _([["Ah, yes, I think I know where %s is. I'll tell you for just %s. What do you say?"]])
money_text[3] = _([["%s? Of course, I know this pilot. I can tell you where they were last heading, but it'll cost you. %s. Deal?"]])
money_text[4] = _([["Ha ha ha! Yes, I've seen %s around! Will I tell you where? Heck no! Not unless you pay me, of course.… %s should be sufficient."]])
money_text[5] = _([["You're looking for %s? I tell you what: give me %s and I'll tell you. Otherwise, get lost!"]])

IdoPay       = _("Pay the sum")
IdonnoPay    = _("Give up")
IkickYourAss = _("Threaten the pilot")

poor_text  = _("You don't have enough money.")

not_scared_text    = {}
not_scared_text[1] = _([["As if the likes of you would ever try to fight me!"]])
not_scared_text[2] = _("The pilot simply sighs and cuts the connection.")
not_scared_text[3] = _([["What a lousy attempt to scare me."]])
not_scared_text[4] = _([["Was I not clear enough the first time? Piss off!"]])

scared_text    = {}
scared_text[1] = _("As it becomes clear that you have no problem with blasting a ship to smithereens, the pilot tells you that %s is supposed to have business in %s soon.")
scared_text[2] = _([["OK, OK, I'll tell you! You can find %s in the %s system! Leave me alone!"]])

cold_text    = {}
cold_text[1] = _("When you ask for information about %s, they tell you that this outlaw has already been killed by someone else.")
cold_text[2] = _([["Didn't you hear? That outlaw's dead. Got blown up in an asteroid field is what I heard."]])
cold_text[3] = _([["Ha ha, you're still looking for that outlaw? You're wasting your time; they've already been taken care of."]])
cold_text[4] = _([["Ah, sorry, that target's already dead. Blown to smithereens by a mercenary. I saw the scene, though! It was glorious."]])

noinfo_text    = {}
noinfo_text[1] = _("The pilot asks you to give them one good reason to give you that information.")
noinfo_text[2] = _([["What if I know where your target is and I don't want to tell you, eh?"]])
noinfo_text[3] = _([["Piss off! I won't tell anything to the likes of you!"]])
noinfo_text[4] = _([["And why exactly should I give you that information?"]])
noinfo_text[5] = _([["And why should I help you, eh? Get lost!"]])

advice_text  = _([["Hi there", says the pilot. "You seem to be lost." As you explain that you're looking for an outlaw pilot and have no idea where to find your target, the pilot laughs. "So, you've taken a Seek and Destroy job, but you have no idea how it works. Well, there are two ways to get information on an outlaw: first way is to land on a planet and ask questions at the bar. The second way is to ask pilots in space. By the way, pilots of the same faction of your target are most likely to have information, but won't give it easily. Good luck with your search!"]])

breef_text = _("%s is a notorious %s pilot who is wanted by the authorities, dead or alive. Any citizen who can find and neutralize %s by any means necessary will be given %s as a reward. %s authorities have lost track of this pilot in the %s system. It is very likely that the target is no longer there, but this system may be a good place to start an investigation.")

flee_text = _("You had a chance to neutralize %s, and you wasted it! Now you have to start all over. Maybe some other pilots in %s know where your target is going.")

Tflee_text = _("That was close, but unfortunately, %s ran away. Maybe some other pilots in this system know where your target is heading.")

pay_text    = {}
pay_text[1] = _("An officer hands you your pay.")
pay_text[2] = _("No one will miss this outlaw pilot! The bounty has been deposited into your account.")

osd_title = _("Seek and Destroy")
osd_msg    = {}
osd_msg1_r = _("Fly to %s and search for clues")
osd_msg[1] = " "
osd_msg[2] = _("Kill %s")
osd_msg[3] = _("Land in %s territory to collect your bounty")
osd_msg["__save"] = true

npc_desc = _("Shifty Person")
bar_desc = _("This person might be an outlaw, a pirate, or even worse, a bounty hunter. You normally wouldn't want to get close to this kind of person, but they may be a useful source of information.")

-- Mission details
misn_title  = _("Seek And Destroy Mission, starting in %s")
misn_desc   = _("The %s pilot known as %s is wanted dead or alive by %s authorities. He was last seen in the %s system.")

function create ()
   paying_faction = planet.cur():faction()

   -- Choose the target faction among Pirate and FLF
   adm_factions = {faction.get("Pirate"), faction.get("FLF")}
   fact = {}
   for i, j in ipairs(adm_factions) do
      if paying_faction:areEnemies(j) then
         fact[#fact+1] = j
      end
   end
   target_faction = fact[rnd.rnd(1,#fact)]

   if target_faction == nil then
      misn.finish(false)
   end

   local systems = getsysatdistance(system.cur(), 1, 5,
      function(s)
         local p = s:presences()[target_faction:nameRaw()]
         return p ~= nil and p > 0
      end)

   -- Create the table of system the player will visit now (to claim)
   nbsys = rnd.rnd(5, 9) -- Total number of available systems (in case the player misses the target first time)
   pisys = rnd.rnd(2, 4) -- System where the target will be
   mysys = {}

   if #systems <= nbsys then
      -- Not enough systems
      misn.finish(false)
   end

   mysys[1] = systems[ rnd.rnd(1, #systems) ]

   -- There will probably be lot of failure in this loop.
   -- Just increase the mission probability to compensate.
   for i = 2, nbsys do
      thesys = systems[ rnd.rnd(1, #systems) ]
      -- Don't re-use the previous system
      if thesys == mysys[i-1] then
         misn.finish(false)
      end
      mysys[i] = thesys
   end

   if not misn.claim(mysys) then
      misn.finish(false)
   end

   if target_faction == faction.get("FLF") then
      name = pilot_name()
      ships = {"Lancelot", "Vendetta", "Pacifier"}
   else -- default Pirate
      name = pirate_name()
      ships = {"Pirate Shark", "Pirate Vendetta", "Pirate Admonisher"}
   end

   ship = ships[rnd.rnd(1,#ships)]
   credits = 500000 + rnd.rnd()*200000 + rnd.sigma()*10000
   cursys = 1

   -- Set mission details
   misn.setTitle(misn_title:format(mysys[1]:name()))
   misn.setDesc(misn_desc:format(target_faction:name(), name, paying_faction:name(), mysys[1]:name()))
   misn.setReward(creditstring(credits))
   marker = misn.markerAdd(mysys[1], "computer")

   -- Store the table
   mysys["__save"] = true
end

-- Test if an element is in a list
function elt_inlist(elt, list)
   for i, elti in ipairs(list) do
      if elti == elt then
         return true
      end
   end
   return false
end

function accept ()
   misn.accept()

   stage = 0
   increment = false
   last_sys = system.cur()
   tk.msg("", breef_text:format(name, target_faction:name(), name,
            creditstring(credits), paying_faction:name(), mysys[1]:name()))
   jumphook = hook.enter("enter")
   hailhook = hook.hail("hail")
   landhook = hook.land("land")

   osd_msg[1] = osd_msg1_r:format(mysys[1]:name())
   osd_msg[2] = osd_msg[2]:format(name)
   osd_msg[3] = osd_msg[3]:format(paying_faction:name())
   misn.osdCreate(osd_title, osd_msg)
end

function enter ()
   hailed = {}

   -- Increment the target if needed
   if increment then
      increment = false
      cursys = cursys + 1
   end

   if stage <= 2 and system.cur() == mysys[cursys] then
      -- This system will contain the pirate
      -- cursys > pisys means the player has failed once (or more).
      if cursys == pisys or (cursys > pisys and rnd.rnd() > .5) then
         stage = 2
      end

      if stage == 0 then  -- Clue system
         if not var.peek("got_advice") then -- A bounty hunter who explains how it works
            var.push("got_advice", true)
            spawn_advisor ()
         end
      elseif stage == 2 then  -- Target system
         misn.osdActive(2)
         
         -- Get the position of the target
         jp  = jump.get(system.cur(), last_sys)
         if jp ~= nil then
            x = 8000 * rnd.rnd() - 4000
            y = 8000 * rnd.rnd() - 4000
            pos = jp:pos() + vec2.new(x,y)
         else
            pos = nil
         end

         -- Spawn the target
         pilot.toggleSpawn(false)
         pilot.clear()

         target_ship = pilot.add(ship, target_faction, pos)
         target_ship:rename(name)
         target_ship:setHilight(true)
         target_ship:setVisplayer()
         target_ship:setHostile()

         death_hook = hook.pilot(target_ship, "death", "target_death")
         pir_jump_hook = hook.pilot(target_ship, "jump", "target_flee")
         pir_land_hook = hook.pilot(target_ship, "land", "target_land")
         jumpout = hook.jumpout("player_flee")
      end
   end
   last_sys = system.cur()
end

function spawn_advisor ()
   jp     = jump.get(system.cur(), last_sys)
   x = 4000 * rnd.rnd() - 2000
   y = 4000 * rnd.rnd() - 2000
   pos = jp:pos() + vec2.new(x,y)

   advisor = pilot.add("Lancelot", "Mercenary", pos, nil, {ai="baddie_norun"})
   hailie = hook.timer(2, "hailme")

   hailed[#hailed+1] = advisor
end

function hailme()
    advisor:hailPlayer()
    hailie2 = hook.pilot(advisor, "hail", "hail_ad")
end

function hail_ad()
   hook.rm(hailie)
   hook.rm(hailie2)
   tk.msg("", advice_text) -- Give advice to the player
end

-- Player hails a ship for info
function hail(p)
   if p:leader() == player.pilot() then
      -- Don't want the player hailing their own escorts.
      return
   end

   if system.cur() == mysys[cursys] and stage == 0
         and not elt_inlist(p, hailed) then
      hailed[#hailed+1] = p -- A pilot can be hailed only once

      if cursys+1 >= nbsys then -- No more claimed system : need to finish the mission
         tk.msg("", cold_text[rnd.rnd(1,#cold_text)]:format(name))
         misn.finish(false)
      else

         -- If hailed pilot is enemy to the target, there is less chance he knows
         if target_faction:areEnemies(p:faction()) then
            know = (rnd.rnd() > .9)
         else
            know = (rnd.rnd() > .3)
         end

         -- If hailed pilot is enemy to the player, there is less chance he tells
         if p:hostile() then
            tells = (rnd.rnd() > .95)
         else
            tells = (rnd.rnd() > .5)
         end

         if not know then -- NPC does not know the target
            tk.msg("", dono_text[rnd.rnd(1, #dono_text)]:format(name))
         elseif tells then
            local s = clue_text[rnd.rnd(1, #clue_text)]
            tk.msg("", s:format(name, mysys[cursys+1]:name()))
            next_sys()
            p:setHostile(false)
         else
            space_clue(p)
         end
      end

      player.commClose()
   end
end

-- The NPC knows the target. The player has to convince him to give info
function space_clue(p)
   -- FLF are loyal to each other.
   local loyal = false
   local flf = faction.get("FLF")
   if target_faction == flf and p:faction() == flf then
      loyal = true
   end

   if loyal or p:hostile() then
      local s = noinfo_text[rnd.rnd(1, #noinfo_text)]
      local choice = tk.choice("", s, IdonnoPay, IkickYourAss)
      if choice == 1 then
         return
      else -- Threaten the pilot
         if isScared(p) and rnd.rnd() < .5 then
            local s = scared_text[rnd.rnd(1, #scared_text)]
            tk.msg("", s:format(name, mysys[cursys+1]:name()))
            next_sys()
            p:control()
            p:runaway(player.pilot())
         else
            tk.msg("", not_scared_text[rnd.rnd(1, #not_scared_text)])

            -- Clean the previous hook if it exists
            if attack then
               hook.rm(attack)
            end
            attack = hook.pilot(p, "attacked", "clue_attacked")
         end
      end
   else -- Pilot wants payment
      price = (5 + 5*rnd.rnd()) * 1000
      local s = money_text[rnd.rnd(1,#money_text)]
      choice = tk.choice("", s:format(name,creditstring(price)),
            IdoPay, IdonnoPay, IkickYourAss)

      if choice == 1 then
         if player.credits() >= price then
            player.pay(-price)
            tk.msg("", clue_text[rnd.rnd(1,#clue_text)]:format(name, mysys[cursys+1]:name()))
            next_sys()
            p:setHostile(false)
         else
            tk.msg("", poor_text)
         end
      elseif choice == 2 then
         return
      else -- Threaten the pilot
         -- Everybody except the pirates takes offence if you threaten them
         if p:faction() ~= faction.get("Pirate") then
            faction.modPlayerSingle(p:faction(), -1)
         end

         if isScared(p) then
            local s = scared_text[rnd.rnd(1, #scared_text)]
            tk.msg("", s:format(name, mysys[cursys+1]:name()))
            next_sys()
            p:control()
            p:runaway(player.pilot())
         else
            tk.msg("", not_scared_text[rnd.rnd(1, #not_scared_text)])

            -- Clean the previous hook if it exists
            if attack then
               hook.rm(attack)
            end
            attack = hook.pilot(p, "attacked", "clue_attacked")
         end
      end
   end
end

-- Player attacks an informant who has refused to give info
function clue_attacked(p, attacker)
   -- FLF has no fear.
   local flf = faction.get("FLF")
   if target_faction == flf and p:faction() == flf then
      hook.rm(attack)
      return
   end

   -- Target was hit sufficiently to get more talkative
   if (attacker == player.pilot() or attacker:leader() == player.pilot())
         and p:health() < 100 then
      p:control()
      p:runaway(player.pilot())
      local s = scared_text[rnd.rnd(1, #scared_text)]
      tk.msg("", s:format(name, mysys[cursys+1]:name()))
      next_sys()
      hook.rm(attack)
   end
end

-- Decides if the pilot is scared by the player
function isScared(t)
   -- FLF has no fear.
   local flf = faction.get("FLF")
   if target_faction == flf and t:faction() == flf then
      return false
   end

   local pstat = player.pilot():stats()
   local tstat = t:stats()

   -- If target is stronger, no fear
   if tstat.armour+tstat.shield > 1.1 * (pstat.armour+pstat.shield)
         and rnd.rnd() < 0.95 then
      return false
   end

   -- If target is quicker, no fear
   if tstat.speed_max > pstat.speed_max and rnd.rnd() < 0.95 then
      if t:hostile() then
         t:control()
         t:runaway(player.pilot())
      end
      return false
   end

   if rnd.rnd() < 0.2 then
      return false
   end

   return true
end

-- Spawn NPCs at bar, that give info
function land ()
   -- Player flees from combat
   if stage == 2 then
      player_flee()

   -- Player seek for a clue
   elseif system.cur() == mysys[cursys] and stage == 0 then
      if rnd.rnd() < .3 then -- NPC does not know the target
         know = 0
      elseif rnd.rnd() < .5 then -- NPC wants money
         know = 1
         price = (5 + 5*rnd.rnd()) * 1000
      else -- NPC tells the clue
         know = 2
      end
      mynpc = misn.npcAdd("clue_bar", npc_desc, portrait.get("Pirate"), bar_desc)

   -- Player wants to be paid
   elseif planet.cur():faction() == paying_faction and stage == 4 then
      tk.msg("", pay_text[rnd.rnd(1,#pay_text)])
      player.pay(credits)
      paying_faction:modPlayer(rnd.rnd(1,2))
      misn.finish(true)
   end
end

-- The player ask for clues in the bar
function clue_bar()
   if cursys+1 >= nbsys then
      tk.msg("", cold_text[rnd.rnd(1, #cold_text)]:format(name))
      misn.finish(false)
   else

      if know == 0 then -- NPC does not know the target
         tk.msg("", dono_text[rnd.rnd(1,#dono_text)]:format(name))
      elseif know == 1 then -- NPC wants money
         local s = money_text[rnd.rnd(1,#money_text)]
         choice = tk.choice("", s:format(name, creditstring(price)),
               IdoPay, IdonnoPay)

         if choice == 1 then
            if player.credits() >= price then
               player.pay(-price)
               tk.msg("", clue_text[rnd.rnd(1,#clue_text)]:format(name, mysys[cursys+1]:name()))
               next_sys()
            else
               tk.msg("", poor_text)
            end
         else
            -- End of function
         end

      else -- NPC tells the clue
         local s = clue_text[rnd.rnd(1, #clue_text)]
         tk.msg("", s:format(name, mysys[cursys+1]:name()))
         next_sys()
      end

   end
   misn.npcRm(mynpc)
end

function next_sys ()
   misn.markerMove (marker, mysys[cursys+1])
   osd_msg[1] = osd_msg1_r:format(mysys[cursys+1]:name())
   misn.osdCreate(osd_title, osd_msg)
   increment = true
end

function player_flee ()
   tk.msg("", flee_text:format(name, system.cur():name()))
   stage = 0
   misn.osdActive(1)

   hook.rm(death_hook)
   hook.rm(pir_jump_hook)
   hook.rm(pir_land_hook)
   hook.rm(jumpout)
end

function target_flee ()
   -- Target ran away. Unfortunately, we cannot continue the mission
   -- on the other side because the system has not been claimed...
   tk.msg("", Tflee_text:format(name))
   pilot.toggleSpawn(true)
   stage = 0
   misn.osdActive(1)

   hook.rm(death_hook)
   hook.rm(pir_jump_hook)
   hook.rm(pir_land_hook)
   hook.rm(jumpout)
end

function target_death ()
   stage = 4
   hook.rm(death_hook)
   hook.rm(pir_jump_hook)
   hook.rm(pir_land_hook)
   hook.rm(jumpout)

   misn.osdActive(3)
   misn.markerRm (marker)
   pilot.toggleSpawn(true)
end
