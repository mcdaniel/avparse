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
    "\nUSAGE: avparse [-f <input file>] [-h] [-d]\n" \
    "\n" \
    "where:\n" \
	"    -f - use file input from text file, where <input file> is the filename.\n" \
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
%type <wndval> wind
%type <cndval> condexpr
%type <cvgval> covexpr

%%

avmetar: 
	avmetar_expression
	| avmetar avmetar_expression
	;

avmetar_expression:
	AIRPORT ZULUTIME wind VISIBILITY condexpr covexpr TEMPERATURE ALTIMETER EOL {
		$$ = allocate_avparser_reading(avout);
		$$->field = $1;
		parse_zulu_time($2, &$$->rtime);
		$$->rwind = *$3;
		free($3);
		$$->rviz = parse_visibility($4);
		/* Condition parsing */
		$$->rcvrg = $6;
		parse_temperature($7, &$$->rtemp);
		$$->raltm = parse_altimeter($8);

		printf( "Airport: [X %s]\n", $$->field ); /* free( $1 ); */
	}
	|
	AIRPORT ZULUTIME wind VISIBILITY covexpr TEMPERATURE ALTIMETER EOL {
		$$ = allocate_avparser_reading(avout);
		$$->field = $1;
		parse_zulu_time($2, &$$->rtime);
		$$->rwind = *$3;
		free($3);
		$$->rviz = parse_visibility($4);
		$$->rcvrg = $5;
		parse_temperature($6, &$$->rtemp);
		$$->raltm = parse_altimeter($7);

		printf( "Airport: [X %s]\n", $$->field ); /* free( $1 ); */
	}
	;

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
	    parse_condition($1, $$);
	    $$->next = NULL;	
    } 
    |
    condexpr CONDITION {
	    $$ = malloc(sizeof(avreading_condition));
	    parse_condition($2, $$);
	    $$->next = $1;
    }
    ;

covexpr: COVERAGE {
		$$ = malloc(sizeof(avreading_coverage));
		parse_coverage($1, $$);
		$$->next = NULL;
	}
	| covexpr COVERAGE {
		$$ = malloc(sizeof(avreading_coverage));
		parse_coverage($2, $$);
		$1->next = $$;
		$$ = $1;
	}
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
