local formation = require "scripts/formation"

local escorthelper = {}


function escorthelper.playerform()
   local form_names = {}
   for k, v in ipairs(formation.keys) do
      form_names[k] = v:gsub("_", " "):gsub("^%l", string.upper)
   end

   form_names[#form_names+1] = "None"

   local choice = tk.choice("Formation", "Choose a formation.",
                            table.unpack(form_names))

   player.pilot():memory().formation = formation.keys[choice]
   var.push("player_formation", formation.keys[choice])
end


function escorthelper.issue_orders()
   local n, s = tk.choice(_("Escort Orders"),
         _("Select the order to give to your escorts."),
         _("Attack Target"), _("Hold Formation"), _("Return To Ship"),
         _("Clear Orders"), _("Cancel"))
   local order = nil
   local data = nil
   if s == _("Attack Target") then
      order = "e_attack"
      data = player.pilot():target()
      player.msg(string.format(_("#FEscorts:#0 Attacking %s."), data:name()))
   elseif s == _("Hold Formation") then
      order = "e_hold"
      player.msg(_("#FEscorts#0: Holding formation."))
   elseif s == _("Return To Ship") then
      order = "e_return"
      player.msg(_("#FEscorts:#0 Returning to ship."))
   elseif s == _("Clear Orders") then
      order = "e_clear"
      player.msg(_("#FEscorts:#0 Clearing orders."))
   end

   if order then
      for i, p in ipairs(player.pilot():followers()) do
         if p:exists() then
            player.pilot():msg(p, order, data)
         end
      end
   end
end


return escorthelper
