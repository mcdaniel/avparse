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
#include <avparse.h>

/** Functional Prototypes **/

avparser_out * allocate_avparser_struct( void );
void release_avparser_struct( avparser_out *avp );
avreading *allocate_avparser_reading( void );
void release_avparser_reading( avreading *avr );

#define AVFLDPARSE_INCLUDED
#endif