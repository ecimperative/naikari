/*
 * See Licensing and Copyright notice in naev.h
 */


/** @cond */
#include <float.h>
#include "SDL.h"
/** @endcond */

#include "map_overlay.h"

#include "array.h"
#include "conf.h"
#include "font.h"
#include "gui.h"
#include "input.h"
#include "log.h"
#include "naev.h"
#include "nstring.h"
#include "opengl.h"
#include "pilot.h"
#include "player.h"
#include "space.h"


/**
 * Structure for map overlay size.
 */
typedef struct MapOverlayRadiusConstraint_ {
   int i; /**< this radius... */
   int j; /**< plus this radius... */
   double dist; /**< ... is at most this big. */
} MapOverlayRadiusConstraint;


/**
 * @brief An overlay map marker.
 */
typedef struct ovr_marker_s {
   unsigned int id; /**< ID of the marker. */
   char *text; /**< Marker display text. */
   int type; /**< Marker type. */
   Vector2d pos; /**< Marker position. */
   MapOverlayPos mo; /**< Overlay layout data. */
} ovr_marker_t;
static unsigned int mrk_idgen = 0; /**< ID generator for markers. */
static ovr_marker_t *ovr_markers = NULL; /**< Overlay markers. */


static Uint32 ovr_opened = 0; /**< Time last opened. */
static int ovr_open = 0; /**< Is the overlay open? */
static double ovr_res = 10.; /**< Resolution. */
static const double ovr_text_pixbuf = 5.; /**< Extra margin around overlay text. */
/* Rem: high pix_buffer ovr_text_pixbuff allows to do less iterations. */

/*
 * Prototypes
 */
static void force_collision( float *ox, float *oy,
      float x, float y, float w, float h,
      float mx, float my, float mw, float mh );
static void ovr_optimizeLayout( int items, const Vector2d** pos,
      MapOverlayPos** mo );
static void ovr_refresh_uzawa_overlap( float *forces_x, float *forces_y,
      float x, float y, float w, float h, const Vector2d** pos,
      MapOverlayPos** mo, int items, int self,
      float *offx, float *offy, float *offdx, float *offdy );
/* Markers. */
static void ovr_mrkRenderAll( double res );
static void ovr_mrkCleanup(  ovr_marker_t *mrk );
static ovr_marker_t *ovr_mrkNew (void);


/**
 * @brief Check to see if the map overlay is open.
 */
int ovr_isOpen (void)
{
   return !!ovr_open;
}

/**
 * @brief Handles input to the map overlay.
 */
int ovr_input( SDL_Event *event )
{
   int mx, my;
   double x, y;

   /* We only want mouse events. */
   if (event->type != SDL_MOUSEBUTTONDOWN)
      return 0;

   /* Player must not be NULL. */
   if (player_isFlag(PLAYER_DESTROYED) || (player.p == NULL))
      return 0;

   /* Player must not be dead. */
   if (pilot_isFlag(player.p, PILOT_DEAD))
      return 0;

   /* Mouse targeting only uses left and right buttons. */
   if (event->button.button != SDL_BUTTON_LEFT &&
            event->button.button != SDL_BUTTON_RIGHT)
      return 0;

   /* Translate from window to screen. */
   mx = event->button.x;
   my = event->button.y;
   gl_windowToScreenPos( &mx, &my, mx, my );

   /* Click must be within overlay bounds. */
   if ((mx < gui_getMapOverlayBoundLeft())
         || (mx > gui_getMapOverlayBoundLeft() + map_overlay_width())
         || (my < gui_getMapOverlayBoundBottom())
         || (my > gui_getMapOverlayBoundBottom() + map_overlay_height()))
      return 0;

   /* Translate to space coords. */
   x = ((double)mx - (double)map_overlay_center_x()) * ovr_res;
   y = ((double)my - (double)map_overlay_center_y()) * ovr_res;

   return input_clickPos( event, x, y, 1., 10. * ovr_res, 15. * ovr_res );
}


/**
 * @brief Refreshes the map overlay recalculating the dimensions it should have.
 *
 * This should be called if the planets or the likes change at any given time.
 */
