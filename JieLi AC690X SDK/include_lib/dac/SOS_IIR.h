#ifndef SOS_IIR_H_INCLUDED
#define SOS_IIR_H_INCLUDED
typedef struct
{
  /*0*/  int(*mem)[2];
  /*4*/  const int(*coeff)[5];
  /*8*/  int npoint;
    //int nChannel; default stereo
  /*12*/  int nSOS;
  /*16*/  int global_gain;
}SOSIIR_Cfg;

void SOSIIR(int *in, int *out, SOSIIR_Cfg *cfg);
#endif // SOS_IIR_H_INCLUDED
