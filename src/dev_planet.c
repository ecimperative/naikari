/*
 * See Licensing and Copyright notice in naev.h
 */

/**
 * @file dev_planet.c
 *
 * @brief Handles the planet stuff.
 */


/** @cond */
#include <stdlib.h>

#include "naev.h"
/** @endcond */

#include "dev_planet.h"

#include "conf.h"
#include "dev_uniedit.h"
#include "nfile.h"
#include "nstring.h"
#include "nxml.h"
#include "physics.h"


/**
 * @brief Saves a planet.
 *
 *    @param p Planet to save.
 *    @return 0 on success.
 */
int dpl_savePlanet( const Planet *p )
{
   xmlDocPtr doc;
   xmlTextWriterPtr writer;
   char *file, *cleanName;
   int i;
   int inhabited = 0;

   /* Create the writer. */
   writer = xmlNewTextWriterDoc(&doc, 0);
   if (writer == NULL) {
      WARN(_("testXmlwriterDoc: Error creating the xml writer"));
      return -1;
   }

   /* Set the writer parameters. */
   xmlw_setParams( writer );

   /* Start writer. */
   xmlw_start(writer);
   xmlw_startElem( writer, "asset" );

   /* Attributes. */
   xmlw_attr( writer, "name", "%s", p->name );

   /* Explicit virtualness. */
   if (p->real == ASSET_VIRTUAL)
      xmlw_elemEmpty( writer, "virtual" );

   /* Position. */
   if (p->real == ASSET_REAL) {
      xmlw_startElem( writer, "pos" );
      xmlw_elem( writer, "x", "%f", p->pos.x );
      xmlw_elem( writer, "y", "%f", p->pos.y );
      xmlw_endElem( writer ); /* "pos" */
   }

   /* GFX. */
   if (p->real == ASSET_REAL) {
      xmlw_startElem( writer, "GFX" );
      xmlw_elem( writer, "space", "%s", p->gfx_spacePath );
      xmlw_elem( writer, "exterior", "%s", p->gfx_exteriorPath );
      xmlw_endElem( writer ); /* "GFX" */
   }

   /* Presence. */
   if (faction_isFaction(p->faction)) {
      xmlw_startElem( writer, "presence" );
      xmlw_elem( writer, "faction", "%s", faction_name( p->faction ) );
      xmlw_elem( writer, "value", "%f", p->presenceAmount );
      xmlw_elem( writer, "range", "%d", p->presenceRange );
      xmlw_endElem( writer );
   }

   /* General. */
   if (p->real == ASSET_REAL) {
      xmlw_startElem( writer, "general" );
      xmlw_elem( writer, "class", "%s", p->class );
      xmlw_elem( writer, "population", "%"PRIu64, p->population );
      if (p->rdr_range_mod != 1.)
         xmlw_elem( writer, "rdr_range_mod", "%f", p->rdr_range_mod*100 - 100 );
      xmlw_startElem( writer, "services" );
      if (planet_hasService( p, PLANET_SERVICE_LAND )) {
         if (p->land_func == NULL)
            xmlw_elemEmpty( writer, "land" );
         else
            xmlw_elem( writer, "land", "%s", p->land_func );
      }
      if (planet_hasService( p, PLANET_SERVICE_REFUEL )) {
         xmlw_elemEmpty( writer, "refuel" );
         inhabited = 1;
      }
      if (planet_hasService( p, PLANET_SERVICE_BAR )) {
         xmlw_elemEmpty( writer, "bar" );
         inhabited = 1;
      }
      if (planet_hasService( p, PLANET_SERVICE_MISSIONS )) {
         xmlw_elemEmpty( writer, "missions" );
         inhabited = 1;
      }
      if (planet_hasService( p, PLANET_SERVICE_COMMODITY )) {
         xmlw_elemEmpty( writer, "commodity" );
         inhabited = 1;
      }
      if (planet_hasService( p, PLANET_SERVICE_OUTFITS )) {
         xmlw_elemEmpty( writer, "outfits" );
         inhabited = 1;
      }
      if (planet_hasService( p, PLANET_SERVICE_SHIPYARD )) {
         xmlw_elemEmpty( writer, "shipyard" );
         inhabited = 1;
      }
      if (planet_hasService( p, PLANET_SERVICE_BLACKMARKET ))
         xmlw_elemEmpty( writer, "blackmarket" );
      if (planet_isFlag( p, PLANET_NOMISNSPAWN ))
         xmlw_elemEmpty( writer, "nomissionspawn" );
      if (inhabited && (p->population == 0))
         xmlw_elemEmpty( writer, "uninhabited" );
      xmlw_endElem( writer ); /* "services" */
      if (planet_hasService( p, PLANET_SERVICE_LAND )) {
         if (faction_isFaction(p->faction)) {
            xmlw_startElem( writer, "commodities" );
            for (i=0; i<array_size(p->commodities); i++) {
               if (p->commodities[i]->standard == 0)
                  xmlw_elem( writer, "commodity", "%s", p->commodities[i]->name );
            }
            xmlw_endElem( writer ); /* "commodities" */
         }

         xmlw_elem( writer, "description", "%s", p->description );
         if (planet_hasService( p, PLANET_SERVICE_BAR ))
            xmlw_elem( writer, "bar", "%s", p->bar_description );
      }
      xmlw_endElem( writer ); /* "general" */
   }

   /* Tech. */
   if (planet_hasService( p, PLANET_SERVICE_LAND ))
      tech_groupWrite( writer, p->tech );

   xmlw_endElem( writer ); /** "planet" */
   xmlw_done( writer );

   /* No need for writer anymore. */
   xmlFreeTextWriter( writer );

   /* Write data. */
   cleanName = uniedit_nameFilter( p->name );
   asprintf( &file, "%s/%s.xml", conf.dev_save_asset, cleanName );
   if (xmlSaveFileEnc( file, doc, "UTF-8" ) < 0)
      WARN("Failed writing '%s'!", file);

   /* Clean up. */
   xmlFreeDoc(doc);
   free(cleanName);
   free(file);

   return 0;
}


/**
 * @brief Saves all the star planets.
 *
 *    @return 0 on success.
 */
int dpl_saveAll (void)
{
   int i;
   const Planet *p;

   p = planet_getAll();

   /* Write planets. */
   for (i=0; i<array_size(p); i++)
      dpl_savePlanet( &p[i] );

   return 0;
}