void ovr_refresh (void)
{
   double max_x, max_y;
   int i, items, jumpitems, planetitems;
   Planet *pnt;
   JumpPoint *jp;
   ovr_marker_t *mrk;
   const Vector2d **pos;
   MapOverlayPos **mo;
   char buf[STRMAX_SHORT];

   /* Must be open. */
   if (!ovr_isOpen())
      return;

   /* Calculate max size. */
   items = 0;
   pos = calloc(array_size(cur_system->jumps)
            + array_size(cur_system->planets)
            + array_size(ovr_markers),
         sizeof(Vector2d*));
   mo = calloc(array_size(cur_system->jumps)
            + array_size(cur_system->planets)
            + array_size(ovr_markers),
         sizeof(MapOverlayPos*));
   max_x = 0.;
   max_y = 0.;
   for (i=0; i<array_size(cur_system->jumps); i++) {
      jp = &cur_system->jumps[i];
      max_x = MAX( max_x, ABS(jp->pos.x) );
      max_y = MAX( max_y, ABS(jp->pos.y) );
      if (!jp_isUsable(jp) || !jp_isKnown(jp))
         continue;
      /* Initialize the map overlay stuff. */
      snprintf(buf, sizeof(buf), "%s%s", jump_getSymbol(jp),
            (sys_isKnown(jp->target) || sys_isMarked(jp->target)) ?
               _(jp->target->name) : _("Unknown"));
      pos[items] = &jp->pos;
      mo[items]  = &jp->mo;
      mo[items]->radius = jumppoint_gfx->sw / 2.;
      mo[items]->text_width = gl_printWidthRaw(&gl_smallFont, buf);
      mo[items]->noshrink = 0;
      items++;
   }
   jumpitems = items;
   for (i=0; i<array_size(cur_system->planets); i++) {
      pnt = cur_system->planets[i];
      max_x = MAX( max_x, ABS(pnt->pos.x) );
      max_y = MAX( max_y, ABS(pnt->pos.y) );
      if ((pnt->real != ASSET_REAL) || !planet_isKnown(pnt))
         continue;
      /* Initialize the map overlay stuff. */
      snprintf( buf, sizeof(buf), "%s%s", planet_getSymbol(pnt), _(pnt->name) );
      pos[items] = &pnt->pos;
      mo[items]  = &pnt->mo;
      mo[items]->radius = pnt->radius / 2.;  /* halved since it's awkwardly large if drawn to scale relative to the player. */
      /* +2.0 represents a margin used by the SDF shader. */
      mo[items]->text_width = gl_printWidthRaw( &gl_smallFont, buf );
      mo[items]->noshrink = 0;
      items++;
   }
   planetitems = items;
   for (i=0; i<array_size(ovr_markers); i++) {
      mrk = &ovr_markers[i];
      max_x = MAX(max_x, ABS(mrk->pos.x));
      max_y = MAX(max_y, ABS(mrk->pos.y));
      if (mrk->text == NULL)
         continue;
      pos[items] = &mrk->pos;
      mo[items] = &mrk->mo;
      mo[items]->radius = 15.;
      mo[items]->text_width = gl_printWidthRaw(&gl_smallFont, mrk->text);
      mo[items]->noshrink = 1;
      items++;
   }

   /* We need to calculate the radius of the rendering from the maximum radius of the system. */
   ovr_res = 2. * 1.2 * MAX( max_x / map_overlay_width(), max_y / map_overlay_height() );
   for (i=0; i<items; i++) {
      if (i >= planetitems)
         break;
      
      mo[i]->radius = MAX(2. + mo[i]->radius/ovr_res,
            (i < jumpitems) ? 5. : 7.5);
   }

   /* Nothing in the system so we just set a default value. */
   if (items == 0)
      ovr_res = 50.;

   /* Compute text overlap and try to minimize it. */
   ovr_optimizeLayout( items, pos, mo );

   /* Free the moos. */
   free( mo );
   free( pos );
}


/**
 * @brief Makes a best effort to fit the given assets' overlay indicators and labels fit without collisions.
 */
