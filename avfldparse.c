/*//////////////////////////////////////////////////////////////////////////////
//
//  File          : avfldparse.c
//  Description   : This flie contains code for the field processing for
//                  the avparse library.
//
//   Author       : Patrick McDaniel (pdmcdan@gmail.com)
//   Created      : Wed Sep 11 08:25:35 EDT 2019
*/

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <avfldparse.h>

/* Functions */

/* Local data */

/* List of cloud coverages */
const char *avr_coverage_strings[] = { 
	"Sky clear", "Few", "Scattered", "Broken", "Overcast", "Uknown"
};

/****

   Structure Processing Functions 

****/

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : allocate_avparser_struct
// Description  : allocate/initialize the avparser structure
//
// Inputs       : none
// Outputs      : a pointer to the new structure 
*/

avparser_out * allocate_avparser_struct( void ) {

	/* Local variables */
	avparser_out *out;

	/* Create structure if allocation successful */
	if ( (out = malloc(sizeof(avparser_out))) == NULL ) {
		AVPARSE_FATAL_ERROR("Memory allocation failed, aborting");
		exit(-1);
	}

	/* Clear and return the parser structure */
	memset(out, 0x0, sizeof(avparser_out));
	return( out );
}

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : release_avparser_struct
// Description  : releases the avparser structure and its contents
//
// Inputs       : avp - point to the avparser structure
// Outputs      : a pointer to the new structure 
*/

void release_avparser_struct( avparser_out *avp ) {

	/* Local variables */
	avreading *ptr, *tmp;

	/* Walk the structure and clean up the contents of readings */
	ptr = avp->readings;
	while (ptr != NULL) {
		tmp = ptr;
		ptr = ptr->next;
		release_avparser_reading(tmp);
	}

	/* Release the base structure and return */
	free( avp );
	return;
}

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : allocate_avparser_reading
// Description  : allocate/initialize the reading structure
//
// Inputs       : none
// Outputs      : a pointer to the new structure 
*/

avreading *allocate_avparser_reading( void ) {

	/* Local variables */
	avreading *out;

	/* Create structure if allocation successful */
	if ( (out = malloc(sizeof(avreading))) == NULL ) {
		AVPARSE_FATAL_ERROR("Memory allocation failed");
		exit(-1);
	}

	/* Clear and return the reading structure */
	memset(out, 0x0, sizeof(avreading));
	return( out );
}

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : release_avparser_struct
// Description  : releases the avparser structure and its contents
//
// Inputs       : avr - pointer to the reading structure
// Outputs      : a pointer to the new structure 
*/

void release_avparser_reading( avreading *avr ) {

	/* Local variables */

	/* Release the base structure and return */
	free( avr );
	return;
}

/****

	Parsing Functions 

****/

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : parse_zulu_time
// Description  : parse the zulu time from the metar reading
//
// Inputs       : tstr - the string containing the time data
//              : avt - the time structure to read into
// Outputs      : the time (converted to local time)
*/

time_t parse_zulu_time( char *tstr, avreading_time *avt ) {

	/* Local variables */
	int day, hr, mn;
	char tempstr[128];
	struct tm *ltime, adjtime;
	time_t now;

	/* Scan out the data */
	if ( sscanf(tstr, "%2d%2d%2dZ", &day, &hr, &mn) != 3 ) {
		snprintf(tempstr, 128, "Bad ZULU time in aviation data [%s]", tstr);
		AVPARSE_FATAL_ERROR(tempstr);
		exit(-1);
	}

	/* Get the local time, find the offset */
	now = time(NULL);
	ltime = localtime(&now);
	printf( "The local offset is %ld [%s], zulu time = %s\n.", ltime->tm_gmtoff, ltime->tm_zone, tstr );

	/* Setup time to adjust */
	adjtime.tm_sec = 0;
	adjtime.tm_min = mn;
	adjtime.tm_hour = hr;
	adjtime.tm_mday = day;
	adjtime.tm_year = ltime->tm_year;
	adjtime.tm_mon = ltime->tm_mon;

	/* Check to see if month and/or year rolled over */
	if ( (day != ltime->tm_mday) && (day < ltime->tm_mday) ) {
		adjtime.tm_mon = ltime->tm_mon + 1;
		if ( ltime->tm_year == 12 ) {
			adjtime.tm_year = ltime->tm_year + 1;
		}
	}

	avt->zulu = mktime(&adjtime);
	avt->local = avt->zulu + ltime->tm_gmtoff;
	printf( "The adjusted Zulu time is %s\n", ctime(&avt->zulu));


/* https://www.gnu.org/software/libc/manual/html_node/Broken_002ddown-Time.html */

	return( avt->zulu );
}

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : parse_wind
// Description  : parse the wind information from the metar reading
//
// Inputs       : tstr - the string containing the time data
//              : avw - the wind structure to read from
//              : gust - flag indicating gust information included
// Outputs      : the current wind direction
*/

