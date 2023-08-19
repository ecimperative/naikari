/*
 * See Licensing and Copyright notice in naev.h
 */

/**
 * @file background.c
 *
 * @brief Handles displaying backgrounds.
 */

/** @cond */
#include "naev.h"
/** @endcond */

#include "background.h"

#include "array.h"
#include "camera.h"
#include "conf.h"
#include "gui.h"
#include "log.h"
#include "ndata.h"
#include "nebula.h"
#include "nlua.h"
#include "nlua_bkg.h"
#include "nlua_col.h"
#include "nlua_tex.h"
#include "nlua_camera.h"
#include "nluadef.h"
#include "nstring.h"
#include "nxml.h"
#include "opengl.h"
#include "pause.h"
#include "player.h"
#include "rng.h"


/**
 * @brief Represents a background image like say a Nebula.
 */
typedef struct background_image_s {
   unsigned int id; /**< Background id. */
   glTexture *image; /**< Image to display. */
   double x; /**< X center of the image. */
   double y; /**< Y center of the image. */
   double move; /**< How many pixels it moves for each pixel the player moves. */
   double scale; /**< How the image should be scaled. */
   glColour col; /**< Colour to use. */
} background_image_t;
static background_image_t *bkg_image_arr_bk = NULL; /**< Background image array to display (behind stars). */
static background_image_t *bkg_image_arr_ft = NULL; /**< Background image array to display (in front of stars). */


static unsigned int bkg_idgen = 0; /**< ID generator for backgrounds. */


/**
 * @brief Backgrounds.
 */
static nlua_env bkg_cur_env = LUA_NOREF; /**< Current Lua state. */
static nlua_env bkg_def_env = LUA_NOREF; /**< Default Lua state. */
static int bkg_L_renderbg = LUA_NOREF; /**< Background rendering function. */
static int bkg_L_renderfg = LUA_NOREF; /**< Foreground rendering function. */
static int bkg_L_renderov = LUA_NOREF; /**< Overlay rendering function. */


/*
 * Background stars.
 */
#define STAR_BUF     250 /**< Area to leave around screen for stars, more = less repetition */
static gl_vbo *star_vertexVBO = NULL; /**< Star Vertex VBO. */
static unsigned int nstars = 0; /**< Total stars. */
static GLfloat star_x = 0.; /**< Star X movement. */
static GLfloat star_y = 0.; /**< Star Y movement. */


/*
 * Background dust (similar to stars, but scrolling).
 */
static gl_vbo *dust_vertexVBO = NULL; /**< Dust Vertex VBO. */
static unsigned int ndust; /**< Total dust particles. */
static GLfloat dust_x = 0.; /**< Dust X movement. */
static GLfloat dust_y = 0.; /**< Dust Y movement. */


/*
 * Prototypes.
 */
static void background_renderImages( background_image_t *bkg_arr );
static nlua_env background_create( const char *path );
static void background_clearCurrent (void);
static void background_clearImgArr( background_image_t **arr );
/* Sorting. */
static int bkg_compare( const void *p1, const void *p2 );
static void bkg_sort( background_image_t *arr );


/**
 * @brief Initializes background stars.
 *
 *    @param n Number of stars to add (per 1280x720 screen).
 */
