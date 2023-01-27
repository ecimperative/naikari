/*
 * See Licensing and Copyright notice in naev.h
 */



#ifndef SHIPSTATS_H
#  define SHIPSTATS_H


#include "nxml.h"
#include "nlua.h"


/**
 * @brief Lists all the possible types.
 *
 * Syntax:
 *    SS_TYPE_#1_#2
 *
 * #1 is D for double, A for absolute double, I for integer or B for boolean.
 * #2 is the name.
 */
typedef enum ShipStatsType_ {
   SS_TYPE_NIL,               /**< Invalid type. */

   /* Freighter-type. */
   SS_TYPE_D_JUMP_DELAY,      /**< Modulates the time that passes during a hyperspace jump. */
   SS_TYPE_D_LAND_DELAY,      /**< Modulates the time that passes during landing. */
   SS_TYPE_D_CARGO_INERTIA,   /**< Modifies the effect of cargo_mass. */

   /* Stealth. */
   SS_TYPE_D_RDR_RANGE, /**< Radar range. */
   SS_TYPE_D_RDR_RANGE_MOD, /**< Radar range modifier. */
   SS_TYPE_D_RDR_JUMP_RANGE, /**< Jump detection range. */
   SS_TYPE_D_RDR_JUMP_RANGE_MOD, /**< Jump detection range modifier. */
   SS_TYPE_D_RDR_ENEMY_RANGE_MOD, /**< Enemy radar range modifier. */

   /* Forward mounts. */
   SS_TYPE_D_FORWARD_DAMAGE, /**< Damage done by cannons. */
   SS_TYPE_D_FORWARD_FIRERATE, /**< Firerate of cannons. */
   SS_TYPE_D_FORWARD_ENERGY, /**< Energy usage of cannons. */
   SS_TYPE_D_FORWARD_HEAT, /**< Heat generation for cannons. */
   SS_TYPE_P_FORWARD_DAMAGE_AS_DISABLE, /**< Damage converted to disable. */

   /* Turrets. */
   SS_TYPE_D_TURRET_DAMAGE, /**< Damage done by turrets. */
   SS_TYPE_D_TURRET_FIRERATE, /**< Firerate of turrets. */
   SS_TYPE_D_TURRET_ENERGY, /**< Energy usage of turrets. */
   SS_TYPE_D_TURRET_HEAT, /**< Heat generation for turrets. */
   SS_TYPE_P_TURRET_DAMAGE_AS_DISABLE, /**< Damage converted to disable. */

   /* Launchers. */
   SS_TYPE_D_LAUNCH_DAMAGE, /**< Launch damage for missiles. */
   SS_TYPE_D_LAUNCH_RATE, /**< Launch rate for missiles. */
   SS_TYPE_D_LAUNCH_RANGE, /**< Launch range for missiles. */
   SS_TYPE_D_AMMO_CAPACITY, /**< Capacity of launchers. */
   SS_TYPE_D_LAUNCH_RELOAD, /**< Regeneration rate of launcher ammo. */
   SS_TYPE_P_LAUNCH_DAMAGE_AS_DISABLE, /**< Damage converted to disable. */

   /* Fighter Bays. */
   SS_TYPE_D_FBAY_DAMAGE,     /**< Fighter bay fighter damage bonus (all weapons). */
   SS_TYPE_D_FBAY_HEALTH,     /**< Fighter bay fighter health bonus (shield and armour). */
   SS_TYPE_D_FBAY_MOVEMENT,   /**< Fighter bay fighter movement bonus (turn, thrust, and speed). */
   SS_TYPE_D_FBAY_CAPACITY,   /**< Capacity of fighter bays. */
   SS_TYPE_D_FBAY_RATE,       /**< Launch rate for fighter bays. */
   SS_TYPE_D_FBAY_RELOAD,     /**< Regeneration rate of fighters. */

   /* Misc. */
   SS_TYPE_D_HEAT_DISSIPATION, /**< Ship heat dissipation. */
   SS_TYPE_D_STRESS_DISSIPATION, /**< Ship stress dissipation. */
   SS_TYPE_D_MASS,            /**< Ship mass. */
   SS_TYPE_D_ENGINE_LIMIT_REL, /**< Modifier for the ship's engine limit. */
   SS_TYPE_D_LOOT_MOD,        /**< Affects boarding rewards. */
   SS_TYPE_D_TIME_MOD,        /**< Time dilation modifier. */
   SS_TYPE_D_TIME_SPEEDUP,    /**< Makes the pilot operate at a higher dt. */
   SS_TYPE_D_COOLDOWN_TIME,   /**< Speeds up or slows down the cooldown time. */
   SS_TYPE_D_JUMP_DISTANCE,   /**< Modifies the distance from a jump point at which the pilot can jump. */

   /* Movement. */
   SS_TYPE_A_SPEED, /**< Speed modifier. */
   SS_TYPE_D_SPEED_MOD, /**< Speed multiplier. */
   SS_TYPE_A_TURN, /**< Turn modifier (in deg/s). */
   SS_TYPE_D_TURN_MOD, /**< Turn multiplier. */
   SS_TYPE_A_THRUST, /**< Acceleration modifier. */
   SS_TYPE_D_THRUST_MOD, /**< Acceleration multiplier. */
   /* Health. */
   SS_TYPE_A_ENERGY, /**< Energy modifier. */
   SS_TYPE_D_ENERGY_MOD, /**< Energy multiplier. */
   SS_TYPE_A_ENERGY_REGEN, /**< Energy regeneration modifier. */
   SS_TYPE_D_ENERGY_REGEN_MOD, /**< Energy regeneration multiplier. */
   SS_TYPE_A_ENERGY_REGEN_MALUS, /**< Flat energy regeneration modifier (not multiplied). */
   SS_TYPE_A_ENERGY_LOSS, /**< Flat energy modifier (not multiplied, applied linearly). */
   SS_TYPE_A_SHIELD, /**< Shield modifier. */
   SS_TYPE_D_SHIELD_MOD, /**< Shield multiplier. */
   SS_TYPE_A_SHIELD_REGEN, /**< Shield regeneration modifier. */
   SS_TYPE_D_SHIELD_REGEN_MOD, /**< Shield regeneration multiplier. */
   SS_TYPE_A_SHIELD_REGEN_MALUS, /**< Flat shield regeneration modifier (not multiplied). */
   SS_TYPE_A_ARMOUR, /**< Armour modifier. */
   SS_TYPE_D_ARMOUR_MOD, /**< Armour multiplier. */
   SS_TYPE_A_ARMOUR_REGEN, /**< Armour regeneration modifier. */
   SS_TYPE_D_ARMOUR_REGEN_MOD, /**< Armour regeneration multiplier. */
   SS_TYPE_A_ARMOUR_REGEN_MALUS, /**< Flat armour regeneration modifier (not multiplied). */

   SS_TYPE_A_CPU_MAX, /**< Maximum CPU modifier. */
   SS_TYPE_D_CPU_MOD, /**< CPU multiplier. */
   SS_TYPE_A_ENGINE_LIMIT, /**< Engine's mass limit. */

   SS_TYPE_P_ABSORB, /**< Damage absorption. */

   /* Nebula. */
   SS_TYPE_P_NEBULA_ABSORB_SHIELD, /**< Shield nebula resistance. */
   SS_TYPE_P_NEBULA_ABSORB_ARMOUR, /**< Armour nebula resistance. */

   SS_TYPE_I_FUEL, /**< Fuel bonus. */
   SS_TYPE_I_CARGO, /**< Cargo bonus. */
   SS_TYPE_D_CARGO_MOD, /**< Cargo space multiplier. */

   SS_TYPE_B_INSTANT_JUMP,    /**< Do not require brake or chargeup to jump. */
   SS_TYPE_B_REVERSE_THRUST,  /**< Ship slows down rather than turning on reverse. */
   SS_TYPE_B_ASTEROID_SCAN,   /**< Ship can gather informations from asteroids. */

   /*
    * End of list.
    */
   SS_TYPE_SENTINEL           /**< Sentinel for end of types. */
} ShipStatsType;

