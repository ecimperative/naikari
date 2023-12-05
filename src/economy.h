/*
 * See Licensing and Copyright notice in naev.h
 */

#ifndef ECONOMY_H
#  define ECONOMY_H

/** @cond */
#include <stdint.h>
/** @endcond */

#include "credits.h"
#include "opengl.h"
#include "space.h"


/*
 * Economy stuff.
 */
int economy_init (void);
void economy_addQueuedUpdate (void);
int economy_execQueued (void);
int economy_update( unsigned int dt );
int economy_refresh (void);
void economy_destroy (void);

/*
 * Price stuff.
 */
int economy_getAveragePlanetPrice( const Commodity *com, const Planet *p, credits_t *mean, double *std);
credits_t economy_getPrice( const Commodity *com, const StarSystem *sys, const Planet *p );
credits_t economy_getPriceAtTime( const Commodity *com, const StarSystem *sys, const Planet *p, ntime_t t );

/*
 * Calculating the sinusoidal economy values
 */
void economy_initialiseCommodityPrices(void);
int economy_getAveragePrice( const Commodity *com, credits_t *mean, double *std );
void economy_initialiseSingleSystem( StarSystem *sys, Planet *planet );

#endif /* ECONOMY_H */
