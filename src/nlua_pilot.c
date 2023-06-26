/*
 * See Licensing and Copyright notice in naev.h
 */

/**
 * @file nlua_pilot.c
 *
 * @brief Handles the Lua pilot bindings.
 *
 * These bindings control the planets and systems.
 */

/** @cond */
#include "naev.h"
/** @endcond */

#include "nlua_commodity.h"
#include "nlua_pilot.h"

#include "ai.h"
#include "array.h"
#include "camera.h"
#include "damagetype.h"
#include "escort.h"
#include "gui.h"
#include "land_outfits.h"
#include "log.h"
#include "nlua.h"
#include "nlua_col.h"
#include "nlua_faction.h"
#include "nlua_jump.h"
#include "nlua_outfit.h"
#include "nlua_planet.h"
#include "nlua_ship.h"
#include "nlua_system.h"
#include "nlua_vec2.h"
#include "nluadef.h"
#include "pilot.h"
#include "pilot_heat.h"
#include "player.h"
#include "rng.h"
#include "space.h"
#include "weapon.h"

/*
 * From ai.c
 */
extern Pilot *cur_pilot;


/*
 * Prototypes.
 */
static Task *pilotL_newtask( lua_State *L, Pilot* p, const char *task );
static int outfit_compareActive( const void *slot1, const void *slot2 );
static int pilotL_setFlagWrapper( lua_State *L, int flag );


/* Pilot metatable methods. */
static int pilotL_add(lua_State *L);
static int pilotL_remove(lua_State *L);
static int pilotL_clear(lua_State *L);
static int pilotL_toggleSpawn(lua_State *L);
static int pilotL_getPilots(lua_State *L);
static int pilotL_getHostiles(lua_State *L);
static int pilotL_getVisible(lua_State *L);
static int pilotL_eq(lua_State *L);
static int pilotL_name(lua_State *L);
static int pilotL_id(lua_State *L);
static int pilotL_exists(lua_State *L);
static int pilotL_target(lua_State *L);
static int pilotL_setTarget(lua_State *L);
static int pilotL_inrange(lua_State *L);
static int pilotL_nav(lua_State *L);
static int pilotL_activeWeapset(lua_State *L);
static int pilotL_weapset(lua_State *L);
static int pilotL_weapsetHeat(lua_State *L);
static int pilotL_actives(lua_State *L);
static int pilotL_outfits(lua_State *L);
static int pilotL_ammo(lua_State *L);
static int pilotL_outfitByID(lua_State *L);
static int pilotL_rename(lua_State *L);
static int pilotL_position(lua_State *L);
static int pilotL_velocity(lua_State *L);
static int pilotL_dir(lua_State *L);
static int pilotL_ew(lua_State *L);
static int pilotL_temp(lua_State *L);
static int pilotL_mass(lua_State *L);
static int pilotL_faction(lua_State *L);
static int pilotL_spaceworthy(lua_State *L);
static int pilotL_setPosition(lua_State *L);
static int pilotL_setVelocity(lua_State *L);
static int pilotL_setDir(lua_State *L);
static int pilotL_broadcast(lua_State *L);
static int pilotL_comm(lua_State *L);
static int pilotL_setFaction(lua_State *L);
static int pilotL_setHostile(lua_State *L);
static int pilotL_setFriendly(lua_State *L);
static int pilotL_setInvincible(lua_State *L);
static int pilotL_setInvincPlayer(lua_State *L);
static int pilotL_setHide(lua_State *L);
static int pilotL_setInvisible(lua_State *L);
static int pilotL_setNoRender(lua_State *L);
static int pilotL_setVisplayer(lua_State *L);
static int pilotL_setVisible(lua_State *L);
static int pilotL_setHilight(lua_State *L);
static int pilotL_getColour(lua_State *L);
static int pilotL_getHostile(lua_State *L);
static int pilotL_flags(lua_State *L);
static int pilotL_setActiveBoard(lua_State *L);
static int pilotL_setNoDeath(lua_State *L);
static int pilotL_disable(lua_State *L);
static int pilotL_cooldown(lua_State *L);
static int pilotL_setCooldown(lua_State *L);
static int pilotL_setNoJump(lua_State *L);
static int pilotL_setNoLand(lua_State *L);
static int pilotL_setNoClear(lua_State *L);
static int pilotL_outfitAdd(lua_State *L);
static int pilotL_outfitRm(lua_State *L);
static int pilotL_setFuel(lua_State *L);
static int pilotL_intrinsicReset(lua_State *L);
static int pilotL_intrinsicSet(lua_State *L);
static int pilotL_intrinsicGet(lua_State *L);
static int pilotL_changeAI(lua_State *L);
static int pilotL_setTemp(lua_State *L);
static int pilotL_setHealth(lua_State *L);
static int pilotL_setEnergy(lua_State *L);
static int pilotL_fillAmmo(lua_State *L);
static int pilotL_setNoBoard(lua_State *L);
static int pilotL_setNoDisable(lua_State *L);
static int pilotL_setSpeedLimit( lua_State *L);
static int pilotL_getHealth(lua_State *L);
static int pilotL_getEnergy(lua_State *L);
static int pilotL_getLockon(lua_State *L);
static int pilotL_getStats(lua_State *L);
static int pilotL_getShipStat(lua_State *L);
static int pilotL_cargoFree(lua_State *L);
static int pilotL_cargoHas(lua_State *L);
static int pilotL_cargoAdd(lua_State *L);
static int pilotL_cargoRm(lua_State *L);
static int pilotL_cargoList(lua_State *L);
static int pilotL_pay(lua_State *L);
static int pilotL_credits(lua_State *L);
static int pilotL_value(lua_State *L);
static int pilotL_ship(lua_State *L);
static int pilotL_idle(lua_State *L);
static int pilotL_control(lua_State *L);
static int pilotL_memory(lua_State *L);
static int pilotL_task(lua_State *L);
static int pilotL_taskname(lua_State *L);
static int pilotL_taskdata(lua_State *L);
static int pilotL_taskclear(lua_State *L);
static int pilotL_moveto(lua_State *L);
static int pilotL_face(lua_State *L);
static int pilotL_brake(lua_State *L);
static int pilotL_follow(lua_State *L);
static int pilotL_attack(lua_State *L);
static int pilotL_runaway(lua_State *L);
static int pilotL_gather(lua_State *L);
static int pilotL_localjump(lua_State *L);
static int pilotL_hyperspace(lua_State *L);
static int pilotL_land(lua_State *L);
static int pilotL_hailPlayer(lua_State *L);
static int pilotL_msg(lua_State *L);
static int pilotL_leader(lua_State *L);
static int pilotL_setLeader(lua_State *L);
static int pilotL_followers(lua_State *L);
static int pilotL_hookClear(lua_State *L);
static int pilotL_choosePoint(lua_State *L);
static const luaL_Reg pilotL_methods[] = {
   /* General. */
   {"add", pilotL_add},
   {"rm", pilotL_remove},
   {"get", pilotL_getPilots},
   {"getHostiles", pilotL_getHostiles},
   {"getVisible", pilotL_getVisible},
   {"__eq", pilotL_eq},
   /* Info. */
   {"name", pilotL_name},
   {"id", pilotL_id},
   {"exists", pilotL_exists},
   {"target", pilotL_target},
   {"setTarget", pilotL_setTarget},
   {"inrange", pilotL_inrange},
   {"nav", pilotL_nav},
   {"activeWeapset", pilotL_activeWeapset},
   {"weapset", pilotL_weapset},
   {"weapsetHeat", pilotL_weapsetHeat},
   {"actives", pilotL_actives},
   {"outfits", pilotL_outfits},
   {"ammo", pilotL_ammo},
   {"outfitByID", pilotL_outfitByID},
   {"rename", pilotL_rename},
   {"pos", pilotL_position},
   {"vel", pilotL_velocity},
   {"dir", pilotL_dir},
   {"ew", pilotL_ew},
   {"temp", pilotL_temp},
   {"mass", pilotL_mass},
   {"cooldown", pilotL_cooldown},
   {"faction", pilotL_faction},
   {"spaceworthy", pilotL_spaceworthy},
   {"health", pilotL_getHealth},
   {"energy", pilotL_getEnergy},
   {"lockon", pilotL_getLockon},
   {"stats", pilotL_getStats},
   {"shipstat", pilotL_getShipStat},
   {"colour", pilotL_getColour},
   {"hostile", pilotL_getHostile},
   {"flags", pilotL_flags},
   /* System. */
   { "clear", pilotL_clear },
   { "toggleSpawn", pilotL_toggleSpawn },
   /* Modify. */
   { "changeAI", pilotL_changeAI },
   { "setTemp", pilotL_setTemp },
   { "setHealth", pilotL_setHealth },
   { "setEnergy", pilotL_setEnergy },
   { "fillAmmo", pilotL_fillAmmo },
   { "setNoBoard", pilotL_setNoBoard },
   { "setNoDisable", pilotL_setNoDisable },
   { "setSpeedLimit", pilotL_setSpeedLimit },
   { "setPos", pilotL_setPosition },
   { "setVel", pilotL_setVelocity },
   { "setDir", pilotL_setDir },
   { "setFaction", pilotL_setFaction },
   { "setHostile", pilotL_setHostile },
   { "setFriendly", pilotL_setFriendly },
   { "setInvincible", pilotL_setInvincible },
   { "setInvincPlayer", pilotL_setInvincPlayer },
   { "setHide", pilotL_setHide },
   { "setInvisible", pilotL_setInvisible },
   { "setNoRender", pilotL_setNoRender },
   { "setVisplayer", pilotL_setVisplayer },
   { "setVisible", pilotL_setVisible },
   { "setHilight", pilotL_setHilight },
   { "setActiveBoard", pilotL_setActiveBoard },
   { "setNoDeath", pilotL_setNoDeath },
   { "disable", pilotL_disable },
   { "setCooldown", pilotL_setCooldown },
   { "setNoJump", pilotL_setNoJump },
   { "setNoLand", pilotL_setNoLand },
   { "setNoClear", pilotL_setNoClear },
   /* Talk. */
   { "broadcast", pilotL_broadcast },
   { "comm", pilotL_comm },
   /* Outfits. */
   { "outfitAdd", pilotL_outfitAdd },
   { "outfitRm", pilotL_outfitRm },
   { "setFuel", pilotL_setFuel },
   { "intrinsicReset", pilotL_intrinsicReset },
   { "intrinsicSet", pilotL_intrinsicSet },
   { "intrinsicGet", pilotL_intrinsicGet },
   /* Ship. */
   {"ship", pilotL_ship},
   {"cargoFree", pilotL_cargoFree},
   {"cargoHas", pilotL_cargoHas},
   {"cargoAdd", pilotL_cargoAdd},
   {"cargoRm", pilotL_cargoRm},
   {"cargoList", pilotL_cargoList},
   {"pay", pilotL_pay},
   {"credits", pilotL_credits},
   {"value", pilotL_value},
   /* Manual AI control. */
   {"idle", pilotL_idle},
   {"control", pilotL_control},
   {"memory", pilotL_memory},
   {"task", pilotL_task},
   {"taskname", pilotL_taskname},
   {"taskdata", pilotL_taskdata},
   {"taskClear", pilotL_taskclear},
   {"moveto", pilotL_moveto},
   {"face", pilotL_face},
   {"brake", pilotL_brake},
   {"follow", pilotL_follow},
   {"attack", pilotL_attack},
   {"runaway", pilotL_runaway},
   {"gather", pilotL_gather},
   {"localjump", pilotL_localjump},
   {"hyperspace", pilotL_hyperspace},
   {"land", pilotL_land},
   /* Misc. */
   { "hailPlayer", pilotL_hailPlayer },
   { "msg", pilotL_msg },
   { "leader", pilotL_leader },
   { "setLeader", pilotL_setLeader },
   { "followers", pilotL_followers },
   { "hookClear", pilotL_hookClear },
   { "choosePoint", pilotL_choosePoint },
   {0,0}
}; /**< Pilot metatable methods. */



/**
 * @brief Loads the pilot library.
 *
 *    @param env Environment to load library into.
 *    @return 0 on success.
 */
int nlua_loadPilot( nlua_env env )
{
   nlua_register(env, PILOT_METATABLE, pilotL_methods, 1);

   /* Pilot always loads ship. */
   nlua_loadShip(env);

   return 0;
}


/**
 * @brief Wrapper to simplify flag setting stuff.
 */
static int pilotL_setFlagWrapper( lua_State *L, int flag )
{
   Pilot *p;
   int state;

   NLUA_CHECKRW(L);

   /* Get the pilot. */
   p = luaL_validpilot(L,1);

   /* Get state. */
   if (lua_isnoneornil(L,2))
      state = 1;
   else
      state = lua_toboolean(L, 2);

   /* Set or remove the flag. */
   if (state)
      pilot_setFlag( p, flag );
   else
      pilot_rmFlag( p, flag );

   return 0;
}


/**
 * @brief Lua bindings to interact with pilots.
 *
 * This will allow you to create and manipulate pilots in-game.
 *
 * An example would be:
 * @code
 * p = pilot.add( "Llama", "Miner" ) -- Create a Miner Llama
 * p:setFriendly() -- Make it friendly
 * @endcode
 *
 * @luamod pilot
 */
/**
 * @brief Gets pilot at index.
 *
 *    @param L Lua state to get pilot from.
 *    @param ind Index position to find the pilot.
 *    @return Pilot found at the index in the state.
 */
LuaPilot lua_topilot( lua_State *L, int ind )
{
   return *((LuaPilot*) lua_touserdata(L,ind));
}
/**
 * @brief Gets pilot at index or raises error if there is no pilot at index.
 *
 *    @param L Lua state to get pilot from.
 *    @param ind Index position to find pilot.
 *    @return Pilot found at the index in the state.
 */
LuaPilot luaL_checkpilot( lua_State *L, int ind )
{
   if (lua_ispilot(L,ind))
      return lua_topilot(L,ind);
   luaL_typerror(L, ind, PILOT_METATABLE);
   return 0;
}
/**
 * @brief Makes sure the pilot is valid or raises a Lua error.
 *
 *    @param L State currently running.
 *    @param ind Index of the pilot to validate.
 *    @return The pilot (doesn't return if fails - raises Lua error ).
 */
Pilot* luaL_validpilot( lua_State *L, int ind )
{
   Pilot *p;

   /* Get the pilot. */
   p  = pilot_get(luaL_checkpilot(L,ind));
   if (p==NULL) {
      NLUA_ERROR(L,_("Pilot is invalid."));
      return NULL;
   }

   return p;
}
/**
 * @brief Pushes a pilot on the stack.
 *
 *    @param L Lua state to push pilot into.
 *    @param pilot Pilot to push.
 *    @return Newly pushed pilot.
 */
LuaPilot* lua_pushpilot( lua_State *L, LuaPilot pilot )
{
   LuaPilot *p;
   p = (LuaPilot*) lua_newuserdata(L, sizeof(LuaPilot));
   *p = pilot;
   luaL_getmetatable(L, PILOT_METATABLE);
   lua_setmetatable(L, -2);
   return p;
}
/**
 * @brief Checks to see if ind is a pilot.
 *
 *    @param L Lua state to check.
 *    @param ind Index position to check.
 *    @return 1 if ind is a pilot.
 */
int lua_ispilot( lua_State *L, int ind )
{
   int ret;

   if (lua_getmetatable(L,ind)==0)
      return 0;
   lua_getfield(L, LUA_REGISTRYINDEX, PILOT_METATABLE);

   ret = 0;
   if (lua_rawequal(L, -1, -2))  /* does it have the correct mt? */
      ret = 1;

   lua_pop(L, 2);  /* remove both metatables */
   return ret;
}


/**
 * @brief Returns a suitable jumpin spot for a given pilot.
 * @usage point = pilot.choosePoint( f, i, g )
 *
 *    @luatparam Faction f Faction the pilot will belong to.
 *    @luatparam boolean i Wether to ignore rules.
 *    @luatparam boolean g Wether to behave as guerilla (spawn in deep space)
 *    @luatreturn Planet|Vec2|Jump A randomly chosen suitable spawn point.
 * @luafunc choosePoint
 */
static int pilotL_choosePoint(lua_State *L)
{
   LuaFaction lf;
   int ignore_rules, guerilla;
   Planet *planet = NULL;
   JumpPoint *jump = NULL;
   Vector2d vp;

   lf = faction_get( luaL_checkstring(L,1) );

   ignore_rules = 0;
   if (lua_isboolean(L,2) && lua_toboolean(L,2))
      ignore_rules = 1;

   guerilla = 0;
   if (lua_isboolean(L,3) && lua_toboolean(L,3))
      guerilla = 1;

   pilot_choosePoint( &vp, &planet, &jump, lf, ignore_rules, guerilla );

   if (planet != NULL)
      lua_pushplanet(L, planet->id );
   else if (jump != NULL)
      lua_pushsystem(L, jump->from->id);
   else
      lua_pushvector(L, vp);

   return 1;
}


/**
 * @brief Adds a ship with an AI and faction to the system.
 *
 * How the "source" argument works (by type of value passed):<br/>
 * <ul>
 *    <li>nil: spawns pilot randomly entering from jump points with
 *       presence of their faction or taking off from non-hostile
 *       planets</li>
 *    <li>planet: pilot takes off from the planet</li>
 *    <li>system: jumps pilot in from the system</li>
 *    <li>vec2: pilot is created at the position (no jump/takeoff)</li>
 *    <li>true: Acts like nil, but does not avoid jump points with no
 *       presence</li>
 * </ul>
 *
 * Arguments that can be passed to the "parameters" parameter:<br/>
 * <ul>
 *    <li>"ai" (string): AI to give the pilot. Defaults to the faction's
 *       AI.</li>
 *    <li>"naked" (boolean): Whether or not to have the pilot spawn
 *       without outfits. Defaults to false.</li>
 *    <li>"noequip" (boolean): Whether or not to skip the equip script
 *       (and use the ship's default outfits). Defaults to false.</li>
 * </ul>
 * <br/>
 *
 * @usage p = pilot.add("Empire Shark", nil, "Empire") -- Creates a standard Empire Shark.
 * @usage p = pilot.add("Hyena", "Pirate", _("Pirate Hyena")) -- Just adds the pilot (will jump in or take off).
 * @usage p = pilot.add("Llama", "Trader", nil, _("Trader Llama"), {ai="dummy"}) -- Overrides AI with dummy ai.
 * @usage p = pilot.add("Gawain", "Civilian", vec2.new( 1000, 200 )) -- Pilot won't jump in, will just appear.
 * @usage p = pilot.add("Empire Pacifier", "Empire", system.get("Goddard")) -- Have the pilot jump in from the system.
 * @usage p = pilot.add("Goddard", "Goddard", planet.get("Zhiru") , _("Goddard Goddard")) -- Have the pilot take off from a planet.
 *
 *    @luatparam string shipname Raw (untranslated) name of the ship to
 *       add.
 *    @luatparam Faction faction Faction to give the pilot.
 *    @luatparam[opt] System|Planet|vec2|boolean source Where to create
 *       the pilot; see above for a complete explanation.
 *    @luatparam[opt] string pilotname Translated name to give the
 *       pilot. Defaults to the translated version of shipname.
 *    @luatparam[opt] table parameters Table of extra keyword arguments.
 *       See above for supported arguments.
 *    @luatreturn Pilot The created pilot.
 * @luafunc add
 */
