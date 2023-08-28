/*
 * See Licensing and Copyright notice in naev.h
 */

/**
 * @file nlua_misn.c
 *
 * @brief Handles the mission Lua bindings.
 */


/** @cond */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "naev.h"
/** @endcond */

#include "nlua_misn.h"

#include "array.h"
#include "gui_osd.h"
#include "land.h"
#include "log.h"
#include "mission.h"
#include "music.h"
#include "ndata.h"
#include "nlua.h"
#include "nlua_audio.h"
#include "nlua_bkg.h"
#include "nlua_camera.h"
#include "nlua_commodity.h"
#include "nlua_faction.h"
#include "nlua_hook.h"
#include "nlua_music.h"
#include "nlua_planet.h"
#include "nlua_player.h"
#include "nlua_system.h"
#include "nlua_tex.h"
#include "nlua_tk.h"
#include "nluadef.h"
#include "npc.h"
#include "nstring.h"
#include "nxml.h"
#include "player.h"
#include "rng.h"
#include "shiplog.h"
#include "toolkit.h"


/**
 * @brief Mission Lua bindings.
 *
 * An example would be:
 * @code
 * misn.setNPC( "Keer", "empire/unique/keer.png", _("You see here Commodore Keer.") )
 * @endcode
 *
 * @luamod misn
 */


/*
 * prototypes
 */


/*
 * libraries
 */
/* Mission methods */
static int misn_setTitle( lua_State *L );
static int misn_setDesc( lua_State *L );
static int misn_setReward( lua_State *L );
static int misn_setNPC( lua_State *L );
static int misn_factions( lua_State *L );
static int misn_accept( lua_State *L );
static int misn_finish( lua_State *L );
static int misn_markerAdd( lua_State *L );
static int misn_markerMove( lua_State *L );
static int misn_markerRm( lua_State *L );
static int misn_cargoNew( lua_State *L );
static int misn_cargoAdd( lua_State *L );
static int misn_cargoRm( lua_State *L );
static int misn_cargoJet( lua_State *L );
static int misn_osdCreate( lua_State *L );
static int misn_osdDestroy( lua_State *L );
static int misn_osdActive( lua_State *L );
static int misn_osdGetActiveItem( lua_State *L );
static int misn_npcAdd( lua_State *L );
static int misn_npcRm( lua_State *L );
static int misn_claim( lua_State *L );
static const luaL_Reg misn_methods[] = {
   { "setTitle", misn_setTitle },
   { "setDesc", misn_setDesc },
   { "setReward", misn_setReward },
   { "setNPC", misn_setNPC },
   { "factions", misn_factions },
   { "accept", misn_accept },
   { "finish", misn_finish },
   { "markerAdd", misn_markerAdd },
   { "markerMove", misn_markerMove },
   { "markerRm", misn_markerRm },
   { "cargoNew", misn_cargoNew },
   { "cargoAdd", misn_cargoAdd },
   { "cargoRm", misn_cargoRm },
   { "cargoJet", misn_cargoJet },
   { "osdCreate", misn_osdCreate },
   { "osdDestroy", misn_osdDestroy },
   { "osdActive", misn_osdActive },
   { "osdGetActive", misn_osdGetActiveItem },
   { "npcAdd", misn_npcAdd },
   { "npcRm", misn_npcRm },
   { "claim", misn_claim },
   {0,0}
}; /**< Mission Lua methods. */


/**
 * @brief Registers all the mission libraries.
 *
 *    @param env Lua environment.
 *    @return 0 on success.
 */
int misn_loadLibs( nlua_env env )
{
   nlua_loadStandard(env);
   nlua_loadMisn(env);
   nlua_loadHook(env);
   nlua_loadCamera(env);
   nlua_loadTex(env);
   nlua_loadBackground(env);
   nlua_loadMusic(env);
   nlua_loadAudio(env);
   nlua_loadTk(env);
   return 0;
}
/*
 * individual library loading
 */
/**
 * @brief Loads the mission Lua library.
 *    @param env Lua environment.
 */
int nlua_loadMisn( nlua_env env )
{
   nlua_register(env, "misn", misn_methods, 0);
   return 0;
}


