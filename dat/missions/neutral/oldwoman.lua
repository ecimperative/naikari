--[[
<?xml version='1.0' encoding='utf8'?>
<mission name="An old woman">
 <flags>
  <unique />
 </flags>
 <avail>
  <priority>25</priority>
  <chance>3</chance>
  <location>Bar</location>
  <faction>Dvaered</faction>
  <cond>planet.cur():class() ~= "0" and planet.cur():class() ~= "1"</cond>
 </avail>
</mission>
--]]
-- Note: additional conditionals present in mission script!
--[[
--
-- MISSION: The complaining grandma
-- DESCRIPTION: The player is taking an old woman from Dvaered space to Sirius space.
-- The old woman keeps muttering about how the times have changed and how it used to be when she was young.
--
--]]

require "cargo_common"
require "missions/neutral/common"


-- Localization, choosing a language if Naev is translated for non-english-speaking locales.
text1 = _([[You decide to ask the old woman if there's something you can help her with.

"As a matter of fact, there is," she creaks. "I want to visit my cousin, she lives on %s, you know, in the %s system, it's in Sirius space. But I don't have a ship and those blasted passenger lines around here don't fly to Sirius space! I tell you, customer service really has gone down the gutter over the years. In my space faring days, there would always be some transport ready to take you anywhere! But now look at me, I'm forced to get to the spaceport bar to see if there's a captain willing to take me! It's a disgrace, that's what it is. What a galaxy we live in! But I ramble. You seem like you've got time on your hands. Fancy making a trip down to %s? I'll pay you a decent fare, of course."]])
text2 = _([["Oh, that's good of you." The old woman gives you a wrinkly smile. "I haven't seen my cousin in such a long time, it'll be great to see how she's doing, and we can talk about old times. Ah, old times. It was all so different then. The space ways were much safer, for one. And people were politer to each other too, oh yes!"

You escort the old lady to your ship, trying not to listen to her rambling. Perhaps it would be a good ideä to get her to her destination as quickly as you can.]])

text3 = _([[You help the old lady to the spacedock elevator. She keeps grumbling about how spaceports these days are so inconvenient and how advertisement holograms are getting quite cheeky of late, they wouldn't allow that sort of thing in her day. But once you deliver her to the exit terminal, she smiles at you.

"Thank you, young captain, I don't know what I would have done without you. It seems there are still decent folk out there even now. Take this, as a token of my appreciation."

The lady hands you a credit chip. Then she disappears through the terminal. Well, that was quite a passenger!]])

complaints = {}
complaints[1] = _([["You youngsters and your newfangled triple redundancy plasma feedback shunts. In my day, we had to use simple monopole instaconductors to keep our hyperdrives running!"]])
complaints[2] = _([["Tell me, youngster, why is everyone so preoccupied with Soromid enhancements these days? Cybernetic implants were good enough for us, why can't they be for you?"]])
complaints[3] = _([["My mother was from Earth, you know. She even took me there once, when I was a little girl. I still remember it like it happened yesterday. They don't make planets like that anymore these days, oh no."]])
complaints[4] = _([["What did you say this thing was? A microblink reënforced packet transmitter? No, no, don't try to explain how it works. I'm too old for this. Anything more complex than a subspace pseudo-echo projector is beyond my understanding."]])
complaints[5] = _([["You don't know how good you have it these days. When I was young, we needed to reprogram our own electronic countermeasures to deal with custom pirate tracking systems."]])
complaints[6] = _([["Time passes by so quickly. I remember when this was all space."]])
complaints[7] = _([["All this automation is making people lax, I tell you. My uncle ran a tight ship. If he caught you cutting corners, you'd be defragmenting the sub-ion matrix filters for a week!"]])

OSDtitle = _("The old woman")
OSD = {}
OSD[1] = _("Land on %s (%s system) to drop off the old woman")

NPCname = _("An old woman")
NPCdesc = _("You see a wrinkled old lady, a somewhat unusual sight in a spaceport bar. She's purposefully looking around.")
misndesc = _("An aging lady has asked you to ferry her to %s.")
misnreward = _("Fair monetary compensation")

log_text = _([[You escorted an old woman to her cousin in Sirius space. She was nice, albeit somewhat overly chatty.]])


function create ()
    cursys = system.cur()

    local planets = {}
    getsysatdistance(cursys, 1, 6,
        function(s)
            for i, v in ipairs(s:planets()) do
                if v:faction() == faction.get("Sirius") and v:canLand()
                        and (v:class() == "G" or v:class() == "H"
                            or v:class() == "K" or v:class() == "L"
                            or v:class() == "M" or v:class() == "O"
                            or v:class() == "Q") then
                    planets[#planets + 1] = {v, s}
                end
            end
            return false
        end)

    if #planets == 0 then
        misn.finish(false)
    end

    local index = rnd.rnd(1, #planets)
    destplanet = planets[index][1]
    destsys = planets[index][2]

    curplanet = planet.cur()
    misn.setNPC(NPCname, "neutral/unique/oldwoman.png", NPCdesc)
end


function accept ()
    if tk.yesno("", text1:format(destplanet:name(), destsys:name(), destplanet:name())) then
        tk.msg("", text2)
        local commod = misn.cargoNew(N_("Old Woman"), N_("An old woman who you agreed to Sirius space."))
        oldwoman = misn.cargoAdd(commod, 0)

        misn.accept()
        misn.setDesc(misndesc:format(destplanet:name()))
        misn.setReward(misnreward)
        OSD[1] = OSD[1]:format(destplanet:name(), destsys:name())
        misn.osdCreate(OSDtitle, OSD)
        misn.markerAdd(destsys, "high")

        dist_total = cargo_calculateDistance(system.cur(), planet.cur():pos(), destsys, destplanet)
        complaint = 0

        hook.date(time.create(0, 0, 300), "date")
        hook.land("land")
    else
        misn.finish()
    end
end

-- Date hook.
function date()
    local dist_now = cargo_calculateDistance(system.cur(), player.pos(), destsys, destplanet) 
    local complaint_now = math.floor(((dist_total - dist_now) / dist_total) * #complaints + 0.5)
    if complaint_now > complaint then
        complaint = complaint_now
        tk.msg("", complaints[complaint])
    end
    -- Uh... yeah.
end

-- Land hook.
function land()
    if planet.cur() == destplanet then
        tk.msg("", text3)
        player.pay(500000) -- 500K
        addMiscLog(log_text)
        misn.finish(true)
    end
end