static void ovr_optimizeLayout( int items, const Vector2d** pos, MapOverlayPos** mo )
{
   int i, j, k, iter;
   float cx, cy, r, sx, sy;
   float x, y, w, h, mx, my, mw, mh;
   float fx, fy, best, bx, by, val;
   float *forces_xa, *forces_ya, *off_buffx, *off_buffy, *off_0x, *off_0y, old_bx, old_by, *off_dx, *off_dy;

   /* Parameters for the map overlay optimization. */
   const int max_iters = 15;    /**< Maximum amount of iterations to do. */
   const float kx      = .015;  /**< x softness factor. */
   const float ky      = .045;  /**< y softness factor (moving along y is more likely to be the right solution). */
   const float eps_con = 1.3;   /**< Convergence criterion. */

   if (items <= 0)
      return;

   /* Fix radii which fit together. */
   MapOverlayRadiusConstraint cur, *fits = array_create(MapOverlayRadiusConstraint);
   uint8_t *must_shrink = malloc( items );
   for (cur.i=0; cur.i<items; cur.i++) {
      if (mo[cur.i]->noshrink)
         continue;
      for (cur.j=cur.i+1; cur.j<items; cur.j++) {
         if (mo[cur.j]->noshrink)
            continue;
         cur.dist = hypot( pos[cur.i]->x - pos[cur.j]->x, pos[cur.i]->y - pos[cur.j]->y ) / ovr_res;
         if (cur.dist < mo[cur.i]->radius + mo[cur.j]->radius)
            array_push_back( &fits, cur );
      }
   }
   while (array_size( fits ) > 0) {
      float shrink_factor = 0.;
      memset(must_shrink, 0, items);
      for (i = 0; i<array_size(fits); i++) {
         /* Abort shrinking if we're about to divide by zero. */
         if (mo[fits[i].i]->radius + mo[fits[i].j]->radius == 0.) {
            shrink_factor = 0.;
            break;
         }
         r = fits[i].dist / (mo[fits[i].i]->radius + mo[fits[i].j]->radius);
         if (r >= 1)
            array_erase( &fits, &fits[i], &fits[i+1] );
         else {
            shrink_factor = MAX( shrink_factor, r - FLT_EPSILON );
            must_shrink[fits[i].i] = must_shrink[fits[i].j] = 1;
         }
      }
      for (i=0; i<items; i++) {
         if (must_shrink[i])
            mo[i]->radius *= shrink_factor;
      }
      /* If shrink_factor is zero and we don't exit now, we're in an
       * infinite loop condition. */
      if (shrink_factor == 0.)
         break;
   }
   free( must_shrink );
   array_free( fits );

   /* Limit shrinkage. */
   for (i=0; i<items; i++)
      mo[i]->radius = MAX( mo[i]->radius, 4 );

   /* Initialization offset list. */
   off_0x = calloc( items, sizeof(float) );
   off_0y = calloc( items, sizeof(float) );

   /* Initialize all items. */
   for (i=0; i<items; i++) {
      /* Test to see what side is best to put the text on.
       * We actually compute the text overlap also so hopefully it will alternate
       * sides when stuff is clustered together. */

      x = pos[i]->x/ovr_res - ovr_text_pixbuf;
      y = pos[i]->y/ovr_res - ovr_text_pixbuf;
      w = mo[i]->text_width + 2*ovr_text_pixbuf;
      h = gl_smallFont.h + 2*ovr_text_pixbuf;

      const float tx[4] = {
         mo[i]->radius + ovr_text_pixbuf + .1,
         -mo[i]->radius - .1 - w,
         -mo[i]->text_width / 2.,
         -mo[i]->text_width / 2.};
      const float ty[4] = {
         -gl_smallFont.h / 2.,
         -gl_smallFont.h / 2.,
         mo[i]->radius + ovr_text_pixbuf + .1,
         -mo[i]->radius - .1 - h};

      /* Check all combinations. */
      bx = 0.;
      by = 0.;
      best = HUGE_VALF;
      for (k=0; k<4; k++) {
         val = 0.;

         /* Test intersection with the planet indicators. */
         for (j=0; j<items; j++) {
            fx = fy = 0.;
            mw = 2.*mo[j]->radius;
            mh = mw;
            mx = pos[j]->x/ovr_res - mw/2.;
            my = pos[j]->y/ovr_res - mh/2.;

            force_collision( &fx, &fy, x+tx[k], y+ty[k], w, h, mx, my, mw, mh );

            val += ABS(fx) + ABS(fy);
         }
         /* Keep best. */
         if (k == 0 || val < best) {
            bx = tx[k];
            by = ty[k];
            best = val;
         }
         if (val==0.)
            break;
      }

      /* Store offsets. */
      off_0x[i] = bx;
      off_0y[i] = by;
   }

   /* Uzawa optimization algorithm.
    * We minimize the (weighted) L2 norm of vector of offsets and radius changes
    * Under the constraint of no interpenetration
    * As the algorithm is Uzawa, this constraint won't necessary be attained.
    * This is similar to a contact problem is mechanics. */

   /* Initialize the matrix that stores the dual variables (forces applied between objects).
    * matrix is column-major, this means it is interesting to store in each column the forces
    * recieved by a given object. Then these forces are summed to obtain the total force on the object.
    * Odd lines are forces from objects and Even lines from other texts. */

   forces_xa = calloc( 2*items*items, sizeof(float) );
   forces_ya = calloc( 2*items*items, sizeof(float) );

   /* And buffer lists. */
   off_buffx = calloc( items, sizeof(float) );
   off_buffy = calloc( items, sizeof(float) );
   off_dx    = calloc( items, sizeof(float) );
   off_dy    = calloc( items, sizeof(float) );

   /* Main Uzawa Loop. */
   for (iter=0; iter<max_iters; iter++) {
      val = 0; /* This stores the stagnation indicator. */
      for (i=0; i<items; i++) {
         cx = pos[i]->x / ovr_res;
         cy = pos[i]->y / ovr_res;
         /* Compute the forces. */
         ovr_refresh_uzawa_overlap(
               forces_xa, forces_ya,
               cx + off_dx[i] + off_0x[i] - ovr_text_pixbuf,
               cy + off_dy[i] + off_0y[i] - ovr_text_pixbuf,
               mo[i]->text_width + 2*ovr_text_pixbuf,
               gl_smallFont.h + 2*ovr_text_pixbuf,
               pos, mo, items, i, off_0x, off_0y, off_dx, off_dy );

         /* Do the sum. */
         sx = sy = 0.;
         for (j=0; j<2*items; j++) {
            sx += forces_xa[2*items*i+j];
            sy += forces_ya[2*items*i+j];
         }

         /* Store old version of buffers. */
         old_bx = off_buffx[i];
         old_by = off_buffy[i];

         /* Update positions (in buffer). Diagonal stiffness. */
         off_buffx[i] = kx * sx;
         off_buffy[i] = ky * sy;

         val = MAX( val, ABS(old_bx-off_buffx[i]) + ABS(old_by-off_buffy[i]) );
      }

      /* Offsets are actually updated once the first loop is over. */
      for (i=0; i<items; i++) {
         off_dx[i] = off_buffx[i];
         off_dy[i] = off_buffy[i];
      }

      /* Test stagnation. */
      if (val <= eps_con)
         break;
   }

   /* Permanently add the initialization offset to total offset. */
   for (i=0; i<items; i++) {
      mo[i]->text_offx = off_dx[i] + off_0x[i];
      mo[i]->text_offy = off_dy[i] + off_0y[i];
   }

   /* Free the forces matrix and the various buffers. */
   free( forces_xa );
   free( forces_ya );
   free( off_buffx );
   free( off_buffy );
   free( off_0x );
   free( off_0y );
   free( off_dx );
   free( off_dy );
}