/**
 * @brief Tries to run a mission, but doesn't err if it fails.
 *
 *    @param misn Mission that owns the function.
 *    @param func Name of the function to call.
 *    @return -1 on error, 1 on misn.finish() call, 2 if mission got deleted
 *            and 0 normally.
 */
int misn_tryRun( Mission *misn, const char *func )
{
   int ret;

   /* Get the function to run. */
   misn_runStart( misn, func );
   if (lua_isnil( naevL, -1 )) {
      lua_pop(naevL,1);
      return 0;
   }
   ret = misn_runFunc( misn, func, 0 );
   return ret;
}


/**
 * @brief Runs a mission function.
 *
 *    @param misn Mission that owns the function.
 *    @param func Name of the function to call.
 *    @return -1 on error, 1 on misn.finish() call, 2 if mission got deleted
 *            and 0 normally.
 */
int misn_run( Mission *misn, const char *func )
{
   int ret;

   /* Run the function. */
   misn_runStart( misn, func );
   ret = misn_runFunc( misn, func, 0 );
   return ret;
}


/**
 * @brief Gets the mission that's being currently run in Lua.
 *
 * This should ONLY be called below an nlua_pcall, so __NLUA_CURENV is set
 */
Mission* misn_getFromLua( lua_State *L )
{
   Mission *misn, **misnptr;

   nlua_getenv(__NLUA_CURENV, "__misn");
   misnptr = lua_touserdata( L, -1 );
   misn = misnptr ? *misnptr : NULL;
   lua_pop( L, 1 );

   return misn;
}


/**
 * @brief Sets up the mission to run misn_runFunc.
 */
void misn_runStart( Mission *misn, const char *func )
{
   Mission **misnptr;
   misnptr = lua_newuserdata( naevL, sizeof(Mission*) );
   *misnptr = misn;
   nlua_setenv( misn->env, "__misn" );

   /* Set the Lua state. */
   nlua_getenv( misn->env, func );
}


/**
 * @brief Runs a mission set up with misn_runStart.
 *
 *    @param misn Mission that owns the function.
 *    @param func Name of the function to call.
 *    @param nargs Number of arguments to pass.
 *    @return -1 on error, 1 on misn.finish() call, 2 if mission got deleted,
 *          3 if the mission got accepted, and 0 normally.
 */
int misn_runFunc( Mission *misn, const char *func, int nargs )
{
   int i, ret;
   const char* err;
   int misn_delete;
   Mission *cur_mission;
   nlua_env env;
   int isaccepted;

   /* Check to see if it is accepted first. */
   isaccepted = misn->accepted;

   /* Set up and run function. */
   env = misn->env;
   ret = nlua_pcall(env, nargs, 0);

   /* The mission can change if accepted. */
   nlua_getenv(env, "__misn");
   cur_mission = *(Mission**) lua_touserdata(naevL, -1);
   lua_pop(naevL, 1);

   if (ret != 0) { /* error has occurred */
      err = (lua_isstring(naevL,-1)) ? lua_tostring(naevL,-1) : NULL;
      if ((err==NULL) || (strcmp(err,NLUA_DONE)!=0)) {
         WARN(_("Mission '%s' -> '%s': %s"),
               cur_mission->data->name, func, (err) ? err : _("unknown error"));
         ret = -1;
      }
      else
         ret = 1;
      lua_pop(naevL,1);
   }

   /* Get delete. */
   nlua_getenv(env, "__misn_delete");
   misn_delete = lua_toboolean(naevL,-1);
   lua_pop(naevL,1);

   /* Mission is finished */
   if (misn_delete) {
      ret = 2;
      mission_cleanup( cur_mission );
      for (i=0; i<MISSION_MAX; i++) {
         if (cur_mission != player_missions[i])
            continue;

         mission_shift(i);
         break;
      }
   }
   /* Mission became accepted. */
   else if (!isaccepted && cur_mission->accepted)
      ret = 3;

   return ret;
}


/**
 * @brief Sets the current mission title.
 *
 *    @luatparam string title Title to use for mission.
 * @luafunc setTitle
 */
