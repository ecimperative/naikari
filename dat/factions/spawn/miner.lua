local scom = require "factions.spawn.lib.common"


-- @brief Spawns a small group of miners
function spawn_patrol ()
   local pilots = {}
   local r = rnd.rnd()
   if r < 0.5 then
      scom.addPilot( pilots, "Llama", 20, {name=_("Miner Llama"), ai="miner"}  )
   elseif r < 0.8 then
      scom.addPilot( pilots, "Koäla", 40, {name=_("Miner Koäla"), ai="miner"} )
   else
      scom.addPilot( pilots, "Mule", 45, {name=_("Miner Mule"), ai="miner"} )
   end

   return pilots
end


-- @brief Creation hook.
function create ( max )
   local weights = {}

    -- Create weights for spawn table
   weights[ spawn_patrol  ] = 100

   -- Create spawn table base on weights
   spawn_table = scom.createSpawnTable( weights )

   -- Calculate spawn data
   spawn_data = scom.choose( spawn_table )

   return scom.calcNextSpawn( 0, scom.presence(spawn_data), max )
end


-- @brief Spawning hook
function spawn ( presence, max )
   -- Over limit
   if presence > max then
      return 5
   end

   -- Actually spawn the pilots
   local pilots = scom.spawn( spawn_data, "Miner" )

   -- Calculate spawn data
   spawn_data = scom.choose( spawn_table )

   return scom.calcNextSpawn( presence, scom.presence(spawn_data), max ), pilots
end
