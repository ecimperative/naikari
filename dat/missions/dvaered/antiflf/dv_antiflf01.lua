--[[
<?xml version='1.0' encoding='utf8'?>
<mission name="Take the Dvaered crew home">
 <flags>
  <unique />
 </flags>
 <avail>
  <priority>19</priority>
  <chance>100</chance>
  <location>None</location>
 </avail>
 <notes>
  <done_evt name="FLF/DV Derelicts">If you choose to help the Dvaered</done_evt>
  <campaign>Doom the FLF</campaign>
 </notes>
</mission>
--]]
--[[
-- This is the first mission in the anti-FLF Dvaered campaign. The player is tasked with ferrying home some Dvaered people.
-- stack variable flfbase_flfshipkilled: Used to determine whether the player destroyed the FLF derelict, as requested. Affects the reward.
-- stack variable flfbase_intro:
--      1 - The player has turned in the FLF agent or rescued the Dvaered crew. Conditional for dv_antiflf02
--      2 - The player has rescued the FLF agent. Conditional for flf_pre02
--      3 - The player has found the FLF base for the Dvaered, or has betrayed the FLF after rescuing the agent. Conditional for dv_antiflf03
--]]

require "missions/dvaered/common"


start_text = _([["Your arrival is timely, citizen," the Dvaered commanding officer tells you. "Listen up. We were in a firefight with a rogue terrorist, but the bastard knocked out our engines and most of our primary systems before we could nail him. Fortunately, I think we inflicted serious damage on him as well, so he should still be around here somewhere. My sensors are down, though, so I can't tell for certain."

The officer draws himself up and assumes the talking-to-subordinates tone that is so typical for Dvaered commanders. "Citizen! You are hereby charged to scout the area, dispose of the enemy ship, then deliver me and my crew to the nearest Dvaered controlled system!"]])

land_killed_text = _([[The Dvaered crew file out of your ship. You didn't really get to know them on this trip, they kept to themselves. The commanding officer brings up the rear of the departing crew, but he stops when he passes by you.

"Well done citizen," he says. "You have done your duty as an upstanding member of society by rendering assistance to an official Dvaered patrol. In addition, you complied with your instructions and destroyed a terrorist that threatened the peace and stability of the region. You have been rewarded appropriately." You check your balance and see that you have indeed been paid a reasonable amount.]])

land_spared_text = _([[The Dvaered crew file out of your ship. You didn't really get to know them on this trip, they kept to themselves. The commanding officer brings up the rear of the departing crew, but he stops when he passes by you.

"Well done citizen," he says. "You have done your duty as an upstanding member of society by rendering assistance to an official Dvaered patrol. However, you failed to comply with instructions, and let a potentially dangerous terrorist get away with his crimes. I will not apply any penalties in light of the situation, but consider yourself reprimanded."]])

finish_text = _([[The officer turns to leave, but then appears to have remembered something, because he turns back at you again.

"Incidentally, citizen. The Dvaered authorities are preparing a campaign against the FLF terrorists. You seem to be an able pilot, and we need a civilian ship as part of our strategy. If you are interested, seek out an official Dvaered liaison."

When he is gone, you find yourself wondering what this campaign he mentioned is all about. There is one way to find out if you are up to it.…]])

misn_title = _("Take the Dvaered crew home")
osd_desc = {_("Land in Dvaered territory to take the Dvaered crew home")}

misn_desc = _("Take the Dvaered crew on board your ship to any Dvaered controlled world or station.")
misn_reward = _("A chance to aid in the effort against the FLF")

log_text = _([[You rescued the crew of a Dvaered ship that was disabled by an FLF ship. The Dvaered officer mentioned that a campaign is being prepared against the FLF terrorists; if you are interested in joining in that operation, you can seek out a Dvaered liaison.]])


function create()
    -- Note: this mission makes no system claims.
    misn.accept()

    tk.msg("", start_text)

    misn.osdCreate(misn_title, osd_desc)
    misn.setDesc(misn_desc)
    misn.setTitle(misn_title)
    misn.setReward(misn_reward)

    local c = misn.cargoNew(N_("Dvaered Ship Crew"), N_("Dvaered crew from a ship that was disabled by the FLF."))
    DVcrew = misn.cargoAdd(c, 0)

    hook.land("land")
end

function land()
    if planet.cur():faction() == faction.get("Dvaered") then
        if var.peek("flfbase_flfshipkilled") then
            player.pay(100000) -- 100K
            tk.msg("", land_killed_text)
        else
            tk.msg("", land_spared_text)
        end
        tk.msg("", finish_text)
    end
    misn.cargoJet(DVcrew)
    var.push("flfbase_intro", 1)
    var.pop("flfbase_flfshipkilled")
    dv_addAntiFLFLog(log_text)
    misn.finish(true)
end

function abort()
    var.pop("flfbase_flfshipkilled")
    misn.finish(false)
end