static int misn_setTitle( lua_State *L )
{
   const char *str;
   Mission *cur_mission;

   str = luaL_checkstring(L,1);

   cur_mission = misn_getFromLua(L);
   free(cur_mission->title);
   cur_mission->title = strdup(str);

   return 0;
}
/**
 * @brief Sets the current mission description.
 *
 * Also sets the mission OSD unless you explicitly force an OSD, however you
 *  can't specify bullet points or other fancy things like with the real OSD.
 *
 *    @luatparam string desc Description to use for mission.
 * @luafunc setDesc
 */
static int misn_setDesc( lua_State *L )
{
   const char *str;
   Mission *cur_mission;

   str = luaL_checkstring(L,1);

   cur_mission = misn_getFromLua(L);
   free(cur_mission->desc);
   cur_mission->desc = strdup(str);

   return 0;
}
/**
 * @brief Sets the current mission reward description.
 *
 *    @luatparam string reward Description of the reward to use.
 * @luafunc setReward
 */
static int misn_setReward( lua_State *L )
{
   const char *str;
   Mission *cur_mission;

   str = luaL_checkstring(L,1);

   cur_mission = misn_getFromLua(L);
   free(cur_mission->reward);
   cur_mission->reward = strdup(str);
   return 0;
}

/**
 * @brief Adds a new marker.
 *
 * Mission markers are shown in the starmap and also automatically
 * hilight the next jump point to the marked system.
 *
 * @usage my_marker = misn.markerAdd( system.get("Gamma Polaris"), "low" )
 *
 * Valid marker types are:<br/>
 *  - "plot": Important plot marker.<br/>
 *  - "high": High importance mission marker (lower than plot).<br/>
 *  - "low": Low importance mission marker (lower than high).<br/>
 *  - "computer": Mission computer marker.<br/>
 *
 *    @luatparam System sys System to mark.
 *    @luatparam[opt="high"] string type Colouring scheme to use.
 *    @luatparam[opt] Planet|string pnt Planet or raw (untranslated)
 *       name of planet to hilight.
 *    @luatreturn number A marker ID to be used with markerMove and markerRm.
 * @luafunc markerAdd
 */
static int misn_markerAdd( lua_State *L )
{
   int id;
   LuaSystem sys;
   const Planet *planet;
   char *planetname;
   const char *stype;
   SysMarker type;
   Mission *cur_mission;

   /* Check parameters. */
   sys = luaL_checksystem(L, 1);
   stype = luaL_optstring(L, 2, "high");

   /* Handle types. */
   if (strcmp(stype, "computer")==0)
      type = SYSMARKER_COMPUTER;
   else if (strcmp(stype, "low")==0)
      type = SYSMARKER_LOW;
   else if (strcmp(stype, "high")==0)
      type = SYSMARKER_HIGH;
   else if (strcmp(stype, "plot")==0)
      type = SYSMARKER_PLOT;
   else {
      NLUA_ERROR(L, _("Unknown marker type: %s"), stype);
      return 0;
   }

   /* Get planet here to avoid memory leaks on error. */
   planetname = NULL;
   if (!lua_isnoneornil(L, 3)) {
      planet = luaL_validplanet(L, 3);
      planetname = strdup(planet->name);
   }

   cur_mission = misn_getFromLua(L);

   /* Add the marker. */
   id = mission_addMarker(cur_mission, -1, sys, planetname, type);

   /* Update system markers. */
   mission_sysMark();

   /* Return the ID. */
   lua_pushnumber( L, id );
   return 1;
}

/**
 * @brief Moves a marker to a new system.
 *
 * @usage misn.markerMove( my_marker, system.get("Delta Pavonis") )
 *
 *    @luatparam number id ID of the mission marker to move.
 *    @luatparam System sys System to move the marker to.
 *    @luatparam[opt] Planet|string pnt Planet or raw (untranslated)
 *       name of planet to hilight.
 * @luafunc markerMove
 */
