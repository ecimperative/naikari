--[[
   The new "brushed" UI.
--]]

require "numstring"
local eh = require "escorthelper"

function create()
   --Get Player
   pp = player.pilot()
   pp = player.pilot()
   pfact = pp:faction()
   pname = player.name()
   pship = pp:ship()

   -- Set default formation
   local savedform = var.peek("player_formation")
   player.pilot():memory().formation = savedform

   --Get sizes
   screen_w, screen_h = gfx.dim()
   deffont_h = gfx.fontSize()
   smallfont_h = gfx.fontSize(true)

   --Colors
   col_shield = colour.new(  42/255,  57/255,  162/255 )
   col_armour = colour.new(  80/255,  80/255,  80/255 )
   col_energy = colour.new(  36/255,  125/255,  51/255 )
   col_fuel   = colour.new(  135/255,  24/255,  50/255 )
   col_heat   = colour.new(  80/255,  27/255,  24/255 )
   col_stress = colour.new(  45/255,  48/255,  102/255 )
   col_ammo   = colour.new( 159/255,  93/255,  15/255 )
   col_top_shield = colour.new(  88/255,  96/255, 156/255 )
   col_top_armour = colour.new( 122/255, 122/255, 122/255 )
   col_top_energy = colour.new(  52/255, 172/255,  71/255 )
   col_top_fuel   = colour.new( 156/255,  88/255, 104/255 )
   col_top_heat   = colour.new( 188/255,  63/255,  56/255 )
   col_top_stress = colour.new(  56/255,  88/255, 156/255 )
   col_top_ammo   = colour.new( 233/255, 131/255,  21/255 )
   col_text = colour.new( 203/255, 203/255, 203/255 )
   col_unkn = colour.new( 130/255, 130/255, 130/255 )
   col_lgray = colour.new( 160/255, 160/255, 160/255 )

   --Images
   local base = "gfx/gui/brushed/"
   local function tex_open( name, sx, sy )
      local t = tex.open( base .. name, sx, sy )
      t:setWrap( "clamp" )
      return t
   end
   main = tex_open( "main.png" )
   ext_right = tex_open( "extRight.png" )
   end_right = tex_open( "endRight.png" )
   popup_bottom = tex_open( "popupBottom.png" )
   popup_bottom_side_left = tex_open( "tooltipRightSideLeft.png" )
   popup_bottom2 = tex_open( "tooltipRightBottom.webp" )
   popup_body = tex_open( "tooltipRight.webp" )
   popup_top = tex_open( "tooltipRightTop.webp" )
   popup_empty = tex_open( "tooltipEmpty.webp" )
   popup_pilot = tex_open( "pilotFrame.png" )
   bar_bg = tex_open( "barBg.webp" )
   bar_frame_light = tex_open( "barFrameLight.png" )
   bar_frame = tex_open( "barFrame.png" )
   bar_light = tex_open( "light.png" )
   bar_light = tex_open( "light.png" )
   bar_lock = tex_open( "lock.webp" )
   planet_pane_t = tex_open( "frame_planet_top.png" )
   planet_pane_m = tex_open( "frame_planet_middle.png" )
   planet_pane_b = tex_open( "frame_planet_bottom.png" )
   planet_bg = tex_open( "planet_image.png" )
   fleet_pane_t = tex_open( "frame_fleet_top.webp" )
   fleet_pane_m = planet_pane_m
   fleet_pane_b = planet_pane_b
   icon_shield = tex_open( "iconShield.png" )
   icon_armour = tex_open( "iconArmour.png" )
   icon_energy = tex_open( "iconEnergy.webp" )
   icon_fuel = tex_open( "iconFuel.png" )
   icon_Kinetic = tex_open( "kinetic.png" )
   icon_Radiation = tex_open( "nuclear.png" )
   icon_EMP = tex_open( "ion.png" )
   icon_Energy = tex_open( "plasma.png" )
   icon_Ion = tex_open( "laser.png" )
   icon_missile = tex_open( "missile.png" )
   icon_projectile = tex_open( "projectile.png" )
   icon_beam = tex_open( "beam.png" )
   icon_weapon2 = tex_open( "weapon2.png" )
   icon_weapon1 = tex_open( "weapon1.png" )
   icon_outfit = tex_open( "outfit.png" )
   icon_pnt_target = tex_open( "iconPntTarg.png" )
   icon_nav_target = tex_open( "iconNavTarg.png" )
   icon_money = tex_open( "iconMoney.png" )
   icon_cargo = tex_open( "iconCargo.webp" )
   icon_missions = tex_open( "iconMissions.png" )
   icon_ship = tex_open( "iconShip.png" )
   icon_weapons = tex_open( "iconWeaps.png" )
   icon_autonav = tex_open( "A.png" )
   icon_lockon = tex_open( "lockon.png" )
   icon_refire = tex_open( "refireCircle.png" )
   icon_lockon2 = tex_open( "lockonCircle.png" )
   field_bg_left = tex_open( "fieldBgLeft1.png" )
   field_bg_center1 = tex_open( "fieldBgCenter1.webp" )
   field_bg_center2 = tex_open( "fieldBgCenter2.webp" )
   field_bg_right1 = tex_open( "fieldBgRight1.png" )
   field_bg_right2 = tex_open( "fieldBgRight2.png" )
   field_frame_left = tex_open( "fieldFrameLeft.png" )
   field_frame_center = tex_open( "fieldFrameCenter.png" )
   field_frame_right = tex_open( "fieldFrameRight.png" )
   target_bg = tex_open( "targetBg.png" )
   target_frame = tex_open( "targetFrame.png" )
   question = tex_open( "question.png" )
   speed_light = tex_open( "speedOn.png" )
   speed_light_double = tex_open( "speedDouble.png" )
   speed_light_off = tex_open( "speedOff.webp" )
   top_bar = tex_open( "topbar.png" )
   top_bar_left = tex_open( "topbar_left.png" )
   top_bar_right = tex_open( "topbar_right.png" )
   top_bar_center = tex_open( "topbarCenter.png" )
   bottom_bar = tex_open( "bottombar.png" )
   bottom_bar_left = tex_open( "bottombar_left.png" )
   bottom_bar_right = tex_open( "bottombar_right.png" )
   button_normal = tex_open( "button.png" )
   button_hilighted = tex_open( "buttonHil2.png" )
   button_mouseover = tex_open( "buttonHil.png" )
   button_pressed = tex_open( "buttonPre.png" )
   button_disabled = tex_open( "buttonDis.png" )
   gui.targetPlanetGFX( tex_open( "radar_planet.png", 2, 2 ) )
   gui.targetPilotGFX(  tex_open( "radar_ship.png", 2, 2 ) )


   --Positions
   --Main is at 0,0

   margin = 14

   --Radar
   radar_x = 263
   radar_y = 5
   radar_w = 114
   radar_h = 120
   gui.radarInit(false, radar_w, radar_h)
   if gui.overlayOpen() then
      gui.radarClose()
   end

   bar_y = 2
   bar_x = 46
   bar_w, bar_h = bar_bg:dim()
   bars = { "shield", "armour", "energy", "fuel" }
   for k,v in ipairs( bars ) do
      _G[ "x_" .. v ] = bar_x + (k-1)*(bar_w + 6)
      _G[ "y_" .. v ] = bar_y
   end

   pl_speed_x = 38
   pl_speed_y = 2

   target_bar_x = 57
   target_bar_y = 92
   bars_target = { "target_shield", "target_armour", "target_energy" }
   for k, v in ipairs( bars_target ) do
      _G[ "x_" .. v ] = target_bar_x + (k-1)*(bar_w + 6)
      _G[ "y_" .. v ] = target_bar_y
   end

   target_image_x = 53
   target_image_y = 172
   target_image_w, target_image_h = target_bg:dim()
   question_w, question_h = question:dim()

   x_name = 102
   y_name = 197

   field_w, field_h = field_bg_left:dim()

   x_dist = 102
   y_dist = 172

   x_speed = 189
   y_speed = 173

   right_side_x = 406
   left_side_w, left_side_h = main:dim()
   end_right_w, end_right_h = end_right:dim()
   popup_left_x = 42
   popup_left_y = 88
   popup_right_x = 432
   popup_right_y = 88

   weapbars = math.max( 3, math.floor((screen_w - 2*margin - left_side_w - end_right_w + 10)/(bar_w + 6)) ) --number of weapon bars that can fit on the screen (minimum 3)

   circle_w, circle_h = icon_refire:dim()

   tbar_center_x = screen_w/2
   tbar_center_w, tbar_center_h = top_bar_center:dim()
   tbar_w, tbar_h = top_bar:dim()
   tbar_y = screen_h - tbar_h - margin
   tbar_left_w, tbar_left_h = top_bar_left:dim()
   tbar_right_w, tbar_right_h = top_bar_right:dim()

   bbar_w, bbar_h = bottom_bar:dim()
   bbar_left_w, bbar_left_h = bottom_bar_left:dim()
   bbar_right_w, bbar_right_h = bottom_bar_right:dim()

   gui.viewport( 0, 0, screen_w, screen_h )

   fields_y = tbar_y + 15
   if screen_w <= 1024 + 2*margin then
      fields_w = (screen_w - tbar_center_w - 2*margin) / 4 - 8
      fields_x = margin
   else
      fields_w = (1024 - tbar_center_w) / 4 - 8
      fields_x = (screen_w - 1024) / 2
   end

   buttons_y = tbar_y + tbar_h - 34
   buttons_w, buttons_h = button_normal:dim()
   buttontypes = { "missions", "cargo", "ship", "weapons" }
   buttons = {}
   for k, v in ipairs(buttontypes) do
      buttons[v] = { x=tbar_center_x-116+(k-1)*60, y=buttons_y, w=buttons_w, h=buttons_h, state="default", icon=_G[ "icon_" .. v ], action=_G["action_" .. v ] }
      buttons[v]["icon_w"], buttons[v]["icon_h"] = _G[ "icon_" .. v]:dim()
   end

   -- Messages
   mesg_x = left_side_w
   mesg_y = end_right_h + 10
   mesg_w = screen_w - mesg_x - 10
   gui.mesgInit( mesg_w, mesg_x, mesg_y )

   -- Planet pane
   ta_pnt_pane_w, ta_pnt_pane_h = planet_pane_t:dim()
   ta_pnt_pane_w_m, ta_pnt_pane_h_m = planet_pane_m:dim()
   ta_pnt_pane_w_b, ta_pnt_pane_h_b = planet_pane_b:dim()
   ta_pnt_pane_x = math.max( screen_w - ta_pnt_pane_w - 16, tbar_center_x + tbar_center_w/2 - 10 )
   ta_pnt_pane_y = tbar_y + tbar_h - 32 - ta_pnt_pane_h

   -- Fleet pane
   ta_flt_pane_w, ta_flt_pane_h = fleet_pane_t:dim()
   ta_flt_pane_w_m, ta_flt_pane_h_m = fleet_pane_m:dim()
   ta_flt_pane_w_b, ta_flt_pane_h_b = fleet_pane_b:dim()
   ta_flt_pane_x = ta_pnt_pane_x - ta_flt_pane_w - 16
   if ta_flt_pane_x < tbar_center_x + tbar_center_w / 2 - 10 then
       ta_flt_pane_x = nil
       ta_flt_pane_y = nil
   else
       ta_flt_pane_y = tbar_y + tbar_h - 32 - ta_flt_pane_h
   end

   -- Planet faction icon
   ta_pnt_fact_x = ta_pnt_pane_x + 140
   ta_pnt_fact_y = ta_pnt_pane_y + 155

   -- Planet image background
   ta_pnt_image_x = ta_pnt_pane_x + 14
   ta_pnt_image_y = ta_pnt_pane_y

   -- Planet image center
   ta_pnt_image_w, ta_pnt_image_h = planet_bg:dim()
   ta_pnt_center_x = ta_pnt_image_x + ta_pnt_image_w / 2
   ta_pnt_center_y = ta_pnt_image_y + ta_pnt_image_h / 2

    -- Set FPS
   gui.fpsPos( 18, left_side_h )

   -- Set OSD
   local osd_y = tbar_y + tbar_h - 50
   local osd_w = 225
   local osd_h = osd_y - 289
   gui.osdInit( 30, tbar_y + tbar_h - 50, osd_w, osd_h )

   first_time = { true, 2 }
   navstring = _("none")

   gui.mouseClickEnable(true)
   gui.mouseMoveEnable(true)

   update_target()
   update_ship()
   update_system()
   update_nav()
   update_cargo()
