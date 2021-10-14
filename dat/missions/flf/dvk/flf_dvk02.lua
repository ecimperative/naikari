--[[
<?xml version='1.0' encoding='utf8'?>
<mission name="FLF Pirate Alliance">
 <flags>
  <unique />
 </flags>
 <avail>
  <priority>10</priority>
  <chance>30</chance>
  <done>Diversion from Raelid</done>
  <location>Bar</location>
  <faction>FLF</faction>
  <cond>faction.playerStanding("FLF") &gt;= 30</cond>
 </avail>
 <notes>
  <campaign>Save the Frontier</campaign>
 </notes>
</mission>
--]]
--[[

   Pirate Alliance

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
local fleet = require "fleet"
require "missions/flf/flf_common"


text = {}

intro_text = _([[Benito looks up at you and sighs. It seems she's been working on a problem for a long while.

"Hello again, {player}. Sorry I'm such a mess at the moment. Maybe you can help with this little problem I have." You ask what the problem is. "Pirates. The damn pirates are making things difficult for us. Activity has picked up in the {sys} system and we don't know why.

"Pirates usually don't cause a huge problem for us because they attack Dvaered ships as well as our ships and Frontier ships, so it sort of evens out. But in that region of space, they're nothing but a nuisance. No Dvaered ships for them to attack, it's just us an the occasional Empire ship. And that's on our most direct route to Frontier space. If pirates are going to be giving us even more trouble there than before, that could slow down – or worse, wreck – our operations."]])

ask_text = _([[You remark that it's strange that pirates are there in the first place. "Yes!" Benito says. "It makes no sense! Pirates are always after civilians and traders to steal their credits and cargo, so why would they be there? We don't carry much cargo, the Empire doesn't carry much cargo… it just doesn't add up!

"My only guess is that maybe they're trying to find our hidden jump to Gilligan's Light, and if that's the case, that could be tremendously bad news. I'm not worried about the damage pirates can do to the Frontier; they've been prevalent in Frontier space for a long while. But if they start attacking Gilligan's Light, that could leave the Frontier in a vulnerable position that the Dvaereds can take advantage of!"

Benito sighs. "Could you help us with this problem, {player}?"]])

ask_again_text = _([["Hello again, {player}. Are you able to help us deal with the pirates in the nebula now?"]])

yes_text = _([["Perfect! I knew you would do it!

"First, we need to do some investigating, find out why they're there and what they want. I suspect they won't tell you without a fight, so I need you to… persuade them. I would recommend boarding one of them and interrogating them personally. I will leave you to decide what to do after that. Let me know how it goes, what you've found out and what you've accomplished. Good luck!"]])

no_text = _([[You see a defeated expression on Benito's face. "OK. Let me know if you change your mind."]])

board_text = _([[You force your way onto the pirate ship and subdue the pirate on board. You seem to have had a good stroke of luck: the pirate is clearly unprepared and has a very poorly secured ship. Clearly, this pirate is new to the job.

Not wanting to give away your suspicions, you order the pirate to tell you what the pirates are doing in the nebula. "What do you think we're doing? Exploring the nebula, of course!" You tell them to elaborate. "Surely you've heard the ghost stories, haven't you? They say ghosts lurk in the nebula, hiding, waiting to strike. Of course we don't believe in the supernatural, but that means there must be alien ships in there! Aliens with technology so advanced they're able to freely traverse the Nebula! If we could capture one of those ships, we'd be both rich and unstoppable!" You ask where they got into the Nebula from. "What's it to you?!" he responds.]])

unboard_text = _([[Undeterred, you pull out a laser gun and put it against the pirate's head, ordering them again to tell you where they got into the Nebula from. This seems to convince the previously boisterous pirate. "W-we have a hidden jump to {system2}! We come in from there! It's on my computer, you can check for yourself!"

You grin. Taking the pirate's suggestion, you look at the ship's computer, and sure enough, you find the hidden jump they were talking about. The map also reveals that there is a station in {system2} called {station}. You decide that this station is a good next place to investigate.]])

station_text = _([[You land on the newly constructed station and begin searching around. Considering its purpose you are impressed by the station's construction and how advanced it is, with equipment, ships, and even station defenses far exceeding those of Sindbad. Piracy must be lucrative indeed, at least for whoever built this station. Either that, or perhaps a great return on investment is anticipated.

You go around the station's bar and start prodding its patrons, posing as a pirate. You find that pirates around the bar generally confirm what the pirate you interrogated before said about trying to find some unknown alien civilization. However, something that one pirate says catches your ear. After rambling for what feels like several periods about alien tech and how great this would be, he continues with something concerning. "And besides, even if we don't find any alien tech, we could still suss out whatever secret jump those FLF fools use to get into the Frontier and get a nice, easy income source in the heart of the Frontier!"]])

station_text2 = _([[You resist the urge to frown; showing your anger in this situation, surrounded by pirates, would be dangerous. To your relief, all of the other pirates at the table seem to disagree. "I'm all for getting some nice alien tech, but pissing off the FLF is a bad move, I'm telling you!" one pirate responds. "Word is they've got enough ships and strength to keep the Dvaereds at bay, so what chance do we have? They'd swoop in blow us all up in an instant! Honestly the only reason they're not a problem for us is because we don't threaten the Frontier all that much and they know that." The rest of the pirates murmur in agreement, and the table goes silent.

You excuse yourself from the table, but decide that rogue pirate is dangerous. You patiently wait until the rogue pirate is alone in a secluded location, then sneak up behind him and put a gun to his back. The pirate suddenly freezes in place, not daring to make any sudden movements.]])

station_text3 = _([[You tell the pirate in no uncertain terms that he should listen to his contemporaries and not piss off the FLF. "U-understood," the pirate responds. "I'm sorry, I won't be encouraging g-going against you again, I promise!" You grin to yourself, then have another idea. With the pirate thoroughly intimidated, you tell him that they will be annihilated if they piss off the FLF, but that if he and his fellow pirates behave themselves, they just might be able to work together with the FLF for a level of profit that far exceeds anything they may expect to get alone.

The pirate shifts slightly in shock. "Y-you want to work with us?" You assure him that if he and his contemporaries behave, aliens or not, this station will lead them to riches beyond their wildest dreams. He closes his hands into a pair of tight fists. "OK. I understand. I will spread the word." You voice your approval and leave him, grinning to yourself. Perfect! Now to return to Sindbad to report your results.…]])

text[5] = _([[A scraggly-looking pirate appears on your viewscreen. You realize this must be the leader of the group. "Bwah ha ha!" he laughs. "That has to be the most pathetic excuse for a ship I've ever seen!" You try to ignore his rude remark and start to explain to him that you just want to talk. "Talk?" he responds. "Why would I want to talk to a normie like you? Why, I'd bet my mates right here could blow you out of the sky even without my help!"
    The pirate immediately cuts his connection. Well, if these pirates won't talk to you, maybe it's time to show him what you're made of. Destroying just one or two of his escorts should do the trick.]])

text[6] = _([[As the Pirate Kestrel is blown out of the sky, it occurs to you that you have made a terrible mistake. Having killed off the leader of the pirate group, you have lost your opportunity to negotiate a trade deal with the pirates. You shamefully transmit your result to Benito via a coded message and abort the mission. Perhaps you will be given another opportunity later.]])

text[7] = _([[The pirate leader comes on your screen once again. "Lucky shot, normie!" he says before promptly terminating the connection once again. Perhaps you need to destroy some more of his escorts so he can see you're just a bit more than a "normie".]])

text[8] = _([[The pirate comes on your view screen once again, but his expression has changed this time. He's hiding it, but you can tell that he's afraid of what you might do to him. You come to the realization that he is finally willing to talk and suppress a sigh of relief.
    "L-look, we got off on the wrong foot, eh? I've misjudged you lot. I guess FLF pilots can fight after all."]])

text[9] = _([[You begin to talk to the pirate about what you and the FLF are after, and the look of fear on the pirate's face fades away. "Supplies? Yeah, we've got supplies, alright. But it'll cost you! Heh, heh, heh..." You inquire as to what the cost might be. "Simple, really. We want to build another base in the %s system. We can do it ourselves, of course, but if we can get you to pay for it, even better! Specifically, we need another %s of ore to build the base. So you bring it back to the Anger system, and we'll call it a deal!
    "Oh yeah, I almost forgot; you don't know how to get to the Anger system, now, do you? Well, since you've proven yourself worthy, I suppose I'll let you in on our little secret." He transfers a file to your ship's computer. When you look at it, you see that it's a map showing a single hidden jump point. "Now, away with you! Meet me in the %s system when you have the loot."]])

text[10] = _([["Ha, you came back after all! Wonderful. I'll just take that ore, then." You hesitate for a moment, but considering the number of pirates around, they'll probably take it from you by force if you refuse at this point. You jettison the cargo into space, which the Kestrel promptly picks up with a tractor beam. "Excellent! Well, it's been a pleasure doing business with you. Send your mates over to the new station whenever you're ready. It should be up and running in just a couple periods or so. And in the meantime, you can consider yourselves one of us! Bwa ha ha!"
    You exchange what must for lack of a better word be called pleasantries with the pirate, with him telling a story about a pitifully armed Mule he recently plundered and you sharing stories of your victories against Dvaered scum. You seem to get along well. You then part ways. Now to report to Benito....]])

text[11] = _([[You greet Benito in a friendly manner as always, sharing your story and telling her the good news before handing her a chip with the map data on it. She seems pleased. "Excellent," she says. "We'll begin sending our trading convoys out right away. We'll need lots of supplies for our next mission! Thank you for your service, %s. Your pay has been deposited into your account. It will be a while before we'll be ready for your next big mission, so you can do some missions on the mission computer in the meantime. And don't forget to visit the Pirate worlds yourself and bring your own ship up to par!
    "Oh, one last thing. Make sure you stay on good terms with the pirates, yeah? The next thing you should probably do is buy a Skull and Bones ship; pirates tend to respect those who use their ships more than those who don't. And make sure to destroy Dvaered scum with the pirates around! That should keep your reputation up." You make a mental note to do what she suggests as she excuses herself and heads off.]])

comm_pirate = _("Har, har, har! You're hailing the wrong ship, buddy. Latest word from the boss is you're a weakling just waiting to be plundered!")
comm_pirate_friendly = _("I guess you're not so bad after all!")
comm_boss_insults = {}
comm_boss_insults[1] = _("You call those weapons? They look more like babies' toys to me!")
comm_boss_insults[2] = _("What a hopeless weakling!")
comm_boss_insults[3] = _("What, did you really think I would be impressed that easily?")
comm_boss_insults[4] = _("Keep hailing all you want, but I don't listen to weaklings!")
comm_boss_insults[5] = _("We'll have your ship plundered in no time at all!")
comm_boss_incomplete = _("Don't be bothering me without the loot, you hear?")

misn_title = _("Pirate Talks")
misn_desc = _("You are to seek out pirates in the %s system and try to convince them to become trading partners with the FLF.")
misn_reward = _("Supplies for the FLF")

npc_name = _("Benito")
npc_desc = _("You see exhaustion on Benito's face. Perhaps you should see what's up.")

osd_title   = _("Pirate Alliance")
osd_desc    = {}
osd_desc[1] = _("Fly to the %s system")
osd_desc[2] = _("Find pirates and try to talk to (hail) them")
osd_desc["__save"] = true

osd_apnd    = {}
osd_apnd[3] = _("Destroy some of the weaker pirate ships, then try to hail the Kestrel again")
osd_apnd[4] = _("Bring %s of Ore to the Pirate Kestrel in the %s system")

osd_final   = _("Return to FLF base")
osd_desc[3] = osd_final

log_text = _([[You helped the Pirates to build a new base in the Anger system and established a trade alliance between the FLF and the Pirates. Benito suggested that you should buy a Skull and Bones ship from the pirates and destroy Dvaered ships in areas where pirates are to keep your reputation with the pirates up. She also suggested you may want to upgrade your ship now that you have access to the black market.]])


function create ()
   missys = system.get("Tormulex")
   missys2 = system.get("Anger")
   if not misn.claim(missys) then
      misn.finish(false)
   end

   asked = false

   misn.setNPC(npc_name, "flf/unique/benito.png", npc_desc)
end


function accept ()
   local txt = ask_again_text

   if not asked then
      txt = ask_text
      tk.msg("", fmt.f(intro_text, {player=player.name(), sys=missys:name()}))
   end

   if tk.yesno("", fmt.f(txt, {player=player.name()})) then
      tk.msg("", yes_text)

      misn.accept()

      osd_desc[1] = osd_desc[1]:format( missys:name() )
      misn.osdCreate( osd_title, osd_desc )
      misn.setTitle( misn_title )
      misn.setDesc( misn_desc:format( missys:name() ) )
      marker = misn.markerAdd( missys, "plot" )
      misn.setReward( misn_reward )

      stage = 0
      pirates_left = 0
      boss_hailed = false
      boss_impressed = false
      boss = nil
      pirates = nil
      boss_hook = nil

      ore_needed = 40
      credits = 300000
      reputation = 1
      pir_reputation = 10
      pir_starting_reputation = faction.get("Pirate"):playerStanding()

      hook.enter("enter")
   else
      tk.msg("", no_text)
      misn.finish()
   end
end


function pilot_hail_pirate ()
   player.commClose()
   if stage <= 1 then
      player.msg( comm_pirate )
   else
      player.msg( comm_pirate_friendly )
   end
end


function pilot_hail_boss ()
   player.commClose()
   if stage <= 1 then
      if boss_impressed then
         stage = 2
         local standing = faction.get("Pirate"):playerStanding()
         if standing < 25 then
            faction.get("Pirate"):setPlayerStanding( 25 )
         end

         if boss ~= nil then
            boss:changeAI( "pirate" )
            boss:setHostile( false )
            boss:setFriendly()
         end
         if pirates ~= nil then
            for i, j in ipairs( pirates ) do
               if j:exists() then
                  j:changeAI( "pirate" )
                  j:setHostile( false )
                  j:setFriendly()
               end
            end
         end

         tk.msg( "", text[8] )
         tk.msg( "", text[9]:format(
            missys2:name(), tonnestring( ore_needed ), missys2:name() ) )

         player.outfitAdd( "Map: FLF-Pirate Route" )
         if marker ~= nil then misn.markerRm( marker ) end
         marker = misn.markerAdd( missys2, "plot" )

         osd_desc[4] = osd_apnd[4]:format( tonnestring( ore_needed ), missys2:name() )
         osd_desc[5] = osd_final
         misn.osdCreate( osd_title, osd_desc )
         misn.osdActive( 4 )
      else
         if boss_hailed then
            player.msg( comm_boss_insults[ rnd.rnd( 1, #comm_boss_insults ) ] )
         else
            boss_hailed = true
            if stage <= 0 then
               tk.msg( "", text[5] )
               osd_desc[3] = osd_apnd[3]
               osd_desc[4] = osd_final
               misn.osdCreate( osd_title, osd_desc )
               misn.osdActive( 3 )
            else
               tk.msg( "", text[7] )
            end
         end
      end
   elseif player.pilot():cargoHas( "Ore" ) >= ore_needed then
      tk.msg( "", text[10] )
      stage = 3
      player.pilot():cargoRm( "Ore", ore_needed )
      hook.rm( boss_hook )
      hook.land( "land" )
      misn.osdActive( 5 )
      if marker ~= nil then misn.markerRm( marker ) end
   else
      player.msg( comm_boss_incomplete )
   end
end


function pilot_death_pirate ()
   if stage <= 1 then
      pirates_left = pirates_left - 1
      stage = 1
      boss_hailed = false
      if pirates_left <= 0 or rnd.rnd() < 0.25 then
         boss_impressed = true
      end
   end
end


function pilot_death_boss ()
   tk.msg( "", text[6] )
   misn.finish( false )
end


function enter ()
   if stage <= 1 then
      stage = 0
      if system.cur() == missys then
         pilot.clear()
         pilot.toggleSpawn( false )
         local r = system.cur():radius()
         local vec = vec2.new( rnd.rnd( -r, r ), rnd.rnd( -r, r ) )

         boss = pilot.add( "Pirate Kestrel", "Pirate", vec, nil, {ai="pirate_norun"} )
         hook.pilot( boss, "death", "pilot_death_boss" )
         hook.pilot( boss, "hail", "pilot_hail_boss" )
         boss:setHostile()
         boss:setHilight()

         pirates_left = 4
         pirates = fleet.add( pirates_left, "Hyena", "Pirate", vec, _("Pirate Hyena"), {ai="pirate_norun"} )
         for i, j in ipairs( pirates ) do
            hook.pilot( j, "death", "pilot_death_pirate" )
            hook.pilot( j, "hail", "pilot_hail_pirate" )
            j:setHostile()
         end

         misn.osdActive( 2 )
      else
         osd_desc[3] = osd_final
         osd_desc[4] = nil
         misn.osdCreate( osd_title, osd_desc )
         misn.osdActive( 1 )
      end
   elseif stage <= 2 then
      if system.cur() == missys2 then
         local r = system.cur():radius()
         local vec = vec2.new( rnd.rnd( -r, r ), rnd.rnd( -r, r ) )

         boss = pilot.add( "Pirate Kestrel", "Pirate", vec, nil, {ai="pirate_norun"} )
         hook.pilot( boss, "death", "pilot_death_boss" )
         boss_hook = hook.pilot( boss, "hail", "pilot_hail_boss" )
         boss:setFriendly()
         boss:setHilight()
         boss:setVisible()
      end
   end
end


function land ()
   if stage >= 3 and planet.cur():faction() == faction.get( "FLF" ) then
      tk.msg( "", text[11]:format( player.name() ) )
      diff.apply( "Fury_Station" )
      diff.apply( "flf_pirate_ally" )
      player.pay( credits )
      flf_setReputation( 50 )
      faction.get("FLF"):modPlayer( reputation )
      faction.get("Pirate"):modPlayerSingle( pir_reputation )
      flf_addLog( log_text )
      misn.finish( true )
   end
end


function abort ()
   faction.get("Pirate"):setPlayerStanding( pir_starting_reputation )
   local hj1 = nil
   local hj2 = nil
   hj1, hj2 = jump.get( "Tormulex", "Anger" )
   hj1:setKnown( false )
   hj2:setKnown( false )
   misn.finish( false )
end

