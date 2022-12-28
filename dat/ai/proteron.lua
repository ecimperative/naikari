require("ai/tpl/generic")
require("ai/personality/patrol")
local fmt = require "fmt"

-- Settings
mem.armour_run = 40
mem.armour_return = 70
mem.aggressive = true


function create()
   local p = ai.pilot()
   local sprice = p:ship():price()
   ai.setcredits(rnd.rnd(0.35 * sprice, 0.85 * sprice))
   mem.kill_reward = rnd.rnd(0.15 * sprice, 0.2 * sprice)

   -- Get refuel chance
   local standing = p:faction():playerStanding()
   mem.refuel = rnd.rnd(2000, 4000)
   if standing < 20 then
      mem.refuel_no = _("\"Begone, commoner. My fuel isn't for sale.\"")
   elseif standing < 70 then
      if rnd.rnd() < 0.8 then
         mem.refuel_no = _("\"My fuel isn't for sale.\"")
      end
   else
      mem.refuel = mem.refuel * 0.6
   end
   -- Most likely no chance to refuel
   mem.refuel_msg = fmt.f(_("\"I can transfer some fuel for {credits}.\""),
         {credits=fmt.credits(mem.refuel)})

   -- See if can be bribed
   if rnd.rnd() < 0.4 then
      mem.bribe = math.sqrt(p:stats().mass) * (500*rnd.rnd() + 1750)
      mem.bribe_prompt = fmt.f(
            _("\"I can always use some income. {credits} and you were never here.\""),
            {credits=fmt.credits(mem.bribe)})
      mem.bribe_paid = _("\"Get lost before I have to dispose of you.\"")
   else
      bribe_no = {
         _("\"You won't buy your way out of this one.\""),
         _("\"We like to make examples out of scum like you.\""),
         _("\"You've made a huge mistake.\""),
         _("\"Bribery carries a harsh penalty, scum.\""),
         _("\"I'm not interested in your blood money!\""),
         _("\"All the money in the universe won't save you now!\""),
         _("\"Of course common scum like you would try bribery! You die!\""),
      }
      mem.bribe_no = bribe_no[rnd.rnd(1, #bribe_no)]
   end

   mem.loiter = 3 -- This is the amount of waypoints the pilot will pass through before leaving the system

   -- Finish up creation
   create_post()
end

-- taunts
function taunt ( target, offense )

   -- Only 50% of actually taunting.
   if rnd.rnd(0,1) == 0 then
      return
   end

   -- some taunts
   if offense then
      taunts = {
            _("Animals like you don't deserve to live!"),
            _("Begone from this universe, inferior scum!"),
            _("We will cleanse you and all other scum from this universe!"),
            _("Enemies of the state will not be tolerated!"),
            _("Long live the Proteron!"),
            _("War is peace!"),
            _("Freedom is slavery!"),
      }
   else
      taunts = {
            _("How dare you attack the Proteron?!"),
            _("I will have your head!"),
            _("You'll regret that!"),
            _("Your fate has been sealed, dissident!"),
            _("You will pay for your treason!"),
            _("Die along with the old Empire!"),
      }
   end

   ai.pilot():comm(target, taunts[ rnd.rnd(1,#taunts) ])
end


