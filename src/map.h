/*
 * See Licensing and Copyright notice in naev.h
 */


#ifndef MAP_H
#  define MAP_H


#include "attributes.h"
#include "space.h"

#define MAP_WDWNAME     "wdwStarMap" /**< Map window name. */
#define MAP_WIDTH  MAX(1280, SCREEN_W - 100) /**< Map window width. */
#define MAP_HEIGHT  MAX(720, SCREEN_H - 100) /**< Map window height. */

typedef struct MapDecorator_ {
   glTexture* image;
   double x,y;
   int detection_radius;
   int auto_fade;
} MapDecorator;

/* init/exit */
int map_init (void);
void map_exit (void);

/* open the map window */
void map_open (void);
void map_close (void);
int map_isOpen (void);

/* misc */
StarSystem* map_getDestination( int *jumps );
void map_setZoom( double zoom );
void map_select( StarSystem *sys, char shifted );
StarSystem* map_getSelected(void);
void map_cleanup(void);
void map_clear (void);
void map_selectCur(void);
void map_jump (void);

/* manipulate universe stuff */
StarSystem **map_getJumpPath( const char *sysstart, const char *sysend, int ignore_known, int show_hidden,
                              StarSystem **old_data );
int map_map( const Outfit *map );
int map_isUseless( const Outfit* map );

/* Local map stuff. */
int localmap_map (void);
int localmap_isUseless (void);

/* shows a map at x, y (relative to wid) with size w,h  */
void map_show( int wid, int x, int y, int w, int h, double zoom );
int map_center( const char *sys );

/* Internal rendering sort of stuff. */
void map_renderParams( double bx, double by, double xpos, double ypos,
      double w, double h, double zoom, double *x, double *y, double *r );
void map_renderFactionDisks( double x, double y, double r, int editor, double alpha );
void map_renderSystemEnvironment( double x, double y, int editor, double alpha );
void map_renderDecorators( double x, double y, int editor, double alpha );
void map_renderJumps( double x, double y, double r, int editor );
void map_renderSystems( double bx, double by, double x, double y,
      double w, double h, double r, int editor );
void map_renderNames( double bx, double by, double x, double y,
      double w, double h, int editor, double alpha );
void map_updateFactionPresence( const unsigned int wid, const char *name, const StarSystem *sys, int omniscient );
int map_load (void);

#endif /* MAP_H */