static int pilotL_add(lua_State *L)
{
   Ship *ship;
   const char *name, *ai;
   int i, j;
   LuaPilot p;
   double a, r;
   Vector2d vv, vp, vn;
   LuaFaction lf;
   StarSystem *ss;
   Planet *planet;
   JumpPoint *jump;
   PilotFlags flags;
   int ignore_rules;
   Pilot *plt;

   NLUA_CHECKRW(L);

   /* Default values. */
   pilot_clearFlagsRaw(flags);
   vectnull(&vn); /* Need to determine angle. */
   jump = NULL;
   planet = NULL;
   a = 0.;

   /* Parse first argument - Fleet Name */
   name = luaL_checkstring(L, 1);

   /* pull the fleet */
   ship = NULL;
   ship = ship_get(name);
   if (ship == NULL) {
      NLUA_ERROR(L, _("Ship '%s' not found!"), name);
      return 0;
   }
   /* Get pilotname argument if provided. */
   name = luaL_optstring(L, 4, name);
   /* Get faction from string or number. */
   lf = luaL_validfaction(L, 2);

   /* Handle position/origin argument. */
   if (lua_isvector(L, 3)) {
      vp = *lua_tovector(L, 3);
      a = RNGF() * 2. * M_PI;
      vectnull(&vv);
   }
   else if (lua_issystem(L, 3)) {
      ss = system_getIndex(lua_tosystem(L, 3));
      for (i=0; i<array_size(cur_system->jumps); i++) {
         if ((cur_system->jumps[i].target == ss)
               && !jp_isFlag(cur_system->jumps[i].returnJump, JP_EXITONLY)) {
            jump = cur_system->jumps[i].returnJump;
            break;
         }
      }
      if (jump == NULL) {
         if (array_size(cur_system->jumps) > 0) {
            WARN(_("Pilot '%s' jumping in from non-adjacent system '%s' to"
                     " '%s'."),
                  name, ss->name, cur_system->name);
            j = RNG_BASE(0, array_size(cur_system->jumps) - 1);
            jump = cur_system->jumps[j].returnJump;
         }
         else
            WARN(_("Pilot '%s' attempting to jump in from '%s', but '%s' has"
                     " no jump points."),
                  name, ss->name, cur_system->name);
      }
   }
   else if (lua_isplanet(L, 3)) {
      planet = luaL_validplanet(L, 3);
      pilot_setFlagRaw(flags, PILOT_TAKEOFF);
      a = RNGF() * 2. * M_PI;
      r = RNGF() * planet->radius;
      vect_cset(&vp, planet->pos.x + r*cos(a), planet->pos.y + r*sin(a));
      a = RNGF() * 2. * M_PI;
      vectnull(&vv);
   }
   /* Random. */
   else {
      /* Check if we should ignore the strict rules. */
      ignore_rules = 0;
      if (lua_isboolean(L, 3) && lua_toboolean(L, 3))
         ignore_rules = 1;

      /* Choose the spawn point and act in consequence.*/
      pilot_choosePoint(&vp, &planet, &jump, lf, ignore_rules, 0);

      if (planet != NULL) {
         pilot_setFlagRaw(flags, PILOT_TAKEOFF);
         a = RNGF() * 2. * M_PI;
         r = RNGF() * planet->radius;
         vect_cset(&vp, planet->pos.x + r*cos(a), planet->pos.y + r*sin(a));
         a = RNGF() * 2. * M_PI;
         vectnull( &vv );
      }
      else {
         a = RNGF() * 2. * M_PI;
         vectnull(&vv);
      }
   }

   /* Parse final argument - table of optional parameters */
   ai = NULL;
   if (!lua_isnoneornil(L, 5)) {
      if (!lua_istable(L, 5)) {
         NLUA_ERROR(L, _("'parameters' should be a table of options or omitted!"));
         return 0;
      }
      lua_getfield(L, 5, "ai");
      ai = luaL_optstring(L, -1, NULL);
      lua_pop(L, 1);

      lua_getfield(L, 5, "naked");
      if (lua_toboolean(L, -1))
         pilot_setFlagRaw(flags, PILOT_NO_OUTFITS);
      lua_pop(L, 1);

      lua_getfield(L, 5, "noequip");
      if (lua_toboolean(L, -1))
         pilot_setFlagRaw(flags, PILOT_NO_EQUIP);
      lua_pop(L, 1);
   }

   /* Set up velocities and such. */
   if (jump != NULL) {
      space_calcJumpInPos(cur_system, jump->from, &vp, &vv, &a);
      pilot_setFlagRaw(flags, PILOT_HYP_END);
   }

   /* Make sure angle is valid. */
   a = fmod(a, 2. * M_PI);
   if (a < 0.)
      a += 2. * M_PI;

   /* Create the pilot. */
   p = pilot_create(ship, name, lf, ai, a, &vp, &vv, flags, 0, 0);
   lua_pushpilot(L, p);
   plt = pilot_get(p);

   /* Set the memory stuff. */
   if (jump != NULL) {
      LuaJump lj;
      lj.srcid = jump->from->id;
      lj.destid = cur_system->id;

      nlua_getenv(plt->ai->env, AI_MEM);
      lua_pushjump(L, lj);
      lua_setfield(L, -2, "create_jump");
      lua_pop(L,1);
   }
   else if (planet != NULL) {
      nlua_getenv(plt->ai->env, AI_MEM);
      lua_pushplanet(L, planet->id);
      lua_setfield(L, -2, "create_planet");
      lua_pop(L, 1);
   }
   return 1;
}


/**
 * @brief Removes a pilot without explosions or anything.
 *
 * @usage p:rm() -- pilot will be destroyed
 *
 *    @luatparam Pilot p Pilot to remove.
 * @luafunc rm
 */
static int pilotL_remove(lua_State *L)
{
   Pilot *p;

   NLUA_CHECKRW(L);

   /* Get the pilot. */
   p = luaL_validpilot(L,1);

   /* Make sure it's not the player. */
   if (player.p == p) {
      NLUA_ERROR( L, _("Trying to remove the bloody player!") );
      return 0;
   }

   /* Deletes the pilot. */
   pilot_delete(p);

   return 0;
}
/**
 * @brief Clears the current system of pilots. Used for epic battles and such.
 *
 * Pilots which have been set to not clear with pilot.setNoClear(), as
 * well as the player's fighter bay escorts, are exempt and will remain
 * in the system.
 *
 * Be careful with this function. It will most likely cause issues if
 * multiple missions are in the same system. For this reason, it should
 * only be used in a successfully claimed system.
 *
 * @note Clears all global pilot hooks too.
 *
 * @usage pilot.clear()
 *
 * @luasee setNoClear
 * @luafunc clear
 */
static int pilotL_clear(lua_State *L)
{
   (void) L;
   NLUA_CHECKRW(L);
   pilots_clear();
   weapon_clear();
   return 0;
}
/**
 * @brief Disables or enables pilot spawning in the current system.
 *
 * If player jumps the spawn is enabled again automatically. Global spawning takes priority over faction spawning.
 *
 * @usage pilot.toggleSpawn() -- Defaults to flipping the global spawning (true->false and false->true)
 * @usage pilot.toggleSpawn( false ) -- Disables global spawning
 * @usage pliot.toggleSpawn( "Pirates" ) -- Defaults to disabling pirate spawning
 * @usage pilot.toggleSpawn( "Pirates", true ) -- Turns on pirate spawning
 *
 *    @luatparam[opt] Faction fid Faction to enable or disable spawning off. If ommited it works on global spawning.
 *    @luatparam[opt] boolean enable true enables spawn, false disables it.
 *    @luatreturn boolean The current spawn state.
 * @luafunc toggleSpawn
 */
static int pilotL_toggleSpawn(lua_State *L)
{
   int i, b;
   LuaFaction f;

   NLUA_CHECKRW(L);

   /* Setting it directly. */
   if (lua_gettop(L) > 0) {
      if (lua_isfaction(L,1) || lua_isstring(L,1)) {

         f = luaL_validfaction(L,1);
         b = !lua_toboolean(L,2);

         /* Find the faction and set. */
         for (i=0; i<array_size(cur_system->presence); i++) {
            if (cur_system->presence[i].faction != f)
               continue;
            cur_system->presence[i].disabled = b;
            break;
         }

      }
      else if (lua_isboolean(L,1))
         space_spawn = lua_toboolean(L,1);
      else
         NLUA_INVALID_PARAMETER(L);
   }
   /* Toggling. */
   else
      space_spawn = !space_spawn;

   lua_pushboolean(L, space_spawn);
   return 1;
}
/**
 * @brief Gets the pilots available in the system by a certain criteria.
 *
 * @usage p = pilot.get() -- Gets all the pilots
 * @usage p = pilot.get( { faction.get("Empire") } ) -- Only gets empire pilots.
 * @usage p = pilot.get( nil, true ) -- Gets all pilots including disabled
 * @usage p = pilot.get( { faction.get("Empire") }, true ) -- Only empire pilots with disabled
 *
 *    @luatparam Faction|{Faction,...} factions If f is a table of factions, it will only get pilots matching those factions.  Otherwise it gets all the pilots.
 *    @luatparam boolean disabled Whether or not to get disabled ships (default is off if parameter is omitted).
 *    @luatreturn {Pilot,...} A table containing the pilots.
 * @luafunc get
 */
static int pilotL_getPilots(lua_State *L)
{
   int i, j, k, d;
   LuaFaction *factions;
   Pilot * const *pilot_stack;

   /* Whether or not to get disabled. */
   d = lua_toboolean(L,2);

   pilot_stack = pilot_getAll();

   /* Check for belonging to faction. */
   if (lua_istable(L,1) || lua_isfaction(L,1)) {
      if (lua_isfaction(L,1)) {
         factions = array_create(LuaFaction);
         array_push_back( &factions, lua_tofaction(L,1) );
      }
      else {
         /* Get table length and preallocate. */
         factions = array_create_size(LuaFaction, lua_objlen(L, 1));
         /* Load up the table. */
         lua_pushnil(L);
         while (lua_next(L, -2) != 0) {
            if (lua_isfaction(L,-1))
               array_push_back( &factions, lua_tofaction(L, -1) );
            lua_pop(L,1);
         }
      }

      /* Now put all the matching pilots in a table. */
      lua_newtable(L);
      k = 1;
      for (i=0; i<array_size(pilot_stack); i++) {
         for (j=0; j<array_size(factions); j++) {
            if ((pilot_stack[i]->faction == factions[j]) &&
                  (d || !pilot_isDisabled(pilot_stack[i])) &&
                  !pilot_isFlag(pilot_stack[i], PILOT_DELETE)) {
               lua_pushnumber(L, k++); /* key */
               lua_pushpilot(L, pilot_stack[i]->id); /* value */
               lua_rawset(L,-3); /* table[key] = value */
            }
         }
      }

      /* clean up. */
      array_free( factions );
   }
   else if ((lua_isnil(L,1)) || (lua_gettop(L) == 0)) {
      /* Now put all the matching pilots in a table. */
      lua_newtable(L);
      k = 1;
      for (i=0; i<array_size(pilot_stack); i++) {
         if ((d || !pilot_isDisabled(pilot_stack[i])) &&
               !pilot_isFlag(pilot_stack[i], PILOT_DELETE)) {
            lua_pushnumber(L, k++); /* key */
            lua_pushpilot(L, pilot_stack[i]->id); /* value */
            lua_rawset(L,-3); /* table[key] = value */
         }
      }
   }
   else {
      NLUA_INVALID_PARAMETER(L);
   }

   return 1;
}


/**
 * @brief Gets hostile pilots to a pilot within a certain distance.
 *
 *    @luatparam Pilot pilot Pilot to get hostiles of.
 *    @luatparam[opt=infinity] number dist Distance to look for hostiles.
 *    @luatparam[opt=false] boolean disabled Whether or not to count disabled pilots.
 *    @luatreturn {Pilot,...} A table containing the pilots.
 * @luafunc getHostiles
 */
static int pilotL_getHostiles(lua_State *L)
{
   int i, k, dd;
   Pilot *p = luaL_validpilot(L,1);
   double dist = luaL_optnumber(L,2,-1.);
   int dis = lua_toboolean(L,3);
   Pilot * const *pilot_stack;

   if (dist >= 0.)
      dd = dist*dist;

   /* Now put all the matching pilots in a table. */
   pilot_stack = pilot_getAll();
   lua_newtable(L);
   k = 1;
   for (i=0; i<array_size(pilot_stack); i++) {
      /* Must be hostile. */
      if ( !( areEnemies( pilot_stack[i]->faction, p->faction )
               || ( (p->id == PLAYER_ID)
                  && pilot_isHostile(pilot_stack[i]) ) ) )
         continue;
      /* Check if disabled. */
      if (dis && pilot_isDisabled(pilot_stack[i]))
         continue;
      /* Check distance if necessary. */
      if (dist >= 0. &&
            vect_dist2(&pilot_stack[i]->solid->pos, &p->solid->pos) > dd)
         continue;

      lua_pushnumber(L, k++); /* key */
      lua_pushpilot(L, pilot_stack[i]->id); /* value */
      lua_rawset(L,-3); /* table[key] = value */
   }

   return 1;
}


/**
 * @brief Gets a table of pilots visible to a pilot.
 *
 *    @luatparam Pilot pilot Pilot to get visible pilots of.
 *    @luatparam[opt=false] boolean disabled Whether or not to count
 *       disabled pilots.
 *    @luatreturn {Pilot,...} A table containing the pilots.
 * @luafunc getVisible
 */
static int pilotL_getVisible(lua_State *L)
{
   int i, k;
   Pilot *p = luaL_validpilot(L,1);
   int dis = lua_toboolean(L,2);
   Pilot * const *pilot_stack;

   /* Now put all the matching pilots in a table. */
   pilot_stack = pilot_getAll();
   lua_newtable(L);
   k = 1;
   for (i=0; i<array_size(pilot_stack); i++) {
      /* Check if dead. */
      if (pilot_isFlag(pilot_stack[i], PILOT_DELETE))
         continue;
      /* Check if disabled. */
      if (dis && pilot_isDisabled(pilot_stack[i]))
         continue;
      /* Check visibilitiy. */
      if (!pilot_validTarget( p, pilot_stack[i] ))
         continue;

      lua_pushnumber(L, k++); /* key */
      lua_pushpilot(L, pilot_stack[i]->id); /* value */
      lua_rawset(L,-3); /* table[key] = value */
   }

   return 1;
}


/**
 * @brief Checks to see if pilot and p are the same.
 *
 * @usage if p == p2 then -- Pilot 'p' and 'p2' match.
 *
 *    @luatparam Pilot p Pilot to compare.
 *    @luatparam Pilot comp Pilot to compare against.
 *    @luatreturn boolean true if they are the same.
 * @luafunc __eq
 */
static int pilotL_eq(lua_State *L)
{
   LuaPilot p1 = luaL_checkpilot(L,1);
   LuaPilot p2 = luaL_checkpilot(L,2);
   lua_pushboolean(L, p1 == p2);
   return 1;
}

/**
 * @brief Gets the pilot's current (translated) name.
 *
 * @usage name = p:name()
 *
 *    @luatparam Pilot p Pilot to get the name of.
 *    @luatreturn string The current name of the pilot.
 * @luafunc name
 */
static int pilotL_name(lua_State *L)
{
   Pilot *p;

   /* Parse parameters. */
   p = luaL_validpilot(L,1);

   /* Get name. */
   lua_pushstring(L, p->name);
   return 1;
}

/**
 * @brief Gets the ID of the pilot.
 *
 * @usage id = p:id()
 *
 *    @luaparam p Pilot Pilot to get the ID of.
 *    @luareturn number The ID of the current pilot.
 * @luafunc id
 */
static int pilotL_id(lua_State *L)
{
   Pilot *p;

   /* Parse parameters. */
   p = luaL_validpilot(L,1);

   /* Get name. */
   lua_pushnumber(L, p->id);
   return 1;
}

/**
 * @brief Checks to see if pilot is still in the system and alive.
 *
 * Pilots cease to exist if they die or jump out.
 *
 * @usage if p:exists() then -- Pilot still exists
 *
 *    @luatparam Pilot p Pilot to check to see if is still exists.
 *    @luatreturn boolean true if pilot is still exists.
 * @luafunc exists
 */
static int pilotL_exists(lua_State *L)
{
   Pilot *p;
   int exists;

   /* Parse parameters. */
   p  = pilot_get( luaL_checkpilot(L,1) );

   /* Must still be kicking and alive. */
   if (p==NULL)
      exists = 0;
   else if (pilot_isFlag( p, PILOT_DEAD ) || pilot_isFlag( p, PILOT_HIDE ))
      exists = 0;
   else
      exists = 1;

   /* Check if the pilot exists. */
   lua_pushboolean(L, exists);
   return 1;
}


/**
 * @brief Gets the pilot target of the pilot.
 *
 * @usage target = p:target()
 *
 *    @luatparam Pilot p Pilot to get target of.
 *    @luatreturn Pilot|nil nil if no target is selected, otherwise the target of the pilot.
 * @luafunc target
 */
static int pilotL_target(lua_State *L)
{
   Pilot *p;
   p = luaL_validpilot(L,1);
   if (p->target == 0)
      return 0;
   /* Must be targeted. */
   if (p->target == p->id)
      return 0;
   /* Must be valid. */
   if (pilot_get(p->target) == NULL)
      return 0;
   /* Push target. */
   lua_pushpilot(L, p->target);
   return 1;
}


/**
 * @brief Sets the pilot target of the pilot.
 *
 *    @luatparam Pilot p Pilot to get target of.
 *    @luatparam Pilot|nil t Pilot to set the target to or nil to set no target.
 * @luafunc setTarget
 */
