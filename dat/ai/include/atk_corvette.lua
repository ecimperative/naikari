function atk_corvette_init ()
   mem.atk_think = atk_heuristic_big_game_think
   mem.atk = atk_corvette
end


--[[
-- Main control function for corvette behavior.
--]]
function atk_corvette( target )
   target = _atk_com_think(target)
   if target == nil then
      return
   end

   -- Targeting stuff
   ai.hostile(target) -- Mark as hostile
   ai.settarget(target)

   -- See if the enemy is still seeable
   if not _atk_check_seeable(target) then
      return
   end

   -- Get stats about enemy
   local dist = ai.dist(target)
   local range = ai.getweaprange("forward_nonseek")
   local range2 = ai.getweaprange("turret_nonseek")

   if range2 > range then
      range = range2
   end

   -- We first bias towards range
   if dist > range * mem.atk_approach and mem.ranged_ammo > mem.atk_minammo then
      _atk_g_ranged( target, dist )

   -- Close enough to melee
   -- TODO: Corvette-specific attack functions.
   else
      if target:stats().mass < 500 then
        _atk_f_space_sup( target, dist )
      else
        mem.aggressive = true
        _atk_f_flyby( target, dist )
      end
   end
end
