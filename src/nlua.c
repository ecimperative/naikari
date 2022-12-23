/*
 * See Licensing and Copyright notice in naev.h
 */

/**
 * @file nlua.c
 *
 * @brief Handles creating and setting up basic Lua environments.
 */

/** @cond */
#include "physfs.h"

#include "naev.h"
/** @endcond */

#include "nlua.h"

#include "log.h"
#include "lutf8lib.h"
#include "ndata.h"
#include "nfile.h"
#include "nlua_cli.h"
#include "nlua_commodity.h"
#include "nlua_data.h"
#include "nlua_debug.h"
#include "nlua_diff.h"
#include "nlua_faction.h"
#include "nlua_file.h"
#include "nlua_jump.h"
#include "nlua_linopt.h"
#include "nlua_naev.h"
#include "nlua_news.h"
#include "nlua_outfit.h"
#include "nlua_pilot.h"
#include "nlua_planet.h"
#include "nlua_player.h"
#include "nlua_rnd.h"
#include "nlua_shiplog.h"
#include "nlua_system.h"
#include "nlua_time.h"
#include "nlua_var.h"
#include "nlua_vec2.h"
#include "nluadef.h"
#include "nstring.h"


lua_State *naevL = NULL;
nlua_env __NLUA_CURENV = LUA_NOREF;


/*
 * prototypes
 */
static int nlua_require( lua_State* L );
static lua_State *nlua_newState (void); /* creates a new state */
static int nlua_loadBasic( lua_State* L );
/* gettext */
static int nlua_gettext( lua_State *L );
static int nlua_ngettext( lua_State *L );
static int nlua_pgettext(lua_State *L);
static int nlua_gettext_noop( lua_State *L );
static const luaL_Reg gettext_methods[] = {
   {"gettext",  nlua_gettext},
   {"ngettext", nlua_ngettext},
   {"pgettext", nlua_pgettext},
   {"gettext_noop", nlua_gettext_noop},
   {0, 0}
}; /**< Vector metatable methods. */

/**
 * @brief gettext support.
 *
 * @usage _( str )
 *    @luatparam string msgid String to gettext on.
 *    @luatreturn string The string converted to gettext.
 * @luafunc gettext
 */
static int nlua_gettext( lua_State *L )
{
   const char *str;
   str = luaL_checkstring(L, 1);
   lua_pushstring(L, _(str));
   return 1;
}

/**
 * @brief gettext support for singular and plurals.
 *
 * @usage ngettext("%d apple", "%d apples", n)
 *    @luatparam string msgid1 Singular form.
 *    @luatparam string msgid2 Plural form.
 *    @luatparam number n Number of elements.
 *    @luatreturn The string converted to gettext.
 * @luafunc ngettext
 */
static int nlua_ngettext( lua_State *L )
{
   const char *stra, *strb;
   int n;
   stra = luaL_checkstring(L, 1);
   strb = luaL_checkstring(L, 2);
   n = luaL_checkinteger(L,3);
   lua_pushstring(L, n_(stra, strb, n));
   return 1;
}

/**
 * @brief gettext with context support
 *
 * @usage pgettext(context, message)
 *    @luatparam string msgctxt Context of the string.
 *    @luatparam string msgid English text to be translated.
 *    @luatreturn string The string converted with gettext.
 * @luafunc pgettext
 */
static int nlua_pgettext(lua_State *L)
{
   const char *stra, *strb;
   char *buf = NULL;

   stra = luaL_checkstring(L, 1);
   strb = luaL_checkstring(L, 2);
   asprintf(&buf, "%s" GETTEXT_CONTEXT_GLUE "%s", stra, strb);

   lua_pushstring(L, gettext_pgettext(buf, strb));
   free(buf);
   return 1;
}

/**
 * @brief gettext support (noop). Does not actually do anything, but gets detected by gettext.
 *
 * @usage N_( str )
 *    @luatparam string msgid String to gettext on.
 *    @luatreturn string The string converted to gettext.
 * @luafunc gettext_noop
 */
static int nlua_gettext_noop( lua_State *L )
{
   const char *str;
   str = luaL_checkstring(L, 1);
   lua_pushstring(L, str);
   return 1;
}


/**
 * @brief Implements the Lua function math.log2 (base-2 logarithm).
 */
static int nlua_log2( lua_State *L )
{
   double n;
   n = luaL_checknumber(L, 1);
   lua_pushnumber(L, log2(n));
   return 1;
}


