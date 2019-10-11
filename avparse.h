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
#define AVR_MAX_CONDS 5

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

/* Cloud coverage enumerated type */
typedef enum avreading_coverage_enum {
	AVR_SKYCLEAR  = 0, /* 0 Octs */
	AVR_FEW       = 1, /* 1-2 Octs */
	AVR_SCATTERED = 2, /* 3-4 Octs */
	AVR_BROKEN    = 3, /* 5-7 Octs */
	AVR_OVERCAST  = 4, /* 8 Octs */
	AVR_UNKNOWN   = 5  /* ????? */
} avreading_coverage_level;

/* Weather conditions */
typedef enum avreading_condition_enum {
	/* Proximity conditions */
	AVR_CONDITION_VC   = 0,  /* In the vicinity */

	/* Condition desscription modifier */
	AVR_CONDITION_BC   = 1,  /* Patches */
	AVR_CONDITION_BL   = 2,  /* Blowing */
	AVR_CONDITION_DR   = 3,  /* Low drifting */
	AVR_CONDITION_FZ   = 4,  /* Freezing */
	AVR_CONDITION_MI   = 5,  /* Shallow */
	AVR_CONDITION_PR   = 6,  /* Partial */
	AVR_CONDITION_SH   = 7,  /* Showers */
	AVR_CONDITION_TS   = 8,  /* Thunderstorm */

	/* Precipitation */
	AVR_CONDITION_DZ   = 9,  /* Drizzle */
	AVR_CONDITION_GR   = 10, /* Large hail */
	AVR_CONDITION_GS   = 11, /* Small hail, snow pellets */
	AVR_CONDITION_IC   = 12, /* Ice crystals */
	AVR_CONDITION_PL   = 13, /* Ice pellets */
	AVR_CONDITION_RA   = 14, /* Rain */
	AVR_CONDITION_SG   = 15, /* Snow grains */
	AVR_CONDITION_SN   = 16, /* Snow */
	AVR_CONDITION_UP   = 17, /* Unknown percipitation */

	/* Obsucration */
	AVR_CONDITION_BR   = 18, /* Mist */
	AVR_CONDITION_DU   = 19, /* Widespread dust */
	AVR_CONDITION_FG   = 20, /* Fog */
	AVR_CONDITION_FU   = 21, /* Smoke */
	AVR_CONDITION_HZ   = 22, /* Haze */
	AVR_CONDITION_PY   = 23, /* Spray */
	AVR_CONDITION_SA   = 24, /* Sand */
	AVR_CONDITION_VA   = 25, /* Volcanic ash */

	/* Other */
	AVR_CONDITION_DS   = 26, /* Dust storm */
	AVR_CONDITION_FC   = 27, /* Funnel cloud */
	AVR_CONDITION_PO   = 28, /* Dust/sand whirls */
	AVR_CONDITION_SQ   = 29, /* Squalls */
	AVR_CONDITION_SS   = 30, /* Sand storm */

	AVR_CONDITION_MAX  = 31, /* Max condition type */

} avreading_conditions;

/* Weather condition intensity */
typedef enum avreading_condition_intensity_enum {
	AVR_CONDITION_ITENSITY_NON    = 0, /* No intensity information */
	AVR_CONDITION_ITENSITY_LIGHT  = 1, /* Light intensity */
	AVR_CONDITION_ITENSITY_HEAVY  = 2, /* Heavy intensity */
} avreading_condition_intensity;

/* Structure containing a coverage layer */
typedef struct avreading_coverage_struct {
	avreading_coverage_level           coverage;  /* Cloud layer coverage */
	unsigned int                       altitude;  /* Altitude of the coverage layer */
	struct avreading_coverage_struct  *next;      /* Next layer in coverage report */
} avreading_coverage;

/* Structure containing the weather conditions information */
typedef struct avreading_condition_struct {
	avreading_conditions                conditions[AVR_MAX_CONDS];
	avreading_condition_intensity       intensity;
	struct avreading_condition_struct  *next;
} avreading_condition;

/* Structure containing a coverage layer */
typedef struct avreading_temperature_struct {
	int	temperature_celsisus; 
	int temperature_fahrenheit;
	int dewpoint_celsisus;
	int dewpoint_fahrenheit;
} avreading_temperature;

/* Structure for a single reading */
typedef struct avr_struct {
	char                      *field;  /* The airfield of the reading */
	avreading_time             rtime;  /* The time of the reading */
	avreading_wind             rwind;  /* THe wind reading */
	unsigned int               rviz;   /* The visibility (in SM) */
	avreading_coverage        *rcvrg;  /* The list of cloud layers */
	avreading_temperature      rtemp;  /* The temperature/dewpoint */
	float                      raltm;  /* The altimeter reading */
	struct avr_struct         *next;   /* The next item in the structure */
} avreading;

/* Structure for holding all of the readings parsed */
typedef struct av_readings {
	int         no_readings;  /* The nunber of parsed readings */
	avreading  *readings;     /* The readings themeselves */
	avreading  *tail;         /* The last reading in the list */
} avparser_out;

/* Static Helper Data */
extern const char *avr_coverage_strings[]; /* List of cloud coverages */
extern const char *avr_condition_strings[][2]; /* List of weather conditions */

#define AVPARSE_INCLUDED
#endif