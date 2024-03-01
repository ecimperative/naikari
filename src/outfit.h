/*
 * See Licensing and Copyright notice in naev.h
 */



#ifndef OUTFIT_H
#  define OUTFIT_H


#include "collision.h"
#include "commodity.h"
#include "credits.h"
#include "opengl.h"
#include "shipstats.h"
#include "sound.h"
#include "spfx.h"
#include "nlua.h"


/*
 * properties
 */
#define outfit_isProp(o,p)          ((o)->properties & p) /**< Checks an outfit for property. */
/* property flags */
#define OUTFIT_PROP_UNIQUE             (1<<0) /**< Unique item (can only have one). Not sellable.*/
#define OUTFIT_PROP_WEAP_SECONDARY     (1<<10) /**< Is a secondary weapon? */
#define OUTFIT_PROP_WEAP_SPIN          (1<<11) /**< Should weapon spin around? */
#define OUTFIT_PROP_WEAP_BLOWUP_ARMOUR (1<<12) /**< Weapon blows up (armour spfx)
                                                   when timer is up. */
#define OUTFIT_PROP_WEAP_BLOWUP_SHIELD (1<<13) /**< Weapon blows up (shield spfx)
                                                   when timer is up. */

/* Outfit filter labels. [Doc comments are also translator notes and must precede the #define.] */
/* Abbreviation for "Weapon", short enough to use as a tab/column title. */
#define OUTFIT_LABEL_WEAPON p_("outfit_type", " W ")
/* Abbreviation for "Utility", short enough to use as a tab/column title. */
#define OUTFIT_LABEL_UTILITY p_("outfit_type", " U ")
/* Abbreviation for "Structure", short enough to use as a tab/column title. */
#define OUTFIT_LABEL_STRUCTURE p_("outfit_type", " S ")
/* Abbreviation for "Core", short enough to use as a tab/column title. */
#define OUTFIT_LABEL_CORE p_("outfit_type", "Core")
/* Abbreviation for "Other", short enough to use as a tab/column title.
 * (Used for uncategorized outfits.) */
#define OUTFIT_LABEL_OTHER p_("outfit_type", "Other")
/* Abbreviation for "All", short enough to use as a tab/column title.
 * (Used for listing all outfits.) */
#define OUTFIT_LABEL_ALL p_("outfit_type", "All")


/*
 * Needed because some outfittypes call other outfits.
 */
struct Outfit_;


/**
 * @brief Different types of existing outfits.
 *
 * Outfits are organized by the order here
 *
 * @note If you modify this DON'T FORGET TO MODIFY outfit_getType too!!!
 */
typedef enum OutfitType_ {
   OUTFIT_TYPE_NULL, /**< Null type. */
   OUTFIT_TYPE_BOLT, /**< Fixed bolt cannon. */
   OUTFIT_TYPE_BEAM, /**< Fixed beam cannon. */
   OUTFIT_TYPE_TURRET_BOLT, /**< Rotary bolt turret. */
   OUTFIT_TYPE_TURRET_BEAM, /**< Rotary beam turret. */
   OUTFIT_TYPE_LAUNCHER, /**< Launcher. */
   OUTFIT_TYPE_TURRET_LAUNCHER, /**< Turret launcher. */
   OUTFIT_TYPE_AMMO, /**< Launcher ammo. */
   OUTFIT_TYPE_FIGHTER_BAY, /**< Contains other ships. */
   OUTFIT_TYPE_FIGHTER, /**< Ship contained in FIGHTER_BAY. */
   OUTFIT_TYPE_AFTERBURNER, /**< Gives the ship afterburn capability. */
   OUTFIT_TYPE_MODIFICATION, /**< Modifies the ship base features. */
   OUTFIT_TYPE_MAP, /**< Gives the player more knowledge about systems. */
   OUTFIT_TYPE_LOCALMAP, /**< Gives the player more knowledge about the current system. */
   OUTFIT_TYPE_LICENSE, /**< License that allows player to buy special stuff. */
   OUTFIT_TYPE_SENTINEL /**< indicates last type */
} OutfitType;


/**
 * @brief Outfit slot types.
 */
typedef enum OutfitSlotType_ {
   OUTFIT_SLOT_NULL, /**< Invalid slot type. */
   OUTFIT_SLOT_NA, /**< Slot type not applicable. */
   OUTFIT_SLOT_STRUCTURE, /**< Low energy slot. */
   OUTFIT_SLOT_UTILITY, /**< Medium energy slot. */
   OUTFIT_SLOT_WEAPON /**< High energy slot. */
} OutfitSlotType;


/**
 * @brief Outfit slot sizes.
 */