/**
 * @brief Represents relative ship statistics as a linked list.
 *
 * Doubles:
 *  These values are relative so something like -0.15 would be -15%.
 *
 * Absolute and Integers:
 *  These values are just absolute values.
 *
 * Booleans:
 *  Can only be 1.
 */
typedef struct ShipStatList_ {
   struct ShipStatList_ *next; /**< Next pointer. */

   int target;          /**< Whether or not it affects the target. */
   ShipStatsType type;  /**< Type of stat. */
   union {
      double d;         /**< Floating point data. */
      int    i;         /**< Integer data. */
   } d; /**< Stat data. */
} ShipStatList;


/**
 * @brief Represents ship statistics, properties ship can use.
 *
 * Doubles:
 *  These are normalized and centered around 1 so they are in the [0:2]
 *  range, with 1. being default. This value then modulates the stat's base
 *  value.
 *  Example:
 *   0.7 would lower by 30% the base value.
 *   1.2 would increase by 20% the base value.
 *
 * Absolute and Integers:
 *  Absolute values in whatever units it's meant to use.
 *
 * Booleans:
 *  1 or 0 values wher 1 indicates property is set.
 */
typedef struct ShipStats_ {
   /* Movement. */
   double speed;              /**< Speed modifier. */
   double turn;               /**< Turn modifier. */
   double thrust;             /**< Thrust modifier. */
   double speed_mod;          /**< Speed multiplier. */
   double turn_mod;           /**< Turn multiplier. */
   double thrust_mod;         /**< Thrust multiplier. */

   /* Health. */
   double energy;             /**< Energy modifier. */
   double energy_regen;       /**< Energy regeneration modifier. */
   double energy_mod;         /**< Energy multiplier. */
   double energy_regen_mod;   /**< Energy regeneration multiplier. */
   double energy_regen_malus; /**< Energy usage (flat). */
   double energy_loss;        /**< Energy modifier (flat and linear). */
   double shield;             /**< Shield modifier. */
   double shield_regen;       /**< Shield regeneration modifier. */
   double shield_mod;         /**< Shield multiplier. */
   double shield_regen_mod;   /**< Shield regeneration multiplier. */
   double shield_regen_malus; /**< Shield usage (flat). */
   double armour;             /**< Armour modifier. */
   double armour_regen;       /**< Armour regeneration modifier. */
   double armour_mod;         /**< Armour multiplier. */
   double armour_regen_mod;   /**< Armour regeneration multiplier. */
   double armour_regen_malus; /**< Armour regeneration (flat). */

   /* General */
   double cargo_mod;          /**< Cargo space multiplier. */
   double cpu_mod;            /**< CPU multiplier. */
   double cpu_max;            /**< CPU modifier. */
   double absorb;             /**< Flat damage absorption. */

   /* Freighter-type. */
   double jump_delay;      /**< Modulates the time that passes during a hyperspace jump. */
   double land_delay;      /**< Modulates the time that passes during landing. */
   double cargo_inertia;   /**< Lowers the effect of cargo mass. */

   /* Stealth. */
   double rdr_range;       /**< Radar range. */
   double rdr_jump_range;  /**< Jump detection range. */
   double rdr_range_mod;   /**< Radar range modifier. */
   double rdr_jump_range_mod; /**< Jump detection range modifier. */
   double rdr_enemy_range_mod; /**< Enemy radar range modifier. */

   /* Military type. */
   double heat_dissipation; /**< Global ship dissipation. */
   double stress_dissipation; /**< Global stress dissipation. */
   double mass_mod;        /**< Relative mass modification. */

   /* Launchers. */
   double launch_rate;     /**< Fire rate of launchers. */
   double launch_range;    /**< Range of launchers. */
   double launch_damage;   /**< Damage of launchers. */
   double ammo_capacity;   /**< Capacity of launchers. */
   double launch_reload;   /**< Reload rate of launchers. */
   double launch_dam_as_dis; /**< Damage as disable for launchers. */

   /* Fighter bays. */
   double fbay_damage;     /**< Fighter bay fighter damage (all weapons). */
   double fbay_health;     /**< Fighter bay fighter health (armour and shield). */
   double fbay_movement;   /**< Fighter bay fighter movement (thrust, turn, and speed). */
   double fbay_capacity;   /**< Capacity of fighter bays. */
   double fbay_rate;       /**< Launch rate of fighter bays. */
   double fbay_reload;     /**< Reload rate of fighters. */

   /* Fighter/Corvette type. */
   double fwd_heat;        /**< Heat of forward mounts. */
   double fwd_damage;      /**< Damage of forward mounts. */
   double fwd_firerate;    /**< Rate of fire of forward mounts. */
   double fwd_energy;      /**< Consumption rate of forward mounts. */
   double fwd_dam_as_dis;  /**< Damage as disable for forward mounts. */

   /* Destroyer/Cruiser type. */
   double tur_heat;        /**< Heat of turrets. */
   double tur_damage;      /**< Damage of turrets. */
   double tur_firerate;    /**< Rate of fire of turrets. */
   double tur_energy;      /**< Consumption rate of turrets. */
   double tur_dam_as_dis;  /**< Damage as disable for turrets. */

   /* Engine limits. */
   double engine_limit_rel; /**< Engine limit modifier. */
   double engine_limit;     /**< Engine limit. */

   /* Misc. */
   double nebu_absorb_shield; /**< Shield nebula resistance. */
   double nebu_absorb_armour; /**< Armour nebula resistance. */
   int misc_instant_jump;    /**< Do not require brake or chargeup to jump. */
   int misc_reverse_thrust;  /**< Slows down the ship instead of turning it around. */
   int misc_asteroid_scan;   /**< Able to scan asteroids. */
   int fuel;                  /**< Maximum fuel modifier. */
   int cargo;                 /**< Maximum cargo modifier. */
   double loot_mod;           /**< Boarding loot reward bonus. */
   double time_mod;           /**< Time dilation modifier. */
   double time_speedup;       /**< Makes the pilot operate at higher speeds. */
   double cooldown_time;      /**< Modifies cooldown time. */
   double jump_distance;      /**< Modifies how far the pilot can jump from the jump point. */
} ShipStats;


