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

// Definitions
#define YYDEBUG 1 // Enable parsing 
#define AVPARSE_ARGUMENTS "hd"
#define AVPARSE_USAGE \
    "\nUSAGE: avparse [-h] [-d]\n" \
    "\n" \
    "where:\n" \
	"    -h - help mode (display this message)\n" \
    "    -d - debug mode (enables parse trace)\n\n"

// Functional prototypes (to keep the compiler happy)
void yyerror(char *s);
extern char *yytext;
%}

/* Declare the tokens we will be using */
%token AIRPORT
%token ZULUTIME
%token VISIBILITY
%token WIND
%token COVERAGE
%token TEMPERATURE
%token ALTIMETER
%token EOL
%token UNKNOWN

%%

avmetar: 
	AIRPORT ZULUTIME WIND VISIBILITY covexpr TEMPERATURE ALTIMETER EOL

covexpr: COVERAGE
	| covexpr COVERAGE

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