typedef enum OutfitSlotSize_ {
   OUTFIT_SLOT_SIZE_NA, /**< Not applicable slot size. */
   OUTFIT_SLOT_SIZE_LIGHT, /**< Light slot size. */
   OUTFIT_SLOT_SIZE_MEDIUM, /**< Medium slot size. */
   OUTFIT_SLOT_SIZE_HEAVY /**< Heavy slot size. */
} OutfitSlotSize;


/**
 * @brief Ammo AI types.
 */
typedef enum OutfitAmmoAI_ {
   AMMO_AI_UNGUIDED, /**< No AI. */
   AMMO_AI_SEEK, /**< Aims at the target. */
   AMMO_AI_SMART /**< Aims at the target, correcting for relative velocity. */
} OutfitAmmoAI;

/**
 * @brief Pilot slot that can contain outfits.
 */
typedef struct OutfitSlot_ {
   unsigned int spid;   /**< Slot property ID. */
   int exclusive;       /**< Outfit must go exclusively into the slot. */
   OutfitSlotType type; /**< Type of outfit slot. */
   OutfitSlotSize size; /**< Size of the outfit. */
} OutfitSlot;


/**
 * @brief Core damage that an outfit does.
 */
typedef struct Damage_ {
   int type;            /**< Type of damage. */
   double penetration;  /**< Penetration the damage has [0:1], with 1 being 100%. */
   double damage;       /**< Amount of damage, this counts towards killing the ship. */
   double disable;      /**< Amount of disable damage, this counts towards disabling the ship. */
} Damage;


/**
 * @brief Represents the particular properties of a bolt weapon.
 */
typedef struct OutfitBoltData_ {
   double delay;     /**< Delay between shots */
   double speed;     /**< How fast it goes. */
   double range;     /**< How far it goes. */
   double falloff;   /**< Point at which damage falls off. */
   double ew_lockon; /**< Electronic warfare lockon parameter. */
   double energy;    /**< Energy usage */
   Damage dmg;       /**< Damage done. */
   double heatup;    /**< How long it should take for the weapon to heat up (approx). */
   double heat;      /**< Heat per shot. */
   double rdr_range; /**< Radar Optimal Range. */
   double rdr_range_max; /**< Radar Maximum Range. */
   double swivel;    /**< Amount of swivel (semiarc in radians of deviation the weapon can correct). */
   int spread_bolts; /**< Number of extra bolts per shot. */
   double spread_arc; /**< Arc of bolt spread. */

   /* Sound and graphics. */
   glTexture* gfx_space; /**< Normal graphic. */
   glTexture* gfx_end; /**< End graphic with modified hue. */
   double spin;      /**< Graphic spin rate. */
   int sound;        /**< Sound to play on shoot.*/
   int sound_hit;    /**< Sound to play on hit. */
   int spfx_armour;  /**< special effect on hit. */
   int spfx_shield;  /**< special effect on hit. */

   /* collision polygon */
   CollPoly *polygon; /**< Array (array.h): Collision polygons. */
} OutfitBoltData;

/**
 * @brief Represents the particular properties of a beam weapon.
 */
typedef struct OutfitBeamData_ {
   /* Time stuff. */
   double delay;     /**< Delay between usage. */
   double warmup;    /**< How long beam takes to warm up. */
   double duration;  /**< How long the beam lasts active. */
   double min_duration; /**< Minimum duration the beam can be fired for. */

   /* Beam properties. */
   double range;     /**< how far it goes */
   double turn;      /**< How fast it can turn. Only for turrets, in rad/s. */
   double energy;    /**< Amount of energy it drains (per second). */
   Damage dmg;       /**< Damage done. */
   double heatup;    /**< How long it should take for the weapon to heat up (approx). */
   double heat;      /**< Heat per second. */
   double swivel;    /**< Amount of swivel (semiarc in radians of deviation the weapon can correct). */

   /* Graphics and sound. */
   glColour colour;  /**< Color to use for the shader. */
   GLfloat width;    /**< Width of the beam. */
   GLuint shader;    /**< Shader subroutine to use. */
   int spfx_armour;  /**< special effect on hit */
   int spfx_shield;  /**< special effect on hit */
   int sound_warmup; /**< Sound to play when warming up. @todo use. */
   int sound;        /**< Sound to play. */
   int sound_off;    /**< Sound to play when turning off. */
} OutfitBeamData;

/**
 * @brief Represents a particular missile launcher.
 *
 * The properties of the weapon are highly dependent on the ammunition.
 */
