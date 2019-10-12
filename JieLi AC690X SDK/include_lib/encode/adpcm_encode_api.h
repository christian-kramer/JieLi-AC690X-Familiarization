#ifndef adpcm_encode_api_h__
#define adpcm_encode_api_h__

#include "typedef.h"


#define ENCODE_ADPCM_EN     1

#if ENCODE_ADPCM_EN
extern void adpcm_coder_noheader(char indata[], char outdata[],int *index,int *valpred,short len);
extern void adpcm_decoder_noheader(char indata[], short outdata[],int *index,int *valpred,short len);
#endif

#endif // adpcm_encode_api_h__

