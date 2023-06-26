/*
 * See Licensing and Copyright notice in naev.h
 */

/**
 * @file event.c
 *
 * @brief Handles internal events.
 *
 * Events are a lot like events except the player has no control over when
 *  or how they happen.  They can simple do something simple or actually lead up
 *  to and open an entire set of events.
 */


/** @cond */
#include "nstring.h"
#include <stdint.h>
#include <stdlib.h>

#include "naev.h"
/** @endcond */

#include "event.h"

#include "array.h"
#include "cond.h"
#include "hook.h"
#include "log.h"
#include "ndata.h"
#include "nlua.h"
#include "nlua_audio.h"
#include "nlua_bkg.h"
#include "nlua_camera.h"
#include "nlua_evt.h"
#include "nlua_hook.h"
#include "nlua_music.h"
#include "nlua_tex.h"
#include "nlua_tk.h"
#include "nluadef.h"
#include "npc.h"
#include "nxml.h"
#include "nxml_lua.h"
#include "player.h"
#include "rng.h"


#define XML_EVENT_ID          "Events" /**< XML document identifier */
#define XML_EVENT_TAG         "event" /**< XML event tag. */


#define EVENT_FLAG_UNIQUE     (1<<0) /**< Unique event. */


/**
 * @brief Event data structure.
 */
typedef struct EventData_ {
   char *name; /**< Name of the event. */
   char *sourcefile; /**< Source file code. */
   char *lua; /**< Lua code. */
   unsigned int flags; /**< Bit flags. */

   EventTrigger_t trigger; /**< What triggers the event. */
   char *cond; /**< Conditional Lua code to execute. */
   double chance; /**< Chance of appearing. */
   int priority; /**< Event priority: 0 = main plot, 50 = default, 100 = insignificant. */
} EventData;


/*
 * Event data.
 */
static EventData *event_data   = NULL; /**< Allocated event data. */


/*
 * Active events.
 */
static unsigned int event_genid  = 0; /**< Event ID generator. */
static Event_t *event_active     = NULL; /**< Active events. */


/*
 * Prototypes.
 */
static unsigned int event_genID (void);
static int event_cmp( const void* a, const void* b );
static int event_parseFile( const char* file );
static int event_parseXML( EventData *temp, const xmlNodePtr parent );
static void event_freeData( EventData *event );
static int event_create( int dataid, unsigned int *id );
int events_saveActive( xmlTextWriterPtr writer );
int events_loadActive( xmlNodePtr parent );
static int events_parseActive( xmlNodePtr parent );


/**
 * @brief Gets an event.
 */
Event_t *event_get( unsigned int eventid )
{
   int i;
   Event_t *ev;

   /* Iterate. */
   for (i=0; i<array_size(event_active); i++) {
      ev = &event_active[i];
      if (ev->id == eventid)
         return ev;
   }

   return NULL;
}


/**
 * @brief Starts an event.
 *
 *    @param name Name of the event to start.
 *    @param[out] id ID of the newly created event.
 *    @return 0 on success, <0 on error.
 */
int event_start( const char *name, unsigned int *id )
{
   int ret, edat;
   unsigned int eid;

   edat = event_dataID( name );
   if (edat < 0)
      return -1;
   eid  = 0;
   ret  = event_create( edat, &eid );

   if ((ret == 0) && (id != NULL))
      *id = eid;
   return ret;
}


/**
 * @brief Gets the name of the event data.
 *
 *    @param eventid Event to get name of data from.
 *    @return Name of data ev has.
 */
const char *event_getData( unsigned int eventid )
{
   Event_t *ev;

   ev = event_get( eventid );
   if (ev == NULL)
      return NULL;

   return event_data[ ev->data ].name;
}


/**
 * @brief Checks to see if an event is unique.
 *
 *    @param eventid ID of event to see if is unique.
 *    @return 0 if isn't unique, 1 if is.
 */
int event_isUnique( unsigned int eventid )
{
   Event_t *ev;

   ev = event_get( eventid );
   if (ev == NULL)
      return -1;

   return !!(event_data[ ev->data ].flags & EVENT_FLAG_UNIQUE);
}


/**
 * @brief Generates a new event ID.
 */
static unsigned int event_genID (void)
{
   unsigned int id;
   do {
      id = ++event_genid; /* Create unique ID. */
   } while (event_get(id) != NULL);
   return id;
}


