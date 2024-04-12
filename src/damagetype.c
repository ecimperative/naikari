/*
 * See Licensing and Copyright notice in naev.h
 */

/**
 * @file damagetype.c
 *
 * @brief Handles damage types.
 */


/** @cond */
#include <inttypes.h>
#include "SDL.h"

#include "naev.h"
/** @endcond */

#include "damagetype.h"

#include "array.h"
#include "log.h"
#include "ndata.h"
#include "nxml.h"
#include "pause.h"
#include "pilot.h"
#include "rng.h"
#include "shipstats.h"


#define DTYPE_XML_ID     "dtypes"   /**< XML Document tag. */
#define DTYPE_XML_TAG    "dtype"    /**< DTYPE XML node tag. */

/**
 * @struct DTYPE
 *
 * @brief A damage type.
 */
typedef struct DTYPE_ {
   char* name;    /**< Name of the damage type */
   double sdam;   /**< Shield damage multiplier */
   double adam;   /**< Armour damage multiplier */
   double knock;  /**< Knockback */
   double recoil; /**< Recoil */
   size_t soffset; /**< Offset for shield modifier ship statistic. */
   size_t aoffset; /**< Offset for armour modifier ship statistic. */
} DTYPE;

static DTYPE* dtype_types  = NULL;  /**< Total damage types. */


/*
 * prototypes
 */
static int DTYPE_parse( DTYPE *temp, const xmlNodePtr parent );
static void DTYPE_free( DTYPE *damtype );
static DTYPE* dtype_validType( int type );


/**
 * @brief Parses an xml node containing a DTYPE.
 *
 *    @param temp Address to load DTYPE into.
 *    @param parent XML Node containing the DTYPE data.
 *    @return 0 on success.
 */
static int DTYPE_parse( DTYPE *temp, const xmlNodePtr parent )
{
   xmlNodePtr node;
   char *stat;

   /* Clear data. */
   memset( temp, 0, sizeof(DTYPE) );

   xmlr_attr_strd( parent, "name", temp->name );

   /* Extract the data. */
   node = parent->xmlChildrenNode;
   do {
      xml_onlyNodes(node);

      if (xml_isNode(node, "shield")) {
         temp->sdam = xml_getFloat(node);

         xmlr_attr_strd( node, "stat", stat );
         if (stat != NULL) {
            temp->soffset = ss_offsetFromType( ss_typeFromName(stat) );
            free(stat);
         }

         continue;
      }
      else if (xml_isNode(node, "armour")) {
         temp->adam = xml_getFloat(node);

         xmlr_attr_strd( node, "stat", stat );
         if (stat != NULL) {
            temp->aoffset = ss_offsetFromType( ss_typeFromName(stat) );
            free(stat);
         }

         continue;
      }
      xmlr_float(node, "armour", temp->adam);
      xmlr_float(node, "knockback", temp->knock);
      xmlr_float(node, "recoil", temp->recoil);

      WARN(_("Unknown node of type '%s' in damage node '%s'."), node->name, temp->name);
   } while (xml_nextNode(node));

#define MELEMENT(o,s) \
   if (o) WARN(_("DTYPE '%s' invalid '"s"' element"), temp->name) /**< Define to help check for data errors. */
   MELEMENT(temp->sdam<0.,"shield");
   MELEMENT(temp->adam<0.,"armour");
   MELEMENT(temp->knock<0.,"knockback");
   MELEMENT(temp->recoil<0., "recoil");
#undef MELEMENT

   return 0;
}


/**
 * @brief Frees a DTYPE.
 *
 *    @param damtype DTYPE to free.
 */
static void DTYPE_free( DTYPE *damtype )
{
   free(damtype->name);
   damtype->name = NULL;
}


/**
 * @brief Gets the id of a dtype based on name.
 *
 *    @param name Name to match.
 *    @return ID of the damage type or -1 on error.
 */
int dtype_get( char* name )
{
   int i;
   for (i=0; i<array_size(dtype_types); i++)
      if (strcmp(dtype_types[i].name, name)==0)
         return i;
   WARN(_("Damage type '%s' not found in stack."), name);
   return -1;
}


/**
 * @brief Gets the damage type.
 */
static DTYPE* dtype_validType( int type )
{
   if ((type < 0) || (type >= array_size(dtype_types))) {
      WARN(_("Damage type '%d' is invalid."), type);
      return NULL;
   }
   return &dtype_types[ type ];
}


/**
 * @brief Gets the human readable string from damage type.
 */
char* dtype_damageTypeToStr( int type )
{
   DTYPE *dmg = dtype_validType( type );
   if (dmg == NULL)
      return NULL;
   return dmg->name;
}


