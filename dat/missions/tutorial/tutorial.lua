--[[
<?xml version='1.0' encoding='utf8'?>
<mission name="Tutorial">
 <flags>
  <unique />
 </flags>
 <avail>
  <priority>1</priority>
  <location>None</location>
 </avail>
</mission>
--]]
--[[

   Tutorial Mission

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
require "events/tutorial/tutorial_common"
require "missions/neutral/common"


a_commodity = commodity.get("Water")
an_outfit = outfit.get("Heavy Laser Turret")

intro_text  = _([["Welcome to space, {player}!" a voice finally says through the radio. It felt like you must have been waiting a whole period since you were sent into space to begin your flight training, sponsored by the manufacturer of your ship, Melendez Corporation. "I am Ian Structure, and I will be your guide today. On behalf of Melendez Corporation, I would like to say that I believe your new ship, the {shipname}, will serve you well! Our ships are prized for their reliability and affordability. I promise, you won't be disappointed!" You barely resist the temptation to roll your eyes at the remark; you really only bought this ship because it was the only one you could afford. Still, you tactfully thank Ian Structure.]])

movement_text = _([["Alright, let's go over how to pilot your new ship, then! There are two basic ways to pilot your ship: keyboard flight, and mouse flight. I will explain both so you can choose which method is best for you.

"To move via keyboard flight, rotate your ship with {leftkey} and {rightkey}, and thrust to move your ship forward with {accelkey}. You can also use {reversekey} to rotate your ship to the direction opposite of your current movement, which can be useful for bringing your vessel to a stop.

"To move via mouse flight, you must first enable it by pressing {mouseflykey}. While mouse flight is enabled, your ship will automatically turn toward your #bmouse pointer#0. You can then thrust either with {accelkey}, as you would do in keyboard flight, or you can alternatively use the #bmiddle mouse button#0 or either of the #bextra mouse buttons#0.

"Why don't you give both systems a try? Experiment with the flight controls as much as you'd like, then fly over to where {planet} is. You see it on your screen, right? It's the planet right next to you."]])

landing_text = _([["Great job! I know the controls can be difficult to get used to, but if you give it some time, I'm sure you'll be moving your ship like a pro in no time.

"Of course, moving through space is only a small part of navigation, so let's go over an equally important aspect: landing! Yes, yes, I know, you bought this ship to go off-world, but space is an empty place and it's other planets that make the long journeys worthwhile. Luckily, landing with modern spacecraft is a simple, automated procedure! All you have to do is either #bdouble-click#0 on a planet, or if you prefer to use your keyboard, you can target a planet with {target_planet_key} and then press {landkey}. Oh, and if you don't have a planet selected, you can just press {landkey} to land on the nearest landable planet.

"How about you try landing on {planet}? Again, feel free to choose whichever method you prefer."]])

land_text = _([[You watch in amazement as your ship automatically initiates landing procedures, taking you safely through the atmosphere and into the planet's space port, touching down at an empty spot reserved for you. As your hatch opens and you step out of your ship, an exhausted dock worker greets you and makes you sign a form. Once you've done so, she pushes some buttons and you watch in amazement as robotic drones immediately get to work checking your ship for damage and ensuring your fuel tanks are full. Noticing your expression, the worker lets out a chuckle. "First time landing, eh?" she quips. "It'll all be normal to you before long."

"Ah, there you are, {player}!" the voice of Ian Structure interrupts. You look in the direction of the voice and see an obnoxiously dressed man with a huge grin. "I see your Melendez Corporation starship is serving you well." The worker rolls her eyes, mumbles something you can't make out, and leaves to assist another ship which is about to land. Ian Structure seemingly doesn't notice. "I need to go attend to some matters. Why don't you meet my colleague at the bar while you wait? You can navigate to there with those tabs at the bottom. I'll meet up with you later!" You try to protest, but before you can get a word out, he's already gone. Defeated, you figure you might as well go see this colleague, to pass the time if nothing else.]])

approach_text = _([[You approach a well-dressed man who you suppose must be Ian Structure's colleague and ask if indeed he is. The man lets out a small chuckle. "Ah, of course. I do know him, though I do think 'colleague' is stretching it a bit. Let me guess: he excused himself to 'attend to some matters'?" Dumbstruck, you simply slowly nod, causing the man to erupt in laughter. "He's quite the character, I'll say. I've told him a thousand times it's okay to just say he's going on his lunch break.

"Oh, but where are my manners? I haven't introduced myself." He offers his hand, which you shake. "My name is Albert. I'm, you could say, a casual pilot. I take easy jobs here and there, trade commodities, that sort of thing. I was actually stopping by here to grab some cargo for a client, but I got distracted by an enthralling story on the news terminal.

"Hm, say, could you get that cargo for me? I'll give you the credits for it." Albert presses a credit chip into your hand. "I just need 10 t of Food. You should be able to find it in the Commodity tab." Figuring this diversion is better than sitting around doing nothing, you agree to get the cargo for him and he gets back to reading his news article, nodding appreciatively.]])

ask_cargo_text = _([[You ask Albert what cargo it was that he needed again, apologizing for forgetting already. "Oh, that's no problem!" he assures you. "I have trouble remembering stuff too. It's 10 t of Food. You should be able to find it in the Commodity tab. Let me know when you have it!"]])

give_cargo_text = _([[You approach Albert and inform him that you successfully bought the cargo he needs. "Ah, perfect timing!" he responds. "I just got done reading that article. Thanks a bunch!" You point out that the cost of the cargo was less than what Albert gave you and attempt to return the rest of his credits to him, but he waves dismissively. "No, it's OK. Keep it as a thank you for your assistance. You did part of my job for me, after all, so it's only fair that you get a share of the payment!

"Say, I'm going to be on this planet for awhile longer, so do you want to come along with me while you wait for Ian?" You shrug your shoulders and muse that you might as well, seeing as you have nothing better to do. "That's the spirit! My first stop is the mission computer. Just click on the tab labeled 'Missions' to navigate us to there."]])

mission_text = _([[You and Albert approach the mission computer and Albert immediately begins browsing through the available missions. You watch closely as he mutters to himself. "Let's see, this one is too dangerous, this one I don't have enough cargo for, this one doesn't pay enough… aha! Perfect!" Seemingly content with one of the missions he found, he presses the Accept button and the mission disappears from the list of available missions.

"Oh!" he interjects to himself. "Shit, that's the wrong one. Let's abort that…" You see Albert pressing {infokey} on his wrist computer, which causes a window to pop up which seems to be an interface to his ship's computer. He navigates over to the Missions tab on this window, selects the mission he just accepted, and presses the Abort button, which causes the mission to disappear from his active missions. "There," he mutters. He then returns to the mission computer and accepts a different mission this time, presumably the one he meant to accept in the first place.

"Alright," he says this time to you, "I've chosen my mission! Next stop, the outfitter. Click on the 'Outfits' tab to take us there when you're ready."]])

outfits_text = _([[As you enter the outfitter, you can't help but look around in wonder at all the choices. Albert, meanwhile, seems used to the display. He browses the catalog for a moment before finding what he's looking for: a Melendez Ox XL Engine. "Finally!" he exclaims. "I've been looking for this thing all over the place.

"Just one more stop for me! I need to go to the Equipment screen so I can actually put this on my ship. It won't do me much good just sitting in my inventory, eh? Click on the 'Equipment' tab whenever you're ready."]])

equipment_text = _([[You see through Albert's information on the Equipment screen that he has three ships: a Koala, a Quicksilver, and a Gawain, in addition to a modest collection of outfits. He uses the interface to remove the Unicorp Hawk 300 Engine from his Koala, then install his new Melendez Ox XL Engine onto the Engine slot. "All set!" he says. You take a moment to look at your own information on the Equipment screen as well; sure enough, you see the ship you just purchased, along with your comparatively barren collection of outfits.

"Well, I suppose I'm done here now," Albert says. "Hey, thanks again for helping me out on my mission, and it was nice to meet you. Maybe we'll see each other in space some other time!" You part ways with Albert as he goes back into space. With little else to do, you decide to wander around some more while you wait for Ian Structure to show up.

Suddenly, you get a call on your wrist computer from Ian Structure. "Sorry for the wait," he says. "I'm back and ready to continue your flight training! Go ahead and click on the 'Take Off' button whenever you're ready."]])

overlay_text = _([["Welcome back to space, {player}! Let's continue discussing moving around in space. As mentioned before, you can move around space manually, no problem. However, you will often want to travel large distances, and navigating everywhere manually could be a bit tedious. That is why ships from Melendez Corporation always include the latest Autonav technology!

When your ship is piloted with Autonav, while the trip takes just as long in real time, advanced automation technology allows you to step away from your controls, making it seem as though time is passing at a faster rate (a phenomenon we call 'time compression'). And don't worry; if any hostile pilots are detected, the Autonav system automatically alerts you so you can return to your controls, ending time compression. This can be configured in the Options menu.

"Allow me to demonstrate how this system works! To start with, please press {overlaykey} to open your ship's overlay map."]])

autonav_text = _([["As you can see, your overlay map shows several indicator icons. These icons represent objects in the current system that are visible to you: planets, jump points, ships, and asteroids. These icons are color-coded to indicate whether the objects are friendly, neutral, or hostile, and planets are also marked with symbols which serve the same purpose for colorblind accessibility.

"Now that the overlay map is open, you can use your mouse to interact with objects the same way you would with the actual object. In addition, you can fly to any location in the system automatically by #bright-clicking#0 on its corresponding location on the overlay map.

"Why don't you give it a try by using Autonav to fly over to {planet}? Simply #bright-click#0 on your overlay map's {planet} icon and watch in amazement as Autonav takes you there quickly, easily, and painlessly!"]])

target_text = _([["Convenient, isn't it? By using Autonav, the perceived duration of your trip was cut substantially. You will grow to appreciate this feature in time, especially as you travel from system to system delivering goods and such.

"Let's now practice combat. You won't need this if you stick to the safe systems in the southern Empire core, but sadly, you are likely to encounter pirate scum if you venture further out, so you need to know how to defend yourself. Fortunately, your ship comes pre-equipped with state-of-the-art laser cannons for just that reason!

"I will launch a combat practice drone off of {planet} now for you to fight. Don't worry; our drone does not have any weapons and will not harm you. To start with, target the drone by pressing {target_hostile_key}."]])

combat_text = _([["Perfect! For the heat of battle, this method of selecting hostile ships is much safer and more reliable than selection via the mouse.

"Targeting enemy ships is important not only because of the useful information display, but also because it enables automatic weapon tracking systems which help ease aiming. All weapons have some capability to swivel in order to compensate for imprecise aim, but you need to have an enemy ship targeted for this feature to work.

"Your weapons are fired with {primarykey} and {secondarykey}. Give them a try and shoot the practice drone down!"]])

cooldown_text = _([["Excellent work taking out that drone! Of course, that was nothing like a real fight against a real pirate. If you would like some more practice, I would recommend giving the Combat Practice mission a try from any mission computer, which you can try as many times as you like.

"Of course, all that shooting has caused your weapons to heat up a little. This is normal, but too much heat can make your weapons difficult to use, which is why Melendez Corporation recommends using active cooldown when it is safe to do so. Please engage active cooldown and wait for it to cool your ship down completely. You can do this by pressing {autobrake_key} twice."]])

jumping_text = _([["There, your ship is now fully cooled off. Of course you could have cooled off by landing in this case, but not all systems have a friendly planet for you to land on, so Active Cooldown can be life-saving in many situations.

"Anyway, I think it's about time for a change in scenery, don't you? There are many systems in the universe; this one is but a tiny sliver of what can be found out there! Let's take a look at the best interface for traveling between stars: the starmap. To start with, please open the starmap by pressing {starmapkey}.]])

starmap_text = _([["Behold, perhaps the most useful tool in your arsenal! By selecting a destination system that you know the route to and clicking the 'Autonav' button, Autonav automatically plans a route for you and will pilot your ship all the way to the destination system, as long as your ship has enough fuel. Let me show you!

"I've taken the liberty of placing a destination marker on your starmap for the {system} system. All you need to do is click on that system, and then click on the 'Autonav' button. Once you've done that, you can sit back and relax as Autonav takes care of the rest!"]])

fuel_text = _([["Was that easy, or was that easy? As you can see, the trip consumed fuel. Do keep this in mind and plan your routes accordingly; for long journeys, you will want to plan stops so you don't run out at an inopportune time.

"Now, then, why don't we put your knowledge to the test? I have placed a new marker on your starmap showing the location of your final destination: {system}, the heart of the Empire and home to the Emperor's famous warship, the Emperor's Wrath. This time, you will need to find your way to the destination on your own. As a reward, I am authorized to give you {credits} when you complete this test. Good luck!"]])

pay_text = _([[Ian Structure gives you a round of applause as you enter {system}. "Phenominal piloting, {player}! Truly stunning! I must say, you are a natural-born pilot and your new Melendez Corporation starship suits you well. I have no doubt about it, you are ready to make your way in space! Ah, and as promised, your reward for completing the test has been deposited into your account.

"If you ever need a refresher on the information we have gone over, a copy of your pilot's manual can be found in your ship log, which can be found on the ship computer. Good luck, {player}, and I hope to see you again one day as the expert pilot I am sure you will become!" Ian Structure ceases contact and you are now left to your own devices, now knowing how to pilot your ship. You grin to yourself, wondering what to do first with your newfound freedom. The prize money for completing the flight training is just the icing on the cake.]])

movement_log = _([[Basic movement can be accomplished by the movement keys (Accelerate, Turn Left, Turn Right, and Reverse; W, A, D, and S by default). The Reverse key either turns your ship to the direction opposite of your current movement, or thrusts backwards if you have a Reverse Thruster equipped.

Alternatively, you can enable mouse flight by pressing the Mouse Flight key (Ctrl+X by default), which causes your ship to automatically point toward your mouse pointer. You can then thrust with the Accelerate key (W by default), middle mouse button, or either of the extra mouse buttons.]])
objectives_log = _([[The mission on-screen display highlights your current objective for each mission. When you complete one objective, the next objective is highlighted.]])
landing_log = _([[You can land on any planet by either double-clicking on it, or by targeting the planet with the Target Planet button and then pressing the Land key (L by default). The landing procedure is automatic. If no planet is selected, pressing the Land key will initiate the automatic landing procedure for the closest suitable planet if possible.]])
land_log = _([[When you land, your ship is refueled automatically if the planet or station has the "Refuel" service. Most planets and stations have the "Refuel" service, though there are some exceptions.]])
bar_log = _([[The Spaceport Bar allows you to read the news, meet civilians, hire pilots to join your fleet, and sometimes find unique missions. You can click on any patron of the bar and then click on the Approach button to approach them. Some civilians may lend helpful advice.]])
mission_log = _([[You can find basic missions in the official mission database via the Mission Computer (Missions tab while landed). You can review your active missions at any time via the ship computer, which you can open by pressing the Ship Computer key (I by default) or by pressing the Small Menu key (Escape by default) and pressing the "Ship Computer" button.]])
outfits_log = _([[You can buy and sell outfits for your ship at the Outfitter (Outfits tab while landed). You can also buy regional maps, which can help you explore the galaxy more easily, and licenses which are required for higher-end weapons and ships.

The tabs at the top of the outfitter allow you to filter outfits by type: "W" for weapons, "U" for utilities, "S" for structurals, "Core" for cores, and "Other" for anything outside those categories (most notably, maps and licenses). Different planets have different outfits available; if you don't see a specific outfit you're looking for, you can search for it via the "Find Outfits" button.]])
shipyard_log = _([[You can buy new ships at the Shipyard. You can then either buy a ship you want with the "Buy" button, or trade your current ship in for the new ship with the "Trade-In" button. Different planets have different ships available; if you don't see a specific ship you're looking for, you can search for it via the 'Find Ships' button.

Different ships are specialized for different tasks, so you should choose your ship based on what tasks you will be performing.]])
equipment_log = _([[The Equipment screen is available on all planets which have either an outfitter or a shipyard. You can use the Equipment screen to equip your ship with outfits you own and sell outfits you have no more use for. If the planet or station you're landed at has a shipyard, you can also change which ship you're flying and sell unneeded ships. Selling a ship that still has outfits equipped will also lead to those outfits being sold along with the ship.]])
commodity_log = _([[You can buy and sell commodities via the Commodity screen. Commodity prices vary from planet to planet and even over time, so you can use this screen to attempt to make money by buying low and selling high. Here, it's useful to hold the Shift and/or Ctrl keys to adjust how many tonnes of the commodity you're buying or selling at once.

If you're unsure what's profitable to buy or sell, you can press the Open Star Map key (M by default) to view the star map and then click on the "Mode" button for various price overviews. The news terminal at the Spaceport Bar also includes price information for specific nearby planets.]])
autonav_log = _([[You can open your ship's overlay map by pressing the Overlay Map key (Tab by default). Through the overlay map, you can right-click on any location, planet, ship, or jump point to engage Autonav, which will cause you to automatically fly toward what you clicked on. While Autonav is engaged, passage of time will speed up so that you don't have to wait to arrive at your destination in real-time. Time will reset to normal speed if hostile pilots are detected by default. This can be configured from the Options menu, which you can access by pressing the Menu key (Escape by default).]])
combat_log = _([[You can target an enemy ship either by either clicking on it or by pressing the Target Nearest Hostile key (R by default). You can then fire your weapons at them with the Fire Primary Weapon key (Space by default) and the Fire Secondary Weapon key (Left Shift by default).]])
infoscreen_log = _([[You can configure the way your weapons shoot from the Weapons tab of the ship computer, which you can open by pressing the Ship Computer key (I by default) or by pressing the Small Menu key (Escape by default) and pressing the "Ship Computer" button.]])
cooldown_log = _([[As you fire your weapons, they and subsequently your ship get hotter. Too much heat causes your weapons to lose accuracy. You can cool down your ship at any time in space by pressing the Autobrake key (Ctrl+B by default) twice. You can also cool down your ship by landing on any planet or station.]])
jumping_log = _([[Traveling through systems is accomplished through jump points, which you usually need to find by exploring the area, talking to locals, or buying maps. Once you have found a jump point, you can use it by double-clicking on it.]])
jumping_log2 = _([[You can open your starmap by pressing the Open Star Map key (M by default). Through your starmap, you can click on a system and click on the Autonav button to be automatically transported to the system. This only works if you know a valid route to get there.]])
fuel_log = _([[You consume fuel any time you make a jump and can refuel by landing on a friendly planet. Standard engines have enough fuel to make up to three jumps before refueling, though higher-end engines have more fuel capacity and some ships may have their own supplementary fuel tanks.]])
boarding_log = _("To board a ship, you generally must first use disabling weapons, such as ion cannons, to disable it, although some missions and events allow you to board certain ships without disabling them. Once the ship is disabled or otherwise can be boarded, you can board it by either double-clicking on it, or targeting it with the Target Nearest key (T by default) and then pressing the Board Target key (B by default). From there, you generally can steal the ship's credits, cargo, ammo, and/or fuel. Boarding ships can also trigger special mission events.")
nofuel_log = _([[You can hail any other pilot by either double-clicking on them, or by targeting them with the Target Nearest key (T by default) and then pressing the Hail Target key (Y by default). From there, you can ask to be refueled. Most military ships will not be willing to help you, but many civilians and traders will be willing to sell you some fuel for a nominal fee. When you find someone willing to refuel you, you need to stop your ship, which you can do with the Autobrake key (Ctrl+B by default), and wait for them to reach your ship and finish the fuel transfer.

If there are no civilians or traders in the system, you can alternatively attempt to get fuel from a pirate. To do so, you must first hail them and offer a bribe, and if you successfully bribe them, they will often be willing to refuel you if you hail them again and ask for it.]])

misn_title = _("Flight Training")
misn_desc = _("Ian Structure is in the process of teaching you how to fly your ship through a flight training course sponsored by Melendez Corporation.")

log_text = _([[Ian Structure, an employee of Melendez Corporation, taught you how to pilot a ship, claiming afterwards that you are "a natural-born pilot". Along the way, you met another pilot named Albert and helped him complete a mission.]])


function create ()
   missys = system.get("Hakoi")
   jumpsys = system.get("Eneguoz")
   destsys = system.get("Gamma Polaris")
   start_planet = planet.get("Em 1")
   start_planet_r = 200
   dest_planet = planet.get("Em 5")
   dest_planet_r = 200

   if not misn.claim(missys) then
      print(string.format(
               _("Warning: 'Tutorial' mission was unable to claim system %s!"),
               missys:name()))
      misn.finish(false)
   end

   credits = 15000

   misn.setTitle(misn_title)
   misn.setDesc(misn_desc)
   misn.setReward(fmt.credits(credits))

   accept()
end


function accept ()
   misn.accept()

   -- Add all tutorial logs at the start; this avoids missing logs if
   -- the tutorial is aborted early on.
   addTutLog(movement_log)
   addTutLog(objectives_log)
   addTutLog(landing_log)
   addTutLog(land_log)
   addTutLog(bar_log)
   addTutLog(mission_log)
   addTutLog(outfits_log)
   addTutLog(shipyard_log)
   addTutLog(equipment_log)
   addTutLog(commodity_log)
   addTutLog(autonav_log)
   addTutLog(combat_log)
   addTutLog(infoscreen_log)
   addTutLog(cooldown_log)
   addTutLog(jumping_log)
   addTutLog(jumping_log2)
   addTutLog(fuel_log)
   addTutLog(boarding_log)
   addTutLog(nofuel_log)

   timer_hook = hook.timer(5, "timer")
   hook.land("land")
   hook.load("land")
   hook.takeoff("takeoff")
   hook.enter("enter")
   hook.input("input")

   stage = 1
   create_osd()

   tk.msg("", fmt.f(intro_text,
         {player=player.name(), shipname=player.pilot():name()}))
   tk.msg("", fmt.f(movement_text,
         {leftkey=tutGetKey("left"), rightkey=tutGetKey("right"),
            accelkey=tutGetKey("accel"), reversekey=tutGetKey("reverse"),
            mouseflykey=tutGetKey("mousefly"), planet=start_planet:name()}))
end


function create_osd()
   local osd_desc = {
      fmt.f(_("Fly to {planet} ({system} system) with the movement keys ({accelkey}, {leftkey}, {reversekey}, {rightkey})"),
         {planet=start_planet:name(), system=missys:name(),
            accelkey=naev.keyGet("accel"), leftkey=naev.keyGet("left"),
            reversekey=naev.keyGet("reverse"), rightkey=naev.keyGet("right")}),
      fmt.f(_("Land on {planet} ({system} system) by double-clicking on it"),
         {planet=start_planet:name(), system=missys:name()}),
      fmt.f(_("Talk to Ian Structure's colleague at the bar on {planet} ({system} system})"),
         {planet=start_planet:name(), system=missys:name()}),
      fmt.f(_("Buy 10 t of Food from the Commodity tab and then talk to Albert at the bar on {planet} ({system} system)"),
         {planet=start_planet:name(), system=missys:name()}),
      fmt.f(_("Go to the mission computer (Missions tab) on {planet} ({system} system)"),
         {planet=start_planet:name(), system=missys:name()}),
      fmt.f(_("Go to the outfitter (Outfits tab) on {planet} ({system} system)"),
         {planet=start_planet:name(), system=missys:name()}),
      fmt.f(_("Go to the Equipment tab on {planet} ({system} system)"),
         {planet=start_planet:name(), system=missys:name()}),
      _("Press the Take Off button to return to space"),
      fmt.f(_("Press {overlaykey} to open your overlay map"),
         {overlaykey=naev.keyGet("overlay")}),
      fmt.f(_("Autonav to {planet} ({system} system) by right-clicking its icon on the overlay map"),
         {planet=dest_planet:name(), system=missys:name()}),
      fmt.f(_("Press {target_hostile_key} to target the practice drone"),
         {target_hostile_key=naev.keyGet("target_hostile")}),
      fmt.f(_("Face the practice drone using the movement controls, then use {primarykey} and {secondarykey} to fire your weapons and destroy it"),
         {primarykey=naev.keyGet("primary"),
            secondarykey=naev.keyGet("secondary")}),
      fmt.f(_("Engage Active Cooldown by pressing {autobrake_key} twice, then wait for your ship to fully cool down"),
         {autobrake_key=naev.keyGet("autobrake")}),
      fmt.f(_("Press {starmapkey} to open your starmap"),
         {starmapkey=naev.keyGet("starmap")}),
      fmt.f(_("Select {system} by clicking on it in your starmap, then click \"Autonav\" and wait for Autonav to fly you there"),
         {system=jumpsys:name()}),
      fmt.f(_("Fly to {system}"),
         {system=destsys:name()}),
   }

   misn.osdCreate(_("Flight Training"), osd_desc)
   misn.osdActive(stage)
end


function timer ()
   hook.rm(timer_hook)
   timer_hook = hook.timer(1, "timer")

   -- Recreate OSD in case key binds have changed.
   create_osd()

   if stage == 1 then
      if system.cur() == missys
            and player.pos():dist(start_planet:pos()) <= start_planet_r then
         stage = 2
         create_osd()

         tk.msg("", fmt.f(landing_text,
               {planet=start_planet:name(),
                  target_planet_key=tutGetKey("target_planet"),
                  landkey=tutGetKey("land")}))
      end
   elseif stage == 10 then
      if system.cur() == missys
            and player.pos():dist(dest_planet:pos()) <= dest_planet_r then
         stage = 11
         create_osd()
         tk.msg("", fmt.f(target_text,
               {planet=dest_planet:name(),
                  target_hostile_key=tutGetKey("target_hostile")}))
         spawn_drone()
      end
   elseif stage == 13 then
      if player.pilot():temp() <= 250 then
         player.allowLand(true)
         player.pilot():setNoJump(false)
         stage = 14
         create_osd()
         marker = misn.markerAdd(jumpsys, "high")
         tk.msg("", fmt.f(jumping_text, {starmapkey=tutGetKey("starmap")}))
      end
   end
end


function land()
   hook.rm(timer_hook)
   if stage == 2 then
      if planet.cur() ~= start_planet then
         return
      end

      stage = 3
      create_osd()
      tk.msg("", fmt.f(land_text, {player=player.name()}))
   elseif stage == 4 then
      if planet.cur() ~= start_planet then
         return
      end

      npc = misn.npcAdd("approach_albert", _("Albert"),
            "neutral/unique/youngbusinessman2.png",
            _("Albert is still reading the news terminal."), 0)
   end

   if stage == 3 then
      if planet.cur() ~= start_planet then
         return
      end

      npc = misn.npcAdd("approach_albert", _("Ian's Colleague"),
            "neutral/unique/youngbusinessman2.png",
            _("You see a well-dressed young man sitting alone at the bar, looking intently at the news terminal. Perhaps this is the \"colleague\" Ian Structure spoke of."),
            0)
   end
end


function approach_albert()
   if stage == 3 then
      tk.msg("", approach_text)

      stage = 4
      create_osd()
      misn.npcRm(npc)
      npc = misn.npcAdd("approach_albert", _("Albert"),
            "neutral/unique/youngbusinessman2.png",
            _("Albert is still reading the news terminal."), 0)

      -- We intentionally give the player more money than is actually
      -- needed to buy the commodity, to give both a buffer and a
      -- small reward.
      local cost = commodity.get("Food"):priceAtTime(planet.cur(), time.get())
      player.pay(cost * 30, "adjust")
   elseif stage == 4 then
      local ftonnes = player.pilot():cargoHas("Food")
      if ftonnes and ftonnes >= 10 then
         player.pilot():cargoRm("Food", 10)
         tk.msg("", give_cargo_text)

         stage = 5
         create_osd()
         misn.npcRm(npc)
         mission_hook = hook.land("land_mission", "mission")
      else
         tk.msg("", ask_cargo_text)
      end
   end
end


function land_mission()
   hook.rm(mission_hook)
   tk.msg("", fmt.f(mission_text, {infokey=tutGetKey("info")}))

   stage = 6
   create_osd()
   outfits_hook = hook.land("land_outfits", "outfits")
end


function land_outfits()
   hook.rm(outfits_hook)
   tk.msg("", outfits_text)

   stage = 7
   create_osd()
   equipment_hook = hook.land("land_equipment", "equipment")
end


function land_equipment ()
   hook.rm(equipment_hook)
   tk.msg("", equipment_text)

   stage = 8
   create_osd()
end


function takeoff ()
   hook.rm(bar_hook)
   hook.rm(mission_hook)
   hook.rm(outfits_hook)
   hook.rm(shipyard_hook)
   hook.rm(equipment_hook)
   hook.rm(commodity_hook)
end


function enter ()
   hook.rm(timer_hook)
   timer_hook = hook.timer(5, "timer")
   hook.timer(2, "enter_timer")
end


function enter_timer ()
   if stage == 8 then
      stage = 9
      create_osd()
      tk.msg("", fmt.f(overlay_text,
            {player=player.name(), overlaykey=tutGetKey("overlay")}))
   elseif (stage == 11 or stage == 12) and system.cur() == missys then
      stage = 11
      create_osd()
      spawn_drone()
   elseif stage == 15 and system.cur() == jumpsys then
      stage = 16
      create_osd()
      misn.markerMove(marker, destsys)
      tk.msg("", fmt.f(fuel_text,
            {system=destsys:name(), credits=fmt.credits(credits)}))
   elseif stage == 16 and system.cur() == destsys then
      player.pay(credits)
      tk.msg("", fmt.f(pay_text,
            {system=destsys:name(), player=player.name()}))
      addMiscLog(log_text)
      misn.finish(true)
   end
end


function input(inputname, inputpress, arg)
   if not inputpress then
      return
   end

   if stage == 9 and inputname == "overlay" then
      stage = 10
      create_osd()
      hook.safe("safe_overlay")
   elseif stage == 11 and inputname == "target_hostile" then
      stage = 12
      create_osd()
      if not combat_text_shown then
         combat_text_shown = true
         hook.safe("safe_combat")
      end
   elseif stage == 14 and inputname == "starmap" then
      stage = 15
      create_osd()
      hook.safe("safe_starmap")
   end
end


function safe_overlay()
   tk.msg("", fmt.f(autonav_text, {planet=dest_planet:name()}))
end


function safe_combat()
   tk.msg("", fmt.f(combat_text,
         {primarykey=tutGetKey("primary"),
            secondarykey=tutGetKey("secondary")}))
end


function safe_starmap()
   tk.msg("", fmt.f(starmap_text, {system=jumpsys:name()}))
end


function pilot_death ()
   player.allowLand(false)
   player.pilot():setNoJump(true)
   hook.timer(2, "pilot_death_timer")
end


function pilot_death_timer()
   stage = 13
   create_osd()
   tk.msg("", fmt.f(cooldown_text, {autobrake_key=tutGetKey("autobrake")}))
end


function spawn_drone()
   local f = faction.dynAdd(nil, N_("Training"),
         N_("Training Machines Inc."), {ai="baddie_norun"})
   local p = pilot.add("Hyena", f, dest_planet, _("Practice Drone"),
         {naked=true})

   p:outfitAdd("Previous Generation Small Systems")
   p:outfitAdd("Unicorp D-2 Light Plating")
   p:outfitAdd("Beat Up Small Engine")

   p:setHealth(100, 100)
   p:setEnergy(100)
   p:setTemp(0)
   p:setFuel(true)

   p:setHostile()
   p:setVisplayer()
   p:setHilight()
   hook.pilot(p, "death", "pilot_death")
end


function abort()
   if system.cur() == missys then
      player.allowLand(true)
      player.pilot():setNoJump(false)
   end
   misn.finish(false)
end