/**
 * @brief Creates an event.
 *
 *    @param dataid Data to base event off of.
 *    @param id ID to use (0 to generate).
 *    @return 0 on success.
 */
static int event_create( int dataid, unsigned int *id )
{
   Event_t *ev;
   EventData *data;
   unsigned int eid;

   if (event_active==NULL)
      event_active = array_create( Event_t );

   /* Create the event. */
   ev = &array_grow( &event_active );
   memset( ev, 0, sizeof(Event_t) );
   if ((id != NULL) && (*id != 0))
      eid = *id;
   else
      eid = event_genID();
   ev->id = eid;

   /* Add the data. */
   ev->data = dataid;
   data = &event_data[dataid];

   /* Open the new state. */
   ev->env = nlua_newEnv(1);
   nlua_loadStandard(ev->env);
   nlua_loadEvt(ev->env);
   nlua_loadHook(ev->env);
   nlua_loadCamera(ev->env);
   nlua_loadTex(ev->env);
   nlua_loadBackground(ev->env);
   nlua_loadMusic(ev->env);
   nlua_loadAudio(ev->env);
   nlua_loadTk(ev->env);

   /* Load file. */
   if (nlua_dobufenv(ev->env, data->lua, strlen(data->lua), data->sourcefile) != 0) {
      WARN(_("Error loading event file: %s\n"
               "%s\n"
               "Most likely Lua file has improper syntax, please check"),
            data->sourcefile, lua_tostring(naevL, -1));
      return -1;
   }

   /* Run Lua. */
   if ((id==NULL) || (*id==0))
      event_run(eid, "create");

   /* Pass the id parameter, using eid in case event_active was changed
    * by the create function (which can invalidate the ev pointer). */
   if (id != NULL)
      *id = eid;

   return 0;
}


/**
 * @brief Cleans up an event.
 *
 *    @param ev Event to clean up.
 */
static void event_cleanup( Event_t *ev )
{
   /* Free lua env. */
   nlua_freeEnv(ev->env);

   /* Free hooks. */
   hook_rmEventParent(ev->id);

   /* Free NPC. */
   npc_rm_parentEvent(ev->id);

   /* Free claims. */
   if (ev->claims != NULL)
      claim_destroy( ev->claims );
}


/**
 * @brief Removes an event by ID.
 *
 *    @param eventid ID of the event to remove.
 */
void event_remove( unsigned int eventid )
{
   int i;
   Event_t *ev;

   /* Find the event. */
   for (i=0; i<array_size(event_active); i++) {
      ev = &event_active[i];
      if (ev->id == eventid) {
         /* Clean up event. */
         event_cleanup(ev);

         /* Move memory. */
         array_erase( &event_active, &event_active[i], &event_active[i+1] );
         return;
      }
   }

   WARN(_("Event ID '%u' not valid."), eventid);
}


/**
 * @brief Checks to see if an event should be saved.
 */
int event_save( unsigned int eventid )
{
   Event_t *ev;
   ev = event_get(eventid);
   if (ev == NULL)
      return 0;
   return ev->save;
}


/**
 * @brief Check to see if an event is already running.
 *
 *    @param data ID of data event to check if is already running.
 */
int event_alreadyRunning( int data )
{
   int i;
   Event_t *ev;

   /* Find events. */
   for (i=0; i<array_size(event_active); i++) {
      ev = &event_active[i];
      if (ev->data == data)
         return 1;
   }

   return 0;
}


/**
 * @brief Runs all the events matching a trigger.
 *
 *    @param trigger Trigger to match.
 */
void events_trigger( EventTrigger_t trigger )
{
   int i, c;
   int created;

   created = 0;
   for (i=0; i<array_size(event_data); i++) {
      /* Make sure trigger matches. */
      if (event_data[i].trigger != trigger)
         continue;

      /* Make sure chance is succeeded. */
      if (RNGF() > event_data[i].chance)
         continue;

      /* Test uniqueness. */
      if ((event_data[i].flags & EVENT_FLAG_UNIQUE) &&
            (player_eventAlreadyDone( i ) || event_alreadyRunning(i)))
         continue;

      /* Test conditional. */
      if (event_data[i].cond != NULL) {
         c = cond_check(event_data[i].cond);
         if (c<0) {
            WARN(_("Conditional for event '%s' failed to run."), event_data[i].name);
            continue;
         }
         else if (!c)
            continue;
      }

      /* Create the event. */
      event_create( i, NULL );
      created++;
   }

   /* Run claims if necessary. */
   if (created)
      claim_activateAll();
}