void background_initStars(int n)
{
   unsigned int i;
   GLfloat w, h, hw, hh;
   double size;
   GLfloat *star_vertex;

   /* Calculate size. */
   size = SCREEN_W*SCREEN_H + STAR_BUF*STAR_BUF;
   size /= pow2(conf.zoom_far);

   /* Calculate star buffer. */
   w = (SCREEN_W + 2.*STAR_BUF);
   w += conf.zoom_stars * (w / conf.zoom_far - 1.);
   h = (SCREEN_H + 2.*STAR_BUF);
   h += conf.zoom_stars * (h / conf.zoom_far - 1.);
   hw = w / 2.;
   hh = h / 2.;

   /* Calculate stars. */
   size *= n;
   nstars = (unsigned int)(size/(1280.*720.));

   /* Create data. */
   star_vertex = malloc(nstars * sizeof(GLfloat) * 14);

   for (i=0; i<nstars; i++) {
      /* Set the position. */
      star_vertex[14*i+0] = RNGF()*w - hw;
      star_vertex[14*i+1] = RNGF()*h - hh;
      star_vertex[14*i+7] = star_vertex[14*i+0];
      star_vertex[14*i+8] = star_vertex[14*i+1];
      /* Set the brightness. */
      star_vertex[14*i+2] = RNGF()*0.6 + 0.2;
      star_vertex[14*i+9] = star_vertex[14*i+2];
      /* Set relative speed based on brightness (with variation). */
      star_vertex[14*i+3] = star_vertex[14*i+2] + RNGF()*0.2;
      star_vertex[14*i+10] = star_vertex[14*i+3];
      /* Set the color. */
      if (RNGF() < 0.25) {
         /* Less colorful star. */
         star_vertex[14*i+4] = RNGF()*0.2 + 0.8;
         star_vertex[14*i+5] = RNGF()*0.2 + 0.8;
         star_vertex[14*i+6] = RNGF()*0.2 + 0.8;
      }
      else {
         /* More colorful star. */
         star_vertex[14*i+4] = RNGF()*0.8 + 0.2;
         star_vertex[14*i+5] = RNGF()*0.8 + 0.2;
         star_vertex[14*i+6] = RNGF()*0.8 + 0.2;
      }
      star_vertex[14*i+11] = star_vertex[14*i+4];
      star_vertex[14*i+12] = star_vertex[14*i+5];
      star_vertex[14*i+13] = star_vertex[14*i+6];
   }

   /* Recreate VBO. */
   gl_vboDestroy(star_vertexVBO);
   star_vertexVBO = gl_vboCreateStatic(
         nstars * sizeof(GLfloat) * 14, star_vertex);

   free(star_vertex);
}


/**
 * @brief Initializes background dust.
 */
void background_initDust(void)
{
   /* Number of dust particles to add (per 1280x720 screen). */
   const int n = 20;
   unsigned int i;
   GLfloat w, h, hw, hh;
   double size;
   GLfloat *dust_vertex;

   /* Calculate size. */
   size = SCREEN_W*SCREEN_H + STAR_BUF*STAR_BUF;
   size /= pow2(conf.zoom_far);

   /* Calculate dust buffer. */
   w = (SCREEN_W + 2.*STAR_BUF);
   w += conf.zoom_stars * (w / conf.zoom_far - 1.);
   h = (SCREEN_H + 2.*STAR_BUF);
   h += conf.zoom_stars * (h / conf.zoom_far - 1.);
   hw = w / 2.;
   hh = h / 2.;

   /* Calculate dust particles. */
   size *= n;
   ndust = (unsigned int)(size/(1280.*720.));

   /* Create data. */
   dust_vertex = malloc(ndust * sizeof(GLfloat) * 14);

   for (i=0; i<ndust; i++) {
      /* Set the position. */
      dust_vertex[14*i+0] = RNGF()*w - hw;
      dust_vertex[14*i+1] = RNGF()*h - hh;
      dust_vertex[14*i+7] = dust_vertex[14*i+0];
      dust_vertex[14*i+8] = dust_vertex[14*i+1];
      /* Set the brightness. */
      dust_vertex[14*i+2] = RNGF()*0.2 + 0.05;
      dust_vertex[14*i+9] = dust_vertex[14*i+2];
      /* Set relative speed. Note: values less than 0.5 may flicker. */
      dust_vertex[14*i+3] = RNGF()*0.5 + 0.5;
      dust_vertex[14*i+10] = dust_vertex[14*i+3];
      /* Set the color. */
      if (RNGF() < 0.5) {
         /* Less colorful particle. */
         dust_vertex[14*i+4] = RNGF()*0.2 + 0.8;
         dust_vertex[14*i+5] = RNGF()*0.2 + 0.8;
         dust_vertex[14*i+6] = RNGF()*0.2 + 0.8;
      }
      else {
         /* More colorful particle. */
         dust_vertex[14*i+4] = RNGF()*0.8 + 0.2;
         dust_vertex[14*i+5] = RNGF()*0.8 + 0.2;
         dust_vertex[14*i+6] = RNGF()*0.8 + 0.2;
      }
      dust_vertex[14*i+11] = dust_vertex[14*i+4];
      dust_vertex[14*i+12] = dust_vertex[14*i+5];
      dust_vertex[14*i+13] = dust_vertex[14*i+6];
   }

   /* Recreate VBO. */
   gl_vboDestroy(dust_vertexVBO);
   dust_vertexVBO = gl_vboCreateStatic(
         ndust * sizeof(GLfloat) * 14, dust_vertex);

   free(dust_vertex);
}


