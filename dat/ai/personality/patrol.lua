-- Default task to run when idle
function idle ()
   if mem.loiter == nil then mem.loiter = 3 end
   if mem.loiter == 0 then -- Try to leave.
       local planet = ai.landplanet( mem.land_friendly )
       -- planet must exist
       if planet == nil or mem.land_planet == false then
          ai.settimer(0, rnd.uniform(1, 3))
          ai.pushtask("enterdelay")
       else
          mem.land = planet:pos()
          ai.pushtask("hyperspace")
          if not mem.tookoff then
             ai.pushtask("land")
          end
       end
   else -- Stay. Have a beer.
      -- Check to see if we want to patrol waypoints
      if mem.waypoints then
         -- If haven't started patroling, find the closest waypoint
         if not mem._waypoint_cur then
            local dist = math.huge
            local closest = nil
            for k,v in pairs(mem.waypoints) do
               local vd = ai.dist( v )
               if vd < dist then
                  dist = vd
                  closest = k
               end
            end
            mem._waypoint_cur = closest
         else
            mem._waypoint_cur = math.mod( mem._waypoint_cur, #mem.waypoints )+1
         end
         -- Go to the next position
         ai.pushtask( "loiter", mem.waypoints[ mem._waypoint_cur ] )
      else
         -- Go to a random locatioe
         sysrad = rnd.rnd() * system.cur():radius()
         angle = rnd.rnd() * 2 * math.pi
         ai.pushtask("loiter", vec2.new(math.cos(angle) * sysrad, math.sin(angle) * sysrad))
      end
      mem.loiter = mem.loiter - 1
   end
end


-- Settings
mem.waypoints = nil
mem.land_friendly = true -- Land on only friendly by default