static int pilotL_setTarget(lua_State *L)
{
   Pilot *p;
   pilotId_t t;
   p = luaL_validpilot(L,1);
   if (lua_isnoneornil(L,2))
      t = p->id;
   else
      t = luaL_validpilot(L,2)->id;
   p->target = t;
   return 0;
}


/**
 * @brief Checks to see if pilot is in range of pilot.
 *
 * @usage detected, scanned = p:inrange( target )
 *
 *    @luatparam Pilot p Pilot to see if another pilot is in range.
 *    @luatparam Pilot target Target pilot.
 *    @luatreturn boolean True if the pilot is visible at all.
 *    @luatreturn boolean True if the pilot is visible and well-defined (not fuzzy)
 * @luafunc inrange
 */
static int pilotL_inrange(lua_State *L)
{
   Pilot *p, *t;
   int ret;

   /* Parse parameters. */
   p = luaL_validpilot(L,1);
   t = luaL_validpilot(L,2);

   /* Check if in range. */
   ret = pilot_inRangePilot( p, t, NULL );
   if (ret == 1) { /* In range. */
      lua_pushboolean(L,1);
      lua_pushboolean(L,1);
   }
   else if (ret == 0) { /* Not in range. */
      lua_pushboolean(L,0);
      lua_pushboolean(L,0);
   }
   else { /* Detected fuzzy. */
      lua_pushboolean(L,1);
      lua_pushboolean(L,0);
   }
   return 2;
}


/**
 * @brief Gets the nav target of the pilot.
 *
 * This will only terminate when the target following pilot disappears (land, death, jump, etc...).
 *
 * @usage planet, hyperspace = p:nav()
 *
 *    @luatparam Pilot p Pilot to get nav info of.
 *    @luatreturn Planet|nil The pilot's planet target.
 *    @luatreturn System|nil The pilot's hyperspace target.
 * @luafunc nav
 */
static int pilotL_nav(lua_State *L)
{
   LuaSystem ls;
   Pilot *p;

   /* Get pilot. */
   p = luaL_validpilot(L,1);
   if (p->target == 0)
      return 0;

   /* Get planet target. */
   if (p->nav_planet < 0)
      lua_pushnil(L);
   else
      lua_pushplanet( L, cur_system->planets[ p->nav_planet ]->id );

   /* Get hyperspace target. */
   if (p->nav_hyperspace < 0)
      lua_pushnil(L);
   else {
      ls = cur_system->jumps[ p->nav_hyperspace ].targetid;
      lua_pushsystem( L, ls );
   }

   return 2;
}


/**
 * @brief Gets the ID (number from 1 to 10) of the current active weapset.
 *
 * @usage set_id = p:activeWeapset() -- A number from 1 to 10
 *
 *    @luatparam Pilot p Pilot to get active weapset ID of.
 *    @luatparam number current active weapset ID.
 *
 * @luafunc activeWeapset
 */
static int pilotL_activeWeapset(lua_State *L)
{
   Pilot *p = luaL_validpilot(L,1);
   lua_pushnumber( L, p->active_set + 1 );
   return 1;
}


/**
 * @brief Gets the weapset weapon of the pilot.
 *
 * The weapon sets have the following structure: <br />
 * <ul>
 *    <li>name: name of the set. </li>
 *    <li>cooldown: [0:1] value indicating if ready to shoot (1 is
 *       ready).</li>
 *    <li>charge: [0:1] charge level of beam weapon (1 is full).</li>
 *    <li>ammo: Name of the ammo or nil if not applicable.</li>
 *    <li>left: Absolute ammo left or nil if not applicable.</li>
 *    <li>left_p: Relative ammo left [0:1] or nil if not applicable</li>
 *    <li>lockon: Lock-on [0:1] for seeker weapons or nil if not
 *       applicable.</li>
 *    <li>in_arc: Whether or not the target is in targeting arc or nilif
 *       not applicable.</li>
 *    <li>level: Level of the weapon (1 is primary, 2 is secondary, 0
 *       is neither primary nor secondary)).</li>
 *    <li>instant: The instant mode weapon set the weapon is in if
 *       applicable, or nil if the weapon is not in an instant mode
 *       weapon set.</li>
 *    <li>temp: Temperature of the weapon.</li>
 *    <li>type: Type of the weapon.</li>
 *    <li>dtype: Damage type of the weapon.</li>
 *    <li>track: Tracking level of the weapon.</li>
 * </ul>
 *
 * An example would be:
 * @code
 * ws_name, ws = p:weapset( true )
 * print( "Weapnset Name: " .. ws_name )
 * for i, w in ipairs(ws) do
 *    print( "Name: " .. w.name )
 *    print( "Cooldown: " .. tostring(cooldown) )
 *    print( "Level: " .. tostring(level) )
 * end
 * @endcode
 *
 * @usage set_name, slots = p:weapset(true) -- Gets info for all active weapons
 * @usage set_name, slots = p:weapset() -- Get info about the current set
 * @usage set_name, slots = p:weapset(5) -- Get info about the set number 5
 *
 *    @luatparam Pilot p Pilot to get weapset weapon of.
 *    @luatparam[opt] number|boolean id ID of the set to get information
 *       of. Set to true to get all active weapons. Defaults to the
 *       current weapon set.
 *    @luatreturn string The name of the set.
 *    @luatreturn table A table with each slot's information.
 * @luafunc weapset
 */
static int pilotL_weapset(lua_State *L)
{
   Pilot *p, *target;
   int i, j, k, n, ii, ij;
   PilotWeaponSetOutfit *po_list;
   PilotWeaponSetOutfit *temp_po_list;
   PilotOutfitSlot *slot;
   const Outfit *ammo, *o;
   double delay, firemod, enermod, t;
   int id, all, level, level_match;
   int is_lau, is_fb;
   const Damage *dmg;
   int has_beamid;
   int instant;

   /* Parse parameters. */
   all = 0;
   p = luaL_validpilot(L,1);
   if (lua_gettop(L) > 1) {
      if (lua_isnumber(L,2))
         id = luaL_checkinteger(L,2) - 1;
      else if (lua_isboolean(L,2)) {
         all = lua_toboolean(L,2);
         id  = p->active_set;
      }
      else
         NLUA_INVALID_PARAMETER(L);
   }
   else
      id = p->active_set;
   id = CLAMP( 0, PILOT_WEAPON_SETS, id );

   /* Get target. */
   if (p->target != p->id)
      target = pilot_get( p->target );
   else
      target = NULL;

   /* Push name. */
   lua_pushstring( L, pilot_weapSetName( p, id ) );

   /* Push set. */
   po_list = all ? NULL : pilot_weapSetList( p, id );
   n = all ? array_size(p->outfits) : array_size(po_list);

   k = 0;
   lua_newtable(L);
   for (j=0; j<=PILOT_WEAPSET_MAX_LEVELS; j++) {
      /* Level to match. */
      level_match = (j==PILOT_WEAPSET_MAX_LEVELS) ? -1 : j;

      /* Iterate over weapons. */
      for (i=0; i<n; i++) {
         /* Get base look ups. */
         slot = all ?  p->outfits[i] : po_list[i].slot;
         o = slot->outfit;
         if (o == NULL)
            continue;
         is_lau = outfit_isLauncher(o);
         is_fb = outfit_isFighterBay(o);

         /* Must be valid weapon. */
         if (all && !outfit_isBolt(o) && !outfit_isBeam(o)
               && !is_lau && !is_fb)
            continue;

         level = slot->level;

         /* Must match level. */
         if (level != level_match)
            continue;

         /* Must be weapon. */
         if (outfit_isMod(o) || outfit_isAfterburner(o))
            continue;

         instant = -1;
         for (ii=0; ii<PLAYER_WEAPON_SETS; ii++) {
            if (pilot_weapSetTypeCheck(p, ii) != WEAPSET_TYPE_WEAPON)
               continue;

            temp_po_list = pilot_weapSetList(p, ii);
            for (ij=0; ij<array_size(temp_po_list); ij++) {
               if (temp_po_list[ij].slot->id == slot->id) {
                  instant = ii;
                  break;
               }
            }
            if (instant >= 0)
               break;
         }

         /* Set up for creation. */
         lua_pushnumber(L,++k);
         lua_newtable(L);

         /* Name. */
         lua_pushstring(L,"name");
         lua_pushstring(L,slot->outfit->name);
         lua_rawset(L,-3);


         /* Beams require special handling. */
         if (outfit_isBeam(o)) {
            pilot_getRateMod( &firemod, &enermod, p, slot->outfit );

            /* When firing, cooldown is always zero. When recharging,
             * it's the usual 0-1 readiness value.
             */
            lua_pushstring(L,"cooldown");
            has_beamid = (slot->u.beamid > 0);
            if (has_beamid)
               lua_pushnumber(L, 0.);
            else {
               delay = (slot->timer / outfit_delay(o)) * firemod;
               lua_pushnumber( L, CLAMP( 0., 1., 1. -delay ) );
            }
            lua_rawset(L,-3);

            /* When firing, slot->timer represents the remaining duration. */
            lua_pushstring(L,"charge");
            if (has_beamid)
               lua_pushnumber(L, CLAMP( 0., 1., slot->timer / o->u.bem.duration ) );
            else
               lua_pushnumber( L, CLAMP( 0., 1., 1. -delay ) );
            lua_rawset(L,-3);
         }
         else {
            /* Set cooldown. */
            lua_pushstring(L,"cooldown");
            pilot_getRateMod( &firemod, &enermod, p, slot->outfit );
            delay = outfit_delay(slot->outfit) * firemod;
            if (delay > 0.)
               lua_pushnumber( L, CLAMP( 0., 1., 1. - slot->timer / delay ) );
            else
               lua_pushnumber( L, 1. );
            lua_rawset(L,-3);
         }

         /* Ammo name. */
         ammo = outfit_ammo(slot->outfit);
         if (ammo != NULL) {
            lua_pushstring(L,"ammo");
            lua_pushstring(L,ammo->name);
            lua_rawset(L,-3);
         }

         /* Ammo quantity absolute. */
         if ((is_lau || is_fb) &&
               (slot->u.ammo.outfit != NULL)) {
            lua_pushstring(L,"left");
            lua_pushnumber(L, slot->u.ammo.quantity);
            lua_rawset(L,-3);

         /* Ammo quantity relative. */
            lua_pushstring(L,"left_p");
            lua_pushnumber(L, (double)slot->u.ammo.quantity / (double)pilot_maxAmmoO(p,slot->outfit));
            lua_rawset(L,-3);
         }

         /* Launcher lockon. */
         if (is_lau) {
            t = slot->u.ammo.lockon_timer;
            lua_pushstring(L, "lockon");
            if (t <= 0.)
               lua_pushnumber(L, 1.);
            else
               lua_pushnumber(L, 1. - (t / slot->outfit->u.lau.lockon));
            lua_rawset(L,-3);

         /* Is in arc. */
            lua_pushstring(L, "in_arc");
            lua_pushboolean(L, slot->u.ammo.in_arc);
            lua_rawset(L,-3);
         }

         /* Level. */
         lua_pushstring(L,"level");
         lua_pushnumber(L, level+1);
         lua_rawset(L,-3);

         /* Instant weapon. */
         lua_pushstring(L, "instant");
         if (instant >= 0)
            lua_pushnumber(L, instant + 1);
         else
            lua_pushnil(L);
         lua_rawset(L, -3);

         /* Temperature. */
         lua_pushstring(L,"temp");
         lua_pushnumber(L, pilot_heatFirePercent(slot->heat_T));
         lua_rawset(L,-3);

         /* Type. */
         lua_pushstring(L, "type");
         lua_pushstring(L, outfit_getType(slot->outfit));
         lua_rawset(L,-3);

         /* Damage type. */
         if (is_lau && (slot->u.ammo.outfit != NULL))
            dmg = outfit_damage(slot->u.ammo.outfit);
         else
            dmg = outfit_damage(slot->outfit);
         if (dmg != NULL) {
            lua_pushstring(L, "dtype");
            lua_pushstring(L, dtype_damageTypeToStr(dmg->type));
            lua_rawset(L,-3);
         }

         /* Track. */
         if (slot->outfit->type == OUTFIT_TYPE_TURRET_BOLT) {
            lua_pushstring(L, "track");
            if (target != NULL)
               lua_pushnumber(L, pilot_weaponTrack(
                     p, target, slot->outfit->u.blt.rdr_range,
                     slot->outfit->u.blt.rdr_range_max));
            else
               lua_pushnumber(L, -1);
            lua_rawset(L,-3);
         }

         /* Set table in table. */
         lua_rawset(L,-3);
      }
   }
   return 2;
}


/**
 * @brief Gets heat information for a weapon set.
 *
 * Heat is a 0-2 value that corresponds to three separate ranges:
 *
 * <ul>
 *  <li>0: Weapon set isn't overheating and has no penalties.</li>
 *  <li>0-1: Weapon set has reduced accuracy.</li>
 *  <li>1-2: Weapon set has full accuracy penalty plus reduced fire rate.</li>
 * </ul>
 *
 * @usage hmean, hpeak = p:weapsetHeat( true ) -- Gets info for all active weapons
 * @usage hmean, hpeak = p:weapsetHeat() -- Get info about the current set
 * @usage hmean, hpeak = p:weapsetHeat( 5 ) -- Get info about the set number 5
 *
 *    @luatparam Pilot p Pilot to get weapset weapon of.
 *    @luatparam[opt] number|string|boolean id ID of the set to get
 *       information of. Defaults to currently active set. Strings
 *       identifying special weapon sets can also be used here; see
 *       ai.weapset for more information. Set to true to get
 *       information of all weapon sets.
 *    @luatreturn number Mean heat.
 *    @luatreturn number Peak heat.
 * @luafunc weapsetHeat
 */
static int pilotL_weapsetHeat(lua_State *L)
{
   Pilot *p;
   PilotWeaponSetOutfit *po_list;
   PilotOutfitSlot *slot;
   const Outfit *o;
   int i, j, n;
   int id, all, level, level_match;
   double heat, heat_mean, heat_peak, nweapons;
   const char *name;

   /* Defaults. */
   heat_mean = 0.;
   heat_peak = 0.;
   nweapons = 0;

   /* Parse parameters. */
   all = 0;
   p   = luaL_validpilot(L,1);
   if (lua_gettop(L) > 1) {
      if (lua_isnumber(L,2))
         id = luaL_checkinteger(L,2) - 1;
      else if (lua_isboolean(L,2)) {
         all = lua_toboolean(L,2);
         id  = p->active_set;
      }
      else if (lua_isstring(L,2)) {
         name = lua_tostring(L,2);
         id = pilot_weapSetFromString(name);
         if (id == -1) {
            NLUA_ERROR(L, _("'%s' is not a valid weapon set name."), name);
            return 0;
         }
      }
      else
         NLUA_INVALID_PARAMETER(L);
   }
   else
      id = p->active_set;
   id = CLAMP( 0, PILOT_WEAPON_SETS, id );

   /* Push set. */
   po_list = all ? NULL : pilot_weapSetList( p, id );
   n = all ? array_size(p->outfits) : array_size(po_list);

   for (j=0; j<=PILOT_WEAPSET_MAX_LEVELS; j++) {
      /* Level to match. */
      level_match = (j==PILOT_WEAPSET_MAX_LEVELS) ? -1 : j;

       /* Iterate over weapons. */
      for (i=0; i<n; i++) {
         /* Get base look ups. */
         slot = all ?  p->outfits[i] : po_list[i].slot;

         o = slot->outfit;
         if (o == NULL)
            continue;

         level = all ?  slot->level : po_list[i].level;

         /* Must match level. */
         if (level != level_match)
            continue;

         /* Must be weapon. */
         if (outfit_isMod(o) ||
               outfit_isAfterburner(o))
            continue;

         nweapons++;
         heat = pilot_heatFirePercent(slot->heat_T);
         heat_mean += heat;
         if (heat > heat_peak)
            heat_peak = heat;
      }
   }

   /* Post-process. */
   if (nweapons > 0)
      heat_mean /= nweapons;

   lua_pushnumber( L, heat_mean );
   lua_pushnumber( L, heat_peak );

   return 2;
}


/**
 * @brief Gets the active outfits and their states of the pilot.
 *
 * The active outfits have the following structure: <br />
 * <ul>
 *  <li> name: Name of the set. </li>
 *  <li> type: Type of the outfit. </li>
 *  <li> temp: The heat of the outfit's slot. A value between 0 and 1, where 1 is fully overheated. </li>
 *  <li> weapset: The first weapon set that the outfit appears in, if any. </li>
 *  <li> state: State of the outfit, which can be one of { "off", "warmup", "on", "cooldown" }. </li>
 *  <li> duration: Set only if state is "on". Indicates duration value (0 = just finished, 1 = just on). </li>
 *  <li> cooldown: Set only if state is "cooldown". Indicates cooldown value (0 = just ending, 1 = just started cooling down). </li>
 * </ul>
 *
 * An example would be:
 * @code
 * act_outfits = p:actives()
 * print( "Weapnset Name: " .. ws_name )
 * for i, o in ipairs(act_outfits) do
 *    print( "Name: " .. o.name )
 *    print( "State: " .. o.state )
 * end
 * @endcode
 *
 * @usage act_outfits = p:actives() -- Gets the table of active outfits
 *
 *    @luatparam Pilot p Pilot to get active outfits of.
 *    @luatparam[opt=false] boolean sort Whether or not to sort the outfits.
 *    @luatreturn table The table with each active outfit's information.
 * @luafunc actives
 */
