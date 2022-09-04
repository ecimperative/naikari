require("ai/tpl/generic")
require("ai/personality/patrol")

-- Settings
mem.control_rate = 0.5 -- Lower control rate
mem.aggressive = true
mem.land_planet = false

function create ()
   local sprice = ai.pilot():ship():price()
   mem.kill_reward = rnd.rnd(0.25 * sprice, 1 * sprice)

   mem.loiter = 3 -- This is the amount of waypoints the pilot will pass through before leaving the system
   mem.comm_no = _("No response.")
   create_post()
end