/**
 * @brief Displaces the stars, useful with camera.
 */
void background_moveStars( double x, double y )
{
   star_x += (GLfloat)x / 1000.;
   star_y += (GLfloat)y / 1000.;
   dust_x += (GLfloat)x;
   dust_y += (GLfloat)y;

   /* Puffs also need moving. */
   nebu_movePuffs(x, y);
}


/**
 * @brief Renders the starry background or dust particles.
 *
 * For historical reasons, dust particles are treated as "stars" and
 * rendered with the "stars" shader. What was originally called "stars"
 * is these dust particles. These are now separate to reduce their
 * number and brightness, but still rendered with this function to avoid
 * duplicate code.
 *
 *    @param dt Current delta tick.
 *    @param is_stars Whether we are rendering stars. If true, we are
 *       rendering stars. If false, we are rendering dust particles.
 */
void background_renderStars(const double dt, const int is_stars)
{
   (void) dt;
   GLfloat h, w;
   GLfloat x, y, m;
   double z;
   gl_Matrix4 projection;
   int use_lines = 0;

   glUseProgram(shaders.stars.program);

   glLineWidth(1. / gl_screen.scale);
   glPointSize(1. / gl_screen.scale);

   /* Do some scaling for now. */
   z = 1.;
   if (!is_stars) {
      z = cam_getZoom();
      z = 1. * (1. - conf.zoom_stars) + z * conf.zoom_stars;
   }
   projection = gl_view_matrix;
   gl_Matrix4_Translate(&projection, SCREEN_W/2., SCREEN_H/2., 0);
   gl_Matrix4_Scale(&projection, z, z, 1);

   /* Calculate some dimensions. */
   w = (SCREEN_W + 2.*STAR_BUF);
   w += conf.zoom_stars * (w / conf.zoom_far - 1.);
   h = (SCREEN_H + 2.*STAR_BUF);
   h += conf.zoom_stars * (h / conf.zoom_far - 1.);

   /* Decide on shade mode. */
   x = 0;
   y = 0;
   if (!is_stars) {
      if ((player.p != NULL) && !player_isFlag(PLAYER_DESTROYED) &&
            !player_isFlag(PLAYER_CREATING)) {

         /* hyperspace fancy effects */
         if (pilot_isFlag(player.p,PILOT_HYPERSPACE)) {
            /* lines get longer the closer we are to finishing the jump */
            m = MAX(0, HYPERSPACE_STARS_BLUR - player.p->ptimer);
            m /= HYPERSPACE_STARS_BLUR;
            m *= HYPERSPACE_STARS_LENGTH;
            x = m*cos(VANGLE(player.p->solid->vel));
            y = m*sin(VANGLE(player.p->solid->vel));
         }
         else if (dt_mod * VMOD(player.p->solid->vel) > 500. ) {
            /* Very short lines tend to flicker horribly. A stock Llama
             * at 2x speed just so happens to make very short lines. A
             * 5px minimum is long enough to (mostly) alleviate the
             * flickering.
             */
            m = MAX(5, dt_mod*VMOD(player.p->solid->vel)/25. - 20);
            x = m*cos(VANGLE(player.p->solid->vel));
            y = m*sin(VANGLE(player.p->solid->vel));
         }
      }
      /* Check if we want to use lines or dots. If length is less than
       * 2, assume we want a dot. Note: the values we're checking here
       * are modified per-star by the relative speed component. */
      use_lines = (fabs(x) >= 2.) || (fabs(y) >= 2.);
      if (!use_lines) {
         x = y = 0.;
      }
   }

   /* Render. */
   glEnableVertexAttribArray(shaders.stars.vertex);
   glEnableVertexAttribArray(shaders.stars.brightness);
   glEnableVertexAttribArray(shaders.stars.relspeed);
   glEnableVertexAttribArray(shaders.stars.color);
   gl_vboActivateAttribOffset(is_stars ? star_vertexVBO : dust_vertexVBO,
         shaders.stars.vertex, 0, 2, GL_FLOAT,
         7 * sizeof(GLfloat));
   gl_vboActivateAttribOffset(is_stars ? star_vertexVBO : dust_vertexVBO,
         shaders.stars.brightness, 2 * sizeof(GLfloat), 1, GL_FLOAT,
         7 * sizeof(GLfloat));
   gl_vboActivateAttribOffset(is_stars ? star_vertexVBO : dust_vertexVBO,
         shaders.stars.relspeed, 3 * sizeof(GLfloat), 1, GL_FLOAT,
         7 * sizeof(GLfloat));
   gl_vboActivateAttribOffset(is_stars ? star_vertexVBO : dust_vertexVBO,
         shaders.stars.color, 4 * sizeof(GLfloat), 3, GL_FLOAT,
         7 * sizeof(GLfloat));

   gl_Matrix4_Uniform(shaders.stars.projection, projection);
   glUniform2f(shaders.stars.star_xy, is_stars ? star_x : dust_x,
         is_stars ? star_y : dust_y);
   glUniform2f(shaders.stars.wh, w, h);
   glUniform2f(shaders.stars.xy, x, y);
   glUniform1f(shaders.stars.scale, 1 / gl_screen.scale);
   glDrawArrays(use_lines ? GL_LINES : GL_POINTS, 0,
         is_stars ? nstars : ndust);

   /* Disable vertex array. */
   glDisableVertexAttribArray(shaders.stars.vertex);
   glDisableVertexAttribArray(shaders.stars.brightness);
   glDisableVertexAttribArray(shaders.stars.relspeed);
   glDisableVertexAttribArray(shaders.stars.color);

   glLineWidth(1. / gl_screen.scale);
   glPointSize(1. / gl_screen.scale);

   glUseProgram(0);

   /* Check for errors. */
   gl_checkErr();
}