/**
 * @brief Compute a collision between two rectangles and direction to deduce the force.
 */
static void force_collision( float *ox, float *oy,
      float x, float y, float w, float h,
      float mx, float my, float mw, float mh )
{

   /* No contact because of y offset (+tolerance). */
   if ((y+h < my+ovr_text_pixbuf) || (y+ovr_text_pixbuf > my+mh))
      *ox = 0;
   else {
      /* Case A is left of B. */
      if (x+.5*w < mx+.5*mw) {
         *ox += mx-(x+w);
         *ox = MIN(0., *ox);
      }
      /* Case A is to the right of B. */
      else {
         *ox += (mx+mw)-x;
         *ox = MAX(0., *ox);
      }
   }

   /* No contact because of x offset (+tolerance). */
   if ((x+w < mx+ovr_text_pixbuf) || (x+ovr_text_pixbuf > mx+mw))
      *oy = 0;
   else {
      /* Case A is below B. */
      if (y+.5*h < my+.5*mh) {
         *oy += my-(y+h);
         *oy = MIN(0., *oy);
      }
      /* Case A is above B. */
      else {
         *oy += (my+mh)-y;
         *oy = MAX(0., *oy);
      }
   }
}


/**
 * @brief Compute how an element overlaps with text and force to move away.
 */
