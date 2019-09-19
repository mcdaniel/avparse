/*//////////////////////////////////////////////////////////////////////////////
//
//  File          : avparse.y
//  Description   : This aviation grammar definition code for the aviation 
//                  weather reporting parsing library.
//
//   Author       : Patrick McDaniel (pdmcdan@gmail.com)
//   Created      : Sat Sep  7 08:53:47 EDT 2019
*/

%{

// Includes
#include <stdio.h>
#include <unistd.h>
#include <avparse.h>
#include <avfldparse.h>

// Definitions
#define YYDEBUG 1 // Enable parsing 
#define AVPARSE_ARGUMENTS "hdf:"
#define AVPARSE_USAGE \
    "\nUSAGE: avparse [-h] [-d]\n" \
    "\n" \
    "where:\n" \
	"    -h - help mode (display this message)\n" \
    "    -d - debug mode (enables parse trace)\n\n"

// Functional prototypes (to keep the compiler happy) */
void yyerror(char *s);
extern char *yytext;

avparser_out *avout;
FILE *yyin;

%}

/* Declare all of the types of parsed values */
%union {
	int           intval;
	char         *strval;
	avreading    *parsed;
}

/* Declare the tokens we will be using */
%token <strval> AIRPORT
%token <strval> ZULUTIME
%token <strval> VISIBILITY
%token <strval> WIND
%token <strval> WINDGUST
%token <strval> COVERAGE
%token <strval> TEMPERATURE
%token <strval> ALTIMETER
%token <intval> EOL
%token <intval> UNKNOWN

%type <parsed> avmetar_expression

%%

avmetar: 
	avmetar_expression
	| avmetar avmetar_expression
	;

avmetar_expression:
	AIRPORT ZULUTIME wind VISIBILITY covexpr TEMPERATURE ALTIMETER EOL {
		$$ = allocate_avparser_reading();
		if (avout->readings == NULL) {
			avout->readings = avout->tail = $$;
			avout->no_readings = 1;
		} else {
			avout->tail->next = $$;
			avout->tail = $$;
			avout->no_readings ++;
		}
		$$->field = $1;
		parse_zulu_time($2, &$$->rtime);
		
		printf( "Airport: [X %s]\n", $$->field ); /* free( $1 ); */
	}
	;

wind:
	WIND
	|
	WINDGUST
	;

covexpr: COVERAGE
	| covexpr COVERAGE
	;

%%

int main(int argc, char **argv) {

	// Local variables
	char ch, *infile;

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

            default:  // Default (unknown)
                    fprintf( stderr, "Unknown command line option (%c), aborting.\n", ch );
                    return( -1 );
            }
    }

    // Setup the input for the parser
    if ( infile == NULL ) {
    	yyin = stdin;
    } else {
    	yyin =  fopen(infile, "r");
    }

	// Setup the base structure, process the inputs
	avout = allocate_avparser_struct();
	yyparse();
	print_parsed_input(avout);
	release_avparser_struct(avout);

	return( 0 );
}

void yyerror(char *s) {
  fprintf(stderr, "error: %s, token [%s]\n", s, yytext);
}
