--[[

   FLF mission common functions.

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


-- Get a random system with FLF presence.
function flf_getSystem ()
   local choices = {}
   for i, j in ipairs( system.getAll() ) do
      local p = j:presences()
      if p["FLF"] then
         choices[#choices + 1] = j:nameRaw()
      end
   end
   return system.get(choices[rnd.rnd(1, #choices)])
end


-- Get a system generally good for an FLF mission.
-- These are systems which have both FLF and Dvaered presence.
function flf_getTargetSystem ()
   local choices = {}
   for i, j in ipairs( system.getAll() ) do
      local p = j:presences()
      if p["FLF"] and p["Dvaered"] then
         choices[#choices + 1] = j:nameRaw()
      end
   end
   return system.get(choices[rnd.rnd(1, #choices)])
end


-- Get a system with both FLF and Empire presence.
function flf_getEmpireSystem ()
   local choices = {}
   for i, j in ipairs( system.getAll() ) do
      local p = j:presences()
      if p["FLF"] and p["Empire"] then
         choices[#choices + 1] = j:nameRaw()
      end
   end
   return system.get(choices[rnd.rnd(1, #choices)])
end


-- Get a system with both FLF and Pirate presence.
function flf_getPirateSystem ()
   local choices = {}
   for i, j in ipairs( system.getAll() ) do
      local p = j:presences()
      if p[ "FLF" ] and p[ "Pirate" ] then
         choices[#choices + 1] = j:nameRaw()
      end
   end
   return system.get(choices[rnd.rnd(1, #choices)])
end


-- Add an entry to the FLF campaign log.
function flf_addLog( text )
   shiplog.create("flf", p_("log", "FLF"))
   shiplog.append("flf", text)
end