static int misn_markerMove( lua_State *L )
{
   int id;
   LuaSystem sys;
   const Planet *planet;
   char *planetname;
   MissionMarker *marker;
   int i, n;
   Mission *cur_mission;

   /* Handle parameters. */
   id = luaL_checkinteger(L, 1);
   sys = luaL_checksystem(L, 2);

   cur_mission = misn_getFromLua(L);

   /* Check id. */
   marker = NULL;
   n = array_size( cur_mission->markers );
   for (i=0; i<n; i++) {
      if (id == cur_mission->markers[i].id) {
         marker = &cur_mission->markers[i];
         break;
      }
   }
   if (marker == NULL) {
      NLUA_ERROR( L, _("Mission does not have a marker with id '%d'"), id );
      return 0;
   }

   /* Get planet here to avoid memory leaks on error. */
   planetname = NULL;
   if (!lua_isnoneornil(L, 3)) {
      planet = luaL_validplanet(L, 3);
      planetname = strdup(planet->name);
   }

   /* Update system. */
   marker->sys = sys;
   free(marker->planet);
   marker->planet = planetname;

   /* Update system markers. */
   mission_sysMark();
   return 0;
}

/**
 * @brief Removes a mission system marker.
 *
 * @usage misn.markerRm( my_marker )
 *
 *    @luatparam number id ID of the marker to remove.
 * @luafunc markerRm
 */
static int misn_markerRm( lua_State *L )
{
   int id;
   int i, n;
   MissionMarker *marker;
   Mission *cur_mission;

   /* Handle parameters. */
   if (lua_isnil(L, 1))
      /* Allow safely passing nil with no effect. */
      return 0;
   id = luaL_checkinteger(L, 1);

   cur_mission = misn_getFromLua(L);

   /* Check id. */
   marker = NULL;
   n = array_size( cur_mission->markers );
   for (i=0; i<n; i++) {
      if (id == cur_mission->markers[i].id) {
         marker = &cur_mission->markers[i];
         break;
      }
   }
   if (marker == NULL) {
      /* Already removed. */
      return 0;
   }

   /* Remove the marker. */
   free(marker->planet);
   array_erase(&cur_mission->markers, marker, &marker[1]);

   /* Update system markers. */
   mission_sysMark();
   return 0;
}


/**
 * @brief Sets the current mission NPC.
 *
 * This is used in bar missions where you talk to a person. The portraits are
 *  the ones found in GFX_PATH/portraits. (For GFX_PATH/portraits/none.png
 *  you would use "none.png".)
 *
 * Note that this NPC will disappear when either misn.accept() or misn.finish()
 *  is called.
 *
 * @usage misn.setNPC( "Invisible Man", "none.png", _("You see a levitating mug drain itself.") )
 *
 *    @luatparam string name Name of the NPC.
 *    @luatparam string portrait File name of the portrait to use for the NPC.
 *    @luatparam string desc Description of the NPC to use.
 * @luafunc setNPC
 */
static int misn_setNPC( lua_State *L )
{
   char buf[PATH_MAX];
   const char *name, *str, *desc;
   Mission *cur_mission;

   cur_mission = misn_getFromLua(L);

   gl_freeTexture(cur_mission->portrait);
   cur_mission->portrait = NULL;

   free(cur_mission->npc);
   cur_mission->npc = NULL;

   free(cur_mission->npc_desc);
   cur_mission->npc_desc = NULL;

   /* For no parameters just leave having freed NPC. */
   if (lua_gettop(L) == 0)
      return 0;

   /* Get parameters. */
   name = luaL_checkstring(L,1);
   str  = luaL_checkstring(L,2);
   desc = luaL_checkstring(L,3);

   /* Set NPC name and description. */
   cur_mission->npc = strdup(name);
   cur_mission->npc_desc = strdup(desc);

   /* Set portrait. */
   snprintf( buf, sizeof(buf), GFX_PATH"portraits/%s", str );
   cur_mission->portrait = gl_newImage( buf, 0 );

   return 0;
}


/**
 * @brief Gets the factions the mission is available for.
 *
 * @usage f = misn.factions()
 *    @luatreturn {Faction,...} A table containing the factions for whom the mission is available.
 * @luafunc factions
 */
static int misn_factions( lua_State *L )
{
   int i;
   MissionData *dat;
   LuaFaction f;
   Mission *cur_mission;

   cur_mission = misn_getFromLua(L);
   dat = cur_mission->data;

   /* we'll push all the factions in table form */
   lua_newtable(L);
   for (i=0; i<array_size(dat->avail.factions); i++) {
      lua_pushnumber(L,i+1); /* index, starts with 1 */
      f = dat->avail.factions[i];
      lua_pushfaction(L, f); /* value */
      lua_rawset(L,-3); /* store the value in the table */
   }
   return 1;
}
/**
 * @brief Attempts to accept the mission.
 *
 * @usage if not misn.accept() then return end
 *    @luatreturn boolean true if mission was properly accepted.
 * @luafunc accept
 */