static void ovr_refresh_uzawa_overlap( float *forces_x, float *forces_y,
      float x, float y, float w, float h, const Vector2d** pos,
      MapOverlayPos** mo, int items, int self,
      float *offx, float *offy, float *offdx, float *offdy )
{
   int i;
   float mx, my, mw, mh;
   const float pb2 = ovr_text_pixbuf*2.;

   for (i=0; i<items; i++) {
      /* Collisions with planet circles and jp triangles (odd indices). */
      mw = 2.*mo[i]->radius;
      mh = mw;
      mx = pos[i]->x/ovr_res - mw/2.;
      my = pos[i]->y/ovr_res - mh/2.;
      force_collision( &forces_x[2*items*self+2*i+1], &forces_y[2*items*self+2*i+1], x, y, w, h, mx, my, mw, mh );

      if (i == self)
         continue;

      /* Collisions with other texts (even indices) */
      mw = mo[i]->text_width + pb2;
      mh = gl_smallFont.h + pb2;
      mx = pos[i]->x/ovr_res + offdx[i] + offx[i] - ovr_text_pixbuf;
      my = pos[i]->y/ovr_res + offdy[i] + offy[i] - ovr_text_pixbuf;
      force_collision( &forces_x[2*items*self+2*i], &forces_y[2*items*self+2*i], x, y, w, h, mx, my, mw, mh );
   }
}


/**
 * @brief Properly opens or closes the overlay map.
 *
 *    @param open Whether or not to open it.
 */
void ovr_setOpen( int open )
{
   if (open && !ovr_open) {
      ovr_open = 1;
      input_mouseShow();
   }
   else if (ovr_open) {
      ovr_open = 0;
      input_mouseHide();
   }
}


/**
 * @brief Handles a keypress event.
 *
 *    @param type Type of event.
 */
void ovr_key( int type )
{
   if (type > 0) {
      if (ovr_open)
         ovr_setOpen(0);
      else {
         ovr_setOpen(1);

         /* Refresh overlay size. */
         ovr_refresh();
         ovr_opened = SDL_GetTicks();
      }
   }
   else if (type < 0) {
      if (SDL_GetTicks() - ovr_opened > 300)
         ovr_setOpen(0);
   }
}


/**
 * @brief Renders the overlay map.
 *
 *    @param dt Current delta tick.
 */
void ovr_render( double dt )
{
   (void) dt;
   int i, j;
   Pilot * const *pstk;
   AsteroidAnchor *ast;
   double w, h, res;
   double x, y;
   glColour c;

   /* Must be open. */
   if (!ovr_open)
      return;

   /* Player must be alive. */
   if (player_isFlag( PLAYER_DESTROYED ) || (player.p == NULL))
      return;

   /* Default values. */
   w     = map_overlay_width();
   h     = map_overlay_height();
   res   = ovr_res;

   /* The overlay might draw text over other text, e.g. messages. */
   glClear(GL_DEPTH_BUFFER_BIT);

   /* First render the background overlay. */
   if (conf.map_overlay_opacity > 0.) {
      x = gui_getMapOverlayBoundLeft();
      y = gui_getMapOverlayBoundBottom();
      c.r = c.g = c.b = 0.;
      c.a = conf.map_overlay_opacity;
      gl_renderRect(x, y, w, h, &c);
      c.a *= 2;
      gl_renderRectEmpty(x, y, w, h, &c);
   }

   /* Render planets. */
   for (i=0; i<array_size(cur_system->planets); i++)
      if ((cur_system->planets[ i ]->real == ASSET_REAL) && (i != player.p->nav_planet))
         gui_renderPlanet( i, RADAR_RECT, w, h, res, 1 );
   if (player.p->nav_planet > -1)
      gui_renderPlanet( player.p->nav_planet, RADAR_RECT, w, h, res, 1 );

   /* Render jump points. */
   for (i=0; i<array_size(cur_system->jumps); i++)
      if ((i != player.p->nav_hyperspace) && !jp_isFlag(&cur_system->jumps[i], JP_EXITONLY))
         gui_renderJumpPoint( i, RADAR_RECT, w, h, res, 1 );
   if (player.p->nav_hyperspace > -1)
      gui_renderJumpPoint( player.p->nav_hyperspace, RADAR_RECT, w, h, res, 1 );

   /* Render markers. */
   ovr_mrkRenderAll(res);

   /* Render asteroids. */
   for (i=0; i<array_size(cur_system->asteroids); i++) {
      ast = &cur_system->asteroids[i];
      for (j=0; j<ast->nb; j++)
         gui_renderAsteroid(&ast->asteroids[j], RADAR_RECT, w, h, res, 1);
   }

   /* Render pilots. */
   pstk  = pilot_getAll();
   j     = 0;
   for (i=0; i<array_size(pstk); i++) {
      if (pstk[i]->id == PLAYER_ID) /* Skip player. */
         continue;
      if (pstk[i]->id == player.p->target)
         j = i;
      else
         gui_renderPilot( pstk[i], RADAR_RECT, w, h, res, 1 );
   }
   /* Render the targeted pilot */
   if (j!=0)
      gui_renderPilot( pstk[j], RADAR_RECT, w, h, res, 1 );

   /* Check if player has goto target. */
   if (player_isFlag(PLAYER_AUTONAV) && (player.autonav == AUTONAV_POS_APPROACH)) {
      x = player.autonav_pos.x / res + map_overlay_center_x();
      y = player.autonav_pos.y / res + map_overlay_center_y();
      gl_renderCross(x, y, 7., &cRadar_hilight);
   }

   /* Render the player. */
   gui_renderPlayer(w, h, res, 1);
}