static int pilotL_actives(lua_State *L)
{
   Pilot *p;
   int i, k, sort;
   PilotOutfitSlot *o;
   PilotOutfitSlot **outfits;
   const char *str;
   double d;

   /* Parse parameters. */
   p     = luaL_validpilot(L,1);
   sort  = lua_toboolean(L,2);

   k = 0;
   lua_newtable(L);

   if (sort) {
      outfits = array_copy( PilotOutfitSlot*, p->outfits );
      qsort( outfits, array_size(outfits), sizeof(PilotOutfitSlot*), outfit_compareActive );
   }
   else
      outfits  = p->outfits;

   for (i=0; i<array_size(outfits); i++) {
      /* Get active outfits. */
      o = outfits[i];
      if (o->outfit == NULL)
         continue;
      if (!o->active)
         continue;
      if (!outfit_isMod(o->outfit) &&
            !outfit_isAfterburner(o->outfit))
         continue;

      /* Set up for creation. */
      lua_pushnumber(L,++k);
      lua_newtable(L);

      /* Name. */
      lua_pushstring(L,"name");
      lua_pushstring(L,o->outfit->name);
      lua_rawset(L,-3);

      /* Type. */
      lua_pushstring(L, "type");
      lua_pushstring(L, outfit_getType(o->outfit));
      lua_rawset(L,-3);

      /* Heat. */
      lua_pushstring(L, "temp");
      lua_pushnumber(L, 1 - pilot_heatEfficiencyMod(o->heat_T,
                            o->outfit->u.afb.heat_base,
                            o->outfit->u.afb.heat_cap));
      lua_rawset(L,-3);

      /* Find the first weapon set containing the outfit, if any. */
      if (outfits[i]->weapset != -1) {
         lua_pushstring(L, "weapset");
         lua_pushnumber(L, outfits[i]->weapset + 1);
         lua_rawset(L, -3);
      }

      /* State and timer. */
      switch (o->state) {
         case PILOT_OUTFIT_OFF:
            str = "off";
            break;
         case PILOT_OUTFIT_WARMUP:
            str = "warmup";
            if (!outfit_isMod(o->outfit) || o->outfit->u.mod.lua_env == LUA_NOREF)
               d = 1.; /* TODO add warmup stuff to normal active outfits (not sure if necessary though. */
            else
               d = o->progress;
            lua_pushstring(L,"warmup");
            lua_pushnumber(L, d );
            lua_rawset(L,-3);
            break;
         case PILOT_OUTFIT_ON:
            str = "on";
            if (!outfit_isMod(o->outfit) || o->outfit->u.mod.lua_env == LUA_NOREF) {
               d = outfit_duration(o->outfit);
               if (d==0.)
                  d = 1.;
               else if (!isinf(o->stimer))
                  d = o->stimer / d;
            }
            else
               d = o->progress;
            lua_pushstring(L,"duration");
            lua_pushnumber(L, d );
            lua_rawset(L,-3);
            break;
         case PILOT_OUTFIT_COOLDOWN:
            str = "cooldown";
            if (!outfit_isMod(o->outfit) || o->outfit->u.mod.lua_env == LUA_NOREF) {
               d = outfit_cooldown(o->outfit);
               if (d==0.)
                  d = 0.;
               else if (!isinf(o->stimer))
                  d = o->stimer / d;
            }
            else
               d = o->progress;
            lua_pushstring(L,"cooldown");
            lua_pushnumber(L, d );
            lua_rawset(L,-3);
            break;
         default:
            str = "unknown";
            break;
      }
      lua_pushstring(L,"state");
      lua_pushstring(L,str);
      lua_rawset(L,-3);

      /* Set table in table. */
      lua_rawset(L,-3);
   }

   /* Clean up. */
   if (sort)
      array_free(outfits);

   return 1;
}


/**
 * @brief qsort compare function for active outfits.
 */
static int outfit_compareActive( const void *slot1, const void *slot2 )
{
   const PilotOutfitSlot *s1, *s2;

   s1 = *(const PilotOutfitSlot**) slot1;
   s2 = *(const PilotOutfitSlot**) slot2;

   /* Compare weapon set indexes. */
   if (s1->weapset < s2->weapset)
      return +1;
   else if (s1->weapset > s2->weapset)
      return -1;

   /* Compare positions within the outfit array. */
   if (s1->id < s2->id)
      return +1;
   else if (s1->id > s2->id)
      return -1;

   return 0;
}


/**
 * @brief Gets the outfits of a pilot.
 *
 *    @luatparam Pilot p Pilot to get outfits of.
 *    @luatparam[opt=nil] string What slot type to get outfits of. Can be either nil, "weapon", "utility", or "structure".
 *    @luatreturn {Outfit,...} The outfits of the pilot in an ordered list.
 * @luafunc outfits
 */
static int pilotL_outfits(lua_State *L)
{
   int i, j;
   Pilot *p;
   const char *type;
   OutfitSlotType ost;

   /* Parse parameters */
   p     = luaL_validpilot(L,1);
   type  = luaL_optstring(L,2,NULL);

   /* Get type. */
   if (type != NULL) {
      if (strcmp(type,"structure")==0)
         ost = OUTFIT_SLOT_STRUCTURE;
      else if (strcmp(type,"utility")==0)
         ost = OUTFIT_SLOT_UTILITY;
      else if (strcmp(type,"weapon")==0)
         ost = OUTFIT_SLOT_WEAPON;
      else
         NLUA_ERROR(L,_("Unknown slot type '%s'"), type);
   }
   else
      ost = OUTFIT_SLOT_NULL;

   j  = 1;
   lua_newtable( L );
   for (i=0; i<array_size(p->outfits); i++) {

      /* Get outfit. */
      if (p->outfits[i]->outfit == NULL)
         continue;

      /* Only match specific type. */
      if ((ost!=OUTFIT_SLOT_NULL) && (p->outfits[i]->outfit->slot.type!=ost))
         continue;

      /* Set the outfit. */
      lua_pushnumber( L, j++ );
      lua_pushoutfit( L, p->outfits[i]->outfit );
      lua_rawset( L, -3 );
   }

   return 1;
}


/**
 * @brief Gets the ammo of a pilot.
 *
 * Returned table contains inner tables each representing a particular
 * weapon's ammo, with the following keys:<br />
 * <ul>
 *    <li>"name": Raw (untranslated) name of the ammo.</li>
 *    <li>"quantity": The quantity of the ammo the pilot currently has.</li>
 * </ul>
 * <br />
 *
 * @usage
 * -- Remove all ammo
 * for i, amm in ipairs(p:ammo()) do
 *    p:outfitRm(amm.name, amm.quantity)
 * end
 *
 *    @luatparam Pilot p Pilot to get ammo of.
 *    @luatreturn {table,...} Ordered list of ammo info tables; see
 *       above for the contents of the ammo info tables.
 * @luafunc ammo
 */
static int pilotL_ammo(lua_State *L)
{
   Pilot *p;
   PilotOutfitSlot* po;
   const Outfit *o;
   const Outfit *amm;
   int i, j;

   p = luaL_validpilot(L, 1);

   j = 1;
   lua_newtable(L); /* t */
   for (i=0; i<array_size(p->outfits); i++) {
      po = p->outfits[i];
      if (po == NULL)
         continue;

      o = po->outfit;
      if (o == NULL)
         continue;

      amm = outfit_ammo(o);
      if (amm == NULL)
         continue;

      lua_pushnumber(L, j++); /* t, i */

      lua_newtable(L); /* t, i, t */

      lua_pushstring(L, "name"); /* t, i, t, k */
      lua_pushstring(L, amm->name); /* t, i, t, k, s */
      lua_rawset(L, -3); /* t, i, t */

      lua_pushstring(L, "quantity"); /* t, i, t, k */
      lua_pushnumber(L, po->u.ammo.quantity); /* t, i, t, k, n */
      lua_rawset(L, -3); /* t, i, t */

      lua_rawset(L, -3); /* t */
   }

   return 1;
}


/**
 * @brief Gets a pilot's outfit by ID.
 *
 *    @luatparam Pilot p Pilot to get outf of.
 *    @luatparam number id ID of the outfit to get.
 *    @luatreturn Outfit|nil Outfit equipped in the slot or nil otherwise.
 * @luafunc outfitByID
 */
static int pilotL_outfitByID(lua_State *L)
{
   int id;
   Pilot *p;
   /* Parse parameters */
   p     = luaL_validpilot(L,1);
   id    = luaL_checkinteger(L,2)-1;
   if (id < 0 || id >= array_size(p->outfits))
      NLUA_ERROR(L, _("Pilot '%s' outfit ID '%d' is out of range!"), p->name, id);

   if (p->outfits[id]->outfit != NULL)
      lua_pushoutfit( L, p->outfits[id]->outfit );
   else
      lua_pushnil( L );
   return 1;
}


/**
 * @brief Changes the pilot's name.
 *
 * @usage p:rename( _("Black Beard") )
 *
 *    @luatparam Pilot p Pilot to change name of.
 *    @luatparam string name Name to change to.
 * @luafunc rename
 */
static int pilotL_rename(lua_State *L)
{
   const char *name;
   Pilot *p;

   NLUA_CHECKRW(L);

   /* Parse parameters */
   p     = luaL_validpilot(L,1);
   name  = luaL_checkstring(L,2);

   /* Change name. */
   free(p->name);
   p->name = strdup(name);

   return 0;
}

/**
 * @brief Gets the pilot's position.
 *
 * @usage v = p:pos()
 *
 *    @luatparam Pilot p Pilot to get the position of.
 *    @luatreturn Vec2 The pilot's current position.
 * @luafunc pos
 */
static int pilotL_position(lua_State *L)
{
   Pilot *p;

   /* Parse parameters */
   p     = luaL_validpilot(L,1);

   /* Push position. */
   lua_pushvector(L, p->solid->pos);
   return 1;
}

/**
 * @brief Gets the pilot's velocity.
 *
 * @usage vel = p:vel()
 *
 *    @luatparam Pilot p Pilot to get the velocity of.
 *    @luatreturn Vec2 The pilot's current velocity.
 * @luafunc vel
 */
static int pilotL_velocity(lua_State *L)
{
   Pilot *p;

   /* Parse parameters */
   p     = luaL_validpilot(L,1);

   /* Push velocity. */
   lua_pushvector(L, p->solid->vel);
   return 1;
}

/**
 * @brief Gets the pilot's evasion.
 *
 * @usage d = p:ew()
 *
 *    @luatparam Pilot p Pilot to get the evasion of.
 *    @luatreturn number The pilot's current evasion value.
 * @luafunc ew
 */
static int pilotL_ew(lua_State *L)
{
   Pilot *p;

   /* Parse parameters */
   p     = luaL_validpilot(L,1);
   (void) p;

   /* Push direction. */
   lua_pushnumber( L, 0 );
   return 1;
}

/**
 * @brief Gets the pilot's direction.
 *
 * @usage d = p:dir()
 *
 *    @luatparam Pilot p Pilot to get the direction of.
 *    @luatreturn number The pilot's current direction as a number (in degrees).
 * @luafunc dir
 */
static int pilotL_dir(lua_State *L)
{
   Pilot *p;

   /* Parse parameters */
   p     = luaL_validpilot(L,1);

   /* Push direction. */
   lua_pushnumber( L, p->solid->dir * 180./M_PI );
   return 1;
}

/**
 * @brief Gets the temperature of a pilot.
 *
 * @usage t = p:temp()
 *
 *    @luatparam Pilot p Pilot to get temperature of.
 *    @luatreturn number The pilot's current temperature (in kelvin).
 * @luafunc temp
 */
static int pilotL_temp(lua_State *L)
{
   Pilot *p;

   /* Parse parameters */
   p     = luaL_validpilot(L,1);

   /* Push direction. */
   lua_pushnumber( L, p->heat_T );
   return 1;
}

/**
 * @brief Gets the mass of a pilot.
 *
 * @usage m = p:mass()
 *
 *    @luatparam Pilot p Pilot to get mass of.
 *    @luatreturn number The pilot's current mass (in tonnes).
 * @luafunc mass
 */
static int pilotL_mass(lua_State *L)
{
   Pilot *p = luaL_validpilot(L,1);
   lua_pushnumber( L, p->solid->mass );
   return 1;
}

/**
 * @brief Gets the pilot's faction.
 *
 * @usage f = p:faction()
 *
 *    @luatparam Pilot p Pilot to get the faction of.
 *    @luatreturn Faction The faction of the pilot.
 * @luafunc faction
 */
static int pilotL_faction(lua_State *L)
{
   Pilot *p;

   /* Parse parameters */
   p     = luaL_validpilot(L,1);

   /* Push faction. */
   lua_pushfaction(L,p->faction);
   return 1;
}


/**
 * @brief Checks the pilot's spaceworthiness
 *
 * @usage spaceworthy = p:spaceworthy()
 *
 *    @luatparam Pilot p Pilot to get the spaceworthy status of
 *    @luatreturn boolean Whether the pilot's ship is spaceworthy
 * @luafunc spaceworthy
 */
static int pilotL_spaceworthy(lua_State *L)
{
   Pilot *p = luaL_validpilot(L, 1);
   int problems = pilot_reportSpaceworthy(p, NULL, 0);

   /* Push position. */
   lua_pushboolean(L, !problems);
   return 1;
}


/**
 * @brief Sets the pilot's position.
 *
 * @usage p:setPos( vec2.new( 300, 200 ) )
 *
 *    @luatparam Pilot p Pilot to set the position of.
 *    @luatparam Vec2 pos Position to set.
 * @luafunc setPos
 */
static int pilotL_setPosition(lua_State *L)
{
   Pilot *p;
   Vector2d *vec;

   NLUA_CHECKRW(L);

   /* Parse parameters */
   p     = luaL_validpilot(L,1);
   vec   = luaL_checkvector(L,2);

   /* Insert skip in trail. */
   pilot_sample_trails( p, 1 );

   /* Warp pilot to new position. */
   p->solid->pos = *vec;

   /* Update if necessary. */
   if (pilot_isPlayer(p))
      cam_update( 0. );

   return 0;
}

/**
 * @brief Sets the pilot's velocity.
 *
 * @usage p:setVel( vec2.new( 300, 200 ) )
 *
 *    @luatparam Pilot p Pilot to set the velocity of.
 *    @luatparam Vec2 vel Velocity to set.
 * @luafunc setVel
 */
static int pilotL_setVelocity(lua_State *L)
{
   Pilot *p;
   Vector2d *vec;

   NLUA_CHECKRW(L);

   /* Parse parameters */
   p     = luaL_validpilot(L,1);
   vec   = luaL_checkvector(L,2);

   /* Warp pilot to new position. */
   p->solid->vel = *vec;
   return 0;
}

/**
 * @brief Sets the pilot's direction.
 *
 * @note Right is 0, top is 90, left is 180, bottom is 270.
 *
 * @usage p:setDir( 180. )
 *
 *    @luatparam Pilot p Pilot to set the direction of.
 *    @luatparam number dir Direction to set.
 * @luafunc setDir
 */
static int pilotL_setDir(lua_State *L)
{
   Pilot *p;
   double d;

   NLUA_CHECKRW(L);

   /* Parse parameters */
   p     = luaL_validpilot(L,1);
   d     = luaL_checknumber(L,2);

   /* Set direction. */
   p->solid->dir = fmodf( d*M_PI/180., 2*M_PI );
   if (p->solid->dir < 0.)
      p->solid->dir += 2*M_PI;

   return 0;
}

/**
 * @brief Makes the pilot broadcast a message.
 *
 * @usage p:broadcast( "Mayday! Requesting assistance!" )
 * @usage p:broadcast( "Help!", true ) -- Will ignore interference
 *
 *    @luatparam Pilot p Pilot to broadcast the message.
 *    @luatparam string msg Message to broadcast.
 *    @luatparam[opt=false] boolean ignore_int Whether or not it should ignore interference.
 * @luafunc broadcast
 */
static int pilotL_broadcast(lua_State *L)
{
   Pilot *p;
   const char *msg;
   int ignore_int;

   NLUA_CHECKRW(L);

   /* Parse parameters. */
   p           = luaL_validpilot(L,1);
   msg         = luaL_checkstring(L,2);
   ignore_int  = lua_toboolean(L,3);

   /* Broadcast message. */
   pilot_broadcast( p, msg, ignore_int );
   return 0;
}

/**
 * @brief Sends a comm message from one pilot to another.
 *
 * @usage p:comm( _("How are you doing?") ) -- Messages the player
 * @usage p:comm( _("You got this?"), true ) -- Messages the player ignoring interference
 * @usage p:comm( target, _("Heya!") ) -- Messages target
 * @usage p:comm( target, _("Got this?"), true ) -- Messages target ignoring interference
 *
 *    @luatparam Pilot p Pilot sending the comm.
 *    @luatparam[opt] Pilot target Target to send message to. Sends to
 *       the player if omitted.
 *    @luatparam string msg Message to send.
 *    @luatparam[opt=false] boolean ignore_int Whether or not to ignore
 *       interference.
 * @luafunc comm
 */
static int pilotL_comm(lua_State *L)
{
   Pilot *p, *t;
   LuaPilot target;
   const char *msg;
   int ignore_int;

   NLUA_CHECKRW(L);

   /* Parse parameters. */
   p = luaL_validpilot(L,1);
   if (lua_isstring(L,2)) {
      target = 0;
      msg = luaL_checkstring(L,2);
      ignore_int = lua_toboolean(L,3);
   }
   else {
      target = luaL_checkpilot(L,2);
      msg = luaL_checkstring(L,3);
      ignore_int = lua_toboolean(L,4);
   }

   /* Check to see if pilot is valid. */
   if (target == 0)
      t = player.p;
   else {
      t = pilot_get(target);
      if (t == NULL) {
         NLUA_ERROR(L,"Pilot param 2 not found in pilot stack!");
         return 0;
      }
   }

   /* Broadcast message. */
   pilot_message( p, t->id, msg, ignore_int );
   return 0;
}

/**
 * @brief Sets the pilot's faction.
 *
 * @usage p:setFaction( "Empire" )
 * @usage p:setFaction( faction.get( "Dvaered" ) )
 *
 *    @luatparam Pilot p Pilot to change faction of.
 *    @luatparam Faction faction Faction to set by name or faction.
 * @luafunc setFaction
 */
static int pilotL_setFaction(lua_State *L)
{
   Pilot *p;
   LuaFaction fid;

   NLUA_CHECKRW(L);

   /* Parse parameters. */
   p = luaL_validpilot(L,1);
   fid = luaL_validfaction(L,2);

   /* Set the new faction. */
   p->faction = fid;

   return 0;
}


/**
 * @brief Controls the pilot's hostility towards the player.
 *
 * @usage p:setHostile() -- Pilot is now hostile.
 * @usage p:setHostile(false) -- Make pilot non-hostile.
 *
 *    @luatparam Pilot p Pilot to set the hostility of.
 *    @luatparam[opt=true] boolean state Whether to set or unset hostile.
 * @luafunc setHostile
 */
static int pilotL_setHostile(lua_State *L)
{
   Pilot *p;
   int state;

   NLUA_CHECKRW(L);

   /* Get the pilot. */
   p = luaL_validpilot(L,1);

   /* Get state. */
   if (lua_isnone(L,2))
      state = 1;
   else
      state = lua_toboolean(L, 2);

   /* Set as hostile. */
   if (state)
      pilot_setHostile(p);
   else
      pilot_rmHostile(p);

   return 0;
}


/**
 * @brief Controls the pilot's friendliness towards the player.
 *
 * @usage p:setFriendly() -- Pilot is now friendly.
 * @usage p:setFriendly(false) -- Make pilot non-friendly.
 *
 *    @luatparam Pilot p Pilot to set the friendliness of.
 *    @luatparam[opt=true] boolean state Whether to set or unset friendly.
 * @luafunc setFriendly
 */
