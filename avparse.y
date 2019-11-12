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

void yyerror(char *s) {
  fprintf(stderr, "error: %s, token [%s]\n", s, yytext);
}

void set_avparser_input( FILE *in, char *metar ) {

	// Setup the input for the parser
	if ( in == NULL ) {
		yy_scan_string(metar);
	} else {
	   	yyin = in;
	}

	return;
}
