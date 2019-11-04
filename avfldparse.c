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
#include <ctype.h>
#include <avfldparse.h>

/* Functions */

/* Local data */

/* List of cloud coverages */
const char *avr_coverage_strings[] = { 
	"Sky clear", "Few", "Scattered", "Broken", "Overcast", "Uknown"
};

/* List of weather conditions */
const char *avr_condition_strings[AVR_CONDITION_MAX][2] = {
	{ "Condition is unknown",     "??" },
    { "In the vicinity",          "VC" },
    { "Patches",                  "BC" },
    { "Blowing",                  "BL" },
    { "Low drifting",             "DR" },
    { "Freezing",                 "FZ" },
    { "Shallow",                  "MI" },
    { "Partial",                  "PR" },
    { "Showers",                  "SH" },
    { "Thunderstorm",             "TS" },
    { "Drizzle",                  "DZ" },
    { "Large hail",               "GR" },
    { "Small hail, snow pellets", "GS" },
    { "Ice crystals",             "IC" },
    { "Ice pellets",              "PL" },
    { "Rain",                     "RA" },
    { "Snow grains",              "SG" },
    { "Snow",                     "SN" },
    { "Unknown percipitation",    "UP" },
    { "Mist",                     "BR" },
    { "Widespread dust",          "DU" },
    { "Fog",                      "FG" },
    { "Smoke",                    "FU" },
    { "Haze",                     "HZ" },
    { "Spray",                    "PY" },
    { "Sand",                     "SA" },
    { "Volcanic ash",             "VA" },
    { "Dust storm",               "DS" },
    { "Funnel cloud",             "FC" },
    { "Dust/sand whirls",         "PO" },
    { "Squalls",                  "SQ" },
    { "Sand storm",               "SS" },
};

/****

   Base Parsing Functions 

****/

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : avreading_metar_parse
// Description  : parse a METAR string into structure - note that the 
//                this can accept several strings seperated by newline
//
// Inputs       : fl - file handle for metar input (OR)
//                metar - the string containing the METAR
// Outputs      : a pointer to the avreading structure
*/

avparser_out * avreading_metar_parse( FILE *in, char *metar ) {

	/* Allocate structure, parse */
	set_avparser_input( in, metar );
	avout = allocate_avparser_struct();
	yyparse();

	/* Return the parsed data */
	return( avout );
}

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
// Inputs       : avout - parser output structure
// Outputs      : a pointer to the new structure 
*/

