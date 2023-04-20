/*
 * See Licensing and Copyright notice in naev.h
 */



#ifndef SOUND_H
#  define SOUND_H


/** @cond */
#include "SDL_mutex.h"
#include "SDL_rwops.h"
/** @endcond */


extern int sound_disabled;


/*
 * Static configuration.
 */
#define SOUND_FADEOUT         100
#define SOUND_VOICES 128 /**< Maximum number of simultaneous sounds to play, must be at least 16. */
#define SOUND_PILOT_RELATIVE  1     /**< Whether the sound is relative to the pilot (as opposed to the camera). */
#define SOUND_BUFFER_SIZE     128   /**< Size of the buffer (in KiB) to use for music. */


typedef int32_t soundId_t; /**< Type for sound IDs. */
typedef uint32_t voiceId_t; /**< Type for voice IDs. */


/*
 * Environmental features.
 */
typedef enum SoundEnv_e {
   SOUND_ENV_NORMAL, /**< Normal space. */
   SOUND_ENV_NEBULA /**< Nebula space. */
} SoundEnv_t; /**< Type of environment. */


/*
 * sound subsystem
 */
int sound_init (void);
void sound_exit (void);
int sound_update( double dt );
void sound_pause (void);
void sound_resume (void);
int sound_volume( const double vol );
double sound_getVolume (void);
double sound_getVolumeLog (void);
void sound_stopAll (void);
void sound_setSpeed( double s );


/*
 * source management
 */
int source_newRW( SDL_RWops *rw, const char *name, unsigned int flags );
int source_new( const char* filename, unsigned int flags );


/*
 * sound sample management
 */
soundId_t sound_get(const char* name);
double sound_getLength(soundId_t sound);


/*
 * voice management
 */
voiceId_t sound_play(soundId_t sound);
voiceId_t sound_playPos(soundId_t sound, double px, double py,
      double vx, double vy);
void sound_stop(voiceId_t voice);
int sound_updatePos(voiceId_t voice, double px, double py,
      double vx, double vy);
int sound_updateListener(double px, double py, double vx, double vy);


/*
 * Group functions.
 */
int sound_reserve( int num );
int sound_createGroup( int size );
int sound_playGroup(int group, soundId_t sound, int once);
void sound_stopGroup( int group );
void sound_pauseGroup( int group );
void sound_resumeGroup( int group );
void sound_speedGroup( int group, int enable );
void sound_volumeGroup( int group, double volume );


/*
 * Environmental functions.
 */
int sound_env( SoundEnv_t env, double param );

/* Lock for OpenAL operations. */
extern SDL_mutex *sound_lock; /**< Global sound lock, used for all OpenAL calls. */
#define soundLock()        SDL_mutexP(sound_lock)
#define soundUnlock()      SDL_mutexV(sound_lock)


#endif /* SOUND_H */
