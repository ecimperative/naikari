/*
 * See Licensing and Copyright notice in naev.h
 */



#ifndef COMMODITY_H
#  define COMMODITY_H


/** @cond */
#include <stdint.h>
/** @endcond */

#include "credits.h"
#include "opengl.h"



/**
 * @struct CommodityModifier
 *
 * @brief Represents a dictionary of values used to modify a commodity.
 *
 */
struct CommodityModifier_ {
   char *name;
   float value;
   struct CommodityModifier_ *next;
};
typedef struct CommodityModifier_ CommodityModifier;

/**
 * @struct Commodity
 *
 * @brief Represents a commodity.
 */
typedef struct Commodity_ {
   char* name; /**< Name of the commodity. */
   char* description; /**< Description of the commodity. */
   unsigned int standard; /**< Wether or not this commodity is standard. */
   /* Prices. */
   double raw_price; /**< Raw price of the commodity. */
   double price; /**< Base price of the commodity. */
   glTexture* gfx_store; /**< Store graphic. */
   glTexture* gfx_space; /**< Space graphic. */
   CommodityModifier *planet_modifier; /**< The price modifier for different planet types. */
   double period; /**< Period of price fluctuation. */
   double population_modifier; /**< Scale of price modification due to population. */
   CommodityModifier *faction_modifier; /**< Price modifier for different factions. */
   credits_t lastPurchasePrice; /**< Price paid when last purchasing this commodity. */
   int istemp; /**< This commodity is temporary. */
} Commodity;

typedef struct CommodityPrice_ {
  double price; /**< Average price of a commodity on a particular planet */
  double planetPeriod; /**< Minor time period (days) over which commidity price varies */
  double sysPeriod; /** Major time period */
  double planetVariation; /**< Mmount by which a commodity price varies */
  double sysVariation; /**< System level commodity price variation.  At a given time, commodity price is equal to price + sysVariation*sin(2pi t/sysPeriod) + planetVariation*sin(2pi t/planetPeriod) */
  int64_t updateTime; /**< used for averaging and to hold the time last average was calculated. */
  char *name; /**< used for keeping tabs during averaging */
  double sum; /**< used when averaging over jump points during setup, and then for capturing the moving average when the player visits a planet. */
  double sum2; /**< sum of (squared prices seen), used for calc of standard deviation. */
  int cnt; /**< used for calc of mean and standard deviation - number of records in the data. */
} CommodityPrice;

/**
 * @struct Gatherable
 *
 * @brief Represents stuff that can be gathered.
 */
typedef struct Gatherable_ {
   Commodity *type; /**< Type of commodity. */
   Vector2d pos; /**< Position. */
   Vector2d vel; /**< Velocity. */
   double timer; /**< Timer to de-spawn the gatherable. */
   double lifeleng; /**< nb of seconds before de-spawn. */
   int quantity; /**< Quantity of material. */
} Gatherable;


/*
 * Commodity stuff.
 */
Commodity* commodity_get( const char* name );
Commodity* commodity_getW( const char* name );

int commodity_load (void);
void commodity_free (void);


/*
 * Temporary commodities.
 */
int commodity_isTemp( const char* name );
Commodity* commodity_newTemp( const char* name, const char* desc );


/*
 * Gatherable objects
 */
int gatherable_init( Commodity* com, Vector2d pos, Vector2d vel, double lifeleng, int qtt );
void gatherable_render( void );
int gatherable_getClosest( Vector2d pos, double rad );
int gatherable_getPos( Vector2d* pos, Vector2d* vel, int id );
void gatherable_free( void );
void gatherable_update( double dt );
void gatherable_gather( int pilot );


/*
 * Misc stuff.
 */
void commodity_Jettison( int pilot, const Commodity* com, int quantity );
int commodity_compareTech( const void *commodity1, const void *commodity2 );
Commodity ** standard_commodities (void);


#endif /* COMMODITY_H */