static int misn_accept( lua_State *L )
{
   int i, ret;
   Mission *cur_mission, **misnptr;

   ret = 0;

   /* find last mission */
   for (i=0; i<MISSION_MAX; i++)
      if (player_missions[i]->data == NULL)
         break;

   cur_mission = misn_getFromLua(L);

   /* no missions left */
   if (cur_mission->accepted)
      NLUA_ERROR(L, _("Mission already accepted!"));
   else if (i>=MISSION_MAX)
      ret = 1;
   else { /* copy it over */
      *player_missions[i] = *cur_mission;
      memset( cur_mission, 0, sizeof(Mission) );
      cur_mission = player_missions[i];
      cur_mission->accepted = 1; /* Mark as accepted. */

      /* Need to change pointer. */
      misnptr = lua_newuserdata( L, sizeof(Mission*) );
      *misnptr = cur_mission;
      nlua_setenv(cur_mission->env, "__misn");
   }

   lua_pushboolean(L,!ret); /* we'll convert C style return to Lua */
   return 1;
}
/**
 * @brief Finishes the mission.
 *
 *    @luatparam[opt] boolean properly If true and the mission is unique it marks the mission
 *                     as completed.  If false it deletes the mission but
 *                     doesn't mark it as completed.  If the parameter isn't
 *                     passed it just ends the mission (without removing it
 *                     from the player's list of active missions).
 * @luafunc finish
 */
static int misn_finish( lua_State *L )
{
   int b;
   Mission *cur_mission;

   if (lua_isboolean(L,1))
      b = lua_toboolean(L,1);
   else {
      lua_pushstring(L, NLUA_DONE);
      lua_error(L); /* THERE IS NO RETURN */
      return 0;
   }

   cur_mission = misn_getFromLua(L);

   lua_pushboolean( L, 1 );
   nlua_setenv(cur_mission->env, "__misn_delete");

   if (b && mis_isFlag(cur_mission->data,MISSION_UNIQUE))
      player_missionFinished( mission_getID( cur_mission->data->name ) );

   lua_pushstring(L, NLUA_DONE);
   lua_error(L); /* shouldn't return */

   return 0;
}


/**
 * @brief Creates a new temporary commodity meant for missions.
 *
 * Arguments that can be passed to the "params" parameter:<br/>
 * <ul>
 *    <li>"gfx_space" (string): The graphic to use for the commodity
 *       if it is in space as a result of being mined from an
 *       asteroid.</li>
 * </ul>
 * <br/>
 *
 *    @luatparam string cargo Raw (untranslated) name of the cargo to
 *       add. This must not match a cargo name defined in commodity.xml.
 *    @luatparam string decription Raw (untranslated) description of the
 *       cargo to add.
 *    @luatparam[opt=nil] table params Table of extra keyword arguments.
 *       See above for supported arguments.
 *    @luatreturn Commodity The newly created commodity or an existing temporary commodity with the same name.
 * @luafunc cargoNew
 */
