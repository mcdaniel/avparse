
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

// Includes
#include <stdio.h>
#include <avparse.h>
#include <avparse.tab.h>

%}

%% /* The recognition tokens for the aviation data */

[A-Z]{4}                                { yylval.strval = strdup(yytext); return AIRPORT; }
[0-9]{6}Z                               { yylval.strval = strdup(yytext); return ZULUTIME; }
[0-9]{1,2}(SM|NM)                       { yylval.strval = strdup(yytext); return VISIBILITY; }
[0-9]{3}[0-9]{2}KT                      { yylval.strval = strdup(yytext); return WIND; }
[0-9]{3}[0-9]{2}(G[0-9]{2})?KT          { yylval.strval = strdup(yytext); return WINDGUST; }
(SKC|CLR)|((FEW|SCT|BKN|OVC)[0-9]{3})   { yylval.strval = strdup(yytext); return COVERAGE; }
M?[0-9]{2}\/M?[0-9]{2}                  { yylval.strval = strdup(yytext); return TEMPERATURE; }
A[0-9]{4}                               { yylval.strval = strdup(yytext); return ALTIMETER; }
\n                                      { return EOL; }
[ \t]                                   { /* Ignore white space */ }
[^\t\n ]+                               { return UNKNOWN; }

%%

/* 

Code is only needed when using lex parsing only 

*/