static int pilotL_setFriendly(lua_State *L)
{
   Pilot *p;
   int state;

   NLUA_CHECKRW(L);

   /* Get the pilot. */
   p = luaL_validpilot(L,1);

   /* Get state. */
   if (lua_isnone(L,2))
      state = 1;
   else
      state = lua_toboolean(L, 2);

   /* Remove hostile and mark as friendly. */
   if (state)
      pilot_setFriendly(p);
   /* Remove friendly flag. */
   else
      pilot_rmFriendly(p);

   return 0;
}


/**
 * @brief Sets the pilot's invincibility status.
 *
 * @usage p:setInvincible() -- p can not be hit anymore
 * @usage p:setInvincible(true) -- p can not be hit anymore
 * @usage p:setInvincible(false) -- p can be hit again
 *
 *    @luatparam Pilot p Pilot to set invincibility status of.
 *    @luatparam[opt=true] boolean state State to set invincibility.
 * @luafunc setInvincible
 */
static int pilotL_setInvincible(lua_State *L)
{
   return pilotL_setFlagWrapper( L, PILOT_INVINCIBLE );
}


/**
 * @brief Sets the pilot's invincibility status towards the player.
 *
 * @usage p:setInvincPlayer() -- p can not be hit by the player anymore
 * @usage p:setInvincPlayer(true) -- p can not be hit by the player anymore
 * @usage p:setInvincPlayer(false) -- p can be hit by the player again
 *
 *    @luatparam Pilot p Pilot to set invincibility status of (only affects player).
 *    @luatparam[opt=true] boolean state State to set invincibility.
 * @luafunc setInvincPlayer
 */
static int pilotL_setInvincPlayer(lua_State *L)
{
   return pilotL_setFlagWrapper( L, PILOT_INVINC_PLAYER );
}


/**
 * @brief Sets the pilot's hide status.
 *
 * A hidden pilot is neither updated nor drawn. It stays frozen in time
 *  until the hide is lifted.
 *
 * @usage p:setHide() -- p will disappear
 * @usage p:setHide(true) -- p will disappear
 * @usage p:setHide(false) -- p will appear again
 *
 *    @luatparam Pilot p Pilot to set hidden status of.
 *    @luatparam[opt=true] boolean state Whether or not the pilot should
 *       be hidden.
 * @luafunc setHide
 */
static int pilotL_setHide(lua_State *L)
{
   return pilotL_setFlagWrapper( L, PILOT_HIDE );
}


/**
 * @brief Sets the pilot's invisibility status.
 *
 * An invisible pilot is not shown on the radar nor targettable, however, it
 * renders and updates just like normal.
 *
 *    @luatparam Pilot p Pilot to set invisibility status of.
 *    @luatparam[opt=true] boolean state Whether or not the pilot should
 *       be invisible.
 * @luafunc setInvisible
 */
static int pilotL_setInvisible(lua_State *L)
{
   return pilotL_setFlagWrapper( L, PILOT_INVISIBLE );
}


/**
 * @brief Sets the pilot's norender status.
 *
 * The pilot still acts normally but is just not visible and can still take
 * damage. Meant to be used in conjunction with other flags like "invisible".
 *
 *    @luatparam Pilot p Pilot to set norender status of.
 *    @luatparam[opt=true] boolean state true if the pilot should be
 *       given norender status, false if the pilot should be rendered
 *       normally.
 * @luafunc setInvisible
 */
static int pilotL_setNoRender(lua_State *L)
{
   return pilotL_setFlagWrapper( L, PILOT_NORENDER );
}


/**
 * @brief Marks the pilot as always visible for the player.
 *
 * This cancels out ewarfare visibility ranges and only affects the visibility of the player.
 *
 * @usage p:setVisplayer( true )
 *
 *    @luatparam Pilot p Pilot to set player visibility status of.
 *    @luatparam[opt=true] boolean state State to set player visibility.
 * @luafunc setVisplayer
 */
static int pilotL_setVisplayer(lua_State *L)
{
   return pilotL_setFlagWrapper( L, PILOT_VISPLAYER );
}


/**
 * @brief Marks the pilot as always visible for other pilots.
 *
 * This cancels out ewarfare visibility ranges and affects every pilot.
 *
 * @usage p:setVisible( true )
 *
 *    @luatparam Pilot p Pilot to set visibility status of.
 *    @luatparam[opt=true] boolean state State to set visibility.
 * @luafunc setVisible
 */
static int pilotL_setVisible(lua_State *L)
{
   return pilotL_setFlagWrapper( L, PILOT_VISIBLE );
}


/**
 * @brief Makes pilot stand out on radar and the likes.
 *
 * This makes the pilot stand out in the map overlay and radar to increase noticability.
 *
 * @usage p:setHilight( true )
 *
 *    @luatparam Pilot p Pilot to set hilight status of.
 *    @luatparam[opt=true] boolean state State to set hilight.
 * @luafunc setHilight
 */
static int pilotL_setHilight(lua_State *L)
{
   return pilotL_setFlagWrapper( L, PILOT_HILIGHT );
}


/**
 * @brief Allows the pilot to be boarded when not disabled.
 *
 * @usage p:setActiveBoard( true )
 *
 *    @luatparam Pilot p Pilot to set boardability of.
 *    @luatparam[opt=true] boolean state State to set boardability.
 * @luafunc setActiveBoard
 */
static int pilotL_setActiveBoard(lua_State *L)
{
   return pilotL_setFlagWrapper( L, PILOT_BOARDABLE );
}


/**
 * @brief Makes it so the pilot never dies, stays at 1. armour.
 *
 * @usage p:setNoDeath( true ) -- Pilot will never die
 *
 *    @luatparam Pilot p Pilot to set never die state of.
 *    @luatparam[opt=true] boolean state Whether or not to set never die state.
 * @luafunc setNoDeath
 */
static int pilotL_setNoDeath(lua_State *L)
{
   return pilotL_setFlagWrapper( L, PILOT_NODEATH );
}


/**
 * @brief Disables a pilot.
 *
 * @usage p:disable()
 *
 *    @luatparam Pilot p Pilot to disable.
 *    @luatparam[opt=false] boolean temporary Whether or not the disable
 *       should be temporary (i.e. the pilot should automatically become
 *       re-enabled after a period of time, like normal). If this is
 *       false, the pilot will remain disabled until explicitly
 *       re-enabled.
 * @luafunc disable
 */
static int pilotL_disable(lua_State *L)
{
   Pilot *p;
   int permanent;

   NLUA_CHECKRW(L);

   /* Get the pilot. */
   p = luaL_validpilot(L,1);
   permanent = !lua_toboolean(L,2);

   /* Disable the pilot. */
   p->shield = 0.;
   p->stress = p->armour;
   pilot_updateDisable(p, 0);

   if (permanent)
      pilot_setFlag(p, PILOT_DISABLED_PERM);
   else
      pilot_rmFlag(p, PILOT_DISABLED_PERM);

   return 0;
}


/**
 * @brief Gets a pilot's cooldown state.
 *
 * @usage cooldown, braking = p:cooldown()
 *
 *    @luatparam Pilot p Pilot to check the cooldown status of.
 *    @luatreturn boolean Cooldown status.
 *    @luatreturn boolean Cooldown braking status.
 * @luafunc cooldown
 */
static int pilotL_cooldown(lua_State *L)
{
   Pilot *p;

   /* Get the pilot. */
   p = luaL_validpilot(L,1);

   /* Get the cooldown status. */
   lua_pushboolean( L, pilot_isFlag(p, PILOT_COOLDOWN) );
   lua_pushboolean( L, pilot_isFlag(p, PILOT_COOLDOWN_BRAKE) );

   return 2;
}


/**
 * @brief Starts or stops a pilot's cooldown mode.
 *
 * @usage p:setCooldown( true )
 *
 *    @luatparam Pilot p Pilot to modify the cooldown status of.
 *    @luatparam[opt=true] boolean state Whether to enable or disable cooldown.
 * @luafunc setCooldown
 */
static int pilotL_setCooldown(lua_State *L)
{
   Pilot *p;
   int state;

   NLUA_CHECKRW(L);

   /* Get the pilot. */
   p = luaL_validpilot(L,1);

   /* Get state. */
   if (lua_isnone(L,2))
      state = 1;
   else
      state = lua_toboolean(L, 2);

   /* Set status. */
   if (state)
      pilot_cooldown( p );
   else
      pilot_cooldownEnd(p, NULL);

   return 0;
}


/**
 * @brief Enables or disables a pilot's hyperspace engine.
 *
 * @usage p:setNoJump( true )
 *
 *    @luatparam Pilot p Pilot to modify.
 *    @luatparam[opt=true] boolean state true to disallow jumping, false
 *       to allow jumping.
 * @luafunc setNoJump
 */
static int pilotL_setNoJump(lua_State *L)
{
   return pilotL_setFlagWrapper( L, PILOT_NOJUMP );
}


/**
 * @brief Enables or disables landing for a pilot.
 *
 * @usage p:setNoLand( true )
 *
 *    @luatparam Pilot p Pilot to modify.
 *    @luatparam[opt=true] boolean state true to disallow landing, false
 *       to allow landing.
 * @luafunc setNoLand
 */
static int pilotL_setNoLand(lua_State *L)
{
   return pilotL_setFlagWrapper( L, PILOT_NOLAND );
}


/**
 * @brief Enables or disables making the the pilot exempt from pilot.clear().
 *
 * @usage p:setNoClear( true )
 *
 *    @luatparam Pilot p Pilot to modify.
 *    @luatparam[opt=true] boolean state true to exempt the pilot from
 *       pilot.clear(), false to make the pilot affected by
 *       pilot.clear() normally.
 * @luasee clear
 * @luafunc setNoClear
 */
static int pilotL_setNoClear(lua_State *L)
{
   return pilotL_setFlagWrapper( L, PILOT_NOCLEAR );
}


/**
 * @brief Adds an outfit to a pilot.
 *
 * This by default tries to add them to the first empty slot. Will not
 * overwrite existing outfits.
 *
 * @usage added = p:outfitAdd("Laser Cannon", 5)
 *
 *    @luatparam Pilot p Pilot to add outfit to.
 *    @luatparam string|Outfit outfit Outfit or name of the outfit to
 *       add.
 *    @luatparam[opt=1] number q Quantity of the outfit to add.
 *    @luatparam[opt=false] boolean bypass_cpu Whether to skip CPU
 *       checks when adding an outfit.
 *    @luatparam[opt=false] boolean bypass_slot Whether to skip slot
 *       size checks before adding an outfit.
 *    @luatreturn number The number of outfits added.
 * @luafunc outfitAdd
 */
static int pilotL_outfitAdd(lua_State *L)
{
   int i;
   Pilot *p;
   const Outfit *o;
   int ret;
   int q, added, bypass_cpu, bypass_slot;

   NLUA_CHECKRW(L);

   /* Get parameters. */
   p      = luaL_validpilot(L,1);
   o      = luaL_validoutfit(L,2);
   q      = luaL_optinteger(L,3,1);
   bypass_cpu = lua_toboolean(L,4);
   bypass_slot = lua_toboolean(L,5);

   /* Add outfit. */
   added = 0;
   for (i=0; i<array_size(p->outfits); i++) {
      /* Must still have to add outfit. */
      if (q <= 0)
         break;

      /* Must not have outfit already. */
      if (p->outfits[i]->outfit != NULL)
         continue;

      if (bypass_slot) {
         /* Only do a basic slot type check. */
         if (!outfit_fitsSlotType( o, &p->outfits[i]->sslot->slot ))
            continue;
      }
      else {
         /* Do a full slot check. */
         if (!outfit_fitsSlot( o, &p->outfits[i]->sslot->slot ))
            continue;
      }

      if (!bypass_cpu) {
         /* Test if can add outfit (CPU check). */
         ret = pilot_addOutfitTest( p, o, p->outfits[i], 0 );
         if (ret)
            break;
      }

      /* Add outfit - already tested. */
      ret = pilot_addOutfitRaw( p, o, p->outfits[i] );
      if (ret==0)
         pilot_outfitLInit( p, p->outfits[i] );
      pilot_calcStats( p );

      /* Add ammo if needed. */
      if ((ret==0) && (outfit_ammo(o) != NULL))
         pilot_addAmmo( p, p->outfits[i], outfit_ammo(o), pilot_maxAmmoO(p,o) );

      /* We added an outfit. */
      q--;
      added++;
   }

   /* Update the weapon sets. */
   if ((added > 0) && p->autoweap)
      pilot_weaponAuto(p);

   /* Update equipment window if operating on the player's pilot. */
   if (player.p != NULL && player.p == p && added > 0)
      outfits_updateEquipmentOutfits();

   lua_pushnumber(L,added);
   return 1;
}


/**
 * @brief Removes an outfit from a pilot.
 *
 * "all" will remove all outfits except cores.
 * "cores" will remove all cores, but nothing else.
 *
 * @usage p:outfitRm( "all" ) -- Leaves the pilot naked (except for cores).
 * @usage p:outfitRm( "cores" ) -- Strips the pilot of its cores, leaving it dead in space.
 * @usage p:outfitRm( "Neutron Disruptor" ) -- Removes a neutron disruptor.
 * @usage p:outfitRm( "Neutron Disruptor", 2 ) -- Removes two neutron disruptor.
 *
 *    @luatparam Pilot p Pilot to remove outfit from.
 *    @luatparam string|outfit outfit Outfit or name of the outfit to remove.
 *    @luatparam number q Quantity of the outfit to remove.
 *    @luatreturn number The number of outfits removed.
 * @luafunc outfitRm
 */
static int pilotL_outfitRm(lua_State *L)
{
   int i;
   Pilot *p;
   PilotOutfitSlot *po;
   const char *outfit;
   const Outfit *o, *amm;
   int q, removed, matched = 0;
   int temp_r;

   NLUA_CHECKRW(L);

   /* Get parameters. */
   removed = 0;
   p = luaL_validpilot(L, 1);
   q = luaL_optinteger(L, 3, 1);

   if (lua_isstring(L,2)) {
      outfit = luaL_checkstring(L,2);

      /* If outfit is "all", we remove everything except cores. */
      if (strcmp(outfit,"all")==0) {
         for (i=0; i<array_size(p->outfits); i++) {
            if (p->outfits[i]->sslot->required)
               continue;
            pilot_rmOutfitRaw( p, p->outfits[i] );
            removed++;
         }
         pilot_calcStats( p ); /* Recalculate stats. */
         matched = 1;
      }
      /* If outfit is "cores", we remove cores only. */
      else if (strcmp(outfit,"cores")==0) {
         for (i=0; i<array_size(p->outfits); i++) {
            if (!p->outfits[i]->sslot->required)
               continue;
            pilot_rmOutfitRaw( p, p->outfits[i] );
            removed++;
         }
         pilot_calcStats( p ); /* Recalculate stats. */
         matched = 1;
      }
   }

   if (!matched) {
      o = luaL_validoutfit(L,2);

      /* Remove the outfit outfit. */
      for (i=0; i<array_size(p->outfits); i++) {
         /* Must still need to remove. */
         if (q <= 0)
            break;

         po = p->outfits[i];

         /* Must not be NULL. */
         if (po->outfit == NULL)
            continue;

         if (strcmp(po->outfit->name, o->name) == 0) {
            /* Remove outfit. */
            pilot_rmOutfit(p, po);
            q--;
            removed++;
         }
         else {
            amm = outfit_ammo(po->outfit);
            if ((amm != NULL) && (strcmp(amm->name, o->name) == 0)) {
               /* Remove the ammo. */
               temp_r = pilot_rmAmmo(p, po, q);
               q -= temp_r;
               removed += temp_r;
            }
         }
      }
   }

   /* Update equipment window if operating on the player's pilot. */
   if (player.p != NULL && player.p == p && removed > 0)
      outfits_updateEquipmentOutfits();

   lua_pushnumber( L, removed );
   return 1;
}


/**
 * @brief Sets the fuel of a pilot.
 *
 * @usage p:setFuel( true ) -- Sets fuel to max
 *
 *    @luatparam Pilot p Pilot to set fuel of.
 *    @luatparam boolean|number f true sets fuel to max, false sets fuel to 0, a number sets
 *              fuel to that amount in units.
 *    @luatreturn number The amount of fuel the pilot has.
 * @luafunc setFuel
 */
static int pilotL_setFuel(lua_State *L)
{
   Pilot *p;

   NLUA_CHECKRW(L);

   /* Get the pilot. */
   p = luaL_validpilot(L,1);

   /* Get the parameter. */
   if (lua_isboolean(L,2)) {
      if (lua_toboolean(L,2))
         p->fuel = p->fuel_max;
      else
         p->fuel = 0.;
   }
   else if (lua_isnumber(L,2)) {
      p->fuel = CLAMP(0., p->fuel_max, lua_tonumber(L, 2));
   }
   else
      NLUA_INVALID_PARAMETER(L);

   /* Return amount of fuel. */
   lua_pushnumber(L, p->fuel);
   return 1;
}


/**
 * @brief Resets the intrinsic stats of a pilot.
 *
 * @luafunc intrinsicReset
 */
static int pilotL_intrinsicReset(lua_State *L)
{
   Pilot *p = luaL_validpilot(L,1);
   ss_statsInit( &p->intrinsic_stats );
   pilot_calcStats( p );
   return 0;
}


/**
 * @brief Allows setting intrinsic stats of a pilot.
 *
 *    @luatparam Pilot p Pilot to set stat of.
 *    @luatparam string name Name of the stat to set. It is the same as in the xml.
 *    @luatparam number value Value to set the stat to.
 *    @luatparam boolean replace Whether or not to add to the stat or replace it.
 * @luafunc intrinsicSet
 */
static int pilotL_intrinsicSet(lua_State *L)
{
   Pilot *p = luaL_validpilot(L,1);
   const char *name;
   double value;
   int replace;
   /* Case individual parameter. */
   if (!lua_istable(L,2)) {
      name     = luaL_checkstring(L,2);
      value    = luaL_checknumber(L,3);
      replace  = lua_toboolean(L,4);
      ss_statsSet( &p->intrinsic_stats, name, value, replace );
      pilot_calcStats( p );
      return 0;
   }
   replace = lua_toboolean(L,4);
   /* Case set of parameters. */
   lua_pushnil(L);
   while (lua_next(L,2) != 0) {
      name     = luaL_checkstring(L,-2);
      value    = luaL_checknumber(L,-1);
      ss_statsSet( &p->intrinsic_stats, name, value, replace );
      lua_pop(L,1);
   }
   lua_pop(L,1);
   pilot_calcStats( p );
   return 0;
}