/**
 * @brief Loads up an event from an XML node.
 *
 *    @param temp Event to load up.
 *    @param parent Event parent node.
 *    @return 0 on success.
 */
static int event_parseXML( EventData *temp, const xmlNodePtr parent )
{
   xmlNodePtr node, cur;
   char *buf;

   memset( temp, 0, sizeof(EventData) );

   /* Defaults */
   temp->priority = 50;

   /* get the name */
   xmlr_attr_strd(parent, "name", temp->name);
   if (temp->name == NULL)
      WARN(_("Event in %s has invalid or no name"), EVENT_DATA_PATH);

   node = parent->xmlChildrenNode;

   do { /* load all the data */

      /* Only check nodes. */
      xml_onlyNodes(node);

      /* Trigger. */
      if (xml_isNode(node,"trigger")) {
         buf = xml_get(node);
         if (buf == NULL)
            WARN(_("Event '%s': Null trigger type."), temp->name);
         else if (strcmp(buf,"enter")==0)
            temp->trigger = EVENT_TRIGGER_ENTER;
         else if (strcmp(buf,"land")==0)
            temp->trigger = EVENT_TRIGGER_LAND;
         else if (strcmp(buf,"load")==0)
            temp->trigger = EVENT_TRIGGER_LOAD;
         else if (strcmp(buf,"none")==0)
            temp->trigger = EVENT_TRIGGER_NONE;
         else
            WARN(_("Event '%s' has invalid 'trigger' parameter: %s"), temp->name, buf);

         continue;
      }

      /* Flags. */
      else if (xml_isNode(node,"flags")) { /* set the various flags */
         cur = node->children;
         do {
            xml_onlyNodes(cur);
            if (xml_isNode(cur,"unique")) {
               temp->flags |= EVENT_FLAG_UNIQUE;
               continue;
            }
            WARN(_("Event '%s' has unknown flag node '%s'."), temp->name, cur->name);
         } while (xml_nextNode(cur));
         continue;
      }

      /* Notes for the python mission mapping script. */
      else if (xml_isNode(node,"notes")) continue;

      /* Condition. */
      xmlr_strd(node,"cond",temp->cond);

      /* Get chance. */
      xmlr_float(node,"chance",temp->chance);

      /* Get proirity. */
      xmlr_int(node,"priority",temp->priority);

      DEBUG(_("Unknown node '%s' in event '%s'"), node->name, temp->name);
   } while (xml_nextNode(node));

   /* Process. */
   temp->chance /= 100.;

#define MELEMENT(o,s) \
   if (o) WARN(_("Event '%s' missing/invalid '%s' element"), temp->name, s)
   MELEMENT((temp->trigger!=EVENT_TRIGGER_NONE) && (temp->chance==0.),"chance");
   MELEMENT(temp->trigger==EVENT_TRIGGER_NULL,"trigger");
#undef MELEMENT

   return 0;
}


static int event_cmp( const void* a, const void* b )
{
   const EventData *ea, *eb;
   ea = (const EventData*) a;
   eb = (const EventData*) b;
   if (ea->priority < eb->priority)
      return -1;
   else if (ea->priority > eb->priority)
      return +1;
   return strcmp( ea->name, eb->name );
}


/**
 * @brief Loads all the events.
 *
 *    @return 0 on success.
 */
int events_load (void)
{
   int    i;
   char **event_files;

   /* Run over events. */
   event_files = ndata_listRecursive( EVENT_DATA_PATH );
   event_data  = array_create_size( EventData, array_size( event_files ) );
   for ( i = 0; i < array_size( event_files ); i++ ) {
      if (!naev_pollQuit())
         event_parseFile(event_files[i]);
      free( event_files[ i ] );
   }
   array_free( event_files );
   array_shrink( &event_data );

   /* Sort based on priority so higher priority missions can establish claims first. */
   qsort( event_data, array_size(event_data), sizeof(EventData), event_cmp );

   DEBUG( n_("Loaded %d Event", "Loaded %d Events", array_size(event_data) ), array_size(event_data) );

   return 0;
}