typedef struct OutfitLauncherData_ {
   double delay;     /**< Delay between shots. */
   char *ammo_name;  /**< Name of the ammo to use. */
   struct Outfit_ *ammo; /**< Ammo to use. */
   int amount;       /**< Amount of ammo it can store. */
   double reload_time; /**< Time it takes to reload 1 ammo. */

   /* Lock-on information. */
   double lockon;    /**< Time it takes to lock on the target */
   double arc;       /**< Semi-angle of the arc which it will lock on in. */
   double rdr_range; /**< Radar optimal range. */
   double rdr_range_max; /**< Radar maximum range. */
   double swivel;    /**< Amount of swivel (semiarc in radians of deviation the weapon can correct. */
} OutfitLauncherData;

/**
 * @brief Represents ammunition for a launcher.
 */
typedef struct OutfitAmmoData_ {
   double duration;  /**< How long the ammo lives. */
   double resist;    /**< Lowers chance of jamming by this amount */
   OutfitAmmoAI ai;  /**< Smartness of ammo. */

   double speed;     /**< Maximum speed */
   double turn;      /**< Turn velocity in rad/s. */
   double thrust;    /**< Acceleration */
   double energy;    /**< Energy usage */
   Damage dmg;       /**< Damage done. */

   glTexture* gfx_space; /**< Graphic. */
   double spin;      /**< Graphic spin rate. */
   int sound;        /**< sound to play */
   int sound_hit;    /**< Sound to play on hit. */
   int spfx_armour;  /**< special effect on hit */
   int spfx_shield;  /**< special effect on hit */
   const TrailSpec* trail_spec; /**< Trail style if applicable, else NULL. */
   double trail_x_offset;       /**< Offset x. */

   /* collision polygon */
   CollPoly *polygon; /**< Array (array.h): Collision polygons. */
} OutfitAmmoData;

/**
 * @brief Represents a ship modification.
 *
 * These modify the ship's basic properties when equipped on a pilot.
 */
typedef struct OutfitModificationData_ {
   /* Active information (if applicable). */
   int active;       /**< Outfit is active. */
   double duration;  /**< Time the active outfit stays on (in seconds). */
   double cooldown;  /**< Time the active outfit stays off after it's duration (in seconds). */

   /* All the modifiers are based on the outfit's ship stats, nothing here but
    * Lua and active stuff. */

   /* Lua function references. Set to LUA_NOREF if not used. */
   nlua_env lua_env; /**< Lua environment. Shared for each outfit to allow globals. */
   int lua_init;     /**< Run when pilot enters a system. */
   int lua_cleanup;  /**< Run when the pilot is erased. */
   int lua_update;   /**< Run periodically. */
   int lua_ontoggle; /**< Run when toggled. */
   int lua_onhit;    /**< Run when pilot takes damage. */
   int lua_outofenergy; /**< Run when the pilot runs out of energy. */
   int lua_cooldown; /**< Run when cooldown is started or stopped. */
} OutfitModificationData;

/**
 * @brief Represents an afterburner.
 */
typedef struct OutfitAfterburnerData_ {
   /* Internal properties. */
   int sound_on;     /**< Sound of the afterburner turning on */
   int sound;        /**< Sound of the afterburner being on */
   int sound_off;    /**< Sound of the afterburner turning off */
   double thrust;    /**< Percent of thrust increase based on ship base. */
   double speed;     /**< Percent of speed to increase based on ship base. */
   double energy;    /**< Energy usage while active */
   double mass_limit; /**< Limit at which effectiveness starts to drop. */
   double heatup;    /**< How long it takes for the afterburner to overheat. */
   double heat;      /**< Heat per second. */
   double heat_cap;  /**< Temperature at which the outfit overheats (K). */
   double heat_base; /**< Temperature at which the outfit BEGINS to overheat(K). */
} OutfitAfterburnerData;

/**
 * @brief Represents a fighter bay.
 */
typedef struct OutfitFighterBayData_ {
   char *ammo_name;  /**< Name of the ships to use as ammo. */
   struct Outfit_ *ammo; /**< Ships to use as ammo. */
   double delay;     /**< Delay between launches. */
   int amount;       /**< Amount of ammo it can store. */
   double reload_time; /**< Time it takes to reload 1 ammo. */
} OutfitFighterBayData;

/**
 * @brief Represents a fighter for a fighter bay.
 */
typedef struct OutfitFighterData_ {
   char *ship;       /**< Ship to use for fighter. */
   int sound;        /**< Sound to make when launching. */
} OutfitFighterData;

/* Forward declaration */
struct OutfitMapData_s;
typedef struct OutfitMapData_s OutfitMapData_t;

/**
 * @brief A ship outfit, depends radically on the type.
 */