/**
 * @brief Renders all the markers.
 *
 *    @param res Resolution to render at.
 */
static void ovr_mrkRenderAll( double res )
{
   int i;
   ovr_marker_t *mrk;
   double x, y;

   for (i=0; i<array_size(ovr_markers); i++) {
      mrk = &ovr_markers[i];

      x = mrk->pos.x / res + map_overlay_center_x();
      y = mrk->pos.y / res + map_overlay_center_y();
      gui_renderMarker(x, y);

      if (mrk->text != NULL) {
         gl_printMarkerRaw(&gl_smallFont,
               x + mrk->mo.text_offx,
               y + mrk->mo.text_offy,
               &cRadar_hilight, mrk->text);
      }
   }
}


/**
 * @brief Frees up and clears all marker related stuff.
 */
void ovr_mrkFree (void)
{
   /* Clear markers. */
   ovr_mrkClear();

   /* Free array. */
   array_free( ovr_markers );
   ovr_markers = NULL;
}


/**
 * @brief Clears the current markers.
 */
void ovr_mrkClear (void)
{
   int i;
   for (i=0; i<array_size(ovr_markers); i++)
      ovr_mrkCleanup( &ovr_markers[i] );
   array_erase( &ovr_markers, array_begin(ovr_markers), array_end(ovr_markers) );
}


/**
 * @brief Clears up after an individual marker.
 *
 *    @param mrk Marker to clean up after.
 */
static void ovr_mrkCleanup( ovr_marker_t *mrk )
{
   free( mrk->text );
   mrk->text = NULL;
}


/**
 * @brief Creates a new marker.
 *
 *    @return The newly created marker.
 */
static ovr_marker_t *ovr_mrkNew (void)
{
   ovr_marker_t *mrk;

   if (ovr_markers == NULL)
      ovr_markers = array_create(  ovr_marker_t );

   mrk = &array_grow( &ovr_markers );
   memset( mrk, 0, sizeof( ovr_marker_t ) );
   mrk->id = ++mrk_idgen;
   return mrk;
}


/**
 * @brief Creates a new point marker.
 *
 *    @param text Text to display with the marker.
 *    @param x X position of the marker.
 *    @param y Y position of the marker.
 *    @return The id of the newly created marker.
 */
unsigned int ovr_mrkAddPoint( const char *text, double x, double y )
{
   ovr_marker_t *mrk;

   mrk = ovr_mrkNew();
   mrk->type = 0;
   if (text != NULL)
      mrk->text = strdup( text );
   mrk->pos.x = x;
   mrk->pos.y = y;

   ovr_refresh();

   return mrk->id;
}


/**
 * @brief Removes a marker by id.
 *
 *    @param id ID of the marker to remove.
 */
void ovr_mrkRm( unsigned int id )
{
   int i;
   for (i=0; i<array_size(ovr_markers); i++) {
      if (id!=ovr_markers[i].id)
         continue;
      ovr_mrkCleanup( &ovr_markers[i] );
      array_erase( &ovr_markers, &ovr_markers[i], &ovr_markers[i+1] );
      break;
   }

   ovr_refresh();
}


