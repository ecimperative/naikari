/*
 * See Licensing and Copyright notice in naev.h
 */

/**
 * @file opengl.c
 *
 * @brief This file handles most of the more generic opengl functions.
 *
 * The main way to work with opengl in naev is to create glTextures and then
 *  use the blit functions to draw them on the screen.  This system will
 *  handle relative and absolute positions.
 *
 * There are two coordinate systems: relative and absolute.
 *
 * Relative:
 *  * Everything is drawn relative to the player, if it doesn't fit on screen
 *    it is clipped.
 *  * Origin (0., 0.) would be ontop of the player.
 *
 * Absolute:
 *  * Everything is drawn in "screen coordinates".
 *  * (0., 0.) is bottom left.
 *  * (SCREEN_W, SCREEN_H) is top right.
 *
 * Note that the game actually uses a third type of coordinates for when using
 *  raw commands.  In this third type, the (0.,0.) is actually in middle of the
 *  screen.  (-SCREEN_W/2.,-SCREEN_H/2.) is bottom left and
 *  (+SCREEN_W/2.,+SCREEN_H/2.) is top right.
 */


/** @cond */
#include "physfsrwops.h"
#include "SDL.h"
#include "SDL_error.h"
#include "SDL_image.h"

#include "naev.h"
/** @endcond */

#include "opengl.h"

#include "conf.h"
#include "log.h"
#include "render.h"


/*
 * Requirements
 */
#define OPENGL_REQ_MULTITEX         2 /**< 2 is minimum OpenGL 1.2 must have */


glInfo gl_screen; /**< Gives data of current opengl settings. */
static int gl_activated = 0; /**< Whether or not a window is activated. */


static unsigned int colorblind_pp = 0; /**< Colorblind post-process shader id. */


/*
 * Viewport offsets
 */
static int gl_view_x = 0; /* X viewport offset. */
static int gl_view_y = 0; /* Y viewport offset. */
static int gl_view_w = 0; /* Viewport width. */
static int gl_view_h = 0; /* Viewport height. */
gl_Matrix4 gl_view_matrix = {{{0}}};


/*
 * prototypes
 */
/* gl */
static int gl_setupAttributes();
static int gl_createWindow( unsigned int flags );
static int gl_getFullscreenMode (void);
static int gl_getGLInfo (void);
static int gl_defState (void);
static int gl_setupScaling (void);


/*
 *
 * M I S C
 *
 */
/**
 * @brief Takes a screenshot.
 *
 *    @param filename PhysicsFS path (e.g., "screenshots/screenshot042.png") of the file to save screenshot as.
 */
void gl_screenshot( const char *filename )
{
   GLubyte *screenbuf;
   SDL_RWops *rw;
   SDL_Surface *surface;
   int i, w, h;

   /* Allocate data. */
   w           = gl_screen.rw;
   h           = gl_screen.rh;
   screenbuf   = malloc( sizeof(GLubyte) * 3 * w*h );
   surface     = SDL_CreateRGBSurface( 0, w, h, 24, RGBAMASK );

   /* Read pixels from buffer -- SLOW. */
   glPixelStorei(GL_PACK_ALIGNMENT, 1); /* Force them to pack the bytes. */
   glReadPixels( 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, screenbuf );

   /* Convert data. */
   for (i=0; i<h; i++)
      memcpy((GLubyte*)surface->pixels + i*surface->pitch,
            &screenbuf[(h-i-1) * (3*w)], 3*w);
   free( screenbuf );

   /* Save PNG. */
   if (!(rw = PHYSFSRWOPS_openWrite( filename )))
      WARN( _("Aborting screenshot") );
   else
      IMG_SavePNG_RW( surface, rw, 1 );

   /* Check to see if an error occurred. */
   gl_checkErr();

   /* Free memory. */
   SDL_FreeSurface( surface );
}


/*
 *
 * G L O B A L
 *
 */
/**
 * @brief Checks to see if opengl version is at least major.minor.
 *
 *    @param major Major version to check.
 *    @param minor Minor version to check.
 *    @return True if major and minor version are met.
 */
GLboolean gl_hasVersion( int major, int minor )
{
   if (GLVersion.major >= major && GLVersion.minor >= minor)
      return GL_TRUE;
   return GL_FALSE;
}


#ifdef DEBUGGING
/**
 * @brief Checks and reports if there's been an error.
 */