/**
 * @brief Implements the Lua function os.getenv.
 *
 * In the sandbox we only make a fake $HOME visible.
 */
static int nlua_os_getenv( lua_State *L )
{
   const char *var;
   var = luaL_checkstring(L, 1);
   if (strcmp(var, "HOME") != 0)
      return 0;
   lua_pushstring(L, "lua_home");
   return 1;
}


/*
 * @brief Initializes the global Lua state.
 */
void lua_init(void) {
   naevL = nlua_newState();
   nlua_loadBasic(naevL);
}


/*
 * @brief Closes the global Lua state.
 */
void lua_exit(void) {
   lua_close(naevL);
   naevL = NULL;
}


/*
 * @brief Run code from buffer in Lua environment.
 *
 *    @param env Lua environment.
 *    @param buff Pointer to buffer.
 *    @param sz Size of buffer.
 *    @param name Name to use in error messages.
 */
int nlua_dobufenv(nlua_env env,
                  const char *buff,
                  size_t sz,
                  const char *name) {
   if (luaL_loadbuffer(naevL, buff, sz, name) != 0)
      return -1;
   nlua_pushenv(env);
   lua_setfenv(naevL, -2);
   if (nlua_pcall(env, 0, LUA_MULTRET) != 0)
      return -1;
   return 0;
}


/*
 * @brief Run code a file in Lua environment.
 *
 *    @param env Lua environment.
 *    @param filename Filename of Lua script.
 */
int nlua_dofileenv(nlua_env env, const char *filename) {
   if (luaL_loadfile(naevL, filename) != 0)
      return -1;
   nlua_pushenv(env);
   lua_setfenv(naevL, -2);
   if (nlua_pcall(env, 0, LUA_MULTRET) != 0)
      return -1;
   return 0;
}


/*
 * @brief Create an new environment in global Lua state.
 *
 * An "environment" is a table used with setfenv for sandboxing.
 *
 *    @param rw Load libraries in read/write mode.
 */
nlua_env nlua_newEnv(int rw) {
   char packagepath[STRMAX];
   nlua_env ref;
   lua_newtable(naevL);
   lua_pushvalue(naevL, -1);
   ref = luaL_ref(naevL, LUA_REGISTRYINDEX);

   /* Metatable */
   lua_newtable(naevL);
   lua_pushvalue(naevL, LUA_GLOBALSINDEX);
   lua_setfield(naevL, -2, "__index");
   lua_setmetatable(naevL, -2);

   /* Replace require() function with one that considers fenv */
   lua_pushvalue(naevL, -1);
   lua_pushcclosure(naevL, nlua_require, 1);
   lua_setfield(naevL, -2, "require");

   /* Set up paths.
    * "package.path" to look in the data.
    * "package.cpath" unset */
   lua_getglobal(naevL, "package");
   snprintf( packagepath, sizeof(packagepath),
         "?.lua;"LUA_INCLUDE_PATH"?.lua" );
   lua_pushstring(naevL, packagepath);
   lua_setfield(naevL, -2, "path");
   lua_pushstring(naevL, "");
   lua_setfield(naevL, -2, "cpath");
   lua_pop(naevL,1);

   /* Some code expect _G to be it's global state, so don't inherit it */
   lua_pushvalue(naevL, -1);
   lua_setfield(naevL, -2, "_G");

   /* Push whether or not the read/write functionality is used for the different libraries. */
   lua_pushboolean(naevL, rw);
   lua_setfield(naevL, -2, "__RW");

   /* Push if naev is built with debugging. */
#if DEBUGGING
   lua_pushboolean(naevL, 1);
   lua_setfield(naevL, -2, "__debugging");
#endif /* DEBUGGING */

   /* Set up naev namespace. */
   lua_newtable(naevL);
   lua_setfield(naevL, -2, "naev");

   lua_pop(naevL, 1);
   return ref;
}


/*
 * @brief Frees an environment created with nlua_newEnv()
 *
 *    @param env Enviornment to free.
 */
void nlua_freeEnv(nlua_env env) {
   if (naevL != NULL)
      luaL_unref(naevL, LUA_REGISTRYINDEX, env);
}


/*
 * @brief Push environment table to stack
 *
 *    @param env Environment.
 */
void nlua_pushenv(nlua_env env) {
   lua_rawgeti(naevL, LUA_REGISTRYINDEX, env);
}


