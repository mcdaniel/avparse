#ifndef AVFLDPARSE_INCLUDED
/*//////////////////////////////////////////////////////////////////////////////
//
//  File          : avfldparse.h
//  Description   : This flie contains defniitions for the field processing for
//                  the avparse library.
//
//   Author       : Patrick McDaniel (pdmcdan@gmail.com)
//   Created      : Wed Sep 11 08:25:35 EDT 2019
*/

/* Include Files */

/*
    Definitions and Types 
*/

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

/* 
    Functional Prototypes 
*/

avparser_out * init_avparser_struct( void );  /* allocate/initialize the avparser structure */

#define AVFLDPARSE_INCLUDED
#endif