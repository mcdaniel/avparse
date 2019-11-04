#ifndef AVFLDPARSE_INCLUDED
/*//////////////////////////////////////////////////////////////////////////////
//
//  File          : avfldparse.h
//  Description   : This flie contains defniitions for the field processing for
//                  the avparse library.
//
//   Author       : Patrick McDaniel (pdmcdan@gmail.com)
//   Created      : Wed Sep 11 08:25:35 EDT 2019
*/

/** Include Files **/
#include <time.h>
#include <avparse.h>

/* Defines */
#define AVP_NO_GUST 0
#define AVP_GUST 1

/** Functional Prototypes **/

/* Base Parsing Functions */
avparser_out * avreading_metar_parse( FILE *in, char *metar );

/* Structure Processing Functions */
avparser_out *        allocate_avparser_struct( void );
void                  release_avparser_struct( avparser_out *avp );
avreading *           allocate_avparser_reading( avparser_out *avout );
void                  release_avparser_reading( avreading *avr );

/* Parsing Functions */
time_t                parse_zulu_time( char *tstr, avreading_time *avt );
int                   parse_wind( char *tstr, avreading_wind *avw, int gust );
int                   parse_visibility( char *tstr );
avreading_coverage *  parse_coverage( char *cstr, avreading_coverage *coverage );
avreading_condition * parse_conditions( char *cstr, avreading_condition *conds );
int                   parse_temperature( char *cstr, avreading_temperature *temp );
float                 parse_altimeter( char *astr );

/* Output / Debug Functions  */
char *                avreading_to_string( avreading *avr, int ind );
char *                avreading_condition_to_string( avreading_condition *cond, char *str, size_t len );
void                  print_parsed_input( avparser_out *avp );

/* Utility Functions */
size_t                safe_strlcat(char * dst, const char * src, size_t dstsize);


/* Lexer/processing bookeeping functions */
extern void           set_avparser_input( FILE *in, char *metar );
extern void           yyerror(char *s);
extern int            yylex(void);
extern char *         yytext;

#define AVFLDPARSE_INCLUDED
#endif