static int misn_cargoNew( lua_State *L )
{
   const char *cname, *cdesc, *buf;
   char str[STRMAX_SHORT];
   Commodity *cargo;

   /* Parameters. */
   cname    = luaL_checkstring(L,1);
   cdesc    = luaL_checkstring(L,2);

   cargo    = commodity_getW(cname);
   if ((cargo != NULL) && !cargo->istemp)
      NLUA_ERROR(L,_("Trying to create new cargo '%s' that would shadow existing non-temporary cargo!"), cname);

   if (cargo==NULL)
      cargo = commodity_newTemp( cname, cdesc );

   if (!lua_isnoneornil(L,3)) {
      lua_getfield(L,3,"gfx_space");
      buf = luaL_optstring(L,-1,NULL);
      if (buf) {
         gl_freeTexture(cargo->gfx_space);
         snprintf( str, sizeof(str), COMMODITY_GFX_PATH"space/%s", buf );
         cargo->gfx_space = gl_newImage( str, 0 );
      }
   }

   lua_pushcommodity(L, cargo);
   return 1;
}
/**
 * @brief Adds some mission cargo to the player.
 * 
 * Mission cargo cannot be sold, and the player jettisoning the cargo
 * automatically aborts the mission. Mission cargo is also removed
 * automatically when the mission ends.
 *
 * @note You are responsible for ensuring that the player has enough
 *    cargo space for the mission cargo. If you attempt to add more
 *    cargo than the player has space for, the player's ship will become
 *    unspaceworthy until they either increase their cargo capacity,
 *    sell off some other cargo, or abort the mission. For most
 *    missions, you should first check the player's cargo capacity with
 *    pilot.cargoFree() to ensure there's enough space before adding
 *    mission cargo.
 *
 *    @luatparam Commodity|string cargo Type of cargo to add, either as
 *       a Commodity object or as the raw (untranslated) name of a
 *       commodity.
 *    @luatparam number quantity Quantity of cargo to add.
 *    @luatreturn number The id of the cargo which can be used in cargoRm.
 * @luasee cargoRm
 * @luasee cargoJet
 * @luasee pilot.cargoFree
 * @luasee pilot.cargoAdd
 * @luafunc cargoAdd
 */
static int misn_cargoAdd( lua_State *L )
{
   Commodity *cargo;
   int quantity, ret;
   Mission *cur_mission;

   /* Parameters. */
   cargo    = luaL_validcommodity(L,1);
   quantity = luaL_checkint(L,2);

   cur_mission = misn_getFromLua(L);

   /* First try to add the cargo. */
   ret = pilot_addMissionCargo( player.p, cargo, quantity );
   mission_linkCargo( cur_mission, ret );

   lua_pushnumber(L, ret);
   return 1;
}
/**
 * @brief Removes a mission cargo added by misn.cargoAdd().
 *
 *    @luatparam number cargoid Identifier of the mission cargo.
 *    @luatreturn boolean true on success.
 * @luasee cargoAdd
 * @luasee cargoJet
 * @luasee pilot.cargoRm
 * @luafunc cargoRm
 */
static int misn_cargoRm( lua_State *L )
{
   int ret;
   unsigned int id;
   Mission *cur_mission;

   id = luaL_checklong(L,1);

   /* First try to remove the cargo from player. */
   if (pilot_rmMissionCargo( player.p, id, 0 ) != 0) {
      lua_pushboolean(L,0);
      return 1;
   }

   cur_mission = misn_getFromLua(L);

   /* Now unlink the mission cargo if it was successful. */
   ret = mission_unlinkCargo( cur_mission, id );

   lua_pushboolean(L,!ret);
   return 1;
}
/**
 * @brief Jettisons a mission cargo added by misn.cargoAdd().
 *
 * This functions the same as misn.cargoRm(), except it spawns a graphic
 * showing the jettisoned cargo container if the player is in space.
 *
 *    @luatparam number cargoid ID of the cargo to jettison.
 *    @luatreturn boolean true on success.
 * @luasee cargoAdd
 * @luasee cargoRm
 * @luasee pilot.cargoRm
 * @luafunc cargoJet
 */
static int misn_cargoJet( lua_State *L )
{
   int ret;
   unsigned int id;
   Mission *cur_mission;

   id = luaL_checklong(L,1);

   /* First try to remove the cargo from player. */
   if (pilot_rmMissionCargo( player.p, id, 1 ) != 0) {
      lua_pushboolean(L,0);
      return 1;
   }

   cur_mission = misn_getFromLua(L);

   /* Now unlink the mission cargo if it was successful. */
   ret = mission_unlinkCargo( cur_mission, id );

   lua_pushboolean(L,!ret);
   return 1;
}


/**
 * @brief Creates a mission OSD.
 *
 * @note You can index elements by using '\\t' as first character of an element.
 * @note Destroys an osd if it already exists.
 *
 * @usage misn.osdCreate( "My OSD", {"Element 1", "Element 2"})
 *
 *    @luatparam string title Title to give the OSD.
 *    @luatparam {string,...} list List of elements to put in the OSD.
 * @luafunc osdCreate
 */
