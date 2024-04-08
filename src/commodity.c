/*
 * See Licensing and Copyright notice in naev.h
 */

/**
 * @file economy.c
 *
 * @brief Handles economy stuff.
 *
 * Economy is handled with Nodal Analysis.  Systems are modelled as nodes,
 *  jump routes are resistances and production is modelled as node intensity.
 *  This is then solved with linear algebra after each time increment.
 */


/** @cond */
#include <stdio.h>
#include <stdint.h>
#include "physfs.h"

#include "naev.h"
/** @endcond */

#include "commodity.h"

#include "array.h"
#include "economy.h"
#include "hook.h"
#include "log.h"
#include "ndata.h"
#include "nstring.h"
#include "ntime.h"
#include "nxml.h"
#include "pilot.h"
#include "player.h"
#include "rng.h"
#include "space.h"
#include "spfx.h"


#define XML_COMMODITY_ID  "commodity" /**< XML document identifier */


/* commodity stack */
Commodity* commodity_stack = NULL; /**< Contains all the commodities. */
static Commodity** commodity_temp = NULL; /**< Contains all the temporary commodities. */

/* gatherables stack */
static Gatherable* gatherable_stack = NULL; /**< Contains the gatherable stuff floating around. */
static float noscoop_timer = 1.; /**< Timer for the "full cargo" message . */

/* @TODO remove externs. */
extern int *econ_comm;


/*
 * Prototypes.
 */
/* Commodity. */
static void commodity_freeOne( Commodity* com );
static int commodity_parse( Commodity *temp, xmlNodePtr parent );


/**
 * @brief Gets a commodity by name.
 *
 *    @param name Name to match.
 *    @return Commodity matching name.
 */
Commodity* commodity_get( const char* name )
{
   int i;
   for (i=0; i<array_size(commodity_stack); i++)
      if (strcmp(commodity_stack[i].name,name)==0)
         return &commodity_stack[i];
   for (i=0; i<array_size(commodity_temp); i++)
      if (strcmp(commodity_temp[i]->name, name) == 0)
         return commodity_temp[i];

   WARN(_("Commodity '%s' not found in stack"), name);
   return NULL;
}


/**
 * @brief Gets a commodity by name without warning.
 *
 *    @param name Name to match.
 *    @return Commodity matching name.
 */
Commodity* commodity_getW( const char* name )
{
   int i;
   for (i=0; i<array_size(commodity_stack); i++)
      if (strcmp(commodity_stack[i].name, name) == 0)
         return &commodity_stack[i];
   for (i=0; i<array_size(commodity_temp); i++)
      if (strcmp(commodity_temp[i]->name, name) == 0)
         return commodity_temp[i];
   return NULL;
}


/**
 * @brief Frees a commodity.
 *
 *    @param com Commodity to free.
 */
static void commodity_freeOne( Commodity* com )
{
   CommodityModifier *this,*next;
   free(com->name);
   free(com->description);
   gl_freeTexture(com->gfx_store);
   gl_freeTexture(com->gfx_space);
   next = com->planet_modifier;
   com->planet_modifier = NULL;
   while (next != NULL ) {
      this = next;
      next = this->next;
      free(this->name);
      free(this);
   }
   next = com->faction_modifier;
   com->faction_modifier = NULL;
   while (next != NULL ) {
      this=next;
      next=this->next;
      free(this->name);
      free(this);
   }
   /* Clear the memory. */
   memset(com, 0, sizeof(Commodity));
}


/**
 * @brief Function meant for use with C89, C99 algorithm qsort().
 *
 *    @param commodity1 First argument to compare.
 *    @param commodity2 Second argument to compare.
 *    @return -1 if first argument is inferior, +1 if it's superior, 0 if ties.
 */
int commodity_compareTech( const void *commodity1, const void *commodity2 )
{
   const Commodity *c1, *c2;

   /* Get commodities. */
   c1 = * (const Commodity**) commodity1;
   c2 = * (const Commodity**) commodity2;

   /* Compare price. */
   if (c1->price < c2->price)
      return +1;
   else if (c1->price > c2->price)
      return -1;

   /* It turns out they're the same. */
   return strcmp( c1->name, c2->name );
}


/**
 * @brief Return an array (array.h) of standard commodities. Free with array_free. (Don't free contents.)
 */