/**
 * @brief Loads the dtype stack.
 *
 *    @return 0 on success.
 */
int dtype_load (void)
{
   xmlNodePtr node;
   xmlDocPtr doc;

   /* Load and read the data. */
   doc = xml_parsePhysFS( DTYPE_DATA_PATH );
   if (doc == NULL)
      return -1;

   /* Check to see if document exists. */
   node = doc->xmlChildrenNode;
   if (!xml_isNode(node,DTYPE_XML_ID)) {
      ERR("Malformed '"DTYPE_DATA_PATH"' file: missing root element '"DTYPE_XML_ID"'");
      return -1;
   }

   /* Check to see if is populated. */
   node = node->xmlChildrenNode; /* first system node */
   if (node == NULL) {
      ERR("Malformed '"DTYPE_DATA_PATH"' file: does not contain elements");
      return -1;
   }

   /* Load up the individual damage types. */
   dtype_types = array_create(DTYPE);
   do {
      if (naev_pollQuit())
         break;

      xml_onlyNodes(node);

      if (!xml_isNode(node,DTYPE_XML_TAG)) {
         WARN("'"DTYPE_DATA_PATH"' has unknown node '%s'.", node->name);
         continue;
      }

      DTYPE_parse( &array_grow( &dtype_types ), node );

   } while (xml_nextNode(node));
   /* Shrink back to minimum - shouldn't change ever. */
   array_shrink( &dtype_types );

   /* Clean up. */
   xmlFreeDoc(doc);

   return 0;
}


/**
 * @brief Frees the dtype stack.
 */
void dtype_free (void)
{
   int i;

   /* clear the damtypes */
   for (i=0; i<array_size(dtype_types); i++)
      DTYPE_free( &dtype_types[i] );
   array_free( dtype_types );
   dtype_types    = NULL;
}


/**
 * @brief Gets the raw modulation stats of a damage type.
 *
 *    @param type Type to get stats of.
 *    @param[out] shield Shield damage modulator.
 *    @param[out] armour Armour damage modulator.
 *    @param[out] knockback Knockback modulator.
 *    @param[out] recoil Recoil modulator.
 *    @return 0 on success.
 */
int dtype_raw(int type, double *shield, double *armour, double *knockback,
      double *recoil)
{
   DTYPE *dtype = dtype_validType( type );
   if (dtype == NULL)
      return -1;
   if (shield != NULL)
      *shield = dtype->sdam;
   if (armour != NULL)
      *armour = dtype->adam;
   if (knockback != NULL)
      *knockback = dtype->knock;
   if (recoil != NULL)
      *recoil = dtype->recoil;
   return 0;
}


/**
 * @brief Gives the real shield damage, armour damage and knockback modifier.
 *
 *    @param[out] dshield Real shield damage.
 *    @param[out] darmour Real armour damage.
 *    @param[out] knockback Knockback modifier.
 *    @param[out] recoil Recoil modifier.
 *    @param[in] absorb Absorption value.
 *    @param[in] dmg Damage information.
 *    @param[in] s Ship stats to use.
 */
void dtype_calcDamage(double *dshield, double *darmour, double *knockback,
      double *recoil, double absorb, const Damage *dmg, const ShipStats *s)
{
   DTYPE *dtype;
   char *ptr;
   double multiplier;

   /* Must be valid. */
   dtype = dtype_validType( dmg->type );
   if (dtype == NULL)
      return;

   /* Set if non-nil. */
   if (dshield != NULL) {
      if ((dtype->soffset <= 0) || (s == NULL))
         *dshield = dmg->shield_pct * dmg->damage * absorb;
      else {
         /*
          * If an offset has been specified, look for a double at that offset
          * in the ShipStats struct, and used it as a multiplier.
          *
          * The 1. - n logic serves to convert the value from absorption to
          * damage multiplier.
          */
         ptr = (char*) s;
         memcpy(&multiplier, &ptr[dtype->soffset], sizeof(double));
         multiplier = MAX(0., 1. - multiplier);
         *dshield = dmg->shield_pct * dmg->damage * absorb * multiplier;
      }
   }
   if (darmour != NULL) {
      if ((dtype->aoffset) <= 0 || (s == NULL))
         *darmour = dmg->armor_pct * dmg->damage * absorb;
      else {
         ptr = (char*) s;
         memcpy(&multiplier, &ptr[dtype->aoffset], sizeof(double));
         multiplier = MAX(0., 1. - multiplier);
         *darmour = dmg->armor_pct * dmg->damage * absorb * multiplier;
      }
   }

   if (knockback != NULL)
      *knockback = dmg->knockback_pct;
   if (recoil != NULL)
      *recoil = dmg->recoil_pct;
}