static int misn_osdCreate( lua_State *L )
{
   const char *title;
   int nitems;
   char **items;
   int i;
   Mission *cur_mission;

   cur_mission = misn_getFromLua(L);

   /* Must be accepted. */
   if (!cur_mission->accepted) {
      WARN(_("Can't create an OSD on an unaccepted mission!"));
      return 0;
   }

   /* Check parameters. */
   title  = luaL_checkstring(L,1);
   luaL_checktype(L,2,LUA_TTABLE);
   nitems = lua_objlen(L,2);

   /* Destroy OSD if already exists. */
   if (cur_mission->osd != 0) {
      osd_destroy( cur_mission->osd );
      cur_mission->osd = 0;
   }

   /* Allocate items. */
   items = calloc( nitems, sizeof(char *) );

   /* Get items. */
   for (i=0; i<nitems; i++) {
      lua_pushnumber(L,i+1);
      lua_gettable(L,2);
      if (!lua_isstring(L,-1)) {
         free(items);
         luaL_typerror(L, -1, "string");
         return 0;
      }
      items[i] = strdup( lua_tostring(L, -1) );
      lua_pop(L,1);
   }

   /* Create OSD. */
   cur_mission->osd = osd_create( title, nitems, (const char**) items,
         cur_mission->data->avail.priority );
   cur_mission->osd_set = 1; /* OSD was explicitly set. */

   /* Free items. */
   for (i=0; i<nitems; i++)
      free(items[i]);
   free(items);

   return 0;
}


/**
 * @brief Destroys the mission OSD.
 *
 * @luafunc osdDestroy
 */
static int misn_osdDestroy( lua_State *L )
{
   Mission *cur_mission;
   cur_mission = misn_getFromLua(L);

   if (cur_mission->osd != 0) {
      osd_destroy( cur_mission->osd );
      cur_mission->osd = 0;
   }

   return 0;
}


/**
 * @brief Sets active in mission OSD.
 *
 * @note Uses Lua indexes, so 1 is first member, 2 is second and so on.
 *
 *    @luatparam number n Element of the OSD to make active.
 * @luafunc osdActive
 */
static int misn_osdActive( lua_State *L )
{
   int n;
   Mission *cur_mission;

   n = luaL_checkint(L,1);
   n = n-1; /* Convert to C index. */

   cur_mission = misn_getFromLua(L);

   if (cur_mission->osd != 0)
      osd_active( cur_mission->osd, n );

   return 0;
}

/**
 * @brief Gets the active OSD element.
 *
 *    @luatreturn string|nil The full text of the active OSD element or
 *       nil if there is no OSD.
 * @luafunc osdGetActive
 */
static int misn_osdGetActiveItem( lua_State *L )
{
   Mission *cur_mission;
   cur_mission = misn_getFromLua(L);

   char **items = osd_getItems(cur_mission->osd);
   int active = osd_getActive(cur_mission->osd);

   if (!items || active < 0) {
      lua_pushnil(L);
      return 1;
   }

   lua_pushstring(L, items[active]);
   return 1;
}


/**
 * @brief Adds an NPC.
 *
 * @usage npc_id = misn.npcAdd( "my_func", "Mr. Test", "none.webp", "A test." ) -- Creates an NPC.
 *
 *    @luatparam string func Name of the function to run when
 *       approaching, gets passed the npc_id when called.
 *    @luatparam string name Name of the NPC
 *    @luatparam string portrait Portrait file name to use for the NPC
 *       (from GFX_PATH/portraits/).
 *    @luatparam string desc Description associated to the NPC.
 *    @luatparam[opt=50] number priority Optional priority argument
 *       (highest is 0, lowest is 100).
 *    @luatparam[opt=nil] string background Background file name to use (from GFX_PATH/portraits/).
 *    @luatreturn number The ID of the NPC to pass to npcRm.
 * @luafunc npcAdd
 */