void gl_checkHandleError( const char *func, int line )
{
   GLenum err;
   const char* errstr;

   err = glGetError();

   /* No error. */
   if (err == GL_NO_ERROR)
      return;

   switch (err) {
      case GL_INVALID_ENUM:
         errstr = _("GL invalid enum");
         break;
      case GL_INVALID_VALUE:
         errstr = _("GL invalid value");
         break;
      case GL_INVALID_OPERATION:
         errstr = _("GL invalid operation");
         break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
         errstr = _("GL invalid framebuffer operation");
         break;
      case GL_OUT_OF_MEMORY:
         errstr = _("GL out of memory");
         break;

      default:
         errstr = _("GL unknown error");
         break;
   }
   WARN(_("OpenGL error [%s:%d]: %s"), func, line, errstr);
}
#endif /* DEBUGGING */


/**
 * @brief Tries to set up the OpenGL attributes for the OpenGL context.
 *
 *    @return 0 on success.
 */
static int gl_setupAttributes()
{
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); /* Ideally want double buffering. */
   if (conf.fsaa > 1) {
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, conf.fsaa);
   }
   SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

   return 0;
}


/**
 * @brief Tries to apply the configured display mode to the window.
 *
 *    @note Caller is responsible for calling gl_resize/naev_resize afterward.
 *    @return 0 on success.
 */
int gl_setupFullscreen (void)
{
   int display_index;
   int ok;
   SDL_DisplayMode target, closest;

   display_index = SDL_GetWindowDisplayIndex( gl_screen.window );

   if (conf.fullscreen && conf.modesetting) {
      /* Try to use desktop resolution if nothing is specifically set. */
      if (conf.explicit_dim) {
         SDL_GetWindowDisplayMode( gl_screen.window, &target );
         target.w = conf.width;
         target.h = conf.height;
      }
      else
         SDL_GetDesktopDisplayMode( display_index, &target );

      if (SDL_GetClosestDisplayMode( display_index, &target, &closest ) == NULL)
         SDL_GetDisplayMode( display_index, 0, &closest ); /* fall back to the best one */

      SDL_SetWindowDisplayMode( gl_screen.window, &closest );
   }
   ok = SDL_SetWindowFullscreen( gl_screen.window, gl_getFullscreenMode() );
   /* HACK: Force pending resize events to be processed, particularly on Wayland. */
   SDL_PumpEvents();
   SDL_GL_SwapWindow(gl_screen.window);
   SDL_GL_SwapWindow(gl_screen.window);
   return ok;
}


/**
 * @brief Returns the fullscreen configuration as SDL2 flags.
 *
 * @return Appropriate combination of SDL_WINDOW_FULLSCREEN* flags.
 */
static int gl_getFullscreenMode (void)
{
   if (conf.fullscreen)
      return conf.modesetting ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_FULLSCREEN_DESKTOP;
   return 0;
}

/**
 * @brief Creates the OpenGL window.
 *
 *    @return 0 on success.
 */
static int gl_createWindow( unsigned int flags )
{
   int ret;

   flags |= SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
   if (conf.resizable)
      flags |= SDL_WINDOW_RESIZABLE;
   if (conf.borderless)
      flags |= SDL_WINDOW_BORDERLESS;

   /* Create the window. */
   gl_screen.window = SDL_CreateWindow( APPNAME,
         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
         conf.width, conf.height, flags );
   if (gl_screen.window == NULL)
      ERR(_("Unable to create window! %s"), SDL_GetError());

   /* Set focus loss behaviour. */
   SDL_SetHint( SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS,
         conf.minimize ? "1" : "0" );

   /* Create the OpenGL context, note we don't need an actual renderer. */
   gl_setupAttributes();
   gl_screen.context = SDL_GL_CreateContext(gl_screen.window);
   if (!gl_screen.context)
      ERR(_("Unable to create OpenGL context! %s"), SDL_GetError());

   /* Set Vsync. */
   if (conf.vsync) {
      ret = SDL_GL_SetSwapInterval( 1 );
      if (ret == 0)
         gl_screen.flags |= OPENGL_VSYNC;
   } else {
      SDL_GL_SetSwapInterval( 0 );
   }

   /* Finish getting attributes. */
   gl_screen.current_fbo = 0; /* No FBO set. */
   gl_screen.fbo[0] = GL_INVALID_VALUE;
   gl_screen.fbo_tex[0] = GL_INVALID_VALUE;
   gl_screen.fbo[1] = GL_INVALID_VALUE;
   gl_screen.fbo_tex[1] = GL_INVALID_VALUE;
   SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &gl_screen.depth );
   gl_activated = 1; /* Opengl is now activated. */

   return 0;
}