Commodity ** standard_commodities (void)
{
   int i, n;
   Commodity *c, **com;

   n = array_size(commodity_stack);
   com = array_create_size( Commodity*, n );
   for (i=0; i<n; i++) {
      c = &commodity_stack[i];
      if (c->standard)
         array_push_back( &com, c );
   }
   return com;
}


#define MELEMENT(o, s) \
   if (o) WARN(_("Commodity '%s' missing '"s"' element"), temp->name)
/**
 * @brief Loads a commodity.
 *
 *    @param temp Commodity to load data into.
 *    @param parent XML node to load from.
 *    @return Commodity loaded from parent.
 */
static int commodity_parse( Commodity *temp, xmlNodePtr parent )
{
   xmlNodePtr node;
   CommodityModifier *newdict;
   /* Clear memory. */
   memset(temp, 0, sizeof(Commodity));
   temp->period = 200;
   temp->population_modifier = 0.;
   temp->standard = 0;

   /* Parse body. */
   node = parent->xmlChildrenNode;
   do {
      xml_onlyNodes(node);
      xmlr_strd(node, "name", temp->name);
      xmlr_strd(node, "description", temp->description);
      xmlr_int(node, "price", temp->price);
      if (xml_isNode(node,"gfx_space"))
         temp->gfx_space = xml_parseTexture( node,
               COMMODITY_GFX_PATH"space/%s", 1, 1, OPENGL_TEX_MIPMAPS );
      if (xml_isNode(node,"gfx_store")) {
         temp->gfx_store = xml_parseTexture(node,
               COMMODITY_GFX_PATH"%s", 1, 1, OPENGL_TEX_MIPMAPS);
         if (temp->gfx_store == NULL)
            temp->gfx_store = gl_newImage(COMMODITY_GFX_PATH"_default.webp", 0);
         continue;
      }
      if (xml_isNode(node, "standard")) {
         temp->standard = 1;
         continue;
      }
      xmlr_float(node, "population_modifier", temp->population_modifier);
      xmlr_float(node, "period", temp->period);
      if (xml_isNode(node, "planet_modifier")) {
         newdict = malloc(sizeof(CommodityModifier));
         newdict->next = temp->planet_modifier;
         xmlr_attr_strd(node, "type", newdict->name);
         newdict->value = xml_getFloat(node);
         temp->planet_modifier = newdict;
         continue;
      }
      if (xml_isNode(node, "faction_modifier")) {
         newdict = malloc(sizeof(CommodityModifier));
         newdict->next = temp->faction_modifier;
         xmlr_attr_strd(node, "type", newdict->name);
         newdict->value = xml_getFloat(node);
         temp->faction_modifier = newdict;
      }

   } while (xml_nextNode(node));
   if (temp->name == NULL)
      WARN( _("Commodity from %s has invalid or no name"), COMMODITY_DATA_PATH);
   if ((temp->price > 0)) {
      if (temp->gfx_store == NULL) {
         WARN(_("No <gfx_store> node found, using default texture for commodity \"%s\""), temp->name);
         temp->gfx_store = gl_newImage( COMMODITY_GFX_PATH"_default.webp", 0 );
      }
      if (temp->gfx_space == NULL)
         temp->gfx_space = gl_newImage( COMMODITY_GFX_PATH"space/_default.webp", 0 );
   }

   MELEMENT(temp->name == NULL, "name");
   MELEMENT(temp->description == NULL, "description");
   MELEMENT(temp->price == 0, "price");
   MELEMENT(temp->gfx_store == NULL, "gfx_store");

   return 0;
}
#undef MELEMENT


/**
 * @brief Throws cargo out in space graphically.
 *
 *    @param pilot ID of the pilot throwing the stuff out
 *    @param com Commodity to throw out.
 *    @param quantity Quantity thrown out.
 */
void commodity_Jettison( int pilot, const Commodity* com, int quantity )
{
   (void)com;
   int i;
   Pilot* p;
   int n, effect;
   double px,py, bvx, bvy, r,a, vx,vy;

   p   = pilot_get( pilot );

   n   = MAX( 1, RNG(quantity/10, quantity/5) );
   px  = p->solid->pos.x;
   py  = p->solid->pos.y;
   bvx = p->solid->vel.x;
   bvy = p->solid->vel.y;
   for (i=0; i<n; i++) {
      effect = spfx_get("cargo");

      /* Radial distribution gives much nicer results */
      r  = RNGF()*25 - 12.5;
      a  = 2. * M_PI * RNGF();
      vx = bvx + r*cos(a);
      vy = bvy + r*sin(a);

      /* Add the cargo effect */
      spfx_add( effect, px, py, vx, vy, SPFX_LAYER_BACK );
   }
}