static int misn_npcAdd( lua_State *L )
{
   unsigned int id;
   int priority;
   const char *func, *name, *gfx, *desc, *bg;
   char portrait[PATH_MAX], background[PATH_MAX];
   Mission *cur_mission;

   /* Handle parameters. */
   func = luaL_checkstring(L, 1);
   name = luaL_checkstring(L, 2);
   gfx  = luaL_checkstring(L, 3);
   desc = luaL_checkstring(L, 4);

   /* Optional parameters. */
   priority = luaL_optinteger(L, 5, 50);
   bg = luaL_optstring(L, 6, NULL);

   /* Set path. */
   ndata_getPathDefault( portrait, sizeof(portrait), GFX_PATH"portraits/", gfx );
   if (bg!=NULL)
      ndata_getPathDefault( background, sizeof(background), GFX_PATH"portraits/", bg );

   cur_mission = misn_getFromLua(L);

   /* Add npc. */
   id = npc_add_mission( cur_mission->id, func, name, priority, portrait, desc, (bg==NULL) ? bg :background );

   /* Regenerate bar. */
   bar_regen();

   /* Return ID. */
   if (id > 0) {
      lua_pushnumber( L, id );
      return 1;
   }
   return 0;
}


/**
 * @brief Removes an NPC.
 *
 * @usage misn.npcRm( npc_id )
 *
 *    @luatparam number id ID of the NPC to remove.
 * @luafunc npcRm
 */
static int misn_npcRm( lua_State *L )
{
   unsigned int id;
   int ret;
   Mission *cur_mission;

   id = luaL_checklong(L, 1);
   cur_mission = misn_getFromLua(L);
   ret = npc_rm_mission( id, cur_mission->id );

   /* Regenerate bar. */
   bar_regen();

   if (ret != 0)
      NLUA_ERROR(L, _("Invalid NPC ID!"));
   return 0;
}


/**
 * @brief Tries to claim systems or strings.
 *
 * Claiming systems and strings is a way to avoid mission collisions preemptively.
 *
 * Note it does not actually perform the claim if it fails to claim. It also
 *  does not work more than once.
 *
 * @usage if not misn.claim( { system.get("Gamma Polaris") } ) then misn.finish( false ) end
 * @usage if not misn.claim( system.get("Gamma Polaris") ) then misn.finish( false ) end
 * @usage if not misn.claim( 'some_string' ) then misn.finish( false ) end
 * @usage if not misn.claim( { system.get("Gamma Polaris"), 'some_string' } ) then misn.finish( false ) end
 *
 *    @luatparam System|String|{System,String...} params Table of systems/strings to claim or a single system/string.
 *    @luatparam[opt=false] boolean onlytest Whether or not to only test the claim, but not apply it.
 *    @luatreturn boolean true if was able to claim, false otherwise.
 * @luafunc claim
 */
static int misn_claim( lua_State *L )
{
   Claim_t *claim;
   Mission *cur_mission;

   /* Get mission. */
   cur_mission = misn_getFromLua(L);

   /* Check to see if already claimed. */
   if (!claim_isNull(cur_mission->claims)) {
      NLUA_ERROR(L, _("Mission trying to claim but already has."));
      return 0;
   }

   /* Create the claim. */
   claim = claim_create();

   if (lua_istable(L,1)) {
      /* Iterate over table. */
      lua_pushnil(L);
      while (lua_next(L, 1) != 0) {
         if (lua_issystem(L,-1))
            claim_addSys( claim, lua_tosystem( L, -1 ) );
         else if (lua_isstring(L,-1))
            claim_addStr( claim, lua_tostring( L, -1 ) );
         lua_pop(L,1);
      }
   }
   else if (lua_issystem(L, 1))
      claim_addSys( claim, lua_tosystem( L, 1 ) );
   else if (lua_isstring(L, 1))
      claim_addStr( claim, lua_tostring( L, 1 ) );
   else
      NLUA_INVALID_PARAMETER(L);

   /* Only test, but don't apply case. */
   if (lua_toboolean(L,2)) {
      lua_pushboolean( L, !claim_test( claim ) );
      claim_destroy( claim );
      return 1;
   }

   /* Test claim. */
   if (claim_test( claim )) {
      claim_destroy( claim );
      lua_pushboolean(L,0);
      return 1;
   }

   /* Set the claim. */
   cur_mission->claims = claim;
   claim_activate( claim );
   lua_pushboolean(L,1);
   return 1;
}