end

function roundto(num, idp)
   return string.format("%.0" .. (idp or 0) .. "f", num)
end

function round(num)
   return math.floor( num + 0.5 )
end

function largeNumber(number)
   local formatted
   local au_km = 149597870.7
   if number < 1000 then
      formatted = string.format(_("%d mAU"), math.floor(number))
   elseif number < 1e6 then
      formatted = string.format(_("%.1f AU"), number / 1000)
   elseif number < 1e9 then
      formatted = string.format(_("%.1f kAU"), number / 1e6)
   elseif number < 1e12 then
      formatted = string.format(_("%.1f MAU"), number / 1e9)
   elseif number < 1e15 then
      formatted = string.format(_("%.1f GAU"), number / 1e12)
   elseif number < 1e18 then
      formatted = string.format(_("%.1f TAU"), number / 1e15)
   end
   return formatted
end

function update_target()
   ptarget = pp:target()
   if ptarget ~= nil then
      ta_dir = ptarget:dir()
      ta_gfx = ptarget:ship():gfx()
      ta_sx, ta_sy = ta_gfx:spriteFromDir( ta_dir )
      ta_gfx_w, ta_gfx_h, ta_gfx_sw, ta_gfx_sh = ta_gfx:dim()
      ta_fact = ptarget:faction()
      ta_stats = ptarget:stats()

      ta_gfx_aspect = ta_gfx_sw / ta_gfx_sh

      if ta_gfx_aspect >= 1 then
         if ta_gfx_sw > target_image_w then
            ta_gfx_draw_w = target_image_w
            ta_gfx_draw_h = target_image_w / ta_gfx_sw * ta_gfx_sh
         else
            ta_gfx_draw_w = ta_gfx_sw
            ta_gfx_draw_h = ta_gfx_sh
         end
      else
         if ta_gfx_sh > target_h then
            ta_gfx_draw_h = target_image_w
            ta_gfx_draw_w = target_image_w / ta_gfx_sh * ta_gfx_sw
         else
            ta_gfx_draw_w = ta_gfx_sw
            ta_gfx_draw_h = ta_gfx_sh
         end
      end
   end
