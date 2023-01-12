/*
 * See Licensing and Copyright notice in naev.h
 */


#ifndef PILOT_CARGO_H
#  define PILOT_CARGO_H


#include "pilot.h"


/*
 * Normal Cargo.
 */
int pilot_cargoUsed( const Pilot* pilot ); /* gets how much cargo it has onboard */
int pilot_cargoFree( const Pilot* p ); /* cargo space */
int pilot_cargoOwned( const Pilot* pilot, const Commodity* cargo );
int pilot_cargoAdd( Pilot* pilot, const Commodity* cargo,
      int quantity, unsigned int id );
int pilot_cargoRm( Pilot* pilot, const Commodity* cargo, int quantity );
int pilot_cargoMove( Pilot* dest, Pilot* src );
void pilot_cargoCalc( Pilot* pilot );

/*
 * Normal cargo below the abstractions.
 */
int pilot_cargoRmAll( Pilot* pilot, int cleanup );
int pilot_cargoAddRaw( Pilot* pilot, const Commodity* cargo,
      int quantity, unsigned int id );


/*
 * Mission cargo - not to be confused with normal cargo.
 */
unsigned int pilot_addMissionCargo( Pilot* pilot, const Commodity* cargo, int quantity );
int pilot_rmMissionCargo( Pilot* pilot, unsigned int cargo_id, int jettison );


#endif /* PILOT_CARGO_H */