/**
 * @brief Parses an event file.
 */
static int event_parseFile( const char* file )
{
   size_t bufsize;
   xmlNodePtr node;
   xmlDocPtr doc;
   char *filebuf;
   const char *pos, *start_pos;
   EventData *temp;

#ifdef DEBUGGING
   /* To check if event is valid. */
   int ret;
#endif /* DEBUGGING */

   /* Load string. */
   filebuf = ndata_read( file, &bufsize );
   if (filebuf == NULL) {
      WARN(_("Unable to read data from '%s'"), file);
      return -1;
   }

   /* Skip if no XML. */
   pos = strnstr( filebuf, "</event>", bufsize );
   if (pos==NULL) {
      pos = strnstr( filebuf, "function create", bufsize );
      if ((pos != NULL) && !strncmp(pos,"--common",bufsize))
         WARN(_("Event '%s' has create function but no XML header!"), file);
      free(filebuf);
      return 0;
   }

   /* Separate XML header and Lua. */
   start_pos = strnstr( filebuf, "<?xml ", bufsize );
   pos = strnstr( filebuf, "--]]", bufsize );
   if (pos == NULL || start_pos == NULL) {
      WARN(_("Event file '%s' has missing XML header!"), file);
      return -1;
   }

   /* Parse the header. */
   doc = xmlParseMemory( start_pos, pos-start_pos );
   if (doc == NULL) {
      WARN(_("Unable to parse document XML header for Event '%s'"), file);
      return -1;
   }

   /* Get the root node. */
   node = doc->xmlChildrenNode;
   if (!xml_isNode(node,XML_EVENT_TAG)) {
      WARN(_("Malformed '%s' file: missing root element '%s'"), file, XML_EVENT_TAG);
      return -1;
   }

   temp = &array_grow(&event_data);
   event_parseXML( temp, node );
   temp->lua = strdup(filebuf);
   temp->sourcefile = strdup(file);

#ifdef DEBUGGING
   /* Check to see if syntax is valid. */
   ret = luaL_loadbuffer(naevL, temp->lua, strlen(temp->lua), temp->name );
   if (ret == LUA_ERRSYNTAX) {
      WARN(_("Event Lua '%s' syntax error: %s"),
            file, lua_tostring(naevL,-1) );
   } else {
      lua_pop(naevL, 1);
   }
#endif /* DEBUGGING */

   /* Clean up. */
   xmlFreeDoc(doc);
   free(filebuf);

   return 0;
}


/**
 * @brief Frees an EventData structure.
 *
 *    @param event Event Data to free.
 */
static void event_freeData( EventData *event )
{
   free( event->name );
   free( event->lua );
   free( event->sourcefile );
   free( event->cond );
#if DEBUGGING
   memset( event, 0, sizeof(EventData) );
#endif /* DEBUGGING */
}


/**
 * @brief Cleans up and removes active events.
 */
void events_cleanup (void)
{
   int i;

   /* Free active events. */
   for (i=0; i<array_size(event_active); i++)
      event_cleanup( &event_active[i] );
   array_free(event_active);
   event_active = NULL;
}


/**
 * @brief Exits the event subsystem.
 */
void events_exit (void)
{
   int i;

   events_cleanup();

   /* Free data. */
   for (i=0; i<array_size(event_data); i++)
      event_freeData( &event_data[i] );
   array_free(event_data);
   event_data  = NULL;
}


/**
 * @brief Gets the event data id from name.
 *
 *    @param evdata Name of the data.
 *    @return ID matching dataname.
 */
int event_dataID( const char *evdata )
{
   int i;

   for (i=0; i<array_size(event_data); i++)
      if (strcmp(event_data[i].name, evdata)==0)
         return i;
   WARN(_("No event data found matching name '%s'."), evdata);
   return -1;
}


/**
 * @brief Gets the event data name from id.
 *
 *    @param dataid ID of the event data to get name of.
 *    @return Name of the event data.
 */
const char *event_dataName( int dataid )
{
   return event_data[dataid].name;
}


/**
 * @brief Activates all the active event claims.
 */
void event_activateClaims (void)
{
   int i;

   /* Free active events. */
   for (i=0; i<array_size(event_active); i++)
      if (event_active[i].claims != NULL)
         claim_activate( event_active[i].claims );
}