end

function update_nav()
   planet = {}
   nav_pnt, nav_hyp = pp:nav()
   autonav_hyp, autonav_jumps = player.autonavDest()
   if nav_pnt then
      pntflags = nav_pnt:services()

      ta_pnt_gfx = nav_pnt:gfxSpace()
      ta_pnt_gfx_w, ta_pnt_gfx_h = ta_pnt_gfx:dim()
      ta_pntfact = nav_pnt:faction()

      ta_pnt_gfx_aspect = ta_pnt_gfx_w / ta_pnt_gfx_h
      if math.max( ta_pnt_gfx_w, ta_pnt_gfx_h ) > 140 then
         ta_pnt_gfx_draw_w = math.min( 140, 140 * ta_pnt_gfx_aspect )
         ta_pnt_gfx_draw_h = math.min( 140, 140 / ta_pnt_gfx_aspect )
      end

      ta_pnt_faction_gfx = nil
      if ta_pntfact and ta_pntfact:known() then
         ta_pnt_faction_gfx = ta_pntfact:logoTiny()
      end

      planet = { -- Table for convenience.
         name = nav_pnt:name(),
         pos = nav_pnt:pos(),
         class = nav_pnt:class(),
         col = nav_pnt:colour(),
         services = {}
      }

      if pntflags.land then
         services = {
            "land", "refuel", "bar", "missions", "commodity", "outfits",
            "shipyard", "blackmarket",
         }

         for i, v in ipairs(services) do
            table.insert(planet.services, pntflags[v])
         end
         planet.nservices = #planet.services
      end
   end
   if nav_hyp then
      if nav_hyp:known() or nav_hyp:marked() then
         navstring = nav_hyp:name()
      else
         navstring = _("Unknown")
      end
      if autonav_hyp then
         navstring = (navstring .. " (%s)"):format( autonav_jumps )
      end
   else
      navstring = _("none")
   end
end

function update_faction()
end

function update_cargo()
   cargo = pp:cargoFree()
   cargolist = pp:cargoList()

   if not first_time[1] then
      if #cargolist == 0 then
         buttons["cargo"].state = "disabled"
      else
         if buttons["cargo"].state ~= "mouseover" then buttons["cargo"].state = "hilighted" end
      end
   else
      first_time[1] = false
   end
end

function update_ship()
   stats = pp:stats()

   if not first_time[2] then
      if buttons["ship"].state ~= "mouseover" then
         buttons["ship"].state = "hilighted"
      end
      if buttons["weapons"].state ~= "mouseover" then
         buttons["weapons"].state = "hilighted"
      end
   else
      first_time[2] = first_time[2] - 1
   end
end

function update_system()
end

