/*
 * See Licensing and Copyright notice in naev.h
 */



#ifndef SPACE_H
#  define SPACE_H

typedef struct Planet_ Planet;
typedef struct JumpPoint_ JumpPoint;


#include "commodity.h"
#include "explosion.h"
#include "faction.h"
#include "mission.h"
#include "opengl.h"
#include "pilot.h"
#include "tech.h"


#define SYSTEM_SIMULATE_TIME  30. /**< Time to simulate system before player is added. */

#define MAX_HYPERSPACE_VEL    25 /**< Speed to brake to before jumping. */

#define ASSET_VIRTUAL         0 /**< The asset is virtual. */
#define ASSET_REAL            1 /**< The asset is real. */

#define ASTEROID_REF_AREA     500000 /**< The "density" value in an asteroid field means 1 rock per this area. */


/* Asteroid status enum */
enum {
   ASTEROID_VISIBLE,    /**< Asteroid is visible (normal state). */
   ASTEROID_GROWING,    /**< Asteroid is in the process of appearing. */
   ASTEROID_SHRINKING,  /**< Asteroid is in the process of disappearing. */
   ASTEROID_EXPLODING,  /**< Asteroid is in the process of exploding. */
   ASTEROID_INIT,       /**< Asteroid has not been created yet. */
   ASTEROID_INVISIBLE,  /**< Asteroid is not used. */
};


/*
 * planet services
 */
#define PLANET_SERVICE_INHABITED    (1<<0) /**< Planet is inhabited. */
#define PLANET_SERVICE_LAND         (1<<1) /**< Can land. */
#define PLANET_SERVICE_REFUEL       (1<<2) /**< Has refueling. */
#define PLANET_SERVICE_BAR          (1<<3) /**< Has bar and thus news. */
#define PLANET_SERVICE_MISSIONS     (1<<4) /**< Has mission computer. */
#define PLANET_SERVICE_COMMODITY    (1<<5) /**< Can trade commodities. */
#define PLANET_SERVICE_OUTFITS      (1<<6) /**< Can trade outfits. */
#define PLANET_SERVICE_SHIPYARD     (1<<7) /**< Can trade ships. */
#define PLANET_SERVICE_BLACKMARKET  (1<<8) /**< Disables license restrictions on goods. */
#define PLANET_SERVICES_MAX         (PLANET_SERVICE_BLACKMARKET<<1)
#define planet_hasService(p,s)      ((p)->services & s) /**< Checks if planet has service. */
#define planet_addService(p,s)      ((p)->services |= (s)) /**< Adds a planet service. */
#define planet_rmService(p,s)       ((p)->services &= ~(s)) /**< Removes a planet service. */


/*
 * Planet flags.
 */
#define PLANET_KNOWN (1<<0) /**< Planet is known. */
#define PLANET_NOMISNSPAWN (1<<1) /**< No missions spawn nor trigger on this asset. */
#define PLANET_UNINHABITED (1<<2) /**< Force planet to be uninhabited. */
#define PLANET_HILIGHT (1<<3) /**< Planet is hilighted. */
#define planet_isFlag(p,f)    ((p)->flags & (f)) /**< Checks planet flag. */
#define planet_setFlag(p,f)   ((p)->flags |= (f)) /**< Sets a planet flag. */
#define planet_rmFlag(p,f)    ((p)->flags &= ~(f)) /**< Removes a planet flag. */
#define planet_isKnown(p) planet_isFlag(p,PLANET_KNOWN) /**< Checks if planet is known. */


/**
 * @struct MapOverlayPos
 *
 * @brief Saves the layout decisions from positioning labeled objects on the overlay.
 */
typedef struct MapOverlayPos_ {
   float radius; /**< Radius for display on the map overlay. */
   float text_offx; /**< x offset of the caption text. */
   float text_offy; /**< y offset of the caption text. */
   float text_width; /**< width of the caption text. */
   int noshrink; /**< Whether this object is excluded from shrinking. */
} MapOverlayPos;


