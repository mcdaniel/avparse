#ifndef AVPARSE_INCLUDED
/*//////////////////////////////////////////////////////////////////////////////
//
//  File          : avparse.h
//  Description   : This flie contains types and constant defniitions for the
//                  the avparse library.
//
//   Author       : Patrick McDaniel (pdmcdan@gmail.com)
//   Created      : Fri Sep 13 08:37:11 EDT 2019
*/

/** Include Files **/
#include <stdio.h>
#include <time.h>


/** Macros **/
#define AVPARSE_FATAL_ERROR(s) fprintf(stderr, "%s at %s, line %d aborting.\n", s, __FILE__, __LINE__);

/** Definitions and Types **/

/* Time structure for aviation reports */
typedef struct avr_time_struct {
	time_t zulu;  /* GMT/Zulu time */
	time_t local; /* The local time */
} avreading_time;

/* Wind structure */
typedef struct avr_wind_struct {
	int speed;     /* The wind speed, in kts */
	int direction; /* The wind direction (0-359) */
	int gust;      /* The wind gust, if applicable (-1 no gust) */
} avreading_wind;

/* Structure for a single reading */
typedef struct avr_struct {
	char                      *field;  /* The airfield of the reading */
	avreading_time             rtime;  /* The time of the reading */
	avreading_wind             rwind;  /* THe wind reading */
	unsigned int               rviz;   /* The visibility (in SM) */
	struct avr_struct         *next;   /* The next item in the structure */
} avreading;

/* Cloud coverage enumerated type */
typedef enum avr_coverage_enum {
	AVR_SKYCLEAR  = 0, /* 0 Octs */
	AVR_FEW       = 1, /* 1-2 Octs */
	AVR_SCATTERED = 2, /* 3-4 Octs */
	AVR_BROKEN    = 3, /* 5-7 Octs */
	AVR_OVERCAST  = 4, /* 8 Octs */
	AVR_UNKNOWN   = 5  /* ????? */
} avr_coverage_level;

/* Structure containing a coverage layer */
typedef struct avr_coverage_struct {
	avr_coverage_level    coverage;  /* Cloud layer coverage */
	unsigned int          altitude;  /* Altitude of the coverage layer */
	struct avr_coverage  *next;      /* Next layer in coverage report */
} avr_coverage;

/* Structure for holding all of the readings parsed */
typedef struct av_readings {
	int         no_readings;  /* The nunber of parsed readings */
	avreading  *readings;     /* The readings themeselves */
	avreading  *tail;         /* The last reading in the list */
} avparser_out;

#define AVPARSE_INCLUDED
#endif