function renderBar( name, value, light, locked, prefix, mod_x, mod_y, heat, stress )
   local offsets = { 2, 2, 4, 54, 12, -2 } --Bar/Icon x, Bar y, Sheen x, Sheen y, light x, light y

   local vars = { "col", "col_top", "x", "y", "icon" }
   for k, v in ipairs( vars ) do
      if (v == "x" or v == "y") and prefix ~= nil then
         _G[ "l_" .. v ] = _G[ v .. "_" .. prefix .. "_" .. name ]
      else
         _G[ "l_" .. v ] = _G[ v .. "_" .. name ]
      end
   end
   l_x = l_x + mod_x
   l_y = l_y + mod_y
   icon_w, icon_h = l_icon:dim()

   if locked == true then
      gfx.renderTex( bar_lock, l_x + offsets[1], l_y + offsets[2] ) --Lock
   else
      gfx.renderTex( bar_bg, l_x + offsets[1], l_y + offsets[2] ) --Background
      gfx.renderRect( l_x + offsets[1], l_y + offsets[2], bar_w, value/100. * bar_h, l_col ) --Bar

      -- Heat bar (only if heat is specified)
      if heat ~= nil then
         if heat <= 1 then
            heatcol = col_heat
            heatcol_top = col_top_heat
         end
         gfx.renderRect( l_x + offsets[1], l_y + offsets[2], bar_w/2, heat/2 * bar_h * (value/100.), heatcol ) --Heat bar
         if heat < 2 then
            gfx.renderRect( l_x + offsets[1], l_y + offsets[2] + heat/2 * bar_h * (value/100.), bar_w/2, 1, heatcol_top ) --top bit
         end
      end

      -- Stress (disable) bar (only if stress is specified)
      if stress ~= nil then
         gfx.renderRect( l_x + offsets[1] + bar_w/2, l_y + offsets[2], bar_w/2, (stress/100.) * bar_h * (value/100.), col_stress ) --Stress bar
         if stress < 100 then
            gfx.renderRect( l_x + offsets[1] + bar_w/2, l_y + offsets[2] + (stress/100.) * bar_h * value/100., bar_w/2, 1, col_top_stress ) --top bit
         end
      end

      if value < 100 then
         gfx.renderRect( l_x + offsets[1], l_y + offsets[2] + value/100. * bar_h, bar_w, 1, l_col_top ) --lighter area
      end
   end
   gfx.renderTex( l_icon, l_x + offsets[1], l_y + offsets[2] + bar_h/2 - icon_h/2 ) --Icon
   if light ~= false then
      gfx.renderTex( bar_frame_light, l_x, l_y ) --Frame
      local show_light = false
      if name == "fuel" then
         local jumps = player.jumps()
         show_light = jumps == 0
         if autonav_hyp ~= nil then
            show_light = show_light or (jumps ~= nil and jumps < autonav_jumps)
         end
      else
         show_light = value < 20
      end
      if show_light then
         gfx.renderTex( bar_light, l_x + offsets[5], l_y + offsets[6] ) --Warning light
      end
   else
      gfx.renderTex( bar_frame, l_x, l_y ) --Frame
   end
end

function renderWeapBar( weapon, x, y )
   local offsets = { 2, 2, 4, 54, 13, 23, 47 } --third last is y of icon_weapon1, last two are the centers of the two weapon icons
   local outfit_yoffset = 31
   local name_offset = 16
   if weapon ~= nil then
      if weapon.ammo ~= nil then
         width = bar_w/2
      else
          width = bar_w
      end
      if weapon.temp <= 1 then
         heatcol = col_heat
         heatcol_top = col_top_heat
      end

      if weapon.is_outfit then
         icon = outfit.get( weapon.name ):icon()
         icon_w, icon_h = icon:dim()

         if weapon.type == "Afterburner" then
            weap_heat = weapon.temp * 2
         elseif weapon.duration ~= nil then
            weap_heat = (1 - weapon.duration) * 2
         elseif weapon.cooldown ~= nil then
            weap_heat = weapon.cooldown * 2
         else
            weap_heat = 0
         end
      else
         if weapon.dtype ~= nil and weapon.dtype ~= "Unknown" and _G[ "icon_" .. weapon.dtype ]~= nil then
            top_icon = _G[ "icon_" .. weapon.dtype ]
         else
            top_icon = icon_Kinetic
         end

         if weapon.type == "Bolt Cannon" or weapon.type == "Bolt Turret" then
            bottom_icon = icon_projectile
         elseif weapon.type == "Beam" or weapon.type == "Turret Beam" then
            bottom_icon = icon_beam
         elseif weapon.type == "Launcher" or weapon.type == "Turret Launcher" then
            bottom_icon = icon_missile
         elseif weapon.type == "Fighter Bay" then
            bottom_icon = icon_ship
         else
            bottom_icon = icon_projectile
         end

         top_icon_w, top_icon_h = top_icon:dim()
         bottom_icon_w, bottom_icon_h = bottom_icon:dim()

         weap_heat = weapon.temp
      end

      gfx.renderTex( bar_bg, x + offsets[1], y + offsets[2] ) --Background
      gfx.renderRect( x + offsets[1], y + offsets[2], width, weap_heat/2 *bar_h, heatcol ) --Heat bar, mandatory
      if weap_heat < 2 then
         gfx.renderRect( x + offsets[1], y + offsets[2] + weap_heat/2 * bar_h, width, 1, heatcol_top ) --top bit
      end

      if weapon.is_outfit then
         gfx.renderTex( icon_outfit, x + offsets[1], y + offsets[5] )
         gfx.renderTexRaw( icon, x + offsets[1] + bar_w/2 - 20, y + offsets[2] + outfit_yoffset, 40, 40, 1, 1, 0, 0, 1, 1 )
         if weapon.weapset ~= nil then
            local ws_name
            if weapon.weapset == 10 then
               ws_name = "0"
            else
               ws_name = string.format( "%d", weapon.weapset )
            end
            gfx.print( false, ws_name, x + offsets[1], y + offsets[2] + name_offset, col_text, 40, true )
         end
      else
         local col = nil
         if weapon.ammo ~= nil then
            if weapon.left_p ~= nil then
               -- Ammo bar, only if applicable
               gfx.renderRect(x + offsets[1] + width, y + offsets[2],
                     width, weapon.left_p * bar_h, col_ammo)
               if weapon.left_p < 1 then
                  -- top bit
                  gfx.renderRect(x + offsets[1] + width,
                        y + offsets[2] + weapon.left_p * bar_h, width, 1,
                        col_top_ammo)
               end
            end
            if not weapon.in_arc and player.pilot():target() ~= nil then
               col = col_lgray
            end

            if weapon.lockon ~= nil then
               gfx.renderTexRaw( icon_lockon2, x + offsets[1] + bar_w/2 - circle_w/2, y + offsets[2] + offsets[6] - circle_h/2, circle_w, circle_h * weapon.lockon, 1, 1, 0, 0, 1, weapon.lockon) --Lock-on indicator
            end
            gfx.renderTexRaw( icon_refire, x + offsets[1] + bar_w/2 - circle_w/2, y + offsets[2] + offsets[7] - circle_h/2, circle_w, circle_h * weapon.cooldown, 1, 1, 0, 0, 1, weapon.cooldown) --Cooldown indicator
            --Icon
            gfx.renderTex( icon_weapon2, x + offsets[1], y + offsets[2] )
         else
            --Icon
            gfx.renderTexRaw( icon_refire, x + offsets[1] + bar_w/2 - circle_w/2, y + offsets[2] + offsets[7] - circle_h/2, circle_w, circle_h * weapon.cooldown, 1, 1, 0, 0, 1, weapon.cooldown) --Cooldown indicator
            gfx.renderTex( icon_weapon1, x + offsets[1], y + offsets[5] )
         end

         --Weapon-specific icon and number if applicable
         gfx.renderTex(bottom_icon,
            x + offsets[1] + bar_w/2 - bottom_icon_w/2,
            y + offsets[2] + offsets[7] - bottom_icon_h/2)
         if weapon.instant ~= nil then
            local ws_name
            if weapon.instant == 10 then
               ws_name = "0"
            else
               ws_name = string.format("%d", weapon.instant)
            end
            gfx.print(false, ws_name,
               x + offsets[1], y + offsets[2] + name_offset,
               col_text, 40, true)
         else
            gfx.renderTex(top_icon,
               x + offsets[1] + bar_w/2 - top_icon_w/2,
               y + offsets[2] + offsets[6] - top_icon_h/2, col)
         end
      end
      if weapon.is_outfit then
         gfx.renderTex( bar_frame_light, x, y ) -- Frame with light
         if weapon.state == "on" then
            gfx.renderTex( bar_light, x + 12, y - 2 ) -- Active light
         end
      else
         gfx.renderTex( bar_frame, x, y ) --Frame
      end
   else
      gfx.renderTex( bar_lock, x + offsets[1], y + offsets[2] )
      gfx.renderTex( bar_frame, x, y ) --Frame
   end