/**
 * @struct Planet
 *
 * @brief Represents a planet.
 */
typedef struct Planet_ {
   int id; /**< Planet ID. */
   char* name; /**< planet name */
   Vector2d pos; /**< position in star system */
   double radius; /**< Radius of the planet. WARNING: lazy-loaded with gfx_space. */

   /* Planet details. */
   char *class; /**< Planet type. Uses Star Trek classification system (https://stexpanded.fandom.com/wiki/Planet_classifications) */
   factionId_t faction; /**< planet faction */
   uint64_t population; /**< Population of the planet. */

   /* Asset details. */
   double presenceAmount; /**< The amount of presence this asset exerts. */
   double rdr_range_mod;  /**< Modifier for detection range of the asset. */
   int presenceRange; /**< The range of presence exertion of this asset. */
   int real; /**< If the asset is tangible or not. */

   /* Landing details. */
   int can_land;      /**< Whether or not the player can land. */
   int land_override; /**< Forcibly allows the player to either be able to land or not (+1 is land, -1 is not, 0 otherwise). */
   char *land_func; /**< Landing function to execute. */
   char *land_msg; /**< Message on landing. */
   char *bribe_msg; /**< Bribe message. */
   char *bribe_ack_msg; /**< Bribe ACK message. */
   credits_t bribe_price;   /**< Cost of bribing. */
   int bribed; /**< If planet has been bribed. */

   /* Landed details. */
   char* description; /**< planet description */
   char* bar_description; /**< spaceport bar description */
   unsigned int services; /**< what services they offer */
   Commodity **commodities; /**< array: what commodities they sell */
   CommodityPrice *commodityPrice; /**< array: the base cost of a commodity on this planet */
   tech_group_t *tech; /**< Planet tech. */

   /* Graphics. */
   glTexture* gfx_space; /**< graphic in space */
   char *gfx_spaceName; /**< Name to load texture quickly with. */
   char *gfx_spacePath; /**< Name of the gfx_space for saving purposes. */
   char *gfx_exterior; /**< Don't actually load the texture */
   char *gfx_exteriorPath; /**< Name of the gfx_exterior for saving purposes. */

   /* Misc. */
   unsigned int flags; /**< flags for planet properties */
   MapOverlayPos mo;   /**< Overlay layout data. */
   int hilights; /**< Times planet has been flagged for hilighting. */
} Planet;


/*
 * star system flags
 */
#define SYSTEM_KNOWN (1<<0) /**< System is known. */
#define SYSTEM_MARKED (1<<1) /**< System is marked by a regular mission. */
#define SYSTEM_CMARKED (1<<2) /**< System is marked by a computer mission. */
#define SYSTEM_CLAIMED (1<<3) /**< System is claimed by a mission. */
#define SYSTEM_DISCOVERED (1<<4) /**< System has been discovered. This is a temporary flag used by the map. */
#define SYSTEM_HIDDEN (1<<5) /**< System is temporarily hidden from view. */
#define SYSTEM_CMARK_HILIGHT (1<<6) /**< System includes a hilighted computer mission mark. */
#define sys_isFlag(s,f)    ((s)->flags & (f)) /**< Checks system flag. */
#define sys_setFlag(s,f)   ((s)->flags |= (f)) /**< Sets a system flag. */
#define sys_rmFlag(s,f)    ((s)->flags &= ~(f)) /**< Removes a system flag. */
#define sys_isKnown(s)     (sys_isFlag((s),SYSTEM_KNOWN)) /**< Checks if system is known. */
#define sys_isMarked(s)    sys_isFlag((s),SYSTEM_MARKED) /**< Checks if system is marked. */


/*
 * Forward declaration.
 */
typedef struct StarSystem_ StarSystem;


/**
 * @brief Represents presence in a system
 */