/*
 * Safety.
 */
int ss_check (void);

/*
 * Loading.
 */
ShipStatList* ss_listFromXML( xmlNodePtr node );
void ss_free( ShipStatList *ll );

/*
 * Manipulation
 */
int ss_statsInit( ShipStats *stats );
int ss_statsMerge( ShipStats *dest, const ShipStats *src );
int ss_statsModSingle( ShipStats *stats, const ShipStatList* list );
int ss_statsModFromList( ShipStats *stats, const ShipStatList* list );

/*
 * Lookup.
 */
const char* ss_nameFromType( ShipStatsType type );
size_t ss_offsetFromType( ShipStatsType type );
ShipStatsType ss_typeFromName( const char *name );
int ss_statsListDesc( const ShipStatList *ll, char *buf, int len, int newline );
int ss_statsDesc(const ShipStats *s, char *buf, int len, int newline,
      int composite);

/*
 * Manipulation.
 */
int ss_statsSet( ShipStats *s, const char *name, double value, int overwrite );
double ss_statsGet( const ShipStats *s, const char *name );
int ss_statsGetLua( lua_State *L, const ShipStats *s, const char *name, int internal );
int ss_statsGetLuaTable( lua_State *L, const ShipStats *s, int internal );


#endif /* SHIPSTATS_H */