/**
 * @brief Tests to see if an event has claimed a system.
 */
int event_testClaims( unsigned int eventid, int sys )
{
   Event_t *ev;
   ev = event_get( eventid );
   if (ev==NULL) {
      WARN(_("Trying to test claims of unknown event with id '%d'!"), eventid);
      return 0;
   }
   return claim_testSys( ev->claims, sys );
}


/**
 * @brief Checks the event validity and cleans up after them.
 */
void event_checkValidity (void)
{
   int i;
   Event_t *ev;

   /* Iterate. */
   for (i=0; i<array_size(event_active); i++) {
      ev = &event_active[i];

      /* Check if has children. */
      if (hook_hasEventParent( ev->id ) > 0)
         continue;

      /* Must delete. */
      WARN(_("Detected event '%s' without any hooks and is therefore invalid. Removing event."),
            event_dataName( ev->data ));
      event_remove( ev->id );
      i--; /* Keep iteration safe. */
   }
}


/**
 * @brief Saves the player's active events.
 *
 *    @param writer XML Write to use to save events.
 *    @return 0 on success.
 */
int events_saveActive( xmlTextWriterPtr writer )
{
   int i;
   Event_t *ev;

   xmlw_startElem(writer,"events");

   for (i=0; i<array_size(event_active); i++) {
      ev = &event_active[i];
      if (!ev->save) /* Only save events that want to be saved. */
         continue;

      xmlw_startElem(writer,"event");

      xmlw_attr(writer,"name","%s",event_dataName(ev->data));
      xmlw_attr(writer,"id","%u",ev->id);

      /* Claims. */
      xmlw_startElem(writer,"claims");
      claim_xmlSave( writer, ev->claims );
      xmlw_endElem(writer); /* "claims" */

      /* Write Lua magic */
      xmlw_startElem(writer,"lua");
      nxml_persistLua( ev->env, writer );
      xmlw_endElem(writer); /* "lua" */

      xmlw_endElem(writer); /* "event" */
   }

   xmlw_endElem(writer); /* "events" */

   return 0;
}


/**
 * @brief Loads the player's active events from a save.
 *
 *    @param parent Node containing the player's active events.
 *    @return 0 on success.
 */
int events_loadActive( xmlNodePtr parent )
{
   xmlNodePtr node;

   /* cleanup old events */
   events_cleanup();

   node = parent->xmlChildrenNode;
   do {
      if (xml_isNode(node,"events"))
         if (events_parseActive( node ) < 0) return -1;
   } while (xml_nextNode(node));

   return 0;
}


/**
 * @brief Parses the actual individual event nodes.
 *
 *    @param parent Parent node to parse.
 *    @return 0 on success.
 */
static int events_parseActive( xmlNodePtr parent )
{
   char *buf;
   unsigned int id;
   int data;
   xmlNodePtr node, cur;
   Event_t *ev;

   node = parent->xmlChildrenNode;
   do {
      if (!xml_isNode(node,"event"))
         continue;

      xmlr_attr_strd(node,"name",buf);
      if (buf==NULL) {
         WARN(_("Event has missing 'name' attribute, skipping."));
         continue;
      }
      data = event_dataID( buf );
      if (data < 0) {
         WARN(_("Event in save has name '%s' but event data not found matching name. Skipping."), buf);
         free(buf);
         continue;
      }
      free(buf);
      xmlr_attr_uint(node,"id",id);
      if (id==0) {
         WARN(_("Event with data '%s' has invalid 'id' attribute, skipping."), event_dataName(data));
         continue;
      }

      /* Create the event. */
      event_create( data, &id );
      ev = event_get( id );
      if (ev == NULL) {
         WARN(_("Event with data '%s' was not created, skipping."), event_dataName(data));
         continue;
      }
      ev->save = 1; /* Should save by default again. */

      /* Get the data. */
      cur = node->xmlChildrenNode;
      do {
         if (xml_isNode(cur,"lua"))
            nxml_unpersistLua( ev->env, cur );
      } while (xml_nextNode(cur));

      /* Claims. */
      if (xml_isNode(node,"claims"))
         ev->claims = claim_xmlLoad( node );
   } while (xml_nextNode(node));

   return 0;
}