int parse_wind( char *tstr, avreading_wind *avw, int gust ) {

	/* Local variables */
	char tempstr[128];

	/* Scan out the data */
	if ( !gust ) {
		if ( sscanf(tstr, "%3d%2dKT", &avw->direction, &avw->speed) != 2 ) {
			snprintf(tempstr, 128, "Bad wind data in aviation data [%s]", tstr);
			AVPARSE_FATAL_ERROR(tempstr);
			exit(-1);
		}
		avw->gust = -1;
	} else {
		if ( sscanf(tstr, "%3d%2dG%2dKT", &avw->direction, &avw->speed, &avw->gust) != 3 ) {
			snprintf(tempstr, 128, "Bad wind data in aviation data [%s]", tstr);
			AVPARSE_FATAL_ERROR(tempstr);
			exit(-1);
		}
	}

	/* Return the wind speed */
	return( avw->direction );
}

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : parse_visibility
// Description  : parse the visibillity
//
// Inputs       : tstr - the string containing the time data
// Outputs      : the visibility in statue miles
*/

int parse_visibility( char *tstr ) {

	/* Local variables */
	char tempstr[128];
	int vis;

	/* Scan out the data */
	if ( sscanf(tstr, "%dSM", &vis) != 1 ) {
		snprintf(tempstr, 128, "Bad visibility data in aviation data [%s]", tstr);
		AVPARSE_FATAL_ERROR(tempstr);
		exit(-1);
	}

	/* Return the visibility */
	return( vis );
}

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : parse_coverage
// Description  : parse the coverage information from a textual data
//
// Inputs       : cstr - the string containing the coverage data
// Outputs      : pointer to coverage information
*/

avreading_coverage * parse_coverage( char *cstr, avreading_coverage *coverage ) {

	/* Local variables */
	char cvg[4], tempstr[128];
	int error = 0;

	/* Scan coverage information from data */
	if ( strlen(cstr) < 6 ) {
  		error = (sscanf(cstr, "%3s", cvg) != 1) ;	
    } else {
    	error = (sscanf(cstr, "%3s%3u", cvg, &coverage->altitude ) != 2);
	}

    /* If parsed correctly */
    if ( ! error ) {

	    /* Adjust altitude, process the coverage description */
	    coverage->altitude *= 100;
	    if ( strlen(cvg) != 3 ) {
	    	error = 1;
	    } else if ( (strncasecmp(cvg, "SKC", 3) == 0) || (strncasecmp(cvg, "CLR", 3) == 0) ) {
	    	coverage->coverage = AVR_SKYCLEAR;
	    	coverage->altitude = 0;
	    } else if ( strncasecmp(cvg, "FEW", 3) == 0 ) {
	    	coverage->coverage = AVR_FEW;
	    } else if ( strncasecmp(cvg, "SCT", 3) == 0 ) {
	    	coverage->coverage = AVR_SCATTERED;
	    } else if ( strncasecmp(cvg, "BKN", 3) == 0 ) {
	    	coverage->coverage = AVR_BROKEN;
	    } else if ( strncasecmp(cvg, "OVC", 3) == 0 ) {
	    	coverage->coverage = AVR_OVERCAST;
	    } else {
	    	error = 1;
	    }

	}

    /* Check for error, bail out as necessary */
    if ( error ) {
		snprintf(tempstr, 128, "Bad coverage data in aviation data [%s]", cstr);
		AVPARSE_FATAL_ERROR(tempstr);
		exit(-1);
    }

    /* Return the processed coverage */
    return( coverage );
}