typedef struct SystemPresence_ {
   factionId_t faction; /**< Faction of this presence. */
   double value; /**< Amount of presence. */
   double curUsed; /**< Presence currently used. */
   double timer; /**< Current faction timer. */
   int disabled; /**< Whether or not spawning is disabled for this presence. */
} SystemPresence;


/*
 * Jump point flags.
 */
#define JP_AUTOPOS (1<<0) /**< Automatically position jump point based on system radius. */
#define JP_KNOWN (1<<1) /**< Jump point is known. */
#define JP_HIDDEN (1<<2) /**< Jump point is hidden. */
#define JP_EXITONLY (1<<3) /**< Jump point is exit only */
#define JP_EXPRESS (1<<4) /**< Jump point is immediately visible. */
#define JP_LONGRANGE (1<<5) /**< Jump point is drawn only partially, with a gap. */
#define JP_HILIGHT (1<<6) /**< Jump point is hilighted. */
#define jp_isFlag(j,f)    ((j)->flags & (f)) /**< Checks jump flag. */
#define jp_setFlag(j,f)   ((j)->flags |= (f)) /**< Sets a jump flag. */
#define jp_rmFlag(j,f)    ((j)->flags &= ~(f)) /**< Removes a jump flag. */
#define jp_isKnown(j)     jp_isFlag(j,JP_KNOWN) /**< Checks if jump is known. */
#define jp_isUsable(j)    (jp_isKnown(j) && !jp_isFlag(j,JP_EXITONLY))



/**
 * @brief Represents a jump lane.
 */
typedef struct JumpPoint_ JumpPoint;
struct JumpPoint_ {
   StarSystem *from; /**< System containing this jump point. */
   int targetid; /**< ID of the target star system. */
   StarSystem *target; /**< Target star system to jump to. */
   JumpPoint *returnJump; /**< How to get back. Can be NULL */
   Vector2d pos; /**< Position in the system. */
   double radius; /**< Radius of jump range. */
   unsigned int flags; /**< Flags related to the jump point's status. */
   double rdr_range_mod; /**< Modifier for detection range of the jump point. */
   double angle; /**< Direction the jump is facing. */
   double cosa; /**< Cosinus of the angle. */
   double sina; /**< Sinus of the angle. */
   int sx; /**< X sprite to use. */
   int sy; /**< Y sprite to use. */
   MapOverlayPos mo;   /**< Overlay layout data. */
   int hilights; /**< Times jump has been flagged for hilighting. */
};
extern glTexture *jumppoint_gfx; /**< Jump point graphics. */

/**
 * @brief Represents a type of asteroid.
 */
typedef struct AsteroidType_ {
   char *ID; /**< ID of the asteroid type. */
   glTexture **gfxs; /**< asteroid possible gfxs. */
   Commodity **material; /**< Materials contained in the asteroid. */
   int *quantity; /**< Quantities of materials. */
   double armour; /**< Starting "armour" of the asteroid. */
} AsteroidType;


/**
 * @brief Represents a small player-rendered debris.
 */
typedef struct Debris_ {
   Vector2d pos; /**< Position. */
   Vector2d vel; /**< Velocity. */
   int gfxID; /**< ID of the asteroid gfx. */
   double height; /**< height vs player */
} Debris;


/**
 * @brief Represents a single asteroid.
 */
typedef struct Asteroid_ {
   int id; /**< ID of the asteroid, for targeting. */
   int parent; /**< ID of the anchor parent. */
   Vector2d pos; /**< Position. */
   Vector2d vel; /**< Velocity. */
   int gfxID; /**< ID of the asteroid gfx. */
   double timer; /**< Internal timer for animations. */
   int appearing; /**< 1: appearing, 2: disappaering, 3: exploding, 0 otherwise. */
   int type; /**< The ID of the asteroid type */
   double armour; /**< Current "armour" of the asteroid. */
} Asteroid;



/**
 * @brief Represents an asteroid field anchor.
 */
