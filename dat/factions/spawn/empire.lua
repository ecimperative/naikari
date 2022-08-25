local scom = require "factions.spawn.lib.common"

local formation = require "scripts.formation"

-- @brief Spawns a small patrol fleet.
function spawn_patrol ()
   local pilots = {}
   local r = rnd.rnd()

   if r < 0.5 then
      scom.addPilot(pilots, "Empire Lancelot", 25)
   elseif r < 0.8 then
      scom.addPilot(pilots, "Empire Lancelot", 25)
      scom.addPilot(pilots, "Empire Shark", 20)
   else
      scom.addPilot(pilots, "Empire Pacifier", 70)
   end

   return pilots
end


-- @brief Spawns a medium sized squadron.
function spawn_squad ()
   local pilots = {}
   local r = rnd.rnd()

   if r < 0.5 then
      scom.addPilot(pilots, "Empire Admonisher", 45)
      scom.addPilot(pilots, "Empire Lancelot", 25)
      scom.addPilot(pilots, "Empire Shark", 20)
   elseif r < 0.8 then
      scom.addPilot(pilots, "Empire Admonisher", 45)
      scom.addPilot(pilots, "Empire Lancelot", 25)
   else
      scom.addPilot(pilots, "Empire Pacifier", 110)
      scom.addPilot(pilots, "Empire Lancelot", 25)
      scom.addPilot(pilots, "Empire Shark", 20)
   end

   return pilots
end


-- @brief Spawns a capship with escorts.
function spawn_capship ()
   local pilots = {}
   local r = rnd.rnd()

   -- Generate the capship
   if r < 0.7 then
      scom.addPilot(pilots, "Empire Hawking", 215)
   else
      scom.addPilot(pilots, "Empire Peacemaker", 615)
   end

   -- Generate the escorts
   r = rnd.rnd()
   if r < 0.5 then
      scom.addPilot(pilots, "Empire Lancelot", 25)
      scom.addPilot(pilots, "Empire Lancelot", 25)
      scom.addPilot(pilots, "Empire Shark", 20)
   elseif r < 0.8 then
      scom.addPilot(pilots, "Empire Admonisher", 45)
      scom.addPilot(pilots, "Empire Lancelot", 25)
   else
      scom.addPilot(pilots, "Empire Pacifier", 70)
      scom.addPilot(pilots, "Empire Lancelot", 25)
   end

   return pilots
end



-- @brief Spawns a fleet.
function spawn_fleet ()
   local pilots = {}
   pilots.__formation = formation.random_key()

   scom.addPilot(pilots, "Empire Peacemaker", 165)

   for i=1,(3 + rnd.sigma()) do
      scom.addPilot(pilots, "Empire Hawking", 140)
   end

   for i=1,(10 + 5 * rnd.sigma()) do
      if rnd.rnd() < 0.5 then
          scom.addPilot(pilots, "Empire Shark", 20)
      else
          scom.addPilot(pilots, "Empire Lancelot", 25)
      end
   end

   for i=1,(7 + 3 * rnd.sigma()) do
      if rnd.rnd() < 0.7 then
         scom.addPilot(pilots, "Empire Admonisher", 45)
      else
         scom.addPilot(pilots, "Empire Pacifier", 70)
      end
   end

   return pilots
end



-- @brief Creation hook.
function create (max)
   local weights = {}

    -- Create weights for spawn table
    weights[ spawn_patrol  ] = 100
    weights[ spawn_squad   ] = math.max(1, -80 + 0.80 * max)
    weights[ spawn_capship ] = math.max(1, -500 + 1.70 * max)
    --weights[ spawn_fleet ] = 100

   -- Create spawn table base on weights
   spawn_table = scom.createSpawnTable(weights)

   -- Calculate spawn data
   spawn_data = scom.choose(spawn_table)

   return scom.calcNextSpawn(0, scom.presence(spawn_data), max)
end


-- @brief Spawning hook
function spawn (presence, max)
   -- Over limit
   if presence > max then
      return 5
   end

   -- Actually spawn the pilots
   local pilots = scom.spawn(spawn_data, "Empire")

   -- Calculate spawn data
   spawn_data = scom.choose(spawn_table)

   return scom.calcNextSpawn(presence, scom.presence(spawn_data), max), pilots
end