/**
 * @brief Allows getting an intrinsic stats of a pilot, or gets all of them if name is not specified.
 *
 *    @luatparam Pilot p Pilot to get stat of.
 *    @luatparam[opt=nil] string name Name of the stat to get. It is the same as in the xml.
 *    @luatparam[opt=false] boolean internal Whether or not to use the internal representation.
 *    @luaparam Value of the stat or a table containing all the stats if name is not specified.
 * @luafunc intrinsicGet
 */
static int pilotL_intrinsicGet(lua_State *L)
{
   Pilot *p          = luaL_validpilot(L,1);
   const char *name  = luaL_optstring(L,2,NULL);
   int internal      = lua_toboolean(L,3);
   ss_statsGetLua( L, &p->intrinsic_stats, name, internal );
   return 1;
}


/**
 * @brief Changes the pilot's AI.
 *
 * @usage p:changeAI( "empire" ) -- set the pilot to use the Empire AI
 *
 *    @luatparam Pilot p Pilot to change AI of.
 *    @luatparam string newai Name of Ai to use.
 *
 * @luafunc changeAI
 */
static int pilotL_changeAI(lua_State *L)
{
   Pilot *p;
   const char *str;
   int ret;

   NLUA_CHECKRW(L);

   /* Get parameters. */
   p  = luaL_validpilot(L,1);
   str = luaL_checkstring(L,2);

   /* Get rid of current AI. */
   ai_destroy(p);

   /* Create the new AI. */
   ret = ai_pinit( p, str );
   lua_pushboolean(L, ret);
   return 1;
}


/**
 * @brief Sets the temperature of a pilot.
 *
 * All temperatures are in Kelvins. Note that temperatures cannot go below the base temperature of the Naev galaxy, which is 250K.
 *
 * @usage p:setTemp( 300, true ) -- Sets ship temperature to 300K, as well as all outfits.
 * @usage p:setTemp( 500, false ) -- Sets ship temperature to 500K, but leaves outfits alone.
 * @usage p:setTemp( 0 ) -- Sets ship temperature to the base temperature, as well as all outfits.
 *
 *    @luatparam Pilot p Pilot to set health of.
 *    @luatparam number temp Value to set temperature to. Values below base temperature will be clamped.
 *    @luatparam[opt=false] boolean noslots Whether slots should also be set to this temperature.
 * @luafunc setTemp
 */
static int pilotL_setTemp(lua_State *L)
{
   Pilot *p;
   int i, setOutfits = 1;
   double kelvins;

   NLUA_CHECKRW(L);

   /* Handle parameters. */
   p  = luaL_validpilot(L,1);
   kelvins  = luaL_checknumber(L, 2);
   setOutfits = !lua_toboolean(L,3);

   /* Temperature must not go below base temp. */
   kelvins = MAX(kelvins, CONST_SPACE_STAR_TEMP);

   /* Handle pilot ship. */
   p->heat_T = kelvins;

   /* Handle pilot outfits (maybe). */
   if (setOutfits)
      for (i = 0; i < array_size(p->outfits); i++)
         p->outfits[i]->heat_T = kelvins;

   return 0;
}


/**
 * @brief Sets the health of a pilot.
 *
 * This recovers the pilot's disabled state, although he may become disabled afterwards.
 *
 * @usage p:setHealth( 100, 100 ) -- Sets pilot to full health
 * @usage p:setHealth(  70,   0 ) -- Sets pilot to 70% armour
 * @usage p:setHealth( 100, 100, 0 ) -- Sets pilot to full health and no stress
 *
 *    @luatparam Pilot p Pilot to set health of.
 *    @luatparam number armour Value to set armour to, should be double from 0-100 (in percent).
 *    @luatparam number shield Value to set shield to, should be double from 0-100 (in percent).
 *    @luatparam[opt=0] number stress Value to set stress (disable damage) to, should be double from 0-100 (in percent of current armour).
 * @luafunc setHealth
 */
static int pilotL_setHealth(lua_State *L)
{
   Pilot *p;
   double a, s, st;

   NLUA_CHECKRW(L);

   /* Handle parameters. */
   p  = luaL_validpilot(L,1);
   a  = luaL_checknumber(L, 2);
   s  = luaL_checknumber(L, 3);
   st = luaL_optnumber(L,4,0.);

   a  /= 100.;
   s  /= 100.;
   st /= 100.;

   /* Set health. */
   p->armour = a * p->armour_max;
   p->shield = s * p->shield_max;
   p->stress = st * p->armour;

   /* Clear death hooks if not dead. */
   if (p->armour > 0.) {
      pilot_rmFlag( p, PILOT_DISABLED );
      pilot_rmFlag( p, PILOT_DEAD );
      pilot_rmFlag( p, PILOT_DEATH_SOUND );
      pilot_rmFlag( p, PILOT_EXPLODED );
      pilot_rmFlag( p, PILOT_DELETE );
      if (pilot_isPlayer(p))
         player_rmFlag( PLAYER_DESTROYED );
   }
   pilot_rmFlag( p, PILOT_DISABLED_PERM ); /* Remove permanent disable. */

   /* Update disable status. */
   pilot_updateDisable(p, 0);

   return 0;
}


/**
 * @brief Sets the energy of a pilot.
 *
 * @usage p:setEnergy( 100 ) -- Sets pilot to full energy.
 * @usage p:setEnergy(  70 ) -- Sets pilot to 70% energy.
 *
 *    @luatparam Pilot p Pilot to set energy of.
 *    @luatparam number energy Value to set energy to, should be double from 0-100 (in percent).
 *    @luatparam[opt=false] boolean absolute Whether or not it is being set in relative value or absolute.
 * @luafunc setEnergy
 */
static int pilotL_setEnergy(lua_State *L)
{
   Pilot *p;
   double e;
   int absolute;

   NLUA_CHECKRW(L);

   /* Handle parameters. */
   p      = luaL_validpilot(L,1);
   e      = luaL_checknumber(L,2);
   absolute = lua_toboolean(L,3);

   if (absolute)
      p->energy = CLAMP( 0, p->energy_max, e );
   else
      p->energy = (e/100.) * p->energy_max;

   return 0;
}


/**
 * @brief Fills up the pilot's ammo.
 *
 *    @luatparam Pilot p Pilot to fill ammo.
 * @luafunc fillAmmo
 */
static int pilotL_fillAmmo(lua_State *L)
{
   NLUA_CHECKRW(L);
   Pilot *p = luaL_validpilot(L,1);
   pilot_fillAmmo( p );
   return 0;
}


/**
 * @brief Sets the ability to board the pilot.
 *
 * No parameter is equivalent to true.
 *
 * @usage p:setNoBoard( true ) -- Pilot can not be boarded by anyone
 *
 *    @luatparam Pilot p Pilot to set disable boarding.
 *    @luatparam[opt=true] number noboard If true it disallows boarding of the pilot, otherwise
 *              it allows boarding which is the default.
 * @luafunc setNoBoard
 */
static int pilotL_setNoBoard(lua_State *L)
{
   Pilot *p;
   int enable;

   NLUA_CHECKRW(L);

   /* Handle parameters. */
   p = luaL_validpilot(L, 1);
   if (lua_isnone(L, 2))
      enable = 1;
   else
      enable = lua_toboolean(L, 2);

   /* See if should prevent boarding. */
   if (enable)
      pilot_setFlag(p, PILOT_NOBOARD);
   else
      pilot_rmFlag(p, PILOT_NOBOARD);

   return 0;
}


/**
 * @brief Sets the ability of the pilot to be disabled.
 *
 * No parameter is equivalent to true.
 *
 * @usage p:setNoDisable( true ) -- Pilot can not be disabled anymore.
 *
 *    @luatparam Pilot p Pilot to set disable disabling.
 *    @luatparam[opt=true] boolean disable If true it disallows disabled of the pilot, otherwise
 *              it allows disabling which is the default.
 * @luafunc setNoDisable
 */
static int pilotL_setNoDisable(lua_State *L)
{
   Pilot *p;
   int disable;

   NLUA_CHECKRW(L);

   /* Handle parameters. */
   p  = luaL_validpilot(L, 1);
   if (lua_isnone(L, 2))
      disable = 1;
   else
      disable = lua_toboolean(L, 2);

   /* See if should prevent disabling. */
   if (disable)
      pilot_setFlag(p, PILOT_NODISABLE);
   else
      pilot_rmFlag(p, PILOT_NODISABLE);

   return 0;
}


/**
 * @brief Limits the speed of a pilot.
 *
 * @note Can increase the pilot's speed limit over what would be physically possible.
 *
 * @usage p:setSpeedLimit( 100 ) -- Sets maximumspeed to 100px/s.
 * @usage p:setSpeedLimit( 0 ) removes speed limit.
 *    @luatparam pilot p Pilot to set speed of.
 *    @luatparam number speed Value to set speed to.
 *
 * @luafunc setSpeedLimit
 */
static int pilotL_setSpeedLimit(lua_State* L)
{

   Pilot *p;
   double s;

   NLUA_CHECKRW(L);

   /* Handle parameters. */
   p  = luaL_validpilot(L,1);
   s  = luaL_checknumber(L, 2);

   /* Limit the speed */
   p->speed_limit = s;
   if (s > 0.)
     pilot_setFlag( p, PILOT_HASSPEEDLIMIT );
   else
     pilot_rmFlag( p, PILOT_HASSPEEDLIMIT );

   pilot_updateMass(p);
   return 0;
}


/**
 * @brief Gets the pilot's health.
 *
 * @usage armour, shield, stress, dis = p:health()
 *
 *    @luatparam Pilot p Pilot to get health of.
 *    @luatparam[opt=false] boolean absolute Whether or not it shouldn't be relative and be absolute instead.
 *    @luatreturn number The armour in % [0:100] if relative or absolute value otherwise.
 *    @luatreturn number The shield in % [0:100] if relative or absolute value otherwise.
 *    @luatreturn number The stress in % [0:100].
 *    @luatreturn boolean Indicates if pilot is disabled.
 * @luafunc health
 */
static int pilotL_getHealth(lua_State *L)
{
   Pilot *p = luaL_validpilot(L,1);
   int absolute = lua_toboolean(L,2);

   /* Return parameters. */
   if (absolute) {
      lua_pushnumber(L, p->armour );
      lua_pushnumber(L, p->shield );
   }
   else {
      lua_pushnumber(L,(p->armour_max > 0.) ? p->armour / p->armour_max * 100. : 0. );
      lua_pushnumber(L,(p->shield_max > 0.) ? p->shield / p->shield_max * 100. : 0. );
   }
   lua_pushnumber(L, MIN( 1., p->stress / p->armour ) * 100. );
   lua_pushboolean(L, pilot_isDisabled(p));

   return 4;
}


/**
 * @brief Gets the pilot's energy.
 *
 * @usage energy = p:energy()
 *
 *    @luatparam Pilot p Pilot to get energy of.
 *    @luatparam[opt=false] boolean absolute Whether or not to return the absolute numeric value instead of the relative value.
 *    @luatreturn number The energy of the pilot in % [0:100].
 * @luafunc energy
 */
static int pilotL_getEnergy(lua_State *L)
{
   Pilot *p = luaL_validpilot(L,1);
   int absolute = lua_toboolean(L,2);

   if (absolute)
      lua_pushnumber(L, p->energy );
   else
      lua_pushnumber(L, (p->energy_max > 0.) ? p->energy / p->energy_max * 100. : 0. );

   return 1;
}


/**
 * @brief Gets the lockons on the pilot.
 *
 * @usage lockon = p:lockon()
 *
 *    @luatparam Pilot p Pilot to get lockons of.
 *    @luatreturn number The number of lockons on the pilot.
 * @luafunc lockon
 */
static int pilotL_getLockon(lua_State *L)
{
   Pilot *p;

   /* Get the pilot. */
   p  = luaL_validpilot(L,1);

   /* Return. */
   lua_pushnumber(L, p->lockons );
   return 1;
}


#define PUSH_DOUBLE( L, name, value ) \
lua_pushstring( L, name ); \
lua_pushnumber( L, value ); \
lua_rawset( L, -3 )
#define PUSH_INT( L, name, value ) \
lua_pushstring( L, name ); \
lua_pushinteger( L, value ); \
lua_rawset( L, -3 )
/**
 * @brief Gets stats of the pilot.
 *
 * Some of the stats are:<br />
 * <ul>
 *  <li> cpu </li>
 *  <li> cpu_max </li>
 *  <li> fuel </li>
 *  <li> fuel_max </li>
 *  <li> fuel_consumption </li>
 *  <li> mass </li>
 *  <li> thrust </li>
 *  <li> speed </li>
 *  <li> speed_max </li>
 *  <li> turn </li>
 *  <li> absorb </li>
 *  <li> armour </li>
 *  <li> shield </li>
 *  <li> energy </li>
 *  <li> armour_regen </li>
 *  <li> shield_regen </li>
 *  <li> energy_regen </li>
 *  <li> jump_delay </li>
 *  <li> jumps </li>
 * </ul>
 *
 * @usage stats = p:stats() print(stats.armour)
 *
 *    @luatparam Pilot p Pilot to get stats of.
 *    @luatreturn table A table containing the stats of p.
 * @luafunc stats
 */
static int pilotL_getStats(lua_State *L)
{
   Pilot *p;

   /* Get the pilot. */
   p  = luaL_validpilot(L,1);

   /* Create table with information. */
   lua_newtable(L);
   /* Core. */
   PUSH_INT(L, "cpu", p->cpu);
   PUSH_INT(L, "cpu_max", p->cpu_max);
   PUSH_DOUBLE(L, "fuel", p->fuel);
   PUSH_DOUBLE(L, "fuel_max", p->fuel_max);
   PUSH_DOUBLE(L, "fuel_consumption", p->fuel_consumption);
   PUSH_DOUBLE(L, "mass", p->solid->mass);
   /* Movement. */
   PUSH_DOUBLE( L, "thrust", p->thrust / p->solid->mass );
   PUSH_DOUBLE( L, "speed", p->speed );
   PUSH_DOUBLE( L, "turn", p->turn * 180. / M_PI ); /* Convert back to grad. */
   PUSH_DOUBLE( L, "speed_max", solid_maxspeed(p->solid, p->speed, p->thrust) );
   /* Health. */
   PUSH_DOUBLE( L, "absorb", p->dmg_absorb );
   PUSH_DOUBLE( L, "armour", p->armour_max );
   PUSH_DOUBLE( L, "shield", p->shield_max );
   PUSH_DOUBLE( L, "energy", p->energy_max );
   PUSH_DOUBLE( L, "armour_regen", p->armour_regen );
   PUSH_DOUBLE( L, "shield_regen", p->shield_regen );
   PUSH_DOUBLE( L, "energy_regen", p->energy_regen );
   /* Stats. */
   PUSH_DOUBLE( L, "dmg_absorb", p->dmg_absorb );
   PUSH_DOUBLE( L, "rdr_range", p->rdr_range );
   PUSH_DOUBLE( L, "rdr_jump_range", p->rdr_jump_range );
   PUSH_DOUBLE( L, "jump_delay", ntime_convertSeconds( pilot_hyperspaceDelay(p) ) );
   PUSH_INT( L, "jumps", pilot_getJumps(p) );

   return 1;
}
#undef PUSH_DOUBLE
#undef PUSH_INT


/**
 * @brief Gets a shipstat from a Pilot by name, or a table containing all the ship stats if not specified.
 *
 *    @luatparam Pilot p Pilot to get ship stat of.
 *    @luatparam[opt=nil] string name Name of the ship stat to get.
 *    @luatparam[opt=false] boolean internal Whether or not to use the internal representation.
 *    @luareturn Value of the ship stat or a tale containing all the ship stats if name is not specified.
 * @luafunc shipstat
 */
static int pilotL_getShipStat(lua_State *L)
{
   Pilot *p          = luaL_validpilot(L,1);
   const char *str   = luaL_optstring(L,2,NULL);
   int internal      = lua_toboolean(L,3);
   ss_statsGetLua( L, &p->stats, str, internal );
   return 1;
}


/**
 * @brief Gets the free cargo space the pilot has.
 *
 *    @luatparam Pilot p The pilot to get the free cargo space of.
 *    @luatreturn number The free cargo space in tonnes of the player.
 * @luafunc cargoFree
 */
static int pilotL_cargoFree(lua_State *L)
{
   Pilot *p;
   p = luaL_validpilot(L,1);

   lua_pushnumber(L, pilot_cargoFree(p) );
   return 1;
}


/**
 * @brief Checks to see how many tonnes of a specific type of cargo the pilot has.
 *
 *    @luatparam Pilot p The pilot to get the cargo count of.
 *    @luatparam Commodity|string cargo Type of cargo to check, either
 *       as a Commodity object or as the raw (untranslated) name of a
 *       commodity.
 *    @luatreturn number The amount of cargo the player has.
 * @luafunc cargoHas
 */
static int pilotL_cargoHas(lua_State *L)
{
   Pilot *p;
   const Commodity *cargo;
   int quantity;

   p = luaL_validpilot(L, 1);
   cargo = luaL_validcommodity(L, 2);
   quantity = pilot_cargoOwned(p, cargo);
   lua_pushnumber(L, quantity);
   return 1;
}


/**
 * @brief Tries to add cargo to the pilot's ship.
 *
 * @usage n = pilot.cargoAdd( player.pilot(), "Food", 20 )
 *
 *    @luatparam Pilot p The pilot to add cargo to.
 *    @luatparam Commodity|string cargo Type of cargo to add, either as
 *       a Commodity object or as the raw (untranslated) name of a
 *       commodity.
 *    @luatparam number quantity Quantity of cargo to add.
 *    @luatreturn number The quantity of cargo added.
 * @luasee misn.cargoAdd
 * @luafunc cargoAdd
 */
static int pilotL_cargoAdd(lua_State *L)
{
   Pilot *p;
   int quantity;
   Commodity *cargo;

   NLUA_CHECKRW(L);

   /* Parse parameters. */
   p = luaL_validpilot(L, 1);
   cargo = luaL_validcommodity(L, 2);
   quantity = luaL_checknumber(L, 3);

   if (quantity < 0) {
      NLUA_ERROR(L,
            _("Quantity must be positive for pilot.cargoAdd (if removing, use"
               " pilot.cargoRm)"));
      return 0;
   }

   /* Try to add the cargo. */
   quantity = pilot_cargoAdd(p, cargo, quantity, 0);
   lua_pushnumber(L, quantity);
   return 1;
}


