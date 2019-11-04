/*//////////////////////////////////////////////////////////////////////////////
//
//  File          : avparse.c
//  Description   : This file contain the main function for the avparse tool.
//
//   Author       : Patrick McDaniel (pdmcdan@gmail.com)
//   Created      : Mon 04 Nov 2019 04:08:52 AM PST
*/

// Includes
#include <stdio.h>
#include <unistd.h>
#include <avparse.h>
#include <avfldparse.h>

// Definitions
#define AVPARSE_ARGUMENTS "htdf:"
#define AVPARSE_USAGE \
    "\nUSAGE: avparse [-f <input file>] [-h] [-d]\n" \
    "\n" \
    "where:\n" \
	"    -f - use file input from text file, where <input file> is the filename.\n" \
	"    -h - help mode (display this message)\n" \
    "    -d - debug mode (enables parse trace)\n\n"

// Functional prototypes (to keep the compiler happy) */



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

int main(int argc, char **argv) {

	// Local variables
	char ch, *infile;
	int test = 0;

	// Process the command line parameters
    while ((ch = getopt(argc, argv, AVPARSE_ARGUMENTS)) != -1) {

            switch (ch) {
            case 'h': // Help, print usage
                    fprintf( stderr, AVPARSE_USAGE );
                    return( -1 );

            case 'd': // Debug enable
					yydebug = 1;
                    break;

            case 'f': // File input
            		infile = optarg;
            		break;

            case 't': // Perform the test
            		test = 1;
            		break;

            default:  // Default (unknown)
                    fprintf( stderr, "Unknown command line option (%c), aborting.\n", ch );
                    return( -1 );
            }
    }

    // Check for testing of approach
    if ( test ) {
       	avreading_metar_parse(NULL, "KUNV 051253Z 05004KT 10SM SKC 05/03 A3042");
    } else {
    	avreading_metar_parse((infile == NULL) ? stdin : fopen(infile, "r"), NULL);
    }

	/* Print out and free the structure */
	print_parsed_input(avout);
	release_avparser_struct(avout);

	/* Exit the program normally */
	return( 0 );
}