/**
 * @brief Initializes a gatherable object
 *
 *    @param com Type of commodity.
 *    @param pos Position.
 *    @param vel Velocity.
 */
int gatherable_init( Commodity* com, Vector2d pos, Vector2d vel, double lifeleng, int qtt )
{
   Gatherable *g = &array_grow( &gatherable_stack );

   g->type = com;
   g->pos = pos;
   g->vel = vel;
   g->timer = 0.;
   g->quantity = qtt;

   if (lifeleng < 0.)
      g->lifeleng = RNGF()*100. + 50.;
   else
      g->lifeleng = lifeleng;

   return g-gatherable_stack;
}


/**
 * @brief Updates all gatherable objects
 *
 *    @param dt Elapsed time.
 */
void gatherable_update( double dt )
{
   int i;
   Gatherable *g;

   /* Update the timer for "full cargo" message. */
   noscoop_timer += dt;

   for (i=0; i < array_size(gatherable_stack); i++) {
      g = &gatherable_stack[i];
      g->timer += dt;
      g->pos.x += dt*gatherable_stack[i].vel.x;
      g->pos.y += dt*gatherable_stack[i].vel.y;

      /* Remove the gatherable */
      if (g->timer > g->lifeleng) {
         array_erase( &gatherable_stack, g, g+1 );
         i--;
      }
   }
}


/**
 * @brief Frees all the gatherables
 */
void gatherable_free( void )
{
   array_erase( &gatherable_stack, array_begin(gatherable_stack), array_end(gatherable_stack) );
}


/**
 * @brief Renders all the gatherables
 */
void gatherable_render( void )
{
   int i;
   Gatherable *gat;

   for (i=0; i < array_size(gatherable_stack); i++) {
      gat = &gatherable_stack[i];
      gl_blitSprite( gat->type->gfx_space, gat->pos.x, gat->pos.y, 0, 0, NULL );
   }
}


/**
 * @brief Gets the closest gatherable from a given position, within a given radius
 *
 *    @param pos position.
 *    @param rad radius.
 *    @return The id of the closest gatherable, or -1 if none is found.
 */
int gatherable_getClosest( Vector2d pos, double rad )
{
   int i, curg;
   Gatherable *gat;
   double mindist, curdist;

   curg = -1;
   mindist = INFINITY;

   for (i=0; i < array_size(gatherable_stack); i++) {
      gat = &gatherable_stack[i];
      curdist = vect_dist(&pos, &gat->pos);
      if ( (curdist<mindist) && (curdist<rad) ) {
         curg = i;
         mindist = curdist;
      }
   }
   return curg;
}


/**
 * @brief Returns the position and velocity of a gatherable
 *
 *    @param pos pointer to the position.
 *    @param vel pointer to the velocity.
 *    @param id Id of the gatherable in the stack.
 *    @return flag 1->there exists a gatherable 0->elsewere.
 */
int gatherable_getPos( Vector2d* pos, Vector2d* vel, int id )
{
   Gatherable *gat;

   if ((id < 0) || (id > array_size(gatherable_stack)-1) ) {
      vectnull( pos );
      vectnull( vel );
      return 0;
   }

   gat = &gatherable_stack[id];
   *pos = gat->pos;
   *vel = gat->vel;

   return 1;
}


/**
 * @brief See if the pilot can gather anything
 *
 *    @param pilot ID of the pilot
 */