/*
 * @brief Gets variable from enviornment and pushes it to stack
 *
 * This is meant to replace lua_getglobal()
 *
 *    @param env Environment.
 *    @param name Name of variable.
 */
void nlua_getenv(nlua_env env, const char *name) {
   nlua_pushenv(env);               /* env */
   lua_getfield(naevL, -1, name);   /* env, value */
   lua_remove(naevL, -2);           /* value */
}


/*
 * @brief Pops a value from the stack and sets it in the environment.
 *
 * This is meant to replace lua_setglobal()
 *
 *    @param env Environment.
 *    @param name Name of variable.
 */
void nlua_setenv(nlua_env env, const char *name) {
                                    /* value */
   nlua_pushenv(env);               /* value, env */
   lua_insert(naevL, -2);           /* env, value */
   lua_setfield(naevL, -2, name);   /* env */
   lua_pop(naevL, 1);               /*  */
}


/*
 * @brief Registers C functions as lua library in environment
 *
 * This is meant to replace luaL_register()
 *
 *    @param env Environment.
 *    @param libname Name of library table.
 *    @param l Array of functions to register.
 *    @param metatable Library will be used as metatable (so register __index).
 */
void nlua_register(nlua_env env, const char *libname,
                   const luaL_Reg *l, int metatable) {
   if (luaL_newmetatable(naevL, libname)) {
      if (metatable) {
         lua_pushvalue(naevL,-1);
         lua_setfield(naevL,-2,"__index");
      }
      luaL_register(naevL, NULL, l);
   }                                /* lib */
   nlua_getenv(env, "naev");        /* lib, naev */
   lua_pushvalue(naevL, -2);        /* lib, naev, lib */
   lua_setfield(naevL, -2, libname);/* lib, naev */
   lua_pop(naevL, 1);               /* lib  */
   nlua_setenv(env, libname);       /* */
}


/**
 * @brief Wrapper around luaL_newstate.
 *
 *    @return A newly created lua_State.
 */
static lua_State *nlua_newState (void)
{
   lua_State *L;

   /* try to create the new state */
   L = luaL_newstate();
   if (L == NULL) {
      WARN(_("Failed to create new Lua state."));
      return NULL;
   }

   return L;
}


/**
 * @brief Loads specially modified basic stuff.
 *
 *    @param L Lua State to load the basic stuff into.
 *    @return 0 on success.
 */
static int nlua_loadBasic( lua_State* L )
{
   luaL_openlibs(L);

   /* move [un]pack to table.[un]pack as in Lua5.2 */
   lua_getglobal(L, "table");    /* t */
   lua_getglobal(L, "unpack");   /* t, u */
   lua_setfield(L,-2,"unpack");  /* t */
   lua_getglobal(L, "pack");     /* t, u */
   lua_setfield(L,-2,"pack");    /* t */
   lua_pop(L,1);                 /* */
   lua_pushnil(L);               /* nil */
   lua_setglobal(L, "unpack");   /* */
   lua_pushnil(L);               /* nil */
   lua_setglobal(L, "pack");     /* */

   /* Override print to print in the console. */
   lua_register(L, "print", cli_print);
   lua_register(L, "warn",  cli_warn);
   lua_register(L, "debug_print", cli_debug);

   /* Gettext functionality. */
   lua_register(L, "_", nlua_gettext);
   lua_register(L, "N_", nlua_gettext_noop);
   lua_register(L, "n_", nlua_ngettext);
   lua_register(L, "p_", nlua_pgettext);
   luaL_register(L, "gettext", gettext_methods);

   /* Sandbox "io" and "os". */
   lua_newtable(L); /* io table */
   lua_setglobal(L,"io");
   lua_newtable(L); /* os table */
   lua_pushcfunction(L, nlua_os_getenv);
   lua_setfield(L,-2,"getenv");
   lua_setglobal(L,"os");

   /* Special math functions function. */
   lua_getglobal(L,"math");
   lua_pushcfunction(L, nlua_log2);
   lua_setfield(L,-2,"log2");
   lua_pushnil(L);
   lua_setfield(L,-2,"mod"); /* Get rid of math.mod */
   lua_pop(L,1);

   return 0;
}


/**
 * @brief include( string module )
 *
 * Loads a module into the current Lua state from inside the data file.
 *
 *    @param L Lua Environment to load modules into.
 *    @return The return value of the chunk, or true.
 */