avreading *allocate_avparser_reading( avparser_out *avout ) {

	/* Local variables */
	avreading *out;

	/* Create structure if allocation successful, zero */
	if ( (out = malloc(sizeof(avreading))) == NULL ) {
		AVPARSE_FATAL_ERROR("Memory allocation failed");
		exit(-1);
	}
	memset(out, 0x0, sizeof(avreading));

	/* Place in avparser output stuct, as necessary */
	if (avout->readings == NULL) {
		avout->readings = avout->tail = out;
		avout->no_readings = 1;
	} else {
		avout->tail->next = out;
		avout->tail = out;
		avout->no_readings ++;
	}

	/* Return the  weather reading structure */
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
	memset(&adjtime, 0x0, sizeof(adjtime));
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
// Function     : parse_conditions
// Description  : parse the weather conditions information from a textual data
//
// Inputs       : cstr - the string containing the condition data
// Outputs      : pointer to coverage information
*/

avreading_condition * parse_conditions( char *cstr, avreading_condition *conds ) {

	/* Local variables */
	char condstr[3] = { 0x0, 0x0, 0x0 }, tempstr[128];
	int idx = 0, cindex = 0, condnum;

	/* Zero structure */
	memset( conds, 0x0, sizeof(avreading_coverage) );

	/* Read the possible intesity */
	if ( cstr[idx] == '+' ) {
		conds->intensity = AVR_CONDITION_ITENSITY_HEAVY;
		idx ++;
	} else if ( cstr[idx] == '-' ) {
		conds->intensity = AVR_CONDITION_ITENSITY_LIGHT;
		idx ++;
	}

	/* Keep walking the entire string */
	while ( (cindex < AVR_MAX_CONDS) && (cstr[idx] != 0x0) ) {

		/* Sanity check the rest of the string */
		if ( strlen(&cstr[idx]) < 2 ) {
			snprintf(tempstr, 128, "Bad condition data in aviation data [%s]", cstr);
			AVPARSE_FATAL_ERROR(tempstr);
			exit(-1);			
		}

		/* Setup the string, search conditions */
		condstr[0] = toupper(cstr[idx]);
		idx ++;
		condstr[1] = toupper(cstr[idx]); 
		idx ++;

		/* Walk all of the known conditions and check to see if it looks good */
		condnum = AVR_CONDITION_VC;
		while ( (conds->conditions[cindex] == AVR_CONDITION_UN) && 
				(condnum < AVR_CONDITION_MAX) ) {

			/* Is this the correct condition */
			if ( strncmp(condstr, avr_condition_strings[condnum][1], 2) == 0 ) {
				conds->conditions[cindex] = condnum;
			} else {
				condnum ++;
			}

		}

		/* We did not find the condition */
		if ( conds->conditions[cindex] == AVR_CONDITION_UN ) {
			snprintf(tempstr, 128, "Bad condition type in aviation data [%s][%2s]", cstr, condstr);
			AVPARSE_FATAL_ERROR(tempstr);
			exit(-1);	
		}

		/* Move to the next condition */
		cindex ++;
	}

	/* Check to make sure we have exhausted the conditions */
	if ( cstr[idx] != 0x0 ) {
		snprintf(tempstr, 128, "Too many conditions in aviation data [%s]", cstr);
		AVPARSE_FATAL_ERROR(tempstr);
		exit(-1);	
	}

	/* Now return the conditions */
	return( conds );
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

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : parse_temperature
// Description  : parse the temperature and dewpoint, converto to F
//
// Inputs       : cstr - the string containing the temperature data
//              : temp - the structure to put the temperature data into
// Outputs      : the temperature in celsisus
*/

int parse_temperature( char *cstr, avreading_temperature *temp ) {

	/* Local variables */
	char *ptr = cstr;

	/* Read out the temperatiure value */
	if ( ptr[0] == 'M' ) {
		temp->temperature_celsisus = -((ptr[1] - '0')*10 + (ptr[2] - '0'));
		ptr += 4;
	} else {
		temp->temperature_celsisus = (ptr[0] - '0')*10 + (ptr[1] - '0');
		ptr += 3;
	}

	/* Read out the dew point value */
	if ( ptr[0] == 'M' ) {
		temp->dewpoint_celsisus = -((ptr[1] - '0')*10 + (ptr[2] - '0'));
	} else {
		temp->dewpoint_celsisus = (ptr[0] - '0')*10 + (ptr[1] - '0');
	}

	/* Formula for converstion C to F : (0°C × 9/5) + 32 = 32°F */
	temp->temperature_fahrenheit = (temp->temperature_celsisus * 1.8) + 32;
	temp->dewpoint_fahrenheit = (temp->dewpoint_celsisus * 1.8) + 32;

	/* Return the current temperture */
	return( temp->temperature_celsisus );
}

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : parse_altimeter
// Description  : parse the altimeter, convert to inches of mercury
//
// Inputs       : astr - the string containing the altimeter data
// Outputs      : the altimeter reading in inches of mercury
*/

float parse_altimeter( char *astr ) {

	/* Local variables */
	char tempstr[128];
	int reading;

	/* Parse out the altimeter data, return convered value */
	if ( sscanf(astr, "A%d", &reading) != 1 ) {
		snprintf(tempstr, 128, "Bad altimeter data in aviation data [%s]", astr);
		AVPARSE_FATAL_ERROR(tempstr);
		exit(-1);
	}

	/* Convert to inches of mercury */
	return( (float)reading/100.0 );
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
	char *outstr, tempstr[257], timestr[129];
	struct tm * tm_info;
	avreading_condition *condptr;
	avreading_coverage *coverage;

	/* Add the field/time context */
	outstr = malloc(1024);
	memset(outstr, 0x0, 1024);
	snprintf(tempstr, 256, "READING:\n");
	safe_strlcat(outstr, tempstr, 1024);
	snprintf(tempstr, 256, "%*sField: %s\n", ind, "", avr->field);
	safe_strlcat(outstr, tempstr, 1024);

	/* Now do the time */
    tm_info = localtime(&avr->rtime.zulu);
    strftime(timestr, 256, "%r on %A, %B %d %Y", tm_info);
	snprintf(tempstr, 256, "%*sZulu time: %s\n", ind, "", timestr);
	safe_strlcat(outstr, tempstr, 1024);
    tm_info = localtime(&avr->rtime.local);
    strftime(timestr, 256, "%r on %A, %B %d %Y", tm_info);
	snprintf(tempstr, 256, "%*sLocal time: %s\n", ind, "", timestr);
	safe_strlcat(outstr, tempstr, 1024);

	if (avr->rwind.gust != -1) {
		snprintf(tempstr, 256, "%*sWind %d knots at %d, gusting %d knots\n", ind, "", avr->rwind.speed, avr->rwind.direction, avr->rwind.gust);
	} else {
		snprintf(tempstr, 256, "%*sWind %d knots at %d\n", ind, "", avr->rwind.speed, avr->rwind.direction);
	}
	safe_strlcat(outstr, tempstr, 1024);
	snprintf(tempstr, 256, "%*sVisibility: %u statue miles\n", ind, "", avr->rviz);
	safe_strlcat(outstr, tempstr, 1024);

	/* Process the weather conditions */
	condptr = avr->rcond;
	while ( condptr != NULL ) {

		/* Indent the first condition */
		if ( condptr == avr->rcond ) {	
			snprintf(tempstr, 256, "%*s", ind, "");
			safe_strlcat(outstr, tempstr, 1024);
		} else {
			safe_strlcat(outstr, "; ", 1024);		
		}

		/* Encode the condition */
		tempstr[0] = 0x0;
		avreading_condition_to_string(condptr, tempstr, 256);
		safe_strlcat(outstr, tempstr, 1024);
		if ( condptr->next == NULL ) {
			safe_strlcat(outstr, "\n", 1024);
		}
		condptr = condptr->next;
	}
 
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
		safe_strlcat(outstr, tempstr, 1024);
		coverage = coverage->next;
	}

	/* Temperature, Dewpoint */
	snprintf(tempstr, 256, "%*sTemperature: %d Celsisus, %d Fahrenheit\n", ind, "", 
		avr->rtemp.temperature_celsisus, avr->rtemp.temperature_fahrenheit);
	safe_strlcat(outstr, tempstr, 1024);
	snprintf(tempstr, 256, "%*sDew point: %d Celsisus, %d, Fahrenheit\n", ind, "", 
		avr->rtemp.dewpoint_celsisus, avr->rtemp.dewpoint_fahrenheit);
	safe_strlcat(outstr, tempstr, 1024);

	snprintf(tempstr, 256, "%*sAltimeter setting: %4.2f inches\n", ind, "", avr->raltm);
	safe_strlcat(outstr, tempstr, 1024);

	/* Return the new string */
	return(outstr);
}

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : avreading_condition_to_string
// Description  : convert a single condition reading to string
//
// Inputs       : cond - the condition to convert
//                str - the string to place the text into
//                len - the length of the string
// Outputs      : a pointer to the conversion string
*/

char * avreading_condition_to_string( avreading_condition *cond, char *str, size_t len ) {

	/* Local variables */
	char tempstr[128];
	int condidx;

	/* Process the intensity of the condition, if there is one */
	if ( cond->intensity > AVR_CONDITION_ITENSITY_NONE ) {
		if ( cond->intensity == AVR_CONDITION_ITENSITY_LIGHT ) {
			strncpy(str, "Light ", len);
		} else if ( cond->intensity == AVR_CONDITION_ITENSITY_HEAVY ) {
			strncpy(str, "Heavy ", len);
		} else {
			/* Unknown intensity level, error out */
			snprintf(tempstr, 128, "Bad intensity value in parsed aviation data [%d]", cond->intensity);
			AVPARSE_FATAL_ERROR(tempstr);
			exit(-1);			
		}
	}

	/* Now add the conditions */
	condidx = 0;
	while ( (condidx < AVR_MAX_CONDS) && (cond->conditions[condidx] != AVR_CONDITION_UN) ) {

		/* Check for errored condition */
		if ( cond->conditions[condidx] >= AVR_CONDITION_MAX ) {
			/* Unknown intensity level, error out */
			snprintf(tempstr, 128, "Bad condition value in parsed aviation data [%d]", cond->conditions[condidx]);
			AVPARSE_FATAL_ERROR(tempstr);
			exit(-1);					
		}

		/* Now add the condition, move to next */
		if ( condidx > 0 ) {
			safe_strlcat(str, ", ", len-1);
		}
		safe_strlcat(str, avr_condition_strings[cond->conditions[condidx]][0], len-1);
		condidx ++;
	}

	/* EOL, return the string */
	return( str );
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
	char *outstr, *tstr;
	avreading *ptr;
	int lineno = 0;

	/* Add the fields */
	outstr = malloc(4097);
	memset(outstr, 0x0, 4097);

	/* Walk the readings, convert to string and print out */
	ptr = avp->readings;
	while (ptr != NULL) {
		tstr = avreading_to_string(ptr, 2);
		safe_strlcat(outstr, tstr, 4096);
		free(tstr);
		ptr = ptr->next;
		lineno ++;
	}

	printf( "%s", outstr );
	free( outstr );

	/* Return, no return value */
	return;
}
	

/* Utility Functions */

/*/////////////////////////////////////////////////////////////////////////////
//
// Function     : safe_safe_strlcat
// Description  : safe string length concatination 
//                Note: this is a basic version of the BSD safe_strlcat
//
// Inputs       : dst - the destination string
//                src - the source string
//                dst - the length of the destination string (with 0x0 term)
// Outputs      : the number of characters copied
*/

size_t safe_strlcat(char * dst, const char * src, size_t dstsize) {

	/* Local variables */
	size_t dstlen, srclen, cpylen;

	/* Figure out the length to copy, then copy */
	srclen = strlen(src);
	dstlen = strlen(dst);
	if ( srclen + dstlen + 1 < dstsize ) {
		cpylen = srclen;
	} else {
		cpylen = dstsize - (dstlen + 1);
	}

	/* Copy and return */
	strncat(dst, src, cpylen);
	return(cpylen);
}

