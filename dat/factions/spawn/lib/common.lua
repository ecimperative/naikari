local scom = {}


-- @brief Calculates when next spawn should occur
function scom.calcNextSpawn( cur, new, max )
   if cur == 0 then return rnd.rnd(0, 10) end -- Kickstart spawning.

   local stddelay = 10 -- seconds
   -- Seconds. No fleet can ever take more than this to show up.
   local maxdelay = 480
   local stdfleetsize = 1/4 -- The fraction of "max" that gets the full standard delay. Fleets bigger than this portion of max will have longer delays, fleets smaller, shorter.
   local delayweight = 1 -- A scalar for tweaking the delay differences. A bigger number means bigger differences.
   local percent = (cur + new) / max
   local penaltyweight = 1 -- Further delays fleets that go over the presence limit.
   if percent > 1 then
      penaltyweight = 1 + 10 * (percent - 1)
   end

   local fleetratio = (new/max)/stdfleetsize -- This turns into the base delay multiplier for the next fleet.

   return math.min(stddelay * fleetratio * delayweight * penaltyweight, maxdelay)
end


--[[
   @brief Creates the spawn table based on a weighted spawn function table.
      @param weights Weighted spawn function table to use to generate the spawn table.
      @return The matching spawn table.
--]]
function scom.createSpawnTable( weights )
   local spawn_table = {}
   local max = 0

   -- Create spawn table
   for k,v in pairs(weights) do
      max = max + v
      spawn_table[ #spawn_table+1 ] = { chance = max, func = k }
   end

   -- Safety check
   if max == 0 then
      error(_("No weight specified"))
   end

   -- Normalize
   for k,v in ipairs(spawn_table) do
      v["chance"] = v["chance"] / max
   end

   -- Job done
   return spawn_table
end


-- @brief Chooses what to spawn
function scom.choose( stable )
   local r = rnd.rnd()
   for k,v in ipairs( stable ) do
      if r < v["chance"] then
         return v["func"]()
      end
   end
   error(_("No spawn function found"))
end


-- @brief Actually spawns the pilots
function scom.spawn(pilots, faction, guerilla, spawn_hook)
   local spawned = {}

   -- Case no pilots
   if pilots == nil then
      return nil
   end

   local leader = nil
   -- Find a suitable spawn point.
   local origin, origin_type = pilot.choosePoint(faction, false, guerilla)
   for i = 1, #pilots do
      local v = pilots[i]
      local params = v.params or {}
      if not leader then
         if pilots.__formation ~= nil then
            leader:memory().formation = pilots.__formation
         end
      end
      local pfact = params.faction or faction
      local p = pilot.add(v.pilot, pfact, origin, params.name, params)
      local mem = p:memory()
      -- Mark that it was spawned naturally.
      mem.natural = true
      mem.spawn_origin_type = origin_type

      -- 
      -- If should have a leader, set leader.
      if not pilots.__nofleet then
         p:setLeader(leader)
      end

      -- Trigger spawn hook, if any.
      if spawn_hook ~= nil then
         naik.hookTrigger(spawn_hook, p)
      end

      spawned[#spawned + 1] = {pilot=p, presence=v.presence}
   end
   return spawned
end


-- @brief adds a pilot to the table
function scom.addPilot( pilots, name, presence, params )
   pilots[ #pilots+1 ] = { pilot=name, presence=presence, params=params }
   pilots.__presence = (pilots.__presence or 0) + presence
end


-- @brief Gets the presence value of a group of pilots
function scom.presence( pilots )
   return pilots.__presence or 0
end


-- @brief Default decrease function
function scom.decrease( cur, max, timer )
   return timer
end

return scom