/**
 * @brief Tries to remove cargo from the pilot's ship.
 *
 * @usage n = pilot.cargoRm(player.pilot(), "Food", 20)
 * @usage n = pilot.cargoRm(player.pilot(), "all") -- Removes all cargo from the player
 *
 *    @luatparam Pilot p The pilot to remove cargo from.
 *    @luatparam Commodity|string cargo Type of cargo to remove, either
 *       as a Commodity object or as the raw (untranslated) name of a
 *       commodity. You can also pass the special value "all" to remove
 *       all cargo from the pilot, except for mission cargo.
 *    @luatparam number quantity Quantity of the cargo to remove.
 *    @luatreturn number The number of cargo removed.
 * @luasee misn.cargoRm
 * @luafunc cargoRm
 */
static int pilotL_cargoRm(lua_State *L)
{
   Pilot *p;
   const char *str;
   int quantity;
   Commodity *cargo;

   NLUA_CHECKRW(L);

   /* Parse parameters. */
   p = luaL_validpilot(L, 1);

   if (lua_isstring(L, 2)) {
      str = lua_tostring(L, 2);

      /* Check for special strings. */
      if (strcmp(str, "all") == 0) {
         quantity = pilot_cargoRmAll(p, 0);
         lua_pushnumber(L, quantity);
         return 1;
      }
   }

   /* No special string handling, just handle as a normal commodity. */
   cargo = luaL_validcommodity(L, 2);
   quantity = luaL_checknumber(L, 3);

   if (quantity < 0) {
      NLUA_ERROR(L,
            _("Quantity must be positive for pilot.cargoRm (if adding, use"
               " pilot.cargoAdd)"));
      return 0;
   }

   /* Try to remove the cargo. */
   quantity = pilot_cargoRm(p, cargo, quantity);

   lua_pushnumber(L, quantity);
   return 1;
}


/**
 * @brief Lists the cargo the pilot has.
 *
 * The list has the following members:<br />
 * <ul>
 * <li><b>name:</b> translated name of the cargo (equivalent to the output of commodity.name()).</li>
 * <li><b>nameRaw:</b> raw (untranslated) name of the cargo (equivalent to the output of commodity.nameRaw()).</li>
 * <li><b>q:</b> quantity of the cargo.</li>
 * <li><b>m:</b> true if cargo is for a mission.</li>
 * </ul>
 *
 * @usage for i, v in ipairs(pilot.cargoList(player.pilot())) do print( string.format("%s: %d", v.name, v.q ) ) end
 *
 *    @luatparam Pilot p Pilot to list cargo of.
 *    @luatreturn table An ordered list with the names of the cargo the pilot has.
 * @luafunc cargoList
 */
static int pilotL_cargoList(lua_State *L)
{
   Pilot *p;
   int i;

   p = luaL_validpilot(L,1);
   lua_newtable(L); /* t */
   for (i=0; i<array_size(p->commodities); i++) {
      lua_pushnumber(L, i+1); /* t, i */

      /* Represents the cargo. */
      lua_newtable(L); /* t, i, t */
      lua_pushstring(L, "name"); /* t, i, t, i */
      lua_pushstring(L, _(p->commodities[i].commodity->name)); /* t, i, t, i, s */
      lua_rawset(L,-3); /* t, i, t */
      lua_pushstring(L, "nameRaw"); /* t, i, t, i */
      lua_pushstring(L, p->commodities[i].commodity->name); /* t, i, t, i, s */
      lua_rawset(L,-3); /* t, i, t */
      lua_pushstring(L, "q"); /* t, i, t, i */
      lua_pushnumber(L, p->commodities[i].quantity); /* t, i, t, i, s */
      lua_rawset(L,-3); /* t, i, t */
      lua_pushstring(L, "m"); /* t, i, t, i */
      lua_pushboolean(L, p->commodities[i].id); /* t, i, t, i, s */
      lua_rawset(L,-3); /* t, i, t */

      lua_rawset(L,-3); /* t */
   }
   return 1;

}


/**
 * @brief Gives the pilot an amount of credits.
 *
 * @usage p:pay(10000) -- Gives the pilot 10,000 credits
 *
 *    @luatparam Pilot p Pilot to give credits to.
 *    @luatparam number amount Amount of credits to give to the pilot.
 * @luasee player.pay
 * @luafunc pay
 */
static int pilotL_pay(lua_State *L)
{
   Pilot *p;
   credits_t amount;

   p = luaL_validpilot(L, 1);
   amount = CLAMP(CREDITS_MIN, CREDITS_MAX,
         (credits_t)round(luaL_checknumber(L, 2)));
   pilot_modCredits(p, amount);

   return 0;
}


/**
 * @brief Gets how many credits the pilot has.
 *
 *    @luatparam Pilot p Pilot to get the credits of.
 *    @luatreturn number The amount of credits the pilot has.
 * @luafunc credits
 */
static int pilotL_credits(lua_State *L)
{
   Pilot *p = luaL_validpilot(L, 1);
   lua_pushnumber(L, p->credits);
   return 1;
}


/**
 * @brief Gets the total value of the pilot's ship and equipped outfits.
 *
 *    @luatparam Pilot p Pilot to get the value of.
 *    @luatreturn number Total cost of the pilot's ship and equipped
 *       outfits in credits, excluding unique outfits.
 */
static int pilotL_value(lua_State *L)
{
   Pilot *p = luaL_validpilot(L, 1);
   lua_pushnumber(L, pilot_worth(p));
   return 1;
}


/**
 * @brief Gets the pilot's colour based on hostility or friendliness to the player.
 *
 * @usage p:colour()
 *
 *    @luatparam Pilot p Pilot to get the colour of.
 *    @luatreturn Colour The pilot's colour.
 * @luafunc colour
 */
static int pilotL_getColour(lua_State *L)
{
   Pilot *p;
   const glColour *col;

   /* Get the pilot. */
   p = luaL_validpilot(L,1);

   col = pilot_getColour(p);
   lua_pushcolour( L, *col );

   return 1;
}


/**
 * @brief Returns whether the pilot is hostile to the player.
 *
 * @usage p:hostile()
 *
 *    @luatparam Pilot p Pilot to get the hostility of.
 *    @luatreturn boolean The pilot's hostility status.
 * @luafunc hostile
 */
static int pilotL_getHostile(lua_State *L)
{
   Pilot *p;

   /* Get the pilot. */
   p = luaL_validpilot(L,1);

   /* Push value. */
   lua_pushboolean( L, pilot_isHostile( p ) );
   return 1;
}


/**
 * @brief Small struct to handle flags.
 */
struct pL_flag {
   char *name; /**< Name of the flag. */
   int id;     /**< Id of the flag. */
};
static const struct pL_flag pL_flags[] = {
   {.name = "carried", .id = PILOT_CARRIED},
   {.name = "hyperspace", .id = PILOT_HYPERSPACE},
   {.name = "hyperspace_end", .id = PILOT_HYP_END},
   {.name = "localjump", .id = PILOT_LOCALJUMP},
   {.name = "hailing", .id = PILOT_HAILING},
   {.name = "boardable", .id = PILOT_BOARDABLE},
   {.name = "boarded", .id = PILOT_BOARDED},
   {.name = "noboard", .id = PILOT_NOBOARD},
   {.name = "boarding", .id = PILOT_BOARDING},
   {.name = "nodisable", .id = PILOT_NODISABLE},
   {.name = "disabled", .id = PILOT_DISABLED},
   {.name = "disabled_perm", .id = PILOT_DISABLED_PERM},
   {.name = "nodeath", .id = PILOT_NODEATH},
   {.name = "invincible", .id = PILOT_INVINCIBLE},
   {.name = "invinc_player", .id = PILOT_INVINC_PLAYER},
   {.name = "hostile", .id = PILOT_HOSTILE},
   {.name = "friendly", .id = PILOT_FRIENDLY},
   {.name = "combat", .id = PILOT_COMBAT},
   {.name = "bribed", .id = PILOT_BRIBED},
   {.name = "distressed", .id = PILOT_DISTRESSED},
   {.name = "landing", .id = PILOT_LANDING},
   {.name = "takingoff", .id = PILOT_TAKEOFF},
   {.name = "norender", .id = PILOT_NORENDER},
   {.name = "visplayer", .id = PILOT_VISPLAYER},
   {.name = "visible", .id = PILOT_VISIBLE},
   {.name = "invisible", .id = PILOT_INVISIBLE},
   {.name = "hide", .id = PILOT_HIDE},
   {.name = "hilight", .id = PILOT_HILIGHT},
   {.name = "afterburner", .id = PILOT_AFTERBURNER},
   {.name = "refueling", .id = PILOT_REFUELING},
   {.name = "cooldown", .id = PILOT_COOLDOWN},
   {.name = "manualcontrol", .id = PILOT_MANUAL_CONTROL},
   {.name = "nojump", .id = PILOT_NOJUMP},
   {.name = "noland", .id = PILOT_NOLAND},
   {.name = "persist", .id = PILOT_PERSIST},
   {.name = "noclear", .id = PILOT_NOCLEAR},
   {NULL, -1}
}; /**< Flags to get. */
/**
 * @brief Gets the pilot's flags.
 *
 * Valid flags are:<br/>
 * <ul>
 *    <li>"carried": Pilot comes from a fighter bay.</li>
 *    <li>"hyperspace": Pilot is performing a jump. "hyperspace_end" and
 *       "localjump" can co-occur with this. If both "hyperspace_end"
 *       and "localjump" are false, "hyperspace" means that the pilot is
 *       entering hyperspace (exiting the system).</li>
 *    <li>"hyperspace_end": pilot is exiting hyperspace (entering the
 *       system).</li>
 *    <li>"localjump": Pilot is performing a local jump.</li>
 *    <li>"hailing": Pilot is hailing the player.</li>
 *    <li>"boardable": Pilot is boardable while active.</li>
 *    <li>"boarded": Pilot has been boarded already.</li>
 *    <li>"noboard": Pilot can't be boarded.</li>
 *    <li>"boarding": Pilot is currently boarding its target.</li>
 *    <li>"nodisable": Pilot can't be disabled.</li>
 *    <li>"disabled": Pilot is disabled.</li>
 *    <li>"disabled_perm": Pilot is permanently disabled.</li>
 *    <li>"nodeath": Pilot cannot die, will stay at 1 armor.</li>
 *    <li>"invincible": Pilot cannot be hit.</li>
 *    <li>"invinc_player": Pilot cannot be hit by the player.</li>
 *    <li>"hostile": Pilot is hostile toward the player.</li>
 *    <li>"friendly": Pilot is friendly toward the player.</li>
 *    <li>"combat": Pilot is engaged in combat.</li>
 *    <li>"bribed": Pilot has been bribed.</li>
 *    <li>"distressed": Pilot has distressed already.</li>
 *    <li>"landing": Pilot is currently landing.</li>
 *    <li>"takingoff": Pilot is currently taking off.</li>
 *    <li>"norender": Pilot does not get rendered.</li>
 *    <li>"visplayer": Pilot is always visible to the player.</li>
 *    <li>"visible": Pilot is always visible.</li>
 *    <li>"invisible": Pilot doesn't appear on the radar and cannot be
 *       targeted, but can still do stuff and is rendered.</li>
 *    <li>"hide": Pilot is not updated or rendered and cannot be 
 *       interacted with.</li>
 *    <li>"hilight": Pilot is hilighted on the map.</li>
 *    <li>"afterburner": Pilot has their afterburner activated.</li>
 *    <li>"refueling": Pilot is refueling another pilot.</li>
 *    <li>"cooldown": Pilot is in active cooldown mode.</li>
 *    <li>"manualcontrol": Pilot is under manual control.</li>
 *    <li>"nojump": Pilot cannot jump.</li>
 *    <li>"noland": Pilot cannot land.</li>
 *    <li>"persist": Pilot persists when the player jumps.</li>
 *    <li>"noclear": Pilot isn't removed by pilot.clear().</li>
 * </ul>
 *    @luatparam Pilot p Pilot to get flags of.
 *    @luatreturn table Table with flag names an index, boolean as value.
 * @luafunc flags
 */
static int pilotL_flags(lua_State *L)
{
   int i;
   Pilot *p;

   /* Get the pilot. */
   p = luaL_validpilot(L,1);

   /* Create flag table. */
   lua_newtable(L);
   for (i=0; pL_flags[i].name != NULL; i++) {
      lua_pushboolean( L, pilot_isFlag( p, pL_flags[i].id ) );
      lua_setfield(L, -2, pL_flags[i].name);
   }
   return 1;
}


/**
 * @brief Gets the pilot's ship.
 *
 * @usage s = p:ship()
 *
 *    @luatparam Pilot p Pilot to get ship of.
 *    @luatreturn Ship The ship of the pilot.
 * @luafunc ship
 */
static int pilotL_ship(lua_State *L)
{
   Pilot *p;

   /* Get the pilot. */
   p  = luaL_validpilot(L,1);

   /* Create the ship. */
   lua_pushship(L, p->ship);
   return 1;
}


/**
 * @brief Checks to see if the pilot is idle.
 *
 * @usage idle = p:idle() -- Returns true if the pilot is idle
 *
 *    @luatparam Pilot p Pilot to check to see if is idle.
 *    @luatreturn boolean true if pilot is idle, false otherwise
 * @luafunc idle
 */
static int pilotL_idle(lua_State *L)
{
   Pilot *p;

   /* Get the pilot. */
   p = luaL_validpilot(L,1);

   /* Check to see if is idle. */
   lua_pushboolean(L, p->task==0);
   return 1;
}


/**
 * @brief Sets manual control of the pilot.
 *
 * Note that this will reset the pilot's current task when the state changes.
 *
 * @usage p:control() -- Same as p:control(true), enables manual control of the pilot
 * @usage p:control(false) -- Restarts AI control of the pilot
 *
 *    @luatparam Pilot p Pilot to change manual control settings.
 *    @luatparam[opt=1] boolean enable If true or nil enables pilot manual control, otherwise enables automatic AI.
 * @luasee moveto
 * @luasee brake
 * @luasee follow
 * @luasee attack
 * @luasee runaway
 * @luasee hyperspace
 * @luasee land
 * @luafunc control
 */
static int pilotL_control(lua_State *L)
{
   Pilot *p;
   int enable, hasflag;

   NLUA_CHECKRW(L);

   /* Handle parameters. */
   p  = luaL_validpilot(L,1);
   enable = 1;
   if (lua_gettop(L) > 1)
      enable = lua_toboolean(L, 2);

   hasflag = pilot_isFlag(p, PILOT_MANUAL_CONTROL);
   if (enable) {
      pilot_setFlag(p, PILOT_MANUAL_CONTROL);
      if (pilot_isPlayer(p))
         ai_pinit( p, "player" );
   }
   else {
      pilot_rmFlag(p, PILOT_MANUAL_CONTROL);
      if (pilot_isPlayer(p))
         ai_destroy( p );
      /* Note, we do not set p->ai to NULL, we just clear the tasks and memory.
       * This is because the player always has an ai named "player", which is
       * used for manual control among other things. Basically a pilot always
       * has to have an AI even if it's the player for things to work. */
   }

   /* Clear task if changing state. */
   if (hasflag != enable)
      pilotL_taskclear( L );

   return 0;
}


/**
 * @brief Gets a pilots memory table.
 *
 * The resulting table is indexable and mutable.
 *
 * @usage aggr = p:memory().aggressive
 * @usage p:memory().aggressive = false
 *
 *    @luatparam Pilot p Pilot to read memory of.
 * @luafunc memory
 */
static int pilotL_memory(lua_State *L)
{
   Pilot *p;

   NLUA_CHECKRW(L);

   if (lua_gettop(L) < 1) {
      NLUA_ERROR(L, _("pilot.memory requires 1 argument!"));
      return 0;
   }

   /* Get the pilot. */
   p  = luaL_validpilot(L,1);

   /* Set the pilot's memory. */
   if (p->ai == NULL) {
      NLUA_ERROR(L,_("Pilot does not have AI."));
      return 0;
   }

   nlua_getenv( p->ai->env, AI_MEM );  /* pilotmem */
   lua_rawgeti( naevL, -1, p-> id );   /* pilotmem, table */
   lua_remove( naevL, -2 );            /* table */

   return 1;
}


/**
 * @brief Gets the name and data of a pilot's current task.
 *
 *    @luatparam Pilot p Pilot to get task data of.
 *    @luatreturn string Name of the task.
 *    @luareturn Data of the task.
 * @luafunc task
 */
static int pilotL_task(lua_State *L)
{
   Pilot *p = luaL_validpilot(L,1);
   Task *t  = ai_curTask(p);
   if (t) {
      lua_pushstring(L, t->name);
      if (t->dat != LUA_NOREF) {
         lua_rawgeti(L, LUA_REGISTRYINDEX, t->dat);
         return 2;
      }
      return 1;
   }
   return 0;
}


/**
 * @brief Gets the name of the task the pilot is currently doing.
 *
 *    @luatparam Pilot p Pilot to get task name of.
 *    @luatreturn string Name of the task.
 * @luafunc taskname
 */
static int pilotL_taskname(lua_State *L)
{
   Pilot *p = luaL_validpilot(L,1);
   Task *t  = ai_curTask(p);
   if (t) {
      lua_pushstring(L, t->name);
      return 1;
   }
   return 0;
}


/**
 * @brief Gets the data of the task the pilot is currently doing.
 *
 *    @luatparam Pilot p Pilot to get task data of.
 *    @luareturn Data of the task.
 * @luafunc taskdata
 */
static int pilotL_taskdata(lua_State *L)
{
   Pilot *p = luaL_validpilot(L,1);
   Task *t  = ai_curTask(p);
   if (t && (t->dat != LUA_NOREF)) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, t->dat);
      return 1;
   }
   return 0;
}


/**
 * @brief Clears all the tasks of the pilot.
 *
 * @usage p:taskClear()
 *
 *    @luatparam Pilot p Pilot to clear tasks of.
 * @luafunc taskClear
 */
static int pilotL_taskclear(lua_State *L)
{
   NLUA_CHECKRW(L);
   Pilot *p = luaL_validpilot(L,1);
   ai_cleartasks( p );
   return 0;
}


/**
 * @brief Does a new task.
 */
static Task *pilotL_newtask( lua_State *L, Pilot* p, const char *task )
{
   Task *t;

   /* Must be on manual control. */
   if (!pilot_isFlag( p, PILOT_MANUAL_CONTROL)) {
      NLUA_ERROR( L, _("Pilot is not on manual control.") );
      return 0;
   }

   /* Creates the new task. */
   t = ai_newtask( p, task, 0, 1 );

   return t;
}