static int nlua_require( lua_State* L )
{
   const char *filename;
   size_t bufsize, len_tried_paths;
   int envtab;
   char *buf, *q;
   char path_filename[PATH_MAX], tmpname[PATH_MAX], tried_paths[STRMAX];
   const char *packagepath, *start, *end;
   int i, done;

   /* Environment table to load module into */
   envtab = lua_upvalueindex(1);

   /* Get parameters. */
   filename = luaL_checkstring(L,1);

   /* Check to see if already included. */
   lua_getfield( L, envtab, NLUA_LOAD_TABLE );  /* t */
   if (!lua_isnil(L,-1)) {
      lua_getfield(L,-1,filename);              /* t, f */
      /* Already included. */
      if (!lua_isnil(L,-1)) {
         lua_remove(L, -2);                     /* val */
         return 1;
      }
      lua_pop(L,2);                             /* */
   }
   /* Must create new NLUA_LOAD_TABLE table. */
   else {
      lua_newtable(L);              /* t */
      lua_setfield(L, envtab, NLUA_LOAD_TABLE); /* */
   }

   /* Hardcoded libraries. */
   if (strcmp(filename,"utf8")==0) {
      luaopen_utf8(L);                          /* val */
      lua_getfield(L, envtab, NLUA_LOAD_TABLE); /* val, t */
      lua_pushvalue(L, -2);                     /* val, t, val */
      lua_setfield(L, -2, filename);            /* val, t */
      lua_pop(L, 1);                            /* val */
      return 1;
   }

   /* Get paths to check. */
   lua_getglobal(naevL, "package");
   if (!lua_istable(L,-1)) {
      lua_pop(L,2);
      NLUA_ERROR(L, _("require: package.path not found."));
   }
   lua_getfield(naevL, -1, "path");
   if (!lua_isstring(L,-1)) {
      lua_pop(L,2);
      NLUA_ERROR(L, _("require: package.path not found."));
   }
   packagepath = lua_tostring(L,-1);
   lua_pop(L,2);

   /* Parse path. */
   buf = NULL;
   done = 0;
   start = packagepath;
   tried_paths[0] = '\0';
   while (!done) {
      end = strchr( start, ';' );
      if (end == NULL) {
         done = 1;
         end = &start[ strlen(start) ];
      }
      strncpy( tmpname, start, end-start );
      tmpname[ end-start ] = '\0';
      q = strchr( tmpname, '?' );
      if (q==NULL) {
         snprintf( path_filename, sizeof(path_filename), "%s%s", tmpname, filename );
      }
      else {
         *q = '\0';
         snprintf( path_filename, sizeof(path_filename), "%s%s%s", tmpname, filename, q+1 );
      }
      start = end+1;

      /* Replace all '.' before the last '.' with '/' as they are a security risk. */
      q = strrchr( path_filename, '.' );
      for (i=0; i < q-path_filename; i++)
         if (path_filename[i]=='.')
            path_filename[i] = '/';

      /* Try to load the file. */
      if (PHYSFS_exists( path_filename )) {
         buf = ndata_read( path_filename, &bufsize );
         if (buf != NULL)
            break;
      }

      /* Didn't get to load it. */
      len_tried_paths = strlen( tried_paths );
      strncat( tried_paths, "\n   ", sizeof(tried_paths)-len_tried_paths-1 );
      strncat( tried_paths, path_filename, sizeof(tried_paths)-len_tried_paths-1 );
   }


   /* Must have buf by now. */
   if (buf == NULL) {
      NLUA_ERROR(L, _("require: %s not found in ndata.\nTried:%s"), filename, tried_paths);
      return 1;
   }

   /* Try to process the Lua. */
   if (luaL_loadbuffer(L, buf, bufsize, path_filename) != 0) {
      lua_error(L);
      return 1;
   }

   lua_pushvalue(L, envtab);
   lua_setfenv(L, -2);

   /* run the buffer */
   lua_pushstring(L, filename); /* pass name as first parameter */
#if 0
   if (lua_pcall(L, 1, 1, 0) != 0) {
      /* will push the current error from the dobuffer */
      lua_error(L);
      return 1;
   }
#endif
   lua_call(L, 1, 1);

   /* Mark as loaded. */
   /* val */
   if (lua_isnil(L,-1)) {
      lua_pop(L, 1);
      lua_pushboolean(L, 1);
   }
   lua_getfield(L, envtab, NLUA_LOAD_TABLE); /* val, t */
   lua_pushvalue(L, -2);                     /* val, t, val */
   lua_setfield(L, -2, filename);            /* val, t */
   lua_pop(L, 1);                            /* val */

   /* cleanup, success */
   free(buf);
   return 1;
}


