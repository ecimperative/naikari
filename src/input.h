/*
 * See Licensing and Copyright notice in naev.h
 */



#ifndef INPUT_H
#  define INPUT_H


/** @cond */
#include "SDL.h"
/** @endcond */


#include "pilot.h"


#define NMOD_NONE    0
#define NMOD_SHIFT   (1<<0)
#define NMOD_CTRL    (1<<1)
#define NMOD_ALT     (1<<2)
#define NMOD_META    (1<<3)
#define NMOD_ANY     0xFFFF /**< Comfort thing SDL is lacking. */


#define KEY_PRESS    ( 1.) /**< Key is pressed. */
#define KEY_RELEASE  (-1.) /**< Key is released. */


/* Layouts (used with input_setDefault() */
enum {
   LAYOUT_ARROWS, /**< Arrow keys layout. */
   LAYOUT_WASD, /**< WASD layout (for QWERTY keyboards). */
   LAYOUT_ZQSD /**< ZQSD layout (for AZERTY keyboards). */
};


/* input types */
typedef enum {
   KEYBIND_NULL, /**< Null keybinding. */
   KEYBIND_KEYBOARD, /**< Keyboard keybinding. */
   KEYBIND_JAXISPOS, /**< Joystick axis positive side keybinding. */
   KEYBIND_JAXISNEG, /**< Joystick axis negative side keybinding. */
   KEYBIND_JBUTTON, /**< Joystick button keybinding. */
   KEYBIND_JHAT_UP, /**< Joystick hat up direction keybinding. */
   KEYBIND_JHAT_DOWN, /**< Joystick hat down direction keybinding. */
   KEYBIND_JHAT_LEFT, /**< Joystick hat left direction keybinding. */
   KEYBIND_JHAT_RIGHT /**< Joystick hat right direction keybinding. */
} KeybindType; /**< Keybind types. */


extern const char *keybind_info[][ 3 ];
extern const int   input_numbinds;


/*
 * set input
 */
void input_setDefault(int layout);
SDL_Keycode input_keyConv( const char *name );
void input_setKeybind( const char *keybind, KeybindType type, SDL_Keycode key, SDL_Keymod mod );
const char *input_modToText( SDL_Keymod mod );
SDL_Keycode input_getKeybind( const char *keybind, KeybindType *type, SDL_Keymod *mod );
void input_getKeybindDisplay( const char *keybind, char *buf, int len );
const char *input_getKeybindDescription( const char *keybind );
const char *input_keyAlreadyBound( KeybindType type, SDL_Keycode key, SDL_Keymod mod );

/*
 * Misc.
 */
SDL_Keymod input_translateMod( SDL_Keymod mod );
void input_enableAll (void);
void input_disableAll (void);
void input_toggleEnable( const char *key, int enable );
int input_clickPos( SDL_Event *event, double x, double y, double zoom, double minpr, double minr );
int input_clickedJump( int jump, int autonav );
int input_clickedPlanet( int planet, int autonav );
int input_clickedAsteroid( int field, int asteroid );
int input_clickedPilot(pilotId_t pilot, int autonav);
void input_clicked( void *clicked );
int input_isDoubleClick( void *clicked );

/*
 * handle input
 */
void input_handle( SDL_Event* event );

/*
 * init/exit
 */
void input_init (void);
void input_exit (void);

/*
 * Updating.
 */
void input_update( double dt );


/*
 * Mouse.
 */
void input_mouseShow (void);
void input_mouseHide (void);


#endif /* INPUT_H */
