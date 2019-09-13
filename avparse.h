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

/** Definitions and Types **/

/* Structure for a single reading */
typedef struct avr_struct {
	char                      *field;  // The airfield of the reading
	struct avr_struct *next;   // The next item in the structure
} avreading;

/* Structure for holding all of the readings parsed */
typedef struct av_readings {
	int         no_readings;  // The nunber of parsed readings
	avreading  *readings;     // The readings themeselves
	avreading  *tail;         // The last reading in the list
} avparser_out;

#define AVPARSE_INCLUDED
#endif