/**
 * @brief Loads the standard Naev Lua API.
 *
 * Loads the modules:
 *  - naev
 *  - var
 *  - space
 *    - planet
 *    - system
 *    - jumps
 *  - time
 *  - player
 *  - pilot
 *  - rnd
 *  - diff
 *  - faction
 *  - vec2
 *  - outfit
 *  - commodity
 *
 * Only is missing:
 *  - misn
 *  - tk
 *  - hook
 *  - music
 *  - ai
 *
 *    @param env Environment.
 *    @return 0 on success.
 */
int nlua_loadStandard( nlua_env env )
{
   int r;

   r = 0;
   r |= nlua_loadNaev(env);
   r |= nlua_loadVar(env);
   r |= nlua_loadPlanet(env);
   r |= nlua_loadSystem(env);
   r |= nlua_loadJump(env);
   r |= nlua_loadTime(env);
   r |= nlua_loadPlayer(env);
   r |= nlua_loadPilot(env);
   r |= nlua_loadRnd(env);
   r |= nlua_loadDiff(env);
   r |= nlua_loadFaction(env);
   r |= nlua_loadVector(env);
   r |= nlua_loadOutfit(env);
   r |= nlua_loadCommodity(env);
   r |= nlua_loadNews(env);
   r |= nlua_loadShiplog(env);
   r |= nlua_loadFile(env);
   r |= nlua_loadData(env);
   r |= nlua_loadLinOpt(env);
   r |= nlua_loadDebug(env);

   return r;
}



/**
 * @brief Gets a trace from Lua.
 */
int nlua_errTrace( lua_State *L )
{
   /* Handle special done case. */
   const char *str = luaL_checkstring(L,1);
   if (strcmp(str,NLUA_DONE)==0)
      return 1;

   /* Otherwise execute "debug.traceback( str, int )". */
   lua_getglobal(L, "debug");
   if (!lua_istable(L, -1)) {
      lua_pop(L, 1);
      return 1;
   }
   lua_getfield(L, -1, "traceback");
   if (!lua_isfunction(L, -1)) {
      lua_pop(L, 2);
      return 1;
   }
   lua_pushvalue(L, 1);
   lua_pushinteger(L, 2);
   lua_call(L, 2, 1);
   return 1;
}


/*
 * @brief Wrapper around lua_pcall() that handles errors and enviornments
 *
 *    @param env Environment.
 *    @param nargs Number of arguments to pass.
 *    @param nresults Number of return values to take.
 */
int nlua_pcall( nlua_env env, int nargs, int nresults ) {
   int errf, ret, prev_env;

#if DEBUGGING
   int top = lua_gettop(naevL);
   lua_pushcfunction(naevL, nlua_errTrace);
   lua_insert(naevL, -2-nargs);
   errf = -2-nargs;
#else /* DEBUGGING */
   errf = 0;
#endif /* DEBUGGING */

   prev_env = __NLUA_CURENV;
   __NLUA_CURENV = env;

   ret = lua_pcall(naevL, nargs, nresults, errf);

   __NLUA_CURENV = prev_env;

#if DEBUGGING
   lua_remove(naevL, top-nargs);
#endif /* DEBUGGING */

   return ret;
}


/**
 * @brief Gets the reference of a global in a lua environment.
 *
 *    @param env Environment.
 *    @param name Name of the global to get.
 *    @return LUA_NOREF if no global found, reference otherwise.
 */
int nlua_refenv( nlua_env env, const char *name )
{
   nlua_getenv( env, name );
   if (!lua_isnil( naevL, -1 ))
      return luaL_ref( naevL, LUA_REGISTRYINDEX );
   lua_pop(naevL, 1);
   return LUA_NOREF;
}


/**
 * @brief Gets the reference of a global in a lua environment if it matches a type.
 *
 *    @param env Environment.
 *    @param name Name of the global to get.
 *    @param type Type to match, e.g., LUA_TFUNCTION.
 *    @return LUA_NOREF if no global found, reference otherwise.
 */
int nlua_refenvtype( nlua_env env, const char *name, int type )
{
   nlua_getenv( env, name );
   if (lua_type( naevL, -1 ) == type)
      return luaL_ref( naevL, LUA_REGISTRYINDEX );
   lua_pop(naevL, 1);
   return LUA_NOREF;
}