end

function renderField( text, x, y, w, col, icon )
   local offsets = { 3, 14, 6 } --Sheen x and y, Icon x
   local onetwo = 1

   gfx.renderTex( field_bg_left, x, y )
   drawn_w = 14
   while drawn_w < w - 14 do
      gfx.renderTex( _G[ "field_bg_center" .. tostring(onetwo) ], x + drawn_w, y )
      if onetwo == 1 then
         onetwo = 2
      else
         onetwo = 1
      end
      drawn_w = drawn_w + 2
   end
   gfx.renderTex( _G[ "field_bg_right" .. tostring(onetwo) ], x + w - 14, y )

   if icon ~= nil then
      local icon_w, icon_h = icon:dim()
      gfx.renderTex( icon, x + offsets[3], y + 11 - icon_h/2 )
      gfx.print( true, text, x+offsets[3]+icon_w+2, y+field_h/2-smallfont_h/2, col, w-(offsets[3]+icon_w+2), true )
   else
      gfx.print( true, text, x, y + field_h/2 - smallfont_h/2, col, w, true )
   end

   --gfx.renderTex( field_frame, x, y ) --Frame

   gfx.renderTex( field_frame_left, x, y )
   if w > 28 then
      gfx.renderTexRaw( field_frame_center, x+14, y, w-28, field_h, 1, 1, 0, 0, 1, 1 )
   end
   if w >= 28 then
      gfx.renderTex( field_frame_right, x+w-14, y )
   else
      gfx.renderTex( field_frame_right, x+14, y )
   end
end

function renderButton( button )
   local v_button = buttons[button]

   if v_button.state == "hilighted" then
      gfx.renderTex( button_hilighted, v_button.x, v_button.y )
   elseif v_button.state == "mouseover" then
      gfx.renderTex( button_mouseover, v_button.x, v_button.y )
   elseif v_button.state == "disabled" then
      gfx.renderTex( button_disabled, v_button.x, v_button.y )
   elseif v_button.state == "pressed" then
      gfx.renderTex( button_pressed, v_button.x, v_button.y )
   else
      gfx.renderTex( button_normal, v_button.x, v_button.y )
   end

   gfx.renderTex( v_button.icon, v_button.x+v_button.w/2-v_button.icon_w/2, v_button.y+v_button.h/2-v_button.icon_h/2 )
end