typedef struct AsteroidAnchor_ {
   int id; /**< ID of the anchor, for targeting. */
   Vector2d pos; /**< Position in the system (from center). */
   double density; /**< Density of the field. */
   Asteroid *asteroids; /**< Asteroids belonging to the field. */
   int nb; /**< Number of asteroids. */
   Debris *debris; /**< Debris belonging to the field. */
   int ndebris; /**< Number of debris. */
   double radius; /**< Radius of the anchor. */
   double area; /**< Field's area. */
   int *type; /**< Types of asteroids. */
   int ntype; /**< Number of types. */
} AsteroidAnchor;


/**
 * @brief Represents an asteroid exclusion zone.
 */
typedef struct AsteroidExclusion_ {
   Vector2d pos; /**< Position in the system (from center). */
   double radius; /**< Radius of the exclusion zone. */
} AsteroidExclusion;


/**
 * @brief Represents a star system.
 *
 * The star system is the basic setting in Naev.
 */
struct StarSystem_ {
   int id; /**< Star system index. */

   /* General. */
   char* name; /**< star system name */
   Vector2d pos; /**< position */
   int stars; /**< Amount of "stars" it has. */
   double rdr_range_mod; /**< Mod for radar range of all ships in the system. */
   double nebu_hue; /**< Hue of the nebula (0. - 1.) */
   double nebu_density; /**< Nebula density (0. - 1000.) */
   double nebu_volatility; /**< Nebula volatility (0. - 1000.) */
   double radius; /**< Default system radius for standard jump points. */
   char *background; /**< Background script. */
   char *features; /**< Extra text on the map indicating special features of the system. */

   /* Planets. */
   Planet **planets; /**< Array (array.h): planets */
   int *planetsid; /**< Array (array.h): IDs of the planets. */
   factionId_t faction; /**< overall faction */

   /* Jumps. */
   JumpPoint *jumps; /**< Array (array.h): Jump points in the system */

   /* Asteroids. */
   AsteroidAnchor *asteroids; /**< Array (array.h): Asteroid fields in the system */
   AsteroidExclusion *astexclude; /**< Array (array.h): Asteroid exclusion zones in the system */

   /* Calculated. */
   double *prices; /**< Handles the prices in the system. */

   /* Presence. */
   SystemPresence *presence; /**< Array (array.h): Pointer to an array of presences in this system. */
   int spilled; /**< If the system has been spilled to yet. */
   double ownerpresence; /**< Amount of presence the owning faction has in a system. */

   /* Markers. */
   int markers_new; /**< Number of new mission markers. */
   int markers_computer; /**< Number of mission computer markers. */
   int markers_low; /**< Number of low mission markers. */
   int markers_high; /**< Number of high mission markers. */
   int markers_plot; /**< Number of plot level mission markers. */

   /* Economy. */
   CommodityPrice *averagePrice;

   /* Misc. */
   unsigned int flags; /**< flags for system properties */
};


extern StarSystem *cur_system; /**< current star system */
extern int space_spawn; /**< 1 if spawning is enabled. */


/*
 * loading/exiting
 */
void space_init( const char* sysname );
int space_load (void);
void space_exit (void);

/*
 * planet stuff
 */
Planet *planet_new (void);
void planet_gfxLoad( Planet *p );
int planet_hasSystem( const char* planetname );
char* planet_getSystem( const char* planetname );
Planet* planet_getAll (void);
Planet* planet_get( const char* planetname );
Planet* planet_getIndex( int ind );
void planet_setKnown( Planet *p );
int planet_index( const Planet *p );
int planet_exists( const char* planetname );
const char *planet_existsCase( const char* planetname );
char **planet_searchFuzzyCase( const char* planetname, int *n );
const char* planet_getServiceName( int service );
int planet_getService( char *name );
credits_t planet_commodityPrice( const Planet *p, const Commodity *c );
credits_t planet_commodityPriceAtTime( const Planet *p, const Commodity *c, ntime_t t );
int planet_averagePlanetPrice( const Planet *p, const Commodity *c, credits_t *mean, double *std);
/* Misc modification. */
int planet_setFaction(Planet *p, factionId_t faction);
/* Land related stuff. */
char planet_getColourChar(const Planet *p);
const char *planet_getSymbol(const Planet *p);
const glColour* planet_getColour(const Planet *p);
void planet_updateLand( Planet *p );