/**
 * @brief Gets some information about the OpenGL window.
 *
 *    @return 0 on success.
 */
static int gl_getGLInfo (void)
{
   int doublebuf;

   SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &gl_screen.r );
   SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &gl_screen.g );
   SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &gl_screen.b );
   SDL_GL_GetAttribute( SDL_GL_ALPHA_SIZE, &gl_screen.a );
   SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &doublebuf );
   SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &gl_screen.fsaa );
   if (doublebuf)
      gl_screen.flags |= OPENGL_DOUBLEBUF;
   if (GLAD_GL_ARB_shader_subroutine && glGetSubroutineIndex && glGetSubroutineUniformLocation && glUniformSubroutinesuiv)
      gl_screen.flags |= OPENGL_SUBROUTINES;
   /* Calculate real depth. */
   gl_screen.depth = gl_screen.r + gl_screen.g + gl_screen.b + gl_screen.a;

   /* Texture information */
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_screen.tex_max);
   glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &gl_screen.multitex_max);

   /* Debug happiness */
   DEBUG(_("OpenGL Drawable Created: %d×%d@%dbpp"), gl_screen.rw, gl_screen.rh, gl_screen.depth);
   DEBUG(_("r: %d, g: %d, b: %d, a: %d, db: %s, fsaa: %d, tex: %d"),
         gl_screen.r, gl_screen.g, gl_screen.b, gl_screen.a,
         gl_has(OPENGL_DOUBLEBUF) ? _("yes") : _("no"),
         gl_screen.fsaa, gl_screen.tex_max);
   DEBUG(_("vsync: %s"), gl_has(OPENGL_VSYNC) ? _("yes") : _("no"));
   DEBUG(_("Renderer: %s"), glGetString(GL_RENDERER));
   DEBUG(_("Version: %s"), glGetString(GL_VERSION));

   /* Now check for things that can be bad. */
   if (gl_screen.multitex_max < OPENGL_REQ_MULTITEX)
      WARN(_("Missing texture units (%d required, %d found)"),
            OPENGL_REQ_MULTITEX, gl_screen.multitex_max );
   if ((conf.fsaa > 1) && (gl_screen.fsaa != conf.fsaa))
      WARN(_("Unable to get requested FSAA level (%d requested, got %d)"),
            conf.fsaa, gl_screen.fsaa );

   return 0;
}


/**
 * @brief Sets the opengl state to it's default parameters.
 *
 *    @return 0 on success.
 */
static int gl_defState (void)
{
   glDisable( GL_DEPTH_TEST ); /* set for doing 2d */
/* glEnable(  GL_TEXTURE_2D ); never enable globally, breaks non-texture blits */
   glEnable(  GL_BLEND ); /* alpha blending ftw */

   /* Set the blending/shading model to use. */
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); /* good blend model */

   gl_checkErr();

   return 0;
}


/**
 * @brief Sets up dimensions in gl_screen, including scaling as needed.
 *
 *    @return 0 on success.
 */
static int gl_setupScaling (void)
{
   double scalew, scaleh;

   /* Get the basic dimensions from SDL2. */
   SDL_GetWindowSize(gl_screen.window, &gl_screen.w, &gl_screen.h);
   SDL_GL_GetDrawableSize(gl_screen.window, &gl_screen.rw, &gl_screen.rh);
   /* Calculate scale factor, if OS has native HiDPI scaling. */
   gl_screen.dwscale = (double)gl_screen.w / (double)gl_screen.rw;
   gl_screen.dhscale = (double)gl_screen.h / (double)gl_screen.rh;

   /* Combine scale factor from OS with the one in Naev's config */
   gl_screen.scale = fmax(gl_screen.dwscale, gl_screen.dhscale) / conf.scalefactor;

   /* New window is real window scaled. */
   gl_screen.nw = (double)gl_screen.rw * gl_screen.scale;
   gl_screen.nh = (double)gl_screen.rh * gl_screen.scale;
   /* Small windows get handled here. */
   if ((gl_screen.nw < RESOLUTION_W_MIN)
         || (gl_screen.nh < RESOLUTION_H_MIN)) {
      if (gl_screen.scale != 1.)
         DEBUG(_("Screen size too small, upscaling..."));
      scalew = RESOLUTION_W_MIN / (double)gl_screen.nw;
      scaleh = RESOLUTION_H_MIN / (double)gl_screen.nh;
      gl_screen.scale *= MAX( scalew, scaleh );
      /* Rescale. */
      gl_screen.nw = (double)gl_screen.rw * gl_screen.scale;
      gl_screen.nh = (double)gl_screen.rh * gl_screen.scale;
   }
   /* Viewport matches new window size. */
   gl_screen.w  = gl_screen.nw;
   gl_screen.h  = gl_screen.nh;
   /* Set scale factors. */
   gl_screen.wscale  = (double)gl_screen.nw / (double)gl_screen.w;
   gl_screen.hscale  = (double)gl_screen.nh / (double)gl_screen.h;
   gl_screen.mxscale = (double)gl_screen.w / (double)gl_screen.rw;
   gl_screen.myscale = (double)gl_screen.h / (double)gl_screen.rh;

   gl_checkErr();

   return 0;
}