/**
 * @brief Render the background.
 */
void background_render( double dt )
{
   if (bkg_L_renderbg != LUA_NOREF) {
      lua_rawgeti( naevL, LUA_REGISTRYINDEX, bkg_L_renderbg );
      lua_pushnumber( naevL, dt );
      if (nlua_pcall( bkg_cur_env, 1, 0 )) {
         WARN( _("Background script 'renderbg' error:\n%s"), lua_tostring(naevL,-1));
         lua_pop( naevL, 1 );
      }
   }

   background_renderStars(dt, 1);
   background_renderImages( bkg_image_arr_bk );
   background_renderStars(dt, 0);
   background_renderImages( bkg_image_arr_ft );

   if (bkg_L_renderfg != LUA_NOREF) {
      lua_rawgeti( naevL, LUA_REGISTRYINDEX, bkg_L_renderfg );
      lua_pushnumber( naevL, dt );
      if (nlua_pcall( bkg_cur_env, 1, 0 )) {
         WARN( _("Background script 'renderfg' error:\n%s"), lua_tostring(naevL,-1));
         lua_pop( naevL, 1 );
      }
   }
}


/**
 * @brief Renders the background overlay.
 */
void background_renderOverlay( double dt )
{
   if (bkg_L_renderov != LUA_NOREF) {
      lua_rawgeti( naevL, LUA_REGISTRYINDEX, bkg_L_renderov );
      lua_pushnumber( naevL, dt );
      if (nlua_pcall( bkg_cur_env, 1, 0 )) {
         WARN( _("Background script 'renderov' error:\n%s"), lua_tostring(naevL,-1));
         lua_pop( naevL, 1 );
      }
   }
}