/*
 * jump stuff
 */
JumpPoint* jump_get( const char* jumpname, const StarSystem* sys );
JumpPoint* jump_getTarget( StarSystem* target, const StarSystem* sys );
const char *jump_getSymbol( JumpPoint *jp );

/*
 * system adding/removing stuff.
 */
void system_reconstructJumps (StarSystem *sys);
void systems_reconstructJumps (void);
void systems_reconstructPlanets (void);
StarSystem *system_new (void);
int system_addPlanet( StarSystem *sys, const char *planetname );
int system_rmPlanet( StarSystem *sys, const char *planetname );
int system_addJump(StarSystem *sys, const char *jumpname, Vector2d pos,
      double rdr_range_mod, unsigned int flags);
int system_rmJump( StarSystem *sys, const char *jumpname );

/*
 * render
 */
void space_render( const double dt );
void space_renderOverlay( const double dt );
void planets_render (void);

/*
 * Presence stuff.
 */
void system_presenceCleanupAll( void );
void system_addPresence(StarSystem *sys, factionId_t faction, double amount,
      int range);
double system_getPresence(const StarSystem *sys, factionId_t faction);
void system_addAllPlanetsPresence( StarSystem *sys );
void space_reconstructPresences( void );
void system_rmCurrentPresence(StarSystem *sys, factionId_t faction,
      double amount);

/*
 * update.
 */
void space_update( const double dt );
unsigned int space_isSimulation( void );

/*
 * Graphics.
 */
void space_gfxLoad( StarSystem *sys );
void space_gfxUnload( StarSystem *sys );

/*
 * Getting stuff.
 */
StarSystem* system_getAll (void);
const char *system_existsCase( const char* sysname );
char **system_searchFuzzyCase( const char* sysname, int *n );
StarSystem* system_get( const char* sysname );
StarSystem* system_getIndex( int id );
int system_index( StarSystem *sys );
int space_sysReachable( StarSystem *sys );
int space_sysReallyReachable( char* sysname );
int space_sysReachableFromSys( StarSystem *target, StarSystem *sys );
char** space_getFactionPlanet(factionId_t *factions, int landable);
char* space_getRndPlanet( int landable, unsigned int services,
      int (*filter)(Planet *p));
double system_getClosest( const StarSystem *sys, int *pnt, int *jp, int *ast, int *fie, double x, double y );
double system_getClosestAng( const StarSystem *sys, int *pnt, int *jp, int *ast, int *fie, double x, double y, double ang );


/*
 * Markers.
 */
int space_addMarker( int sys, SysMarker type );
int space_rmMarker( int sys, SysMarker type );
void space_clearKnown (void);
void space_clearMarkers (void);
void space_clearComputerMarkers (void);
void space_clearComputerMarkerHilights(void);
int system_hasPlanet( const StarSystem *sys );


/*
 * Hyperspace.
 */
int space_canHyperspace( Pilot* p);
int space_hyperspace( Pilot* p );
int space_calcJumpInPos( StarSystem *in, StarSystem *out, Vector2d *pos, Vector2d *vel, double *dir );


/*
 * Asteroids
 */
void asteroid_hit( Asteroid *a, const Damage *dmg );
int space_isInField ( const Vector2d *p );
AsteroidType *space_getType ( int ID );


/*
 * Misc.
 */
void system_setFaction( StarSystem *sys );
void space_checkLand (void);
void space_factionChange (void);
void space_queueLand( Planet *pnt );


#endif /* SPACE_H */