/****

	Output / Debug Functions 

****/

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : avreading_to_string
// Description  : convert the avreading to a simple string
//
// Inputs       : avp - pointer to the avparser output
//                ind - indentation for fields
// Outputs      : a pointer to the new structure 
*/

char * avreading_to_string( avreading *avr, int ind ) {

	/* Local variables */
	char *outstr, tempstr[256];
	avreading_coverage *coverage;

	/* Add the fields */
	outstr = malloc(1024);
	memset(outstr, 0x0, 1024);
	snprintf(tempstr, 256, "READING:\n");
	strncat(outstr, tempstr, 1024);
	snprintf(tempstr, 256, "%*sField: %s\n", ind, "", avr->field);
	strncat(outstr, tempstr, 1024);
	snprintf(tempstr, 256, "%*sZulu time: %s\n", ind, "", ctime(&avr->rtime.zulu));
	strncat(outstr, tempstr, 1024);
	snprintf(tempstr, 256, "%*sLocal time: %s\n", ind, "", ctime(&avr->rtime.local));
	strncat(outstr, tempstr, 1024);
	if (avr->rwind.gust != -1) {
		snprintf(tempstr, 256, "%*sWind %d knots at %d, gusting %d knots\n", ind, "", avr->rwind.speed, avr->rwind.direction, avr->rwind.gust);
	} else {
		snprintf(tempstr, 256, "%*sWind %d knots at %d\n", ind, "", avr->rwind.speed, avr->rwind.direction);
	}
	strncat(outstr, tempstr, 1024);
	snprintf(tempstr, 256, "%*sVisibility: %u statue miles\n", ind, "", avr->rviz);
	strncat(outstr, tempstr, 1024);

	/* Now do the cloud layers */
	coverage = avr->rcvrg;
	while (coverage != NULL) {
		if ((coverage->coverage < AVR_SKYCLEAR) || (coverage->coverage > AVR_OVERCAST)) {
			snprintf(tempstr, 256, "%*sCloud layer %s at %d feet\n", ind, "", 
				avr_coverage_strings[AVR_UNKNOWN], coverage->altitude);
		} else {
			if (coverage->coverage == AVR_SKYCLEAR) {
				snprintf(tempstr, 256, "%*sCloud layer %s\n", ind, "", 
					avr_coverage_strings[coverage->coverage]);
			} else {
				snprintf(tempstr, 256, "%*sCloud layer %s at %d feet\n", ind, "", 
					avr_coverage_strings[coverage->coverage], coverage->altitude);
			}
		}
		strncat(outstr, tempstr, 1024);
		coverage = coverage->next;
	}

	/* Return the new string */
	return(outstr);
}

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : print_parsed_input
// Description  : do a simple output of the parser output
//
// Inputs       : avp - pointer to the avparser output
// Outputs      : a pointer to the new structure 
*/

void print_parsed_input( avparser_out *avp ) {

	/* Local variables */
	char *outstr, *tstr, tempstr[256];
	avreading *ptr;
	int lineno = 0;

	/* Add the fields */
	outstr = malloc(4096);
	memset(outstr, 0x0, 4096);

	/* Walk the readings, convert to string and print out */
	ptr = avp->readings;
	while (ptr != NULL) {
		tstr = avreading_to_string(ptr, 2);
		strncat(outstr, tstr, 4096);
		free(tstr);
		ptr = ptr->next;
	}

	printf( "%s", outstr );
	free( outstr );

	/* Return, no return value */
	return;
}
	

