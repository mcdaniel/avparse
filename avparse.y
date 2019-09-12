/*//////////////////////////////////////////////////////////////////////////////
//
//  File          : avparse.l
//  Description   : This file grammar definition code for the aviation parsing
//                  library.
//
//   Author       : Patrick McDaniel (pdmcdan@gmail.com)
//   Created      : Sat Sep  7 08:53:47 EDT 2019
*/

%{

// Includes
#include <stdio.h>
#include <unistd.h>
#include <avfldparse.h>

// Definitions
#define YYDEBUG 1 // Enable parsing 
#define AVPARSE_ARGUMENTS "hd"
#define AVPARSE_USAGE \
    "\nUSAGE: avparse [-h] [-d]\n" \
    "\n" \
    "where:\n" \
	"    -h - help mode (display this message)\n" \
    "    -d - debug mode (enables parse trace)\n\n"

// Functional prototypes (to keep the compiler happy) */
void yyerror(char *s);
extern char *yytext;

avparser_out *rule;

%}

/* Declare all of the types of parsed values */
%union {
	int    intval;
	char  *strval;
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

%%

avmetar: 
	avmetar_expression
	| avmetar avmetar_expression
	;

avmetar_expression:
	AIRPORT ZULUTIME wind VISIBILITY covexpr TEMPERATURE ALTIMETER EOL {
		printf( "Airport: [%s]\n", $1 ); /* free( $1 ); */
		rule = init_avparser_struct();
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
	char ch;

	// Process the command line parameters
    while ((ch = getopt(argc, argv, AVPARSE_ARGUMENTS)) != -1) {

            switch (ch) {
            case 'h': // Help, print usage
                    fprintf( stderr, AVPARSE_USAGE );
                    return( -1 );

            case 'd': // Debug enable
					yydebug = 1;
                    break;

            default:  // Default (unknown)
                    fprintf( stderr, "Unknown command line option (%c), aborting.\n", ch );
                    return( -1 );
            }
    }

	// Process the inputs
	yyparse();
	return( 0 );
}

void yyerror(char *s) {
  fprintf(stderr, "error: %s, token [%s]\n", s, yytext);
}