/**
 * @brief Initializes SDL/OpenGL and the works.
 *    @return 0 on success.
 */
int gl_init (void)
{
   unsigned int flags;
   GLuint VaoId;

   /* Defaults. */
   memset( &gl_screen, 0, sizeof(gl_screen) );

   flags = SDL_WINDOW_OPENGL | gl_getFullscreenMode();

   /* Initializes Video */
   if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
      WARN(_("Unable to initialize SDL Video: %s"), SDL_GetError());
      return -1;
   }

   /* Create the window. */
   gl_createWindow( flags );

   /* Apply the configured fullscreen display mode, if any. */
   gl_setupFullscreen();

   /* Load extensions. */
   if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
      ERR("Unable to load OpenGL using GLAD");

   /* We are interested in 3.1 because it drops all the deprecated stuff. */
   if ( !GLAD_GL_VERSION_3_1 )
      WARN( "Naev requires OpenGL 3.1, but got OpenGL %d.%d!", GLVersion.major, GLVersion.minor );

   /* Some OpenGL options. */
   glClearColor( 0., 0., 0., 1. );

   /* Set default opengl state. */
   gl_defState();

   /* Handles resetting the viewport and scaling, rw/rh are set in createWindow. */
   gl_resize();

   /* Finishing touches. */
   glClear( GL_COLOR_BUFFER_BIT ); /* must clear the buffer first */
   gl_checkErr();

   /* Initialize subsystems.*/
   gl_initMatrix();
   gl_initTextures();
   gl_initVBO();
   gl_initRender();

   /* Get info about the OpenGL window */
   gl_getGLInfo();

   /* Modern OpenGL requires at least one VAO */
   glGenVertexArrays(1, &VaoId);
   glBindVertexArray(VaoId);

   shaders_load();

   /* Set colorblind shader if necessary. */
   gl_colorblind(conf.colorblind, conf.colorblind_mode);

   /* Set colourspace. */
   glEnable( GL_FRAMEBUFFER_SRGB );

   /* Cosmetic new line. */
   DEBUG_BLANK();

   return 0;
}

/**
 * @brief Handles a window resize and resets gl_screen parameters.
 */
void gl_resize (void)
{
   int i;

   gl_setupScaling();
   glViewport( 0, 0, gl_screen.rw, gl_screen.rh );
   gl_setDefViewport( 0, 0, gl_screen.nw, gl_screen.nh );
   gl_defViewport();

   /* Set up framebuffer. */
   for (i=0; i<2; i++) {
      if (gl_screen.fbo[i] != GL_INVALID_VALUE) {
         glDeleteFramebuffers( 1, &gl_screen.fbo[i] );
         glDeleteTextures( 1, &gl_screen.fbo_tex[i] );
      }
      gl_fboCreate( &gl_screen.fbo[i], &gl_screen.fbo_tex[i], gl_screen.rw, gl_screen.rh );
   }

   gl_checkErr();
}

/**
 * @brief Sets the opengl viewport.
 */
void gl_viewport( int x, int y, int w, int h )
{
   gl_Matrix4 proj;

   proj = gl_Matrix4_Ortho( 0., /* Left edge. */
            gl_screen.nw, /* Right edge. */
            0., /* Bottom edge. */
            gl_screen.nh, /* Top edge. */
            -1., /* near */
            1. ); /* far */

   /* Take into account possible translation. */
   gl_screen.x = x;
   gl_screen.y = y;
   gl_Matrix4_Translate(&proj, x, y, 0);

   /* Set screen size. */
   gl_screen.w = w;
   gl_screen.h = h;

   /* Take into account possible scaling. */
   if (gl_screen.scale != 1.)
      gl_Matrix4_Scale(&proj, gl_screen.wscale, gl_screen.hscale, 1);

   gl_view_matrix = proj;
}


