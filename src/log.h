/*
 * See Licensing and Copyright notice in naev.h
 */




#ifndef LOG_H
#  define LOG_H


/** @cond */
#include <signal.h>
#include <stdio.h>

#include "gettext.h"
/** @endcond */

#include "nstring.h"

#define LOG(str, args...)  (logprintf(stdout, 1, str, ## args))
#define LOGERR(str, args...)  (logprintf(stderr, 1, str, ## args))
#ifdef DEBUG_PARANOID /* Will cause WARNs to blow up */
#define WARN(str, args...) (logprintf(stderr, 0, _("WARNING %s:%d [%s]: "), __FILE__, __LINE__, __func__), logprintf( stderr, 1, str, ## args), raise(SIGINT))
#else /* DEBUG_PARANOID */
#define WARN(str, args...) (logprintf(stderr, 0, _("Warning: [%s] "), __func__), logprintf( stderr, 1, str, ## args))
#endif /* DEBUG_PARANOID */
#define ERR(str, args...)  (logprintf(stderr, 0, _("ERROR %s:%d [%s]: "), __FILE__, __LINE__, __func__), logprintf( stderr, 1, str, ## args), abort())
#ifdef DEBUG
#  undef DEBUG
#  define DEBUG(str, args...) LOG(str, ## args)
#  ifndef DEBUGGING
#    define DEBUGGING 1
#  endif /* DEBUGGING */
#else /* DEBUG */
#  define DEBUG(str, args...) do {;} while (0)
#endif /* DEBUG */
#define DEBUG_BLANK() DEBUG("%s", "")


PRINTF_FORMAT( 3, 4 ) int logprintf( FILE *stream, int newline, const char *fmt, ... );
void log_init (void);
void log_redirect (void);
void log_clean (void);


#endif /* LOG_H */