/**
 * @brief Compares two different backgrounds and sorts them.
 */
static int bkg_compare( const void *p1, const void *p2 )
{
   background_image_t *bkg1, *bkg2;

   bkg1 = (background_image_t*) p1;
   bkg2 = (background_image_t*) p2;

   if (bkg1->move < bkg2->move)
      return -1;
   else if (bkg1->move > bkg2->move)
      return +1;
   return  0;
}


/**
 * @brief Sorts the backgrounds by movement.
 */
static void bkg_sort( background_image_t *arr )
{
   qsort( arr, array_size(arr), sizeof(background_image_t), bkg_compare );
}


/**
 * @brief Adds a new background image.
 */
unsigned int background_addImage( glTexture *image, double x, double y,
      double move, double scale, const glColour *col, int foreground )
{
   background_image_t *bkg, **arr;

   if (foreground)
      arr = &bkg_image_arr_ft;
   else
      arr = &bkg_image_arr_bk;

   /* See if must create. */
   if (*arr == NULL)
      *arr = array_create( background_image_t );

   /* Create image. */
   bkg         = &array_grow( arr );
   bkg->id     = ++bkg_idgen;
   bkg->image  = gl_dupTexture(image);
   bkg->x      = x;
   bkg->y      = y;
   bkg->move   = move;
   bkg->scale  = scale;
   bkg->col    = (col!=NULL) ? *col : cWhite;

   /* Sort if necessary. */
   bkg_sort( *arr );

   return bkg_idgen;
}


/**
 * @brief Renders the background images.
 */
static void background_renderImages( background_image_t *bkg_arr )
{
   int i;
   background_image_t *bkg;
   double px, py, x, y, xs, ys, gx, gy;
   glColour col;

   /* Skip rendering altogether if disabled. */
   if (conf.bg_brightness <= 0.)
      return;

   /* Render images in order. */
   for (i=0; i<array_size(bkg_arr); i++) {
      bkg = &bkg_arr[i];

      cam_getPos(&px, &py);
      x = px + (bkg->x-px)*bkg->move - bkg->scale*bkg->image->sw/2.;
      y = py + (bkg->y-py)*bkg->move - bkg->scale*bkg->image->sh/2.;

      gui_getOffset(&gx, &gy);

      /* calculate position */
      xs = (x-px) + gx + SCREEN_W/2.;
      ys = (y-py) + gy + SCREEN_H/2.;

      col.r = bkg->col.r * conf.bg_brightness;
      col.g = bkg->col.g * conf.bg_brightness;
      col.b = bkg->col.b * conf.bg_brightness;
      col.a = bkg->col.a;
      gl_blitScale(bkg->image, xs, ys,
            bkg->image->sw * bkg->scale, bkg->image->sh * bkg->scale, &col);
   }
}


/**
 * @brief Creates a background Lua state from a script.
 */
static nlua_env background_create( const char *name )
{
   size_t bufsize;
   char path[PATH_MAX];
   char *buf;
   nlua_env env;

   /* Create file name. */
   snprintf( path, sizeof(path), BACKGROUND_PATH"%s.lua", name );

   /* Create the Lua env. */
   env = nlua_newEnv(1);
   nlua_loadStandard(env);
   nlua_loadTex(env);
   nlua_loadCol(env);
   nlua_loadBackground(env);
   nlua_loadCamera(env);

   /* Open file. */
   buf = ndata_read( path, &bufsize );
   if (buf == NULL) {
      WARN( _("Background script '%s' not found."), path);
      nlua_freeEnv(env);
      return LUA_NOREF;
   }

   /* Load file. */
   if (nlua_dobufenv(env, buf, bufsize, path) != 0) {
      WARN( _("Error loading background file: %s\n"
            "%s\n"
            "Most likely Lua file has improper syntax, please check"),
            path, lua_tostring(naevL,-1));
      free(buf);
      nlua_freeEnv(env);
      return LUA_NOREF;
   }
   free(buf);

   return env;
}


/**
 * @brief Initializes the background system.
 */
int background_init (void)
{
   /* Load Lua. */
   bkg_def_env = background_create( "default" );

   return 0;
}


