
/*//////////////////////////////////////////////////////////////////////////////
//
//  File          : avparse.l
//  Description   : This file lexigraphical parsing code for the aviation 
//                  parsing library.
//
//   Author       : Patrick McDaniel (pdmcdan@gmail.com)
//   Created      : Mon Jan  7 15:38:33 EST 2019
*/

/* The preamble containing materials for the code */
%{
	int tokens = 0;

	enum yytokentype {
     AIRPORT = 258,
     ZULUTIME = 259,
     VISIBILITY = 260,
     WIND = 261,
     COVERAGE = 262,
     TEMPERATURE = 263,
     ALTIMETER = 264,
     EOL = 275,
     UNKNOWN = 276
   };
%}

%% /* The recognition tokens for the aviation data */

[A-Z]{4}                                { return AIRPORT; }
[0-9]{6}Z                               { return ZULUTIME; }
[0-9]NM                                 { return VISIBILITY; }
[0-9]{3}[0-9]{2}(G[0-9]{2})?KT          { return WIND; }
(SKC|CLR)|((FEW|SCT|BKN|OVC)[0-9]{3})   { return COVERAGE; }
M?[0-9]{2}\/M?[0-9]{2}                  { return TEMPERATURE; }
A[0-9]{4}                               { return ALTIMETER; }
\n                                      { return EOL; }
[ \t]                                   { /* Ignore white space */ }
[^\t ]+                                 { return UNKNOWN; }

%%

int main(int argc, char **argv) {

	int tok;
	while ((tok=yylex()) != 0)   {
		printf("Token %d [%s]\n", tok, yytext);
	}

	/* Exit successfully */
	return(0);

}