/**
 * @brief Makes the pilot move to a position.
 *
 * Pilot must be under manual control for this to work.
 *
 * @usage p:moveto( v ) -- Goes to v precisely and braking
 * @usage p:moveto( v, true, true ) -- Same as p:moveto( v )
 * @usage p:moveto( v, false ) -- Goes to v without braking compensating velocity
 * @usage p:moveto( v, false, false ) -- Really rough approximation of going to v without braking
 *
 *    @luatparam Pilot p Pilot to tell to go to a position.
 *    @luatparam Vec2 v Vector target for the pilot.
 *    @luatparam[opt=1] boolean brake If true (or nil) brakes the pilot near target position,
 *              otherwise pops the task when it is about to brake.
 *    @luatparam[opt=1] boolean compensate If true (or nil) compensates for velocity, otherwise it
 *              doesn't. It only affects if brake is not set.
 * @luasee control
 * @luafunc moveto
 */
static int pilotL_moveto(lua_State *L)
{
   Pilot *p;
   Task *t;
   Vector2d *vec;
   int brake, compensate;
   const char *tsk;

   NLUA_CHECKRW(L);

   /* Get parameters. */
   p  = luaL_validpilot(L,1);
   vec = luaL_checkvector(L,2);
   if (lua_isnone(L,3))
      brake = 1;
   else
      brake = lua_toboolean(L,3);
   if (lua_isnone(L,4))
      compensate = 1;
   else
      compensate = lua_toboolean(L,4);

   /* Set the task. */
   if (brake) {
      tsk = "__moveto_precise";
   }
   else {
      if (compensate)
         tsk = "__moveto_nobrake";
      else
         tsk = "__moveto_nobrake_raw";
   }
   t        = pilotL_newtask( L, p, tsk );
   lua_pushvector( L, *vec );
   t->dat = luaL_ref(L, LUA_REGISTRYINDEX);

   return 0;
}


/**
 * @brief Makes the pilot face a target.
 *
 * @usage p:face( enemy_pilot ) -- Face enemy pilot
 * @usage p:face( vec2.new( 0, 0 ) ) -- Face origin
 * @usage p:face( enemy_pilot, true ) -- Task lasts until the enemy pilot is faced
 *
 *    @luatparam Pilot p Pilot to add task to.
 *    @luatparam Vec2|Pilot target Target to face.
 *    @luatparam[opt=false] boolean towards Makes the task end when the target is faced (otherwise it's an enduring state).
 * @luafunc face
 */
static int pilotL_face(lua_State *L)
{
   Pilot *p, *pt;
   Vector2d *vec;
   Task *t;
   int towards;

   NLUA_CHECKRW(L);

   /* Get parameters. */
   pt = NULL;
   vec = NULL;
   p  = luaL_validpilot(L,1);
   if (lua_ispilot(L,2))
      pt = luaL_validpilot(L,2);
   else
      vec = luaL_checkvector(L,2);
   if (lua_gettop(L) > 2)
      towards = lua_toboolean(L,3);
   else
      towards = 0;

   /* Set the task. */
   if (towards)
      t     = pilotL_newtask( L, p, "__face_towards" );
   else
      t     = pilotL_newtask( L, p, "__face" );
   if (pt != NULL) {
      lua_pushpilot(L, pt->id);
   }
   else {
      lua_pushvector(L, *vec);
   }
   t->dat = luaL_ref(naevL, LUA_REGISTRYINDEX);

   return 0;
}


/**
 * @brief Makes the pilot brake.
 *
 * Pilot must be under manual control for this to work.
 *
 *    @luatparam Pilot p Pilot to tell to brake.
 * @luasee control
 * @luafunc brake
 */
static int pilotL_brake(lua_State *L)
{
   Pilot *p;

   NLUA_CHECKRW(L);

   /* Get parameters. */
   p = luaL_validpilot(L,1);

   /* Set the task. */
   pilotL_newtask( L, p, "brake" );

   return 0;
}


/**
 * @brief Makes the pilot follow another pilot.
 *
 * Pilot must be under manual control for this to work.
 *
 *    @luatparam Pilot p Pilot to tell to follow another pilot.
 *    @luatparam Pilot pt Target pilot to follow.
 *    @luatparam[opt=false] boolean accurate If true, use a PD controller which
 *              parameters can be defined using the pilot's memory.
 * @luasee control
 * @luasee memory
 * @luafunc follow
 */
static int pilotL_follow(lua_State *L)
{
   Pilot *p, *pt;
   Task *t;
   int accurate;

   NLUA_CHECKRW(L);

   /* Get parameters. */
   p  = luaL_validpilot(L,1);
   pt = luaL_validpilot(L,2);

   if (lua_gettop(L) > 2)
      accurate = lua_toboolean(L,3);
   else
      accurate = 0;

   /* Set the task. */
   if (accurate == 0)
      t = pilotL_newtask( L, p, "follow" );
   else
      t = pilotL_newtask( L, p, "follow_accurate" );

   lua_pushpilot(L, pt->id);
   t->dat = luaL_ref(L, LUA_REGISTRYINDEX);

   return 0;
}


/**
 * @brief Makes the pilot attack another pilot.
 *
 * Pilot must be under manual control for this to work.
 *
 * @usage p:attack( another_pilot ) -- Attack another pilot
 * @usage p:attack() -- Attack nearest pilot.
 *
 *    @luatparam Pilot p Pilot to tell to attack another pilot.
 *    @luatparam[opt] Pilot pt Target pilot to attack (or nil to attack nearest enemy).
 * @luasee control
 * @luafunc attack
 */
static int pilotL_attack(lua_State *L)
{
   Pilot *p, *pt;
   Task *t;
   LuaPilot pid;

   NLUA_CHECKRW(L);

   /* Get parameters. */
   p = luaL_validpilot(L, 1);
   if (!lua_isnoneornil(L, 2)) {
      pt = luaL_validpilot(L, 2);
      pid = pt->id;
   }
   else {
      pid = pilot_getNearestEnemy(p);
      if (pid == 0) /* No enemy found. */
         return 0;
   }

   /* Set the task. */
   t = pilotL_newtask(L, p, "attack_forced");
   lua_pushpilot(L, pid);
   t->dat = luaL_ref(L, LUA_REGISTRYINDEX);

   return 0;
}


/**
 * @brief Makes the pilot runaway from another pilot.
 *
 * By default the pilot tries to jump when running away.
 *
 * @usage p:runaway( p_enemy ) -- Run away from p_enemy
 * @usage p:runaway( p_enemy, true ) -- Run away from p_enemy but do not jump
 *    @luatparam Pilot p Pilot to tell to runaway from another pilot.
 *    @luatparam Pilot tp Target pilot to runaway from.
 *    @luatparam[opt=false] boolean nojump Whether or not the pilot should try to jump when running away.
 * @luasee control
 * @luafunc runaway
 */
static int pilotL_runaway(lua_State *L)
{
   Pilot *p, *pt;
   Task *t;
   int nojump;

   NLUA_CHECKRW(L);

   /* Get parameters. */
   p = luaL_validpilot(L,1);
   pt = luaL_validpilot(L,2);
   nojump = lua_toboolean(L,3);

   /* Set the task. */
   t = pilotL_newtask(L, p, (nojump) ? "__runaway_nojump" : "__runaway");
   lua_pushpilot(L, pt->id);
   t->dat = luaL_ref(L, LUA_REGISTRYINDEX);

   return 0;
}


/**
 * @brief Makes the pilot gather stuff.
 *
 * @usage p:gather( ) -- Try to gather stuff
 * @luasee control
 * @luafunc gather
 */
static int pilotL_gather(lua_State *L)
{
   Pilot *p;
   Task *t;

   NLUA_CHECKRW(L);

   /* Get parameters. */
   p      = luaL_validpilot(L,1);

   /* Set the task. */
   t        = pilotL_newtask( L, p, "gather" );
   t->dat = luaL_ref(L, LUA_REGISTRYINDEX);

   return 0;
}


/**
 * @brief Makes the pilot perform an escape jump.
 *
 * Pilot must be under manual control for this to work.
 *
 * @usage p:localjump()
 *
 *    @luatparam Pilot p Pilot to tell to perform an escape jump.
 * @luasee control
 * @luafunc localjump
 */
static int pilotL_localjump(lua_State *L)
{
   Pilot *p;
   Task *t;

   p = luaL_validpilot(L, 1);

   /* Set the task. */
   t = pilotL_newtask(L, p, "localjump");
   t->dat = luaL_ref(L, LUA_REGISTRYINDEX);

   return 0;
}


/**
 * @brief Tells the pilot to hyperspace.
 *
 * Pilot must be under manual control for this to work.
 *
 *    @luatparam Pilot p Pilot to tell to hyperspace.
 *    @luatparam[opt] System sys Optional System to jump to, uses random if nil.
 *    @luatparam[opt] boolean shoot Whether or not to shoot at targets while running away with turrets.
 * @luasee control
 * @luafunc hyperspace
 */
static int pilotL_hyperspace(lua_State *L)
{
   Pilot *p;
   Task *t;
   StarSystem *ss;
   int i;
   JumpPoint *jp;
   LuaJump lj;
   int shoot;

   NLUA_CHECKRW(L);

   /* Get parameters. */
   p = luaL_validpilot(L,1);
   if ((lua_gettop(L) > 1) && !lua_isnil(L,2))
      ss = luaL_validsystem(L,2);
   else
      ss = NULL;
   shoot = lua_toboolean(L,3);

   /* Set the task. */
   if (shoot)
      t = pilotL_newtask( L, p, "__hyperspace_shoot" );
   else
      t = pilotL_newtask( L, p, "__hyperspace" );
   if (ss == NULL)
      return 0;
   /* Find the jump. */
   for (i=0; i < array_size(cur_system->jumps); i++) {
      jp = &cur_system->jumps[i];
      if (jp->target != ss)
         continue;
      /* Found target. */

      if (jp_isFlag( jp, JP_EXITONLY )) {
         NLUA_ERROR( L, _("Pilot '%s' can't jump out exit only jump '%s'"), p->name, ss->name );
         return 0;
      }

      /* Push jump. */
      lj.srcid  = cur_system->id;
      lj.destid = jp->targetid;
      lua_pushjump(L, lj);
      t->dat = luaL_ref(L, LUA_REGISTRYINDEX);
      return 0;
   }
   /* Not found. */
   NLUA_ERROR( L, _("System '%s' is not adjacent to current system '%s'"), ss->name, cur_system->name );
   return 0;
}


/**
 * @brief Tells the pilot to land
 *
 * Pilot must be under manual control for this to work.
 *
 *    @luatparam Pilot p Pilot to tell to land.
 *    @luatparam[opt] Planet planet Planet to land on, uses random if nil.
 *    @luatparam[opt] boolean shoot Whether or not to shoot at targets while running away with turrets.
 * @luasee control
 * @luafunc land
 */
static int pilotL_land(lua_State *L)
{
   Pilot *p;
   Task *t;
   Planet *pnt;
   int i;
   double a, r;
   Vector2d v;
   int shoot;

   NLUA_CHECKRW(L);

   /* Get parameters. */
   p = luaL_validpilot(L,1);
   if (lua_isnoneornil(L,2))
      pnt = NULL;
   else
      pnt = luaL_validplanet( L, 2 );
   shoot = lua_toboolean(L,3);

   /* Set the task. */
   if (shoot)
      t = pilotL_newtask( L, p, "__land_shoot" );
   else
      t = pilotL_newtask( L, p, "__land" );

   if (pnt != NULL) {
      /* Find the planet. */
      for (i=0; i < array_size(cur_system->planets); i++) {
         if (cur_system->planets[i] == pnt) {
            break;
         }
      }
      if (i >= array_size(cur_system->planets)) {
         NLUA_ERROR( L, _("Planet '%s' not found in system '%s'"), pnt->name, cur_system->name );
         return 0;
      }

      p->nav_planet = i;
      if (p->id == PLAYER_ID)
         gui_setNav();

      /* Copy vector. */
      v = pnt->pos;

      /* Introduce some error. */
      a = RNGF() * 2. * M_PI;
      r = RNGF() * pnt->radius;
      vect_cadd( &v, r*cos(a), r*sin(a) );

      lua_pushvector(L, v);
      t->dat = luaL_ref(L, LUA_REGISTRYINDEX);
   }

   return 0;
}


/**
 * @brief Marks the pilot as hailing the player.
 *
 * Automatically deactivated when pilot is hailed.
 *
 * @usage p:hailPlayer() -- Player will be informed he's being hailed and pilot will have an icon
 *    @luatparam Pilot p Pilot to hail the player.
 *    @luatparam[opt=true] boolean enable If true hails the pilot, if false disables the hailing.
 * @luafunc hailPlayer
 */
static int pilotL_hailPlayer(lua_State *L)
{
   Pilot *p;
   int enable;
   char c;

   NLUA_CHECKRW(L);

   /* Get parameters. */
   p = luaL_validpilot(L,1);
   if (lua_isnone(L,2))
      enable = 1;
   else
      enable = lua_toboolean(L,2);


   /* Set the flag. */
   if (enable) {
      /* Send message. */
      c = pilot_getFactionColourChar( p );
      player_message( _("#%c%s#0 is hailing you."), c, p->name );

      /* Set flag. */
      pilot_setFlag( p, PILOT_HAILING );
      player_hailStart();
   }
   else
      pilot_rmFlag( p, PILOT_HAILING );

   return 0;
}


/**
 * @brief Sends a message to another pilot.
 *
 * Do not confuse with pilot.comm! This is meant to be used by AI and other scripts.
 *
 *    @luatparam Pilot p Pilot to send message.
 *    @luatparam Pilot|{Pilot,...} receiver Pilot(s) to receive message.
 *    @luatparam string type Type of message.
 *    @luaparam[opt] data Data to send with message.
 * @luafunc msg
 */
static int pilotL_msg(lua_State *L)
{
   Pilot *p, *receiver=NULL;
   const char *type;
   unsigned int data;

   NLUA_CHECKRW(L);

   p = luaL_validpilot(L,1);
   type = luaL_checkstring(L,3);
   data = lua_gettop(L) > 3 ? 4 : 0;

   if (!lua_istable(L,2)) {
      receiver = luaL_validpilot(L,2);
      pilot_msg(p, receiver, type, data);
   } else {
      lua_pushnil(L);
      while (lua_next(L, 2) != 0) {
         receiver = luaL_validpilot(L,-1);
         pilot_msg(p, receiver, type, data);
         lua_pop(L, 1);
      }
      lua_pop(L, 1);
   }

   return 0;
}


/**
 * @brief Gets a pilots leader.
 *
 *    @luatparam Pilot p Pilot to get the leader of.
 *    @luatparam[opt=false] boolean recursive Whether or not to recurse
 *       through to find the ultimate leader of the pilot (which is not
 *       subordinate to another pilot).
 *    @luatreturn Pilot|nil The leader or nil.
 * @luafunc leader
 */
static int pilotL_leader(lua_State *L) {
   Pilot *p;
   Pilot *parent;
   LuaPilot pid;
   int recursive;

   p = luaL_validpilot(L, 1);
   recursive = 0;
   if (lua_gettop(L) > 1)
      recursive = lua_toboolean(L, 2);

   pid = p->parent;
   if ((pid != 0) && ((parent = pilot_get(pid)) != NULL)
         && !pilot_isFlag(parent, PILOT_DEAD)
         && !pilot_isFlag(parent, PILOT_HIDE)) {
      if (recursive) {
         while ((parent->parent != 0)
               && ((parent = pilot_get(parent->parent)) != NULL)
               && !pilot_isFlag(parent, PILOT_DEAD)
               && !pilot_isFlag(parent, PILOT_HIDE)) {
            pid = parent->parent;
         }
      }
      lua_pushpilot(L, pid);
   }
   else
      lua_pushnil(L);

   return 1;
}


/**
 * @brief Set a pilots leader.
 *
 * If leader has a leader itself, the leader will instead be set to that
 * pilot's leader.
 *
 *    @luatparam Pilot p Pilot to set the leader of.
 *    @luatparam Pilot|nil leader Pilot to set as leader.
 * @luafunc setLeader
 */
static int pilotL_setLeader(lua_State *L) {
   Pilot *p, *leader, *prev_leader;
   PilotOutfitSlot* dockslot;
   Pilot * const *pilot_stack;
   int i;

   NLUA_CHECKRW(L);

   pilot_stack = pilot_getAll();
   p = luaL_validpilot(L, 1);

   prev_leader = pilot_get(p->parent);

   if (lua_isnil(L, 2)) {
      p->parent = 0;
   } else {
      leader = luaL_validpilot(L, 2);

      if (leader->parent != 0 && pilot_get(leader->parent) != NULL)
         leader = pilot_get(leader->parent);

      p->parent = leader->id;

      /* Reset dock slot */
      dockslot = pilot_getDockSlot( p );
      if (dockslot != NULL)
      {
         dockslot->u.ammo.deployed--;
         p->dockpilot = 0;
         p->dockslot = -1;
      }

      /* TODO: Figure out escort type */
      escort_addList(leader, p->ship->name, ESCORT_TYPE_MERCENARY, p->id, 0);
   }

   /* Remove from previous leader's follower list */
   if (prev_leader != NULL)
      escort_rmList(prev_leader, p->id);

   /* If the pilot has followers, they should be given the new leader as well */
   for (i = 0; i<array_size(pilot_stack); i++) {
      if (pilot_stack[i]->parent == p->id) {
         pilot_stack[i]->parent = p->parent;
      }
   }

   return 0;
}


/**
 * @brief Get all of a pilots followers.
 *
 *    @luatparam Pilot p Pilot to get the followers of.
 *    @luatreturn {Pilot,...} Table of followers.
 * @luafunc followers
 */
static int pilotL_followers(lua_State *L)
{
   int i, idx;
   Pilot *pe;
   Pilot *p = luaL_validpilot(L, 1);

   lua_newtable(L);
   idx = 1;
   for (i=0; i < array_size(p->escorts); i++) {
      /* Make sure the followers are valid. */
      pe = pilot_get( p->escorts[i].id );
      if ((pe==NULL) || pilot_isFlag( pe, PILOT_DEAD ) || pilot_isFlag( pe, PILOT_HIDE ))
         continue;
      lua_pushnumber(L, idx++);
      lua_pushpilot(L, p->escorts[i].id);
      lua_rawset(L, -3);
   }

   return 1;
}


/**
 * @brief Clears the pilot's hooks.
 *
 * Clears all the hooks set on the pilot.
 *
 * @usage p:hookClear()
 *    @luatparam Pilot p Pilot to clear hooks.
 * @luafunc hookClear
 */
static int pilotL_hookClear(lua_State *L)
{
   Pilot *p;

   NLUA_CHECKRW(L);

   p = luaL_validpilot(L,1);
   pilot_clearHooks( p );

   return 0;
}
