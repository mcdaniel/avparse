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
#include <avfldparse.h>

/* Functions */

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
		AVPARSE_FATAL_ERROR("Memory allocation failed, aborting");
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

	Data Processsing / Debug Functions 

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

	/* Add the fields */
	outstr = malloc(1024);
	snprintf(tempstr, 256, "%*sField: %s\n", ind, "", avr->field);
	strncat(outstr, tempstr, 2014);

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

	/* Add the fields */
	outstr = malloc(4096);
	snprintf(tempstr, 256, "READING:\n");
	strncat(outstr, tempstr, 4096);

	/* Walk the readings, convert to string and print out */
	ptr = avp->readings;
	while (ptr != NULL) {
		tstr = avreading_to_string(ptr, 2);
		strncat(outstr, tempstr, 4096);
		free(tstr);
		ptr = ptr->next;
	}

	printf( "%s", outstr );
	free( outstr );

	/* Return, no return value */
	return;
}
	