function render( dt )

   --Values
   armour, shield, stress = pp:health()
   energy = pp:energy()
   fuel = player.fuel() / stats.fuel_max * 100
   heat = math.max( math.min( (pp:temp() - 250)/87.5, 2 ), 0 )
   wset_name, pwset = pp:weapset( true )
   wset_id = string.format( "%d", pp:activeWeapset() )
   if wset_id == 10 then wset_id = 0 end
   wset = {}
   aset = pp:actives( true )
   table.sort( aset, function(a,b)
      local aset = a.weapset or 99
      local bset = b.weapset or 99
      return aset < bset
   end )

   for k, v in ipairs( pwset ) do
      v.is_outfit = false
      if v.level ~= 0 or v.instant ~= nil then
         wset[ #wset + 1 ] = v
      end
   end
   for k, v in ipairs( aset ) do
      v.is_outfit = true
      wset[ #wset + 1 ] = v
   end

   credits, credits_h = player.credits(2)
   autonav = player.autonav()
   lockons = pp:lockon()

   -- Top Bar
   gfx.renderTexRaw( top_bar, margin + tbar_left_w, tbar_y, screen_w - 2*margin - tbar_left_w - tbar_right_w, tbar_h, 1, 1, 0, 0, 1, 1 )
   gfx.renderTex( top_bar_left, margin, tbar_y )
   gfx.renderTex( top_bar_right, screen_w - margin - tbar_right_w, tbar_y )

   -- Bottom Bar
   gfx.renderTexRaw( bottom_bar, margin + bbar_left_w, margin, screen_w - 2*margin - bbar_left_w - bbar_right_w, bbar_h, 1, 1, 0, 0, 1, 1 )
   gfx.renderTex( bottom_bar_left, margin, margin )
   gfx.renderTex( bottom_bar_right, screen_w - margin - bbar_right_w, margin )

   --Main window right
   if #wset > weapbars then
      wbars_right = weapbars
   else
      wbars_right = #wset
   end
   right_side_w = (bar_w + 6)*wbars_right - 1
   gui_w = right_side_w + left_side_w - 10
   mod_x = math.max(margin, math.min(
         screen_w - 2*margin - math.max(gui_w, 1024),
         math.floor((screen_w - 2*margin - gui_w) / 2)))
   mod_y = margin + 12
   gfx.renderTexRaw( ext_right, left_side_w - 10 + mod_x, mod_y, right_side_w, end_right_h, 1, 1, 0, 0, 1, 1 )
   gfx.renderTex( end_right, right_side_x + right_side_w + mod_x, mod_y )

   right_side_h = end_right_h
   for k=1,wbars_right do
      renderWeapBar( wset[k], right_side_x + 6 + (k-1)*(bar_w + 6) + mod_x, bar_y + mod_y )
   end
   if wbars_right ~= #wset then
      --Draw a popup of (theoretically) arbitrary size.
      amount = #wset - wbars_right
      height = math.ceil(amount/3. ) * (bar_h+6) - 3
      right_side_h = right_side_h + height
      gfx.renderTex( popup_bottom2, popup_right_x + mod_x, popup_right_y + mod_y )
      gfx.renderTex( popup_top, popup_right_x + mod_x, popup_right_y + 6 + height + mod_y )
      gfx.renderTexRaw( popup_body, popup_right_x + mod_x, popup_right_y + 6 + mod_y, 165, height, 1, 1, 0, 0, 1, 1 )
      gfx.renderTex( popup_bottom_side_left, popup_right_x + 7 + mod_x, popup_right_y + mod_y )
      gfx.renderTexRaw( popup_bottom_side_left, popup_right_x + 158 + mod_x, popup_right_y + mod_y, -3, 19, 1, 1, 0, 0, 1, 1 )

      local drawn
      for i=1, (amount+1) do
         local x = (i-1) % 3 * (bar_w+6) + popup_right_x + 14
         local y = math.floor( (i-1) / 3. ) * (bar_h+6) + 3 + popup_right_y
         renderWeapBar( wset[ wbars_right + i ], x + mod_x, y + mod_y )
      end
      for i=(amount+1), math.ceil( amount/3. )*3 do
         local x = (i-1) % 3 * (bar_w+6) + popup_right_x + 14
         local y = math.floor( (i-1) / 3. ) * (bar_h+6) + 3 + popup_right_y
         renderWeapBar( nil, x + mod_x, y + mod_y )
      end
      gfx.renderTex( popup_bottom, popup_right_x + mod_x, popup_right_y - 5 + mod_y )
   end

   -- Messages
   local new_mesg_x = left_side_w + mod_x
   local new_mesg_y = right_side_h + 10 + mod_y
   local new_mesg_w = screen_w - new_mesg_x - 10
   if mesg_x ~= new_mesg_x or mesg_y ~= new_mesg_y or mesg_w ~= new_mesg_w then
      mesg_x = new_mesg_x
      mesg_y = new_mesg_y
      mesg_w = new_mesg_w
      gui.mesgInit( mesg_w, mesg_x, mesg_y )
   end

   --Main window left
   gfx.renderTex( main, mod_x, mod_y )
   if not gui.overlayOpen() then
      gui.radarOpen()
      gui.radarRender(radar_x + mod_x, radar_y + mod_y)
   else
      gui.radarClose()
   end

   if lockons > 0 then
      gfx.renderTex( icon_lockon, 379 + mod_x, 30 + mod_y )
   end
   if autonav then
      --gfx.renderTex( icon_autonav, 246 + mod_x, 52 + mod_y )
      gfx.print( false, "A", 246 + mod_x, 52 + mod_y, col_text, 12 )
   end

   for k, v in ipairs( bars ) do --bars = { "shield", "armour", "energy", "fuel" }, remember?
      local ht = nil
      local st = nil
      if v == "armour" then
         ht = heat
         st = stress
      end
      renderBar( v, _G[v], _G[v .. "_light"], nil, nil, mod_x, mod_y, ht, st )
   end

   --Weapon set indicator
   gfx.print( false, wset_id, 383 + mod_x, 72 + mod_y, col_text, 12, true )

   --Speed Lights
   local nlights = 11
   local value = round( pp:vel():mod() * nlights / stats.speed_max )
   if value > nlights * 2 then value = nlights * 2 end
   for i=1, value do
      if i <= nlights then
         gfx.renderTex( speed_light, pl_speed_x - 5 + mod_x, pl_speed_y - 3 + (i-1)*6 + mod_y )
      else
         local imod = (i - 1) % nlights
         gfx.renderTex( speed_light_double, pl_speed_x - 5 + mod_x, pl_speed_y - 3 + imod*6 + mod_y )
      end
   end
   if value < nlights then
      for i=value+1, nlights do
         gfx.renderTex( speed_light_off, pl_speed_x + mod_x, pl_speed_y + (i-1)*6 + mod_y )
      end
   end


   --Popup left
   if ptarget ~= nil then
      ta_detect, ta_scanned = pp:inrange(ptarget)

      if ta_detect then
         gfx.renderTex( popup_pilot, popup_left_x + mod_x, popup_left_y + mod_y ) --Frame

         --Target Image
         gfx.renderTex( target_bg, target_image_x + mod_x, target_image_y + mod_y )
         ta_dist = pp:pos():dist(ptarget:pos())
         if ta_scanned then
            if ta_dir ~= ptarget:dir() then
               update_target()
            end
            ta_armour, ta_shield, ta_stress = ptarget:health()
            ta_heat = math.max( math.min( (ptarget:temp() - 250)/87.5, 2 ), 0 )
            ta_energy = ptarget:energy()
            ta_name = ptarget:name()
            gfx.renderTexRaw( ta_gfx, target_image_x + target_image_w/2 - ta_gfx_draw_w/2 + mod_x, target_image_y + target_image_h/2 - ta_gfx_draw_h/2 + mod_y, ta_gfx_draw_w, ta_gfx_draw_h, ta_sx, ta_sy, 0, 0, 1, 1 )
            renderBar( "shield", ta_shield, false, false, "target", mod_x, mod_y )
            renderBar( "armour", ta_armour, false, false, "target", mod_x, mod_y, ta_heat, ta_stress )
            renderBar( "energy", ta_energy, false, false, "target", mod_x, mod_y )
            renderField( ta_name, x_name + mod_x, y_name + mod_y, 86, col_text )
            renderField( tostring( math.floor(ta_dist) ), x_dist + mod_x, y_dist + mod_y, 86,col_text )
         else
            gfx.renderTex( question, target_image_x + target_image_w/2 - question_w/2 + mod_x, target_image_y + target_image_h/2 - question_h/2 + mod_y )
            renderBar( "shield", 0, false, true, "target", mod_x, mod_y )
            renderBar( "armour", 0, false, true, "target", mod_x, mod_y )
            renderBar( "energy", 0, false, true, "target", mod_x, mod_y )
            renderField( _("Unknown"), x_name + mod_x, y_name + mod_y, 86, col_unkn )
            renderField( tostring( math.floor(ta_dist) ), x_dist + mod_x, y_dist + mod_y, 86, col_text )
         end

         gfx.renderTex( target_frame, target_image_x + mod_x, target_image_y + mod_y )

         --Speed Lights
         local nlights = 7
         local value = round( ptarget:vel():mod() * nlights / ta_stats.speed_max )
         if value > nlights * 2 then value = nlights * 2 end
         for i=1, value do
            if i <= nlights then
               gfx.renderTex( speed_light, x_speed - 5 + mod_x, y_speed - 3 + (i-1)*6 + mod_y )
            else
               local imod = i % nlights
               gfx.renderTex( speed_light_double, x_speed - 5 + mod_x, y_speed - 3 + (imod-1)*6 + mod_y )
            end
         end
         if value < nlights then
            for i=value+1, nlights do
               gfx.renderTex( speed_light_off, x_speed + mod_x, y_speed + (i-1)*6 + mod_y )
            end
         end
      else
         gfx.renderTex( popup_empty, popup_left_ + mod_x, popup_left_y + mod_y )
      end
   else
      gfx.renderTex( popup_empty, popup_left_x + mod_x, popup_left_y + mod_y )
   end
   gfx.renderTex( popup_bottom, popup_left_x + mod_x, popup_left_y - 5 + mod_y )

   if nav_pnt ~= nil then
      renderField( nav_pnt:name(), fields_x + 4, fields_y, fields_w, col_text, icon_pnt_target )
   else
      renderField( _("None"), fields_x + 4, fields_y, fields_w, col_unkn, icon_pnt_target )
   end
   if autonav_hyp ~= nil then
      local name = autonav_hyp:name()
      if not autonav_hyp:known() and not autonav_hyp:marked() then
         name = _("Unknown")
      end
      renderField( name .. " (" .. tostring(autonav_jumps) .. ")", fields_x + fields_w + 12, fields_y, fields_w, col_text, icon_nav_target )
   else
      renderField( _("None"), fields_x + fields_w + 12, fields_y, fields_w, col_unkn, icon_nav_target )
   end
   renderField( credits_h, tbar_center_x + tbar_center_w/2 + 4, fields_y, fields_w, col_text, icon_money )
   renderField( tonnestring_short(cargo), tbar_center_x + tbar_center_w/2 + fields_w + 12, fields_y, fields_w, col_text, icon_cargo )

   --Center
   gfx.renderTex( top_bar_center, tbar_center_x - tbar_center_w/2, tbar_y + tbar_h - tbar_center_h )

   --Time
   local time_str = time.str(time.get())
   local time_str_w = gfx.printDim(false, time_str)
   gfx.print( false, time_str, screen_w/2 - 78, tbar_y + tbar_h - tbar_center_h + 55, col_text, 156, true )

   --System name
   local sysname = system.cur():name()
   local sysname_w = gfx.printDim(false, sysname)
   gfx.print( false, sysname, screen_w/2 - 67, tbar_y + tbar_h - tbar_center_h + 19, col_text, 132, true )

   for k, v in ipairs(buttontypes) do
      renderButton( v )
   end

   -- Planet pane
   if nav_pnt then
      ta_pnt_dist = pp:pos():dist( planet.pos )

      -- Extend the pane depending on the services available.
      services_h = 60
      if pntflags.land then
         services_h = services_h + (20 * planet.nservices)
      end

      -- Render background images.
      gfx.renderTex( planet_pane_t, ta_pnt_pane_x, ta_pnt_pane_y )
      local y
      for yy = ta_pnt_pane_y, ta_pnt_pane_y-services_h, -ta_pnt_pane_h_m do
         y = yy
         gfx.renderTex( planet_pane_m, ta_pnt_pane_x, y )
      end
      gfx.renderTex( planet_pane_b, ta_pnt_pane_x, y - ta_pnt_pane_h_b )
      gfx.renderTex( planet_bg, ta_pnt_image_x, ta_pnt_image_y )

      --Render planet image.
      if ta_pnt_gfx_w > 140 or ta_pnt_gfx_h > 140 then
         gfx.renderTexRaw( ta_pnt_gfx, ta_pnt_center_x - ta_pnt_gfx_draw_w / 2, ta_pnt_center_y - ta_pnt_gfx_draw_h / 2, ta_pnt_gfx_draw_w, ta_pnt_gfx_draw_h, 1, 1, 0, 0, 1, 1)
      else
         gfx.renderTex( ta_pnt_gfx, ta_pnt_center_x - ta_pnt_gfx_w / 2, ta_pnt_center_y - ta_pnt_gfx_h / 2)
      end
      gfx.print( true, _("TARGETED"), ta_pnt_pane_x + 14, ta_pnt_pane_y + 170, col_text )
      gfx.print( true, planet.name, ta_pnt_pane_x + 14, ta_pnt_pane_y + 150, planet.col )
      gfx.print( true, string.format(
            _("DISTANCE: %s"), largeNumber(ta_pnt_dist)),
         ta_pnt_pane_x + 14, ta_pnt_pane_y - 20, col_text )
      gfx.print( true, string.format( _("CLASS: %s"), planet.class ),
         ta_pnt_pane_x + 14, ta_pnt_pane_y - 40, col_text )

      if ta_pnt_faction_gfx then
         gfx.renderTex( ta_pnt_faction_gfx, ta_pnt_fact_x, ta_pnt_fact_y )
      end

      x1, y1 = vec2.get(planet.pos)
      x2, y2 = vec2.get(player.pos())
      ta_pnt_dir = math.atan2(y2 - y1, x2 - x1) + math.pi

      -- Space out the text.
      if pntflags.land then
         gfx.print( true, _("SERVICES:"), ta_pnt_pane_x + 14, ta_pnt_pane_y - 60, col_text )
         for k,v in ipairs(planet.services) do
            gfx.print(true, _(v), ta_pnt_pane_x + 40, ta_pnt_pane_y - 60 - k*20, col_text )
         end
      else
         gfx.print( true, _("SERVICES: none"), ta_pnt_pane_x + 14, ta_pnt_pane_y - 60, col_text )
      end
   end

   -- Fleet functions
   -- TODO: Add an API for and implement fleet command buttons
   if #pp:followers() ~= 0 then
      local base_x, y, panel_y, width, height
      width, height = field_frame_center:dim()
      base_x = nil
      y = tbar_y - height

      local my_buttons = {"formation", "orders"}
      local button_text = {
         formation = _("Set Formation"),
         orders = _("Issue Orders"),
      }
      local button_action = {
         formation = eh.playerform,
         orders = eh.issue_orders,
      }

      if ta_flt_pane_x ~= nil and ta_flt_pane_y ~= nil then
         base_x = ta_flt_pane_x + ta_flt_pane_w/2

         gfx.renderTex( fleet_pane_t, ta_flt_pane_x, ta_flt_pane_y )
         panel_y = ta_flt_pane_y
      end

      for i, v in ipairs( my_buttons ) do
         local text = button_text[v]
         width = gfx.printDim(false, text)

         if buttons[v] == nil then
            buttons[v] = {}
         end

         local button = buttons[v]
         if base_x ~= nil then
            button.x = math.max( 0, base_x - width/2 )
         else
            button.x = 16
         end
         button.y = y
         button.w = width
         button.h = height
         button.action = button_action[v]

         local col = col_text
         if button.state == "mouseover" then
             col = col_lgray
         end

         if base_x ~= nil then
            while y < panel_y do
               panel_y = panel_y - ta_flt_pane_h_m
               gfx.renderTex( fleet_pane_m, ta_flt_pane_x, panel_y )
            end
         end

         renderField( text, button.x, button.y, width, col )

         y = y - height - 2
      end

      if base_x ~= nil then
         gfx.renderTex( fleet_pane_b, ta_flt_pane_x, panel_y - ta_flt_pane_h_b )
      end
   end
end

function mouse_click( button, x, y, state )
   if button ~= 2 then
      return false
   else
      lmouse = state
      pressed = mouseInsideButton( x, y )

      if pressed == nil then
         if not state then
            for k, v in pairs(buttons) do
               if v.state ~= "disabled" and v.state ~= "hilighted" then
                  v.state = "default"
               end
            end
         end
         return false
      else
         if state then
            if pressed.state ~= "disabled" then
               pressed.state = "pressed"
            end
            return true
         else
            if pressed.state ~= "disabled" then
               pressed.state = "default"
               pressed.action()
            end
            return true
         end
      end
   end
end

function mouse_move( x, y )
   pressed = mouseInsideButton( x, y )
   if pressed ~= nil then
      if pressed.state ~= "disabled" and not lmouse then
         pressed.state = "mouseover"
      elseif pressed.state ~= "disabled" and lmouse then
         pressed.state = "pressed"
      end
   else
      for k, v in pairs(buttons) do
         if v.state ~= "disabled" and v.state ~= "hilighted" then
            v.state = "default"
         end
      end
   end
end

function mouseInsideButton( x, y )
   for k, v in pairs(buttons) do
      if x > v.x and x < v.x+v.w and y > v.y and y < v.y+v.h then
         return v
      end
   end
   return nil
end

function action_missions()
   gui.menuInfo( "missions" )
end

function action_cargo()
   gui.menuInfo( "cargo" )
end

function action_ship()
   gui.menuInfo( "ship" )
end

function action_weapons()
   gui.menuInfo( "weapons" )
end

function render_cooldown( percent, seconds )
   local msg = _("Cooling down...\n%.1f seconds remaining"):format( seconds )
   local fail = true
   if cooldown_omsg ~= nil then
      if player.omsgChange( cooldown_omsg, msg, seconds ) then
         fail = false
      end
   end
   if fail then
      cooldown_omsg = player.omsgAdd( msg, seconds )
   end
end

function end_cooldown()
   if cooldown_omsg ~= nil then
      player.omsgRm( cooldown_omsg )
      cooldown_omsg = nil
   end
end