typedef struct Outfit_ {
   char *name;       /**< Name of the outfit. */
   char *typename;   /**< Overrides the base type. */
   int rarity;       /**< Rarity of the outfit. */

   /* General specs */
   OutfitSlot slot;  /**< Slot the outfit fits into. */
   char *license;    /**< Licenses needed to buy it. */
   double mass;      /**< How much weapon capacity is needed. */
   char *limit;      /**< Name to limit to one per ship (ignored if NULL). */

   /* Store stuff */
   credits_t price;  /**< Base sell price. */
   char *description; /**< Store description. */
   char *desc_short; /**< Short outfit description. */
   int priority;     /**< Sort priority, highest first. */

   glTexture* gfx_store; /**< Store graphic. */
   glTexture** gfx_overlays; /**< Array (array.h): Store overlay graphics. */

   unsigned int properties; /**< Properties stored bitwise. */
   unsigned int group; /**< Weapon group to use when autoweap is enabled. */

   /* Stats. */
   ShipStatList *stats; /**< Stat list. */

   /* Type dependent */
   OutfitType type; /**< Type of the outfit. */
   union {
      OutfitBoltData blt;         /**< BOLT */
      OutfitBeamData bem;         /**< BEAM */
      OutfitLauncherData lau;     /**< MISSILE */
      OutfitAmmoData amm;         /**< AMMO */
      OutfitModificationData mod; /**< MODIFICATION */
      OutfitAfterburnerData afb;  /**< AFTERBURNER */
      OutfitFighterBayData bay;   /**< FIGHTER_BAY */
      OutfitFighterData fig;      /**< FIGHTER */
      OutfitMapData_t *map;       /**< MAP */
   } u; /**< Holds the type-based outfit data. */
} Outfit;


/*
 * get
 */
Outfit* outfit_get( const char* name );
Outfit* outfit_getW( const char* name );
const Outfit* outfit_getAll (void);
int outfit_compareTech( const void *outfit1, const void *outfit2 );
/* outfit types */
int outfit_isActive( const Outfit* o );
int outfit_isForward( const Outfit* o );
int outfit_isBolt( const Outfit* o );
int outfit_isBeam( const Outfit* o );
int outfit_isLauncher( const Outfit* o );
int outfit_isAmmo( const Outfit* o );
int outfit_isSeeker( const Outfit* o );
int outfit_isTurret( const Outfit* o );
int outfit_isMod( const Outfit* o );
int outfit_isAfterburner( const Outfit* o );
int outfit_isFighterBay( const Outfit* o );
int outfit_isFighter( const Outfit* o );
int outfit_isMap( const Outfit* o );
int outfit_isLocalMap( const Outfit* o );
int outfit_isLicense( const Outfit* o );
int outfit_isSecondary( const Outfit* o );
const char* outfit_getType( const Outfit* o );
const char* outfit_getTypeBroad( const Outfit* o );
const char* outfit_getAmmoAI( const Outfit *o );

/*
 * Search.
 */
const char *outfit_existsCase( const char* name );
char **outfit_searchFuzzyCase( const char* name, int *n );

/*
 * Filter.
 */
int outfit_filterWeapon( const Outfit *o );
int outfit_filterUtility( const Outfit *o );
int outfit_filterStructure( const Outfit *o );
int outfit_filterCore( const Outfit *o );
int outfit_filterOther( const Outfit *o );

/*
 * get data from outfit
 */
const char *outfit_slotName( const Outfit* o );
const char *slotName( const OutfitSlotType o );
const char *outfit_slotSize( const Outfit* o );
const char *slotSize( const OutfitSlotSize o );
const glColour *outfit_slotSizeColour( const OutfitSlot* os );
OutfitSlotSize outfit_toSlotSize( const char *s );
glTexture* outfit_gfx( const Outfit* o );
CollPoly* outfit_plg( const Outfit* o );
int outfit_spfxArmour( const Outfit* o );
int outfit_spfxShield( const Outfit* o );
const Damage *outfit_damage( const Outfit* o );
double outfit_delay( const Outfit* o );
Outfit* outfit_ammo( const Outfit* o );
int outfit_amount( const Outfit* o );
double outfit_energy( const Outfit* o );
double outfit_heat( const Outfit* o );
double outfit_range( const Outfit* o );
double outfit_speed( const Outfit* o );
double outfit_spin( const Outfit* o );
int outfit_sound( const Outfit* o );
int outfit_soundHit( const Outfit* o );
/* Active outfits. */
double outfit_duration( const Outfit* o );
double outfit_cooldown( const Outfit* o );
/*
 * loading/freeing outfit stack
 */
int outfit_load (void);
int outfit_mapParse(void);
void outfit_free (void);


/*
 * Misc.
 */
int outfit_fitsSlot( const Outfit* o, const OutfitSlot* s );
int outfit_fitsSlotType( const Outfit* o, const OutfitSlot* s );
void outfit_freeSlot( OutfitSlot* s );
glTexture* rarity_texture( int rarity );


#endif /* OUTFIT_H */