/**
 * @brief Sets the default viewport.
 */
void gl_setDefViewport( int x, int y, int w, int h )
{
   gl_view_x  = x;
   gl_view_y  = y;
   gl_view_w  = w;
   gl_view_h  = h;
}


/**
 * @brief Resets viewport to default
 */
void gl_defViewport (void)
{
   gl_viewport( gl_view_x, gl_view_y, gl_view_w, gl_view_h );
}


/**
 * @brief Translates the window position to screen position.
 */
void gl_windowToScreenPos( int *sx, int *sy, int wx, int wy )
{
   wx /= gl_screen.dwscale;
   wy /= gl_screen.dhscale;

   *sx = gl_screen.mxscale * (double)wx - (double)gl_screen.x;
   *sy = gl_screen.myscale * (double)(gl_screen.rh - wy) - (double)gl_screen.y;
}


/**
 * @brief Translates the screen position to windos position.
 */
void gl_screenToWindowPos( int *wx, int *wy, int sx, int sy )
{
   *wx = (sx + (double)gl_screen.x) / gl_screen.mxscale;
   *wy = (double)gl_screen.rh - (sy + (double)gl_screen.y) / gl_screen.myscale;

   *wx *= gl_screen.dwscale;
   *wy *= gl_screen.dhscale;
}


/**
 * @brief Gets the associated min/mag filter from a string.
 *
 *    @param s String to get filter from.
 *    @return Filter.
 */
GLint gl_stringToFilter( const char *s )
{
   if (strcmp(s,"linear")==0)
      return GL_LINEAR;
   else if (strcmp(s,"nearest")==0)
      return GL_NEAREST;
   return 0;
}


/**
 * @brief Gets the associated min/mag filter from a string.
 *
 *    @param s String to get filter from.
 *    @return Filter.
 */
GLint gl_stringToClamp( const char *s )
{
   if (strcmp(s,"clamp")==0)
      return GL_CLAMP_TO_EDGE;
   else if (strcmp(s,"repeat")==0)
      return GL_REPEAT;
   else if (strcmp(s,"mirroredrepeat")==0)
      return GL_MIRRORED_REPEAT;
   return 0;
}


/**
 * @brief Enables or disables the colorblind shader.
 *
 *    @param enable Whether or not to enable or disable the colorblind shader.
 *    @param mode Which colorblind mode to use.
 */
void gl_colorblind(int enable, ColorblindMode mode)
{
   LuaShader_t shader;
   if (enable) {
      if (colorblind_pp != 0)
         return;
      memset(&shader, 0, sizeof(LuaShader_t));
      shader.program = shaders.colorblind.program;
      shader.VertexPosition = shaders.colorblind.VertexPosition;
      shader.ClipSpaceFromLocal = shaders.colorblind.ClipSpaceFromLocal;
      shader.MainTex = shaders.colorblind.MainTex;

      glUseProgram(shaders.colorblind.program);
      glUniform1i(shaders.colorblind.mode, mode);
      glUseProgram(0);
      colorblind_pp = render_postprocessAdd(&shader, PP_LAYER_FINAL, 99);
   } else {
      if (colorblind_pp != 0)
         render_postprocessRm( colorblind_pp );
      colorblind_pp = 0;
   }
}


/**
 * @brief Cleans up OpenGL, the works.
 */
void gl_exit (void)
{
   int i;
   for (i=0; i<2; i++) {
      if (gl_screen.fbo[i] != GL_INVALID_VALUE) {
         glDeleteFramebuffers( 1, &gl_screen.fbo[i] );
         glDeleteTextures( 1, &gl_screen.fbo_tex[i] );
         gl_screen.fbo[i] = GL_INVALID_VALUE;
         gl_screen.fbo_tex[i] = GL_INVALID_VALUE;
      }
   }

   /* Exit the OpenGL subsystems. */
   gl_exitRender();
   gl_exitVBO();
   gl_exitTextures();
   gl_exitMatrix();

   shaders_unload();

   /* Shut down the subsystem */
   SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