/**
 * @brief Loads a background script by name.
 */
int background_load( const char *name )
{
   int ret;
   nlua_env env;
   const char *err;

   /* Free if exists. */
   background_clearCurrent();

   /* Load default. */
   if (name == NULL)
      bkg_cur_env = bkg_def_env;
   /* Load new script. */
   else
      bkg_cur_env = background_create( name );

   /* Comfort. */
   env = bkg_cur_env;
   if (env == LUA_NOREF)
      return -1;

   /* Run Lua. */
   nlua_getenv(env,"background");
   ret = nlua_pcall(env, 0, 0);
   if (ret != 0) { /* error has occurred */
      err = (lua_isstring(naevL,-1)) ? lua_tostring(naevL,-1) : NULL;
      WARN( _("Background -> 'background' : %s"),
            (err) ? err : _("unknown error"));
      lua_pop(naevL, 1);
   }

   /* See if there are render functions. */
   bkg_L_renderbg = nlua_refenv( env, "renderbg" );
   bkg_L_renderfg = nlua_refenv( env, "renderfg" );
   bkg_L_renderov = nlua_refenv( env, "renderov" );

   return ret;
}


/**
 * @brief Destroys the current running background script.
 */
static void background_clearCurrent (void)
{
   if (bkg_cur_env != bkg_def_env) {
      if (bkg_cur_env != LUA_NOREF)
         nlua_freeEnv( bkg_cur_env );
   }
   bkg_cur_env = LUA_NOREF;

   luaL_unref( naevL, LUA_REGISTRYINDEX, bkg_L_renderbg );
   luaL_unref( naevL, LUA_REGISTRYINDEX, bkg_L_renderfg );
   luaL_unref( naevL, LUA_REGISTRYINDEX, bkg_L_renderov );
   bkg_L_renderbg = LUA_NOREF;
   bkg_L_renderfg = LUA_NOREF;
   bkg_L_renderov = LUA_NOREF;
}


/**
 * @brief Cleans up the background stuff.
 */
void background_clear (void)
{
   /* Destroy current background script. */
   background_clearCurrent();

   /* Clear the backgrounds. */
   background_clearImgArr( &bkg_image_arr_bk );
   background_clearImgArr( &bkg_image_arr_ft );
}


/**
 * @brief Clears a background image array.
 *
 *    @param arr Array to clear.
 */
static void background_clearImgArr( background_image_t **arr )
{
   int i;
   background_image_t *bkg;

   for (i=0; i<array_size(*arr); i++) {
      bkg = &((*arr)[i]);
      gl_freeTexture( bkg->image );
   }

   /* Erase it all. */
   array_erase( arr, array_begin(*arr), array_end(*arr) );
}


/**
 * @brief Cleans up and frees memory after the backgrounds.
 */
void background_free (void)
{
   /* Free the Lua. */
   background_clear();
   if (bkg_def_env != LUA_NOREF)
      nlua_freeEnv( bkg_def_env );
   bkg_def_env = LUA_NOREF;

   /* Free the images. */
   array_free( bkg_image_arr_ft );
   bkg_image_arr_ft = NULL;
   array_free( bkg_image_arr_bk );
   bkg_image_arr_bk = NULL;

   /* Free the Lua. */
   if (bkg_cur_env != LUA_NOREF)
      nlua_freeEnv( bkg_cur_env );
   bkg_cur_env = LUA_NOREF;

   gl_vboDestroy(star_vertexVBO);
   star_vertexVBO = NULL;
   gl_vboDestroy(dust_vertexVBO);
   dust_vertexVBO = NULL;

   nstars = 0;
   ndust = 0;
}

/**
 * @brief returns the background images, and number of these
 */
glTexture** background_getTextures (void)
{
  int i;
  glTexture **imgs;

  imgs = array_create_size( glTexture*, array_size( bkg_image_arr_bk ));
  for ( i=0; i<array_size(bkg_image_arr_bk); i++ )
    array_push_back( &imgs, gl_dupTexture(bkg_image_arr_bk[i].image) );
  return imgs;
}
