#ifndef _FCVT_DEFINED_
#define _FCVT_DEFINED_

#define CVTBUFSIZE  64

char * ecvtbuf( double arg, int ndigits, int * decpt, int * sign, char * buf );
char * fcvtbuf( double arg, int ndigits, int * decpt, int * sign, char * buf );

#endif