void gatherable_gather( int pilot )
{
   int i, q;
   Gatherable *gat;
   Pilot* p;
   HookParam hparam[3];

   p = pilot_get(pilot);

   for (i=0; i < array_size(gatherable_stack); i++) {
      gat = &gatherable_stack[i];

      if (vect_dist(&p->solid->pos, &gat->pos)
            < p->ship->gfx_space->sw * PILOT_SIZE_APPROX) {
         /* Add cargo to pilot. */
         q = pilot_cargoAdd(p, gat->type, gat->quantity, 0);

         if (q > 0) {
            if (pilot_isPlayer(p)) {
               player_message(
                  n_("%d kt of %s gathered", "%d kt of %s gathered", q),
                  q, _(gat->type->name));

               /* Run hooks. */
               hparam[0].type    = HOOK_PARAM_STRING;
               hparam[0].u.str   = gat->type->name;
               hparam[1].type    = HOOK_PARAM_NUMBER;
               hparam[1].u.num   = q;
               hparam[2].type    = HOOK_PARAM_SENTINEL;
               hooks_runParam( "gather", hparam );
            }

            /* Remove the object from space. */
            array_erase( &gatherable_stack, gat, gat+1 );

            /* Test if there is still cargo space */
            if ((pilot_cargoFree(p) < 1) && (pilot_isPlayer(p)))
               player_message( _("No more cargo space available") );
         }
         else if ((pilot_isPlayer(p)) && (noscoop_timer > 2.)) {
            noscoop_timer = 0.;
            player_message( _("Cannot gather material: no more cargo space available") );
         }
      }
   }
}


/**
 * @brief Checks to see if a commodity is temporary.
 *
 *    @brief Name of the commodity to check.
 *    @return 1 if temorary, 0 otherwise.
 */
int commodity_isTemp( const char* name )
{
   int i;

   for (i=0; i<array_size(commodity_temp); i++)
      if (strcmp(commodity_temp[i]->name, name) == 0)
         return 1;
   for (i=0; i<array_size(commodity_stack); i++)
      if (strcmp(commodity_stack[i].name,name)==0)
         return 0;

   WARN(_("Commodity '%s' not found in stack"), name);
   return 0;
}


/**
 * @brief Creates a new temporary commodity.
 *
 *    @param name Name of the commodity to create.
 *    @param desc Description of the commodity to create.
 *    @return newly created commodity.
 */
Commodity* commodity_newTemp( const char* name, const char* desc )
{
   Commodity **c;
   if (commodity_temp == NULL)
      commodity_temp = array_create( Commodity* );

   c              = &array_grow(&commodity_temp);
   *c             = calloc( 1, sizeof(Commodity) );
   (*c)->istemp   = 1;
   (*c)->name     = strdup(name);
   (*c)->description = strdup(desc);
   return *c;
}


/**
 * @brief Loads all the commodity data.
 *
 *    @return 0 on success.
 */
int commodity_load (void)
{
   char **commodities = PHYSFS_enumerateFiles( COMMODITY_DATA_PATH );

   commodity_stack = array_create( Commodity );
   econ_comm = array_create( int );
   gatherable_stack = array_create( Gatherable );

   for (size_t i=0; commodities[i]!=NULL; i++) {
      if (naev_pollQuit())
         break;

      xmlNodePtr node;
      xmlDocPtr doc;
      Commodity *c;
      int *e;
      char *file;

      asprintf( &file, "%s%s", COMMODITY_DATA_PATH, commodities[i] );

      /* Load the file. */
      doc = xml_parsePhysFS( file );
      if (doc == NULL)
         return -1;

      node = doc->xmlChildrenNode; /* Commodities node */
      if (strcmp((char*)node->name,XML_COMMODITY_ID)) {
         ERR(_("Malformed %s file: missing root element '%s'"), COMMODITY_DATA_PATH, XML_COMMODITY_ID);
         return -1;
      }

      /* Load commodity. */
      c = &array_grow(&commodity_stack);
      commodity_parse( c, node );

      /* See if should get added to commodity list. */
      if (c->price > 0.) {
         e = &array_grow( &econ_comm );
         *e = array_size(commodity_stack)-1;
      }

      xmlFreeDoc(doc);
   }

   PHYSFS_freeList( commodities );

   DEBUG( n_( "Loaded %d Commodity", "Loaded %d Commodities", array_size(commodity_stack) ), array_size(commodity_stack) );

   return 0;
}


/**
 * @brief Frees all the loaded commodities.
 */
void commodity_free (void)
{
   int i;
   for (i=0; i<array_size(commodity_stack); i++)
      commodity_freeOne( &commodity_stack[i] );
   array_free( commodity_stack );
   commodity_stack = NULL;

   for (i=0; i<array_size(commodity_temp); i++) {
      commodity_freeOne( commodity_temp[i] );
      free( commodity_temp[i] );
   }
   array_free( commodity_temp );
   commodity_temp = NULL;

   /* More clean up. */
   array_free( econ_comm );
   econ_comm = NULL;

   array_free( gatherable_stack );
   gatherable_stack = NULL;
}

