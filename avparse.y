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
#define AVPARSE_ARGUMENTS "htdf:"
#define AVPARSE_USAGE \
    "\nUSAGE: avparse [-f <input file>] [-h] [-d]\n" \
    "\n" \
    "where:\n" \
	"    -f - use file input from text file, where <input file> is the filename.\n" \
	"    -h - help mode (display this message)\n" \
    "    -d - debug mode (enables parse trace)\n\n"

// Functional prototypes (to keep the compiler happy) */
void yyerror(char *s);
extern char *yytext;

avparser_out * avreading_metar_parse( FILE *in, char *metar );


/* Global data */
avparser_out *avout;
FILE *yyin;

%}

/* Declare all of the types of parsed values */
%union {
	int                   intval;
	char                 *strval;
	avreading            *parsed;
	avreading_wind       *wndval;
	avreading_condition  *cndval;
	avreading_coverage   *cvgval;
}

/* Declare the tokens we will be using */
%token <strval> AIRPORT
%token <strval> ZULUTIME
%token <strval> CORRECTION
%token <strval> WIND
%token <strval> WINDGUST
%token <strval> VISIBILITY
%token <strval> CONDITION
%token <strval> COVERAGE
%token <strval> TEMPERATURE
%token <strval> ALTIMETER
%token <intval> EOL
%token <intval> UNKNOWN

%type <parsed> avmetar_expression
%type <parsed> preamble
%type <wndval> wind
%type <cndval> condexpr
%type <cvgval> covexpr

%%

avmetar: 
	avmetar_expression
	| avmetar avmetar_expression
	;

avmetar_expression:
	preamble wind VISIBILITY condexpr covexpr TEMPERATURE ALTIMETER EOL {
		$$ = $1;
		$$->rwind = *$2;
		free($2);
		$$->rviz = parse_visibility($3);
		$$->rcond = $4;
		$$->rcvrg = $5;
		parse_temperature($6, &$$->rtemp);
		$$->raltm = parse_altimeter($7);
	}
	|
	preamble wind VISIBILITY covexpr TEMPERATURE ALTIMETER EOL {
		$$ = $1;
		$$->rwind = *$2;
		free($2);
		$$->rviz = parse_visibility($3);
		$$->rcond = NULL;
		$$->rcvrg = $4;
		parse_temperature($5, &$$->rtemp);
		$$->raltm = parse_altimeter($6);
	}
	;

preamble:
	AIRPORT ZULUTIME { 
		$$ = allocate_avparser_reading(avout);
		$$->field = $1;
		parse_zulu_time($2, &$$->rtime);
		$$->rcorr = 0; 
	}
	|
	AIRPORT ZULUTIME CORRECTION {
		$$ = allocate_avparser_reading(avout);
		$$->field = $1;
		parse_zulu_time($2, &$$->rtime);
		$$->rcorr = 1;
	}

wind:
	WIND {
		$$ = malloc(sizeof(avreading_wind));
		parse_wind($1, $$, AVP_NO_GUST);
	}
	|
	WINDGUST {
		$$ = malloc(sizeof(avreading_wind));
		parse_wind($1, $$, AVP_GUST);
	}
	;

condexpr: CONDITION {
	    $$ = malloc(sizeof(avreading_condition));
	    $$->next = NULL;
	    parse_conditions($1, $$);
    } 
    |
    condexpr CONDITION {
	    $$ = malloc(sizeof(avreading_condition));
	    $$->next = NULL;
	    parse_conditions($2, $$);
	    $1->next = $$;
	    $$ = $1;
    }
    ;

covexpr: COVERAGE {
		$$ = malloc(sizeof(avreading_coverage));
		$$->next = NULL;
		parse_coverage($1, $$);
	}
	| covexpr COVERAGE {
		$$ = malloc(sizeof(avreading_coverage));
		$$->next = NULL;
		parse_coverage($2, $$);
		$1->next = $$;
		$$ = $1;
	}
	;

%%

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

void yyerror(char *s) {
  fprintf(stderr, "error: %s, token [%s]\n", s, yytext);
}

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

	// Setup the input for the parser
	if ( in == NULL ) {
		yy_scan_string(metar);
	} else {
	   	yyin = in;
	}

	/* Allocate structure, parse */
	avout = allocate_avparser_struct();
	yyparse();

	/* Return the parsed data */
	return( avout